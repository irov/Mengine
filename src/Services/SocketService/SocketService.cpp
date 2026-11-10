#include "SocketService.h"

#include "Interface/SocketSystemInterface.h"
#include "Interface/ThreadSystemInterface.h"
#include "Interface/TimeSystemInterface.h"

#include "Kernel/DocumentHelper.h"
#include "Kernel/ReadHelper.h"
#include "Kernel/StatisticHelper.h"
#include "Kernel/ThreadHelper.h"
#include "Kernel/ThreadMutexHelper.h"
#include "Kernel/ThreadMutexScope.h"
#include "Kernel/WriteHelper.h"

#include "Config/StdAlgorithm.h"
#include "Config/StdString.h"
#include "Config/StdUtility.h"

//////////////////////////////////////////////////////////////////////////
SERVICE_FACTORY( SocketService, Mengine::SocketService );
//////////////////////////////////////////////////////////////////////////
namespace Mengine
{
    //////////////////////////////////////////////////////////////////////////
    namespace Detail
    {
        //////////////////////////////////////////////////////////////////////////
        static constexpr uint32_t SocketMagic = 0x53474E4D;
        static constexpr uint16_t SocketVersion = 2;
        static constexpr size_t SocketPacketSize = 1200;
        typedef Mengine::Array<uint8_t, SocketPacketSize> ArraySocketPacket;
        static constexpr size_t SocketHeaderSize = 44;
        static constexpr size_t SocketPayloadSize = SocketPacketSize - SocketHeaderSize;
        static constexpr size_t SocketMessageSize = 64 * 1024;
        static constexpr size_t SocketCommandLimit = 1024;
        static constexpr size_t SocketEventLimit = 1024;
        static constexpr size_t SocketPendingLimit = 512;
        static constexpr size_t SocketReassemblyLimit = 64;
        static constexpr size_t SocketReassemblyByteLimit = 256 * 1024;
        static constexpr uint64_t SocketAckDelayMs = 20;
        static constexpr uint64_t SocketKeepaliveMs = 500;
        static constexpr uint64_t SocketTimeoutMs = 5000;
        static constexpr uint64_t SocketConnectRetryMs = 250;
        static constexpr uint64_t SocketReassemblyTimeoutMs = 2000;
        static const double SOCKET_BANDWIDTH_BYTES_PER_MS = 32.0;
        // Atomic unreliable admission needs a burst that fits one maximum fragmented message.
        static const double SOCKET_BANDWIDTH_CAPACITY = static_cast<double>(SocketMessageSize + ((SocketMessageSize + SocketPayloadSize - 1) / SocketPayloadSize) * SocketHeaderSize);
        static constexpr double SocketLoopbackBandwidthBytesPerMs = 512.0;
        static constexpr double SocketLoopbackBandwidthCapacity = 512.0 * 1024.0;

        enum EPacketFlags : uint16_t
        {
            PacketConnectRequest = 1 << 0,
            PacketConnectAccept = 1 << 1,
            PacketDisconnect = 1 << 2,
            PacketAckOnly = 1 << 3,
            PacketData = 1 << 4
        };
        //////////////////////////////////////////////////////////////////////////
        static bool isLoopbackAddress( const SocketAddress & _address )
        {
            if( _address.family == ESocketAddressFamily::IPv4 )
            {
                return _address.address[0] == 127;
            }
            if( _address.family != ESocketAddressFamily::IPv6 )
            {
                return false;
            }
            for( size_t index = 0; index != 15; ++index )
            {
                if( _address.address[index] != 0 )
                {
                    return false;
                }
            }
            return _address.address[15] == 1;
        }
        //////////////////////////////////////////////////////////////////////////
        static bool equalAddress( const SocketAddress & _left, const SocketAddress & _right )
        {
            if( _left.family != _right.family || _left.port != _right.port )
            {
                return false;
            }
            size_t size = _left.family == ESocketAddressFamily::IPv6 ? 16 : 4;
            return Mengine::StdString::memcmp( _left.address, _right.address, size ) == 0;
        }
        //////////////////////////////////////////////////////////////////////////
    }
    //////////////////////////////////////////////////////////////////////////
    SocketService::SocketService()
        : m_commandCount( 0 )
        , m_nextPeerId( 1 )
        , m_nextConnectionId( 1 )
        , m_maximumPeers( 0 )
        , m_server( false )
        , m_stopping( false )
    {
    }
    //////////////////////////////////////////////////////////////////////////
    SocketService::~SocketService()
    {
    }
    //////////////////////////////////////////////////////////////////////////
    bool SocketService::sequenceNewer_( uint32_t _left, uint32_t _right )
    {
        const uint32_t distance = _left - _right;
        return distance != 0 && distance < UINT32_C(0x80000000);
    }
    //////////////////////////////////////////////////////////////////////////
    bool SocketService::sequenceAcknowledged_( uint32_t _sequence, uint32_t _ack, uint32_t _ackBits )
    {
        if( _sequence == 0 )
        {
            return false;
        }
        if( _ack == 0 )
        {
            return false;
        }
        if( _sequence == _ack )
        {
            return true;
        }
        const uint32_t distance = _ack - _sequence;
        return distance >= 1 && distance <= 32 && (_ackBits & (UINT32_C(1) << (distance - 1))) != 0;
    }
    //////////////////////////////////////////////////////////////////////////
    bool SocketService::sequenceMarkReceived_( uint32_t _sequence, bool * const _initialized, uint32_t * const _latest, uint32_t * const _mask )
    {
        if( *_initialized == false )
        {
            *_initialized = true;
            *_latest = _sequence;
            *_mask = 0;
            return true;
        }

        if( _sequence == *_latest )
        {
            return false;
        }
        if( SocketService::sequenceNewer_( _sequence, *_latest ) == true )
        {
            const uint32_t distance = _sequence - *_latest;
            if( distance > 32 )
            {
                *_mask = 0;
            }
            else if( distance == 32 )
            {
                *_mask = UINT32_C(1) << 31;
            }
            else
            {
                *_mask = (*_mask << distance) | (UINT32_C(1) << (distance - 1));
            }
            *_latest = _sequence;
            return true;
        }

        const uint32_t distance = *_latest - _sequence;
        if( distance == 0 || distance > 32 )
        {
            return false;
        }
        const uint32_t bit = UINT32_C(1) << (distance - 1);
        if( ( *_mask & bit ) != 0 )
        {
            return false;
        }
        *_mask |= bit;
        return true;
    }
    //////////////////////////////////////////////////////////////////////////
    bool SocketService::pendingNeedsSend_( const Peer & _peer, const PendingPacket & _pending, uint64_t _now )
    {
        if( _pending.packetSequence == 0 )
        {
            return true;
        }
        uint32_t baseRto = _peer.rttMs == 0 ? 100 : _peer.rttMs * 2;
        baseRto = Mengine::StdAlgorithm::min<uint32_t>( 1000, baseRto );
        baseRto = Mengine::StdAlgorithm::max<uint32_t>( 100, baseRto );
        uint32_t shift = Mengine::StdAlgorithm::min<uint32_t>( _pending.retryCount, 3 );
        uint32_t rto = baseRto << shift;
        rto = Mengine::StdAlgorithm::min<uint32_t>( 1000, rto );
        if( _now - _pending.lastSendMs < rto )
        {
            return false;
        }

        return true;
    }
    //////////////////////////////////////////////////////////////////////////
    const ServiceRequiredList & SocketService::requiredServices() const
    {
        static const ServiceRequiredList required = {
            SERVICE_ID( SocketSystemInterface ),
            SERVICE_ID( ThreadSystemInterface ),
            SERVICE_ID( TimeSystemInterface )
        };

        return required;
    }
    //////////////////////////////////////////////////////////////////////////
    bool SocketService::_initializeService()
    {
        m_commandMutex = Helper::createThreadMutex( MENGINE_DOCUMENT_FACTORABLE );
        m_eventMutex = Helper::createThreadMutex( MENGINE_DOCUMENT_FACTORABLE );
        if( m_commandMutex == nullptr || m_eventMutex == nullptr )
        {
            return false;
        }

        m_stopping.store( false );
        m_thread = Helper::createThreadIdentity( MENGINE_THREAD_DESCRIPTION( "MNGSocket" ), ETP_ABOVE_NORMAL, [this]( const ThreadIdentityRunnerInterfacePtr & _runner )
        {
            return this->process_( _runner );
        }, 1, MENGINE_DOCUMENT_FACTORABLE );

        return m_thread != nullptr;
    }
    //////////////////////////////////////////////////////////////////////////
    void SocketService::_finalizeService()
    {
        this->stop();
        if( m_thread != nullptr )
        {
            m_thread->join( true );
            m_thread = nullptr;
        }

        this->closeEndpoint_();
        this->clearPeers_();
        m_commands.clear();
        m_commandCount = 0;
        m_events.clear();
        m_eventMutex = nullptr;
        m_commandMutex = nullptr;
    }
    //////////////////////////////////////////////////////////////////////////
    bool SocketService::listen( const SocketAddress & _address, uint32_t _maximumPeers )
    {
        if( _maximumPeers == 0 || _maximumPeers > 1024 )
        {
            return false;
        }
        Command command;
        command.type = ECommandType::Listen;
        command.address = _address;
        command.value = _maximumPeers;
        return this->enqueueCommand_( Mengine::StdUtility::move( command ) );
    }
    //////////////////////////////////////////////////////////////////////////
    bool SocketService::connect( const SocketAddress & _address, SocketPeerId * const _peerId )
    {
        if( _peerId == nullptr )
        {
            return false;
        }
        SocketPeerId peerId = this->allocatePeerId_();
        if( peerId == InvalidSocketPeerId )
        {
            return false;
        }

        Command command;
        command.type = ECommandType::Connect;
        command.peerId = peerId;
        command.address = _address;
        if( this->enqueueCommand_( Mengine::StdUtility::move( command ) ) == false )
        {
            return false;
        }
        *_peerId = peerId;
        return true;
    }
    //////////////////////////////////////////////////////////////////////////
    bool SocketService::disconnect( SocketPeerId _peerId, uint32_t _reason )
    {
        Command command;
        command.type = ECommandType::Disconnect;
        command.peerId = _peerId;
        command.value = _reason;
        return this->enqueueCommand_( Mengine::StdUtility::move( command ) );
    }
    //////////////////////////////////////////////////////////////////////////
    bool SocketService::send( SocketPeerId _peerId, uint8_t _channel, ESocketDelivery _delivery, const void * _data, size_t _size )
    {
        if( _peerId == InvalidSocketPeerId || _channel >= 4 || _size > Detail::SocketMessageSize || ( _data == nullptr && _size != 0 ) )
        {
            return false;
        }
        Command command;
        command.type = ECommandType::Send;
        command.peerId = _peerId;
        command.channel = _channel;
        command.delivery = _delivery;
        command.payload.resize( _size );
        if( _size != 0 )
        {
            Mengine::StdString::memcpy( command.payload.data(), _data, _size );
        }
        return this->enqueueCommand_( Mengine::StdUtility::move( command ) );
    }
    //////////////////////////////////////////////////////////////////////////
    bool SocketService::broadcast( uint8_t _channel, ESocketDelivery _delivery, const void * _data, size_t _size )
    {
        if( _channel >= 4 || _size > Detail::SocketMessageSize || ( _data == nullptr && _size != 0 ) )
        {
            return false;
        }
        Command command;
        command.type = ECommandType::Broadcast;
        command.channel = _channel;
        command.delivery = _delivery;
        command.payload.resize( _size );
        if( _size != 0 )
        {
            Mengine::StdString::memcpy( command.payload.data(), _data, _size );
        }
        return this->enqueueCommand_( Mengine::StdUtility::move( command ) );
    }
    //////////////////////////////////////////////////////////////////////////
    bool SocketService::pollEvent( SocketServiceEvent * const _event )
    {
        if( _event == nullptr )
        {
            return false;
        }
        MENGINE_THREAD_MUTEX_SCOPE( m_eventMutex );
        if( m_events.empty() == true )
        {
            return false;
        }
        *_event = Mengine::StdUtility::move( m_events.front() );
        m_events.pop_front();
        return true;
    }
    //////////////////////////////////////////////////////////////////////////
    void SocketService::stop()
    {
        m_stopping.store( true );
    }
    //////////////////////////////////////////////////////////////////////////
    SocketPeerId SocketService::allocatePeerId_()
    {
        SocketPeerId peerId = m_nextPeerId.load();
        for( ;; )
        {
            if( peerId == InvalidSocketPeerId )
            {
                return InvalidSocketPeerId;
            }
            SocketPeerId nextPeerId = peerId == UINT32_MAX ? InvalidSocketPeerId : peerId + 1;
            if( m_nextPeerId.compare_exchange_weak( peerId, nextPeerId ) == true )
            {
                return peerId;
            }
        }
    }
    //////////////////////////////////////////////////////////////////////////
    bool SocketService::enqueueCommand_( Command && _command )
    {
        MENGINE_THREAD_MUTEX_SCOPE( m_commandMutex );
        if( m_stopping.load() == true )
        {
            return false;
        }

        size_t commandLimit = Detail::SocketCommandLimit;
        if( _command.type == ECommandType::Send || _command.type == ECommandType::Broadcast )
        {
            // Keep capacity for endpoint and disconnect requests during reliable backpressure.
            --commandLimit;
        }
        while( m_commandCount >= commandLimit )
        {
            DequeCommand::iterator unreliable = Mengine::StdAlgorithm::find_if( m_commands.begin(), m_commands.end(), []( const Command & _queued )
            {
                if( _queued.delivery != ESocketDelivery::UnreliableSequenced )
                {
                    return false;
                }
                if( _queued.type == ECommandType::Send )
                {
                    return true;
                }
                if( _queued.type == ECommandType::Broadcast )
                {
                    return true;
                }
                return false;
            } );
            if( unreliable == m_commands.end() )
            {
                return false;
            }
            m_commands.erase( unreliable );
            --m_commandCount;
        }
        m_commands.emplace_back( Mengine::StdUtility::move( _command ) );
        ++m_commandCount;
        return true;
    }
    //////////////////////////////////////////////////////////////////////////
    bool SocketService::process_( const ThreadIdentityRunnerInterfacePtr & _runner )
    {
        if( _runner->isCancel() == true )
        {
            return false;
        }
        if( m_stopping.load() == true )
        {
            this->closeEndpoint_();
            return false;
        }
        uint64_t now = static_cast<uint64_t>(TIME_SYSTEM()->getPlatformTimestamp());
        this->processCommands_( now );
        if( m_stopping.load() == true )
        {
            this->closeEndpoint_();
            return false;
        }
        this->receivePackets_( now );
        this->updatePeers_( now );
        return true;
    }
    //////////////////////////////////////////////////////////////////////////
    void SocketService::processCommands_( uint64_t _now )
    {
        DequeCommand commands;
        {
            MENGINE_THREAD_MUTEX_SCOPE( m_commandMutex );
            commands.swap( m_commands );
        }

        for( Peer & peer : m_peers )
        {
            peer.blockedChannels = 0;
        }

        size_t completed = 0;
        size_t commandCount = commands.size();
        for( size_t index = 0; index != commandCount; ++index )
        {
            Command command = Mengine::StdUtility::move( commands.front() );
            commands.pop_front();
            if( this->processCommand_( command, _now ) == false )
            {
                commands.emplace_back( Mengine::StdUtility::move( command ) );
            }
            else
            {
                ++completed;
            }
            if( m_stopping.load() == true )
            {
                break;
            }
        }

        MENGINE_THREAD_MUTEX_SCOPE( m_commandMutex );
        m_commandCount -= completed;
        while( m_commands.empty() == false )
        {
            commands.emplace_back( Mengine::StdUtility::move( m_commands.front() ) );
            m_commands.pop_front();
        }
        commands.swap( m_commands );
    }
    //////////////////////////////////////////////////////////////////////////
    bool SocketService::processCommand_( Command & _command, uint64_t _now )
    {
        switch( _command.type )
        {
        case ECommandType::Listen:
            this->openEndpoint_( _command.address, true, _command.value );
            break;
        case ECommandType::Connect:
        {
            this->openEndpoint_( _command.address, false, 1 );
            if( m_socket == nullptr )
            {
                break;
            }
            uint32_t connectionId = m_nextConnectionId.fetch_add( 1 );
            if( connectionId == 0 )
            {
                connectionId = m_nextConnectionId.fetch_add( 1 );
            }
            Peer * peer = this->createPeer_( _command.peerId, _command.address, connectionId, EPeerState::Connecting, _now );
            if( peer != nullptr )
            {
                this->sendConnect_( peer, _now );
            }
            break;
        }
        case ECommandType::Disconnect:
            for( size_t index = 0; index != m_peers.size(); ++index )
            {
                if( m_peers[index].id != _command.peerId )
                {
                    continue;
                }
                this->sendDisconnect_( &m_peers[index], _command.value, _now );
                this->removePeer_( index, _command.value, false );
                break;
            }
            break;
        case ECommandType::Send:
        {
            Peer * peer = this->findPeer_( _command.peerId );
            if( peer == nullptr )
            {
                break;
            }
            if( peer->state == EPeerState::Disconnected )
            {
                break;
            }
            if( this->queueMessage_( peer, _command.channel, _command.delivery, _command.payload, _now ) == false )
            {
                if( _command.delivery == ESocketDelivery::ReliableOrdered )
                {
                    return false;
                }
            }
            break;
        }
        case ECommandType::Broadcast:
            if( _command.recipientsInitialized == false )
            {
                for( const Peer & peer : m_peers )
                {
                    if( peer.state != EPeerState::Connected )
                    {
                        continue;
                    }
                    _command.recipients.emplace_back( peer.id );
                }
                _command.recipientsInitialized = true;
            }
            for( size_t index = 0; index != _command.recipients.size(); )
            {
                Peer * peer = this->findPeer_( _command.recipients[index] );
                if( peer != nullptr && peer->state != EPeerState::Disconnected )
                {
                    if( this->queueMessage_( peer, _command.channel, _command.delivery, _command.payload, _now ) == false )
                    {
                        if( _command.delivery == ESocketDelivery::ReliableOrdered )
                        {
                            ++index;
                            continue;
                        }
                    }
                }
                _command.recipients[index] = _command.recipients.back();
                _command.recipients.pop_back();
            }
            if( _command.recipients.empty() == false )
            {
                return false;
            }
            break;
        case ECommandType::Stop:
            m_stopping.store( true );
            this->closeEndpoint_();
            break;
        }
        return true;
    }
    //////////////////////////////////////////////////////////////////////////
    void SocketService::openEndpoint_( const SocketAddress & _address, bool _server, uint32_t _maximumPeers )
    {
        this->closeEndpoint_();
        this->clearPeers_();
        m_server = _server;
        m_maximumPeers = _maximumPeers;
        m_socket = SOCKET_SYSTEM()->createDatagram( MENGINE_DOCUMENT_FACTORABLE );
        if( m_socket == nullptr || m_socket->open( _address.family ) != ESocketResult::Successful )
        {
            this->pushEvent_( SocketServiceEvent{ESocketServiceEventType::TransportError, InvalidSocketPeerId, 0, 1, {}} );
            m_socket = nullptr;
            return;
        }

        m_socket->setReuseAddress( true );
        m_socket->setSendBufferSize( 256 * 1024 );
        m_socket->setReceiveBufferSize( 256 * 1024 );
        m_socket->setBlocking( false );

        SocketAddress bindAddress{};
        bindAddress.family = _address.family;
        bindAddress.port = _server == true ? _address.port : 0;
        if( _server == true )
        {
            Mengine::StdString::memcpy( bindAddress.address, _address.address, sizeof( bindAddress.address ) );
        }

        if( m_socket->bind( bindAddress ) != ESocketResult::Successful )
        {
            this->pushEvent_( SocketServiceEvent{ESocketServiceEventType::TransportError, InvalidSocketPeerId, 0, 2, {}} );
            this->closeEndpoint_();
        }
    }
    //////////////////////////////////////////////////////////////////////////
    void SocketService::closeEndpoint_()
    {
        if( m_socket != nullptr )
        {
            m_socket->close();
        }
        m_socket = nullptr;
    }
    //////////////////////////////////////////////////////////////////////////
    SocketService::Peer * SocketService::findPeer_( SocketPeerId _peerId )
    {
        for( Peer & peer : m_peers )
        {
            if( peer.id == _peerId )
            {
                return &peer;
            }
        }
        return nullptr;
    }
    //////////////////////////////////////////////////////////////////////////
    SocketService::Peer * SocketService::findPeer_( const SocketAddress & _address, uint32_t _connectionId )
    {
        for( Peer & peer : m_peers )
        {
            if( peer.connectionId == _connectionId && Detail::equalAddress( peer.address, _address ) == true )
            {
                return &peer;
            }
        }
        return nullptr;
    }
    //////////////////////////////////////////////////////////////////////////
    SocketService::Peer * SocketService::createPeer_( SocketPeerId _peerId, const SocketAddress & _address, uint32_t _connectionId, EPeerState _state, uint64_t _now )
    {
        if( m_peers.size() >= m_maximumPeers )
        {
            return nullptr;
        }
        Peer peer;
        peer.id = _peerId;
        peer.address = _address;
        peer.connectionId = _connectionId;
        peer.state = _state;
        peer.createdMs = _now;
        peer.lastReceiveMs = _now;
        peer.lastSendMs = _now;
        peer.lastConnectSendMs = 0;
        peer.bandwidthBytesPerMs = Detail::SOCKET_BANDWIDTH_BYTES_PER_MS;
        peer.bandwidthCapacity = Detail::SOCKET_BANDWIDTH_CAPACITY;
        if( Detail::isLoopbackAddress( _address ) == true )
        {
            peer.bandwidthBytesPerMs = Detail::SocketLoopbackBandwidthBytesPerMs;
            peer.bandwidthCapacity = Detail::SocketLoopbackBandwidthCapacity;
        }
        peer.bandwidthTokens = peer.bandwidthCapacity;
        peer.bandwidthTimestampMs = _now;
        m_peers.emplace_back( Mengine::StdUtility::move( peer ) );
        STATISTIC_INC_INTEGER( STATISTIC_SOCKET_PEER_COUNT );

        return &m_peers.back();
    }
    //////////////////////////////////////////////////////////////////////////
    void SocketService::clearPeers_()
    {
        for( const Peer & peer : m_peers )
        {
            MENGINE_UNUSED( peer );

            STATISTIC_DEC_INTEGER( STATISTIC_SOCKET_PEER_COUNT );
            STATISTIC_DEL_INTEGER( STATISTIC_SOCKET_PENDING_RELIABLE_PACKETS, peer.pending.size() );
            STATISTIC_DEL_INTEGER( STATISTIC_SOCKET_PEER_RTT_MS, peer.rttMs );
        }

        m_peers.clear();
    }
    //////////////////////////////////////////////////////////////////////////
    bool SocketService::removePeer_( size_t _index, uint32_t _reason, bool _notify )
    {
        Peer & peer = m_peers[_index];
        SocketPeerId peerId = peer.id;
        if( _notify == true )
        {
            if( this->pushEvent_( SocketServiceEvent{ESocketServiceEventType::PeerDisconnected, peerId, 0, _reason, {}} ) == false )
            {
                peer.state = EPeerState::Disconnected;
                peer.disconnectReason = _reason;
                return false;
            }
        }

        STATISTIC_DEC_INTEGER( STATISTIC_SOCKET_PEER_COUNT );
        STATISTIC_DEL_INTEGER( STATISTIC_SOCKET_PENDING_RELIABLE_PACKETS, peer.pending.size() );
        STATISTIC_DEL_INTEGER( STATISTIC_SOCKET_PEER_RTT_MS, peer.rttMs );

        m_peers.erase( m_peers.begin() + static_cast<ptrdiff_t>(_index) );
        return true;
    }
    //////////////////////////////////////////////////////////////////////////
    void SocketService::sendConnect_( Peer * _peer, uint64_t _now )
    {
        this->sendControl_( _peer, Detail::PacketConnectRequest, 0, _now );
        _peer->lastConnectSendMs = _now;
    }
    //////////////////////////////////////////////////////////////////////////
    void SocketService::sendAccept_( Peer * _peer, uint64_t _now )
    {
        this->sendControl_( _peer, Detail::PacketConnectAccept, 0, _now );
    }
    //////////////////////////////////////////////////////////////////////////
    void SocketService::sendDisconnect_( Peer * _peer, uint32_t _reason, uint64_t _now )
    {
        for( uint32_t index = 0; index != 3; ++index )
        {
            this->sendControl_( _peer, Detail::PacketDisconnect, _reason, _now );
        }
    }
    //////////////////////////////////////////////////////////////////////////
    bool SocketService::queueMessage_( Peer * _peer, uint8_t _channel, ESocketDelivery _delivery, const Data & _payload, uint64_t _now )
    {
        if( _channel >= 4 || _payload.size() > Detail::SocketMessageSize )
        {
            return false;
        }
        uint8_t channelMask = static_cast<uint8_t>(1U << _channel);
        if( (_peer->blockedChannels & channelMask) != 0 )
        {
            return false;
        }
        if( _peer->state != EPeerState::Connected )
        {
            _peer->blockedChannels |= channelMask;
            return false;
        }
        size_t fragmentCountValue = Mengine::StdAlgorithm::max<size_t>( 1, (_payload.size() + Detail::SocketPayloadSize - 1) / Detail::SocketPayloadSize );
        if( fragmentCountValue > 64 )
        {
            return false;
        }
        if( _delivery == ESocketDelivery::ReliableOrdered && _peer->pending.size() + fragmentCountValue > Detail::SocketPendingLimit )
        {
            _peer->blockedChannels |= channelMask;
            return false;
        }
        bool sendImmediately = true;
        for( const PendingPacket & pending : _peer->pending )
        {
            if( SocketService::pendingNeedsSend_( *_peer, pending, _now ) == true )
            {
                sendImmediately = false;
                break;
            }
        }
        const bool reserveUnreliableMessage = _delivery == ESocketDelivery::UnreliableSequenced;
        if( reserveUnreliableMessage == true )
        {
            if( sendImmediately == false )
            {
                return false;
            }
            const size_t messageBytes = _payload.size() + fragmentCountValue * Detail::SocketHeaderSize;
            if( this->consumeBandwidth_( _peer, messageBytes, false, _now ) == false )
            {
                return false;
            }
        }
        uint32_t messageId = _peer->nextMessageId++;
        uint32_t channelSequence;
        if( _delivery == ESocketDelivery::ReliableOrdered )
        {
            channelSequence = _peer->sendReliableChannelSequence[_channel]++;
        }
        else
        {
            channelSequence = _peer->sendUnreliableChannelSequence[_channel]++;
        }

        for( size_t index = 0; index != fragmentCountValue; ++index )
        {
            size_t offset = index * Detail::SocketPayloadSize;
            size_t fragmentSize = Mengine::StdAlgorithm::min( Detail::SocketPayloadSize, _payload.size() - Mengine::StdAlgorithm::min( offset, _payload.size() ) );
            PendingPacket pending;
            pending.messageId = messageId;
            pending.channelSequence = channelSequence;
            pending.totalSize = static_cast<uint32_t>(_payload.size());
            pending.fragmentIndex = static_cast<uint16_t>(index);
            pending.fragmentCount = static_cast<uint16_t>(fragmentCountValue);
            pending.channel = _channel;
            pending.delivery = _delivery;
            pending.payload.resize( fragmentSize );
            if( fragmentSize != 0 )
            {
                Mengine::StdString::memcpy( pending.payload.data(), _payload.data() + offset, fragmentSize );
            }

            if( _delivery == ESocketDelivery::ReliableOrdered )
            {
                if( sendImmediately == true )
                {
                    sendImmediately = this->sendPending_( _peer, &pending, Detail::PacketData, _now, false, false );
                }
                _peer->pending.emplace_back( Mengine::StdUtility::move( pending ) );
                STATISTIC_INC_INTEGER( STATISTIC_SOCKET_PENDING_RELIABLE_PACKETS );
            }
            else
            {
                this->sendPending_( _peer, &pending, Detail::PacketData, _now, false, true );
            }
        }
        return true;
    }
    //////////////////////////////////////////////////////////////////////////
    bool SocketService::sendPending_( Peer * _peer, PendingPacket * _pending, uint16_t _flags, uint64_t _now, bool _retransmit, bool _bandwidthReserved )
    {
        uint32_t packetSequence = _pending->packetSequence;
        if( this->sendPacket_( _peer, _flags, _pending->messageId, _pending->channelSequence, _pending->totalSize, _pending->fragmentIndex, _pending->fragmentCount, _pending->channel, _pending->delivery, _pending->payload.data(),
                               _pending->payload.size(), &packetSequence, _now, _bandwidthReserved ) == false )
        {
            return false;
        }
        _pending->packetSequence = packetSequence;
        _pending->lastSendMs = _now;
        if( _retransmit == true )
        {
            ++_pending->retryCount;
            STATISTIC_INC_INTEGER( STATISTIC_SOCKET_PACKETS_RETRANSMITTED );
        }
        return true;
    }
    //////////////////////////////////////////////////////////////////////////
    bool SocketService::sendControl_( Peer * _peer, uint16_t _flags, uint32_t _reason, uint64_t _now )
    {
        uint32_t sequence = 0;
        return this->sendPacket_( _peer, _flags, _reason, 0, 0, 0, 0, 0, ESocketDelivery::ReliableOrdered, nullptr, 0, &sequence, _now, false );
    }
    //////////////////////////////////////////////////////////////////////////
    void SocketService::sendAcknowledgement_( Peer * _peer, uint32_t _sequence, uint64_t _now )
    {
        uint32_t sequence = 0;
        this->sendPacket_( _peer, Detail::PacketAckOnly, 0, 0, 0, 0, 0, 0, ESocketDelivery::ReliableOrdered, nullptr, 0, &sequence, _now, false, _sequence );
        if( _peer->ackPending == false )
        {
            return;
        }
        this->sendControl_( _peer, Detail::PacketAckOnly, 0, _now );
    }
    //////////////////////////////////////////////////////////////////////////
    bool SocketService::sendPacket_( Peer * _peer, uint16_t _flags, uint32_t _messageId, uint32_t _channelSequence, uint32_t _totalSize, uint16_t _fragmentIndex, uint16_t _fragmentCount, uint8_t _channel, ESocketDelivery _delivery, const uint8_t * _payload, size_t _payloadSize, uint32_t * const _packetSequence, uint64_t _now, bool _bandwidthReserved, uint32_t _ackSequence )
    {
        if( m_socket == nullptr || _payloadSize > Detail::SocketPayloadSize )
        {
            return false;
        }
        const bool control = (_flags & Detail::PacketData) == 0;
        if( _bandwidthReserved == false && this->consumeBandwidth_( _peer, Detail::SocketHeaderSize + _payloadSize, control, _now ) == false )
        {
            return false;
        }

        Detail::ArraySocketPacket packet{};
        uint32_t sequence = *_packetSequence;
        if( sequence == 0 )
        {
            for( ;; )
            {
                sequence = _peer->nextPacketSequence++;
                if( sequence == 0 )
                {
                    continue;
                }
                bool occupied = false;
                for( const PendingPacket & pending : _peer->pending )
                {
                    if( pending.packetSequence != sequence )
                    {
                        continue;
                    }
                    occupied = true;
                    break;
                }
                if( occupied == false )
                {
                    break;
                }
            }
        }
        Helper::writeUint32( packet.data(), Detail::SocketMagic );
        Helper::writeUint16( packet.data() + 4, Detail::SocketVersion );
        Helper::writeUint16( packet.data() + 6, _flags );
        Helper::writeUint32( packet.data() + 8, _peer->connectionId );
        Helper::writeUint32( packet.data() + 12, sequence );
        uint32_t ackSequence = _peer->receiveInitialized == true ? _peer->receiveSequence : 0;
        uint32_t ackBits = _peer->receiveMask;
        if( _ackSequence != 0 )
        {
            ackSequence = _ackSequence;
            ackBits = 0;
        }
        Helper::writeUint32( packet.data() + 16, ackSequence );
        Helper::writeUint32( packet.data() + 20, ackBits );
        Helper::writeUint32( packet.data() + 24, _messageId );
        Helper::writeUint32( packet.data() + 28, _channelSequence );
        Helper::writeUint32( packet.data() + 32, _totalSize );
        Helper::writeUint16( packet.data() + 36, static_cast<uint16_t>(_payloadSize) );
        Helper::writeUint16( packet.data() + 38, _fragmentIndex );
        Helper::writeUint16( packet.data() + 40, _fragmentCount );
        packet[42] = _channel;
        packet[43] = static_cast<uint8_t>(_delivery);
        if( _payloadSize != 0 )
        {
            Mengine::StdString::memcpy( packet.data() + Detail::SocketHeaderSize, _payload, _payloadSize );
        }

        size_t sent = 0;
        ESocketResult result = m_socket->sendTo( _peer->address, packet.data(), Detail::SocketHeaderSize + _payloadSize, &sent );
        if( result != ESocketResult::Successful || sent != Detail::SocketHeaderSize + _payloadSize )
        {
            return false;
        }
        *_packetSequence = sequence;
        if( _ackSequence == 0 )
        {
            _peer->ackPending = false;
        }
        _peer->lastSendMs = _now;
        STATISTIC_ADD_INTEGER( STATISTIC_SOCKET_BYTES_SENT, sent );
        STATISTIC_INC_INTEGER( STATISTIC_SOCKET_PACKETS_SENT );
        return true;
    }
    //////////////////////////////////////////////////////////////////////////
    void SocketService::receivePackets_( uint64_t _now )
    {
        if( m_socket == nullptr )
        {
            return;
        }
        Detail::ArraySocketPacket packet{};
        for( ;; )
        {
            size_t received = 0;
            SocketAddress address;
            ESocketResult result = m_socket->receiveFrom( packet.data(), packet.size(), &received, &address );
            if( result == ESocketResult::WouldBlock || result == ESocketResult::Timeout )
            {
                break;
            }
            if( result != ESocketResult::Successful )
            {
                if( result == ESocketResult::Truncated )
                {
                    continue;
                }
                this->pushEvent_( SocketServiceEvent{ESocketServiceEventType::TransportError, InvalidSocketPeerId, 0, static_cast<uint32_t>(result), {}} );
                break;
            }
            this->processPacket_( address, packet.data(), received, _now );
        }
    }
    //////////////////////////////////////////////////////////////////////////
    void SocketService::processPacket_( const SocketAddress & _address, const uint8_t * _packet, size_t _size, uint64_t _now )
    {
        if( _size < Detail::SocketHeaderSize )
        {
            return;
        }

        uint32_t magic;
        Helper::readUint32( _packet, &magic );

        uint16_t version;
        Helper::readUint16( _packet + 4, &version );

        if( magic != Detail::SocketMagic || version != Detail::SocketVersion )
        {
            return;
        }

        uint16_t flags;
        Helper::readUint16( _packet + 6, &flags );

        uint32_t connectionId;
        Helper::readUint32( _packet + 8, &connectionId );

        uint32_t sequence;
        Helper::readUint32( _packet + 12, &sequence );

        uint32_t ack;
        Helper::readUint32( _packet + 16, &ack );

        uint32_t ackBits;
        Helper::readUint32( _packet + 20, &ackBits );

        uint32_t messageId;
        Helper::readUint32( _packet + 24, &messageId );

        uint32_t channelSequence;
        Helper::readUint32( _packet + 28, &channelSequence );

        uint32_t totalSize;
        Helper::readUint32( _packet + 32, &totalSize );

        uint16_t payloadSize;
        Helper::readUint16( _packet + 36, &payloadSize );

        uint16_t fragmentIndex;
        Helper::readUint16( _packet + 38, &fragmentIndex );

        uint16_t fragmentCount;
        Helper::readUint16( _packet + 40, &fragmentCount );

        uint8_t channel = _packet[42];
        uint8_t deliveryValue = _packet[43];
        if( connectionId == 0 || sequence == 0 || Detail::SocketHeaderSize + payloadSize != _size || channel >= 4 || deliveryValue > static_cast<uint8_t>(ESocketDelivery::UnreliableSequenced) )
        {
            return;
        }

        Peer * peer = this->findPeer_( _address, connectionId );
        if( peer == nullptr )
        {
            if( m_server == false || ( flags & Detail::PacketConnectRequest ) == 0 || m_peers.size() >= m_maximumPeers )
            {
                return;
            }
            SocketPeerId peerId = this->allocatePeerId_();
            if( peerId == InvalidSocketPeerId )
            {
                return;
            }
            peer = this->createPeer_( peerId, _address, connectionId, EPeerState::Connected, _now );
            if( peer == nullptr )
            {
                return;
            }
            if( this->pushEvent_( SocketServiceEvent{ESocketServiceEventType::PeerConnected, peer->id, 0, 0, {}} ) == false )
            {
                size_t index = m_peers.size() - 1;
                this->removePeer_( index, 0, false );
                return;
            }
        }

        if( peer->state == EPeerState::Disconnected )
        {
            return;
        }
        peer->lastReceiveMs = _now;
        STATISTIC_ADD_INTEGER( STATISTIC_SOCKET_BYTES_RECEIVED, _size );
        STATISTIC_INC_INTEGER( STATISTIC_SOCKET_PACKETS_RECEIVED );
        this->acknowledge_( peer, ack, ackBits, _now );
        if( flags == Detail::PacketData )
        {
            if( peer->state != EPeerState::Connected )
            {
                return;
            }
            bool initialized = peer->receiveInitialized;
            uint32_t latest = peer->receiveSequence;
            uint32_t mask = peer->receiveMask;
            bool fresh = SocketService::sequenceMarkReceived_( sequence, &initialized, &latest, &mask );
            ESocketDelivery delivery = static_cast<ESocketDelivery>(deliveryValue);
            if( fresh == false && delivery == ESocketDelivery::UnreliableSequenced )
            {
                peer->ackPending = true;
                return;
            }
            if( this->receiveFragment_( peer, messageId, channelSequence, totalSize, fragmentIndex, fragmentCount, channel, delivery, _packet + Detail::SocketHeaderSize, payloadSize, _now ) == false )
            {
                if( delivery == ESocketDelivery::ReliableOrdered )
                {
                    return;
                }
            }
            if( fresh == false )
            {
                this->sendAcknowledgement_( peer, sequence, _now );
                return;
            }
            peer->receiveInitialized = initialized;
            peer->receiveSequence = latest;
            peer->receiveMask = mask;
            peer->ackPending = true;
            return;
        }
        bool fresh = this->markReceived_( peer, sequence );

        if( (flags & Detail::PacketConnectRequest) != 0 )
        {
            if( m_server == true )
            {
                this->sendAccept_( peer, _now );
            }
            return;
        }

        if( fresh == false )
        {
            return;
        }

        if( (flags & Detail::PacketConnectAccept) != 0 )
        {
            if( peer->state == EPeerState::Connecting )
            {
                if( this->pushEvent_( SocketServiceEvent{ESocketServiceEventType::PeerConnected, peer->id, 0, 0, {}} ) == false )
                {
                    return;
                }
                peer->state = EPeerState::Connected;
            }
            return;
        }

        if( (flags & Detail::PacketDisconnect) != 0 )
        {
            for( size_t index = 0; index != m_peers.size(); ++index )
            {
                if( m_peers[index].id == peer->id )
                {
                    this->removePeer_( index, messageId, true );
                    break;
                }
            }
            return;
        }

    }
    //////////////////////////////////////////////////////////////////////////
    void SocketService::acknowledge_( Peer * _peer, uint32_t _ack, uint32_t _ackBits, uint64_t _now )
    {
        for( size_t index = 0; index < _peer->pending.size(); )
        {
            PendingPacket & pending = _peer->pending[index];
            if( SocketService::sequenceAcknowledged_( pending.packetSequence, _ack, _ackBits ) == false )
            {
                ++index;
                continue;
            }
            if( pending.retryCount == 0 && pending.lastSendMs <= _now )
            {
                uint32_t sample = static_cast<uint32_t>(_now - pending.lastSendMs);
                uint32_t previousRttMs = _peer->rttMs;
                MENGINE_UNUSED( previousRttMs );
                _peer->rttMs = _peer->rttMs == 0 ? sample : (_peer->rttMs * 7 + sample) / 8;
                STATISTIC_DEL_INTEGER( STATISTIC_SOCKET_PEER_RTT_MS, previousRttMs );
                STATISTIC_ADD_INTEGER( STATISTIC_SOCKET_PEER_RTT_MS, _peer->rttMs );
            }
            STATISTIC_DEC_INTEGER( STATISTIC_SOCKET_PENDING_RELIABLE_PACKETS );
            _peer->pending.erase( _peer->pending.begin() + static_cast<ptrdiff_t>(index) );
        }
    }
    //////////////////////////////////////////////////////////////////////////
    bool SocketService::markReceived_( Peer * _peer, uint32_t _sequence )
    {
        return SocketService::sequenceMarkReceived_( _sequence, &_peer->receiveInitialized, &_peer->receiveSequence, &_peer->receiveMask );
    }
    //////////////////////////////////////////////////////////////////////////
    bool SocketService::receiveFragment_( Peer * _peer, uint32_t _messageId, uint32_t _channelSequence, uint32_t _totalSize, uint16_t _fragmentIndex, uint16_t _fragmentCount, uint8_t _channel, ESocketDelivery _delivery, const uint8_t * _payload, size_t _payloadSize, uint64_t _now )
    {
        if( _totalSize > Detail::SocketMessageSize || _fragmentCount == 0 || _fragmentCount > 64 || _fragmentIndex >= _fragmentCount )
        {
            STATISTIC_INC_INTEGER( STATISTIC_SOCKET_PACKETS_REJECTED );
            return false;
        }
        size_t expectedFragments = Mengine::StdAlgorithm::max<size_t>( 1, (_totalSize + Detail::SocketPayloadSize - 1) / Detail::SocketPayloadSize );
        if( _fragmentCount != expectedFragments )
        {
            return false;
        }
        size_t offset = _fragmentIndex * Detail::SocketPayloadSize;
        size_t expectedSize = Mengine::StdAlgorithm::min( Detail::SocketPayloadSize, _totalSize - offset );
        if( _payloadSize != expectedSize )
        {
            return false;
        }
        if( _delivery == ESocketDelivery::ReliableOrdered )
        {
            uint32_t expected = _peer->receiveChannelSequence[_channel];
            if( SocketService::sequenceNewer_( expected, _channelSequence ) == true )
            {
                return true;
            }
            if( _channelSequence != expected )
            {
                return false;
            }
        }
        Reassembly * reassembly = nullptr;
        for( Reassembly & item : _peer->reassemblies )
        {
            if( _delivery == ESocketDelivery::ReliableOrdered && item.delivery == ESocketDelivery::ReliableOrdered && item.channel == _channel )
            {
                if( item.messageId != _messageId )
                {
                    return false;
                }
            }
            if( item.messageId == _messageId && item.channel == _channel )
            {
                reassembly = &item;
                break;
            }
        }
        if( reassembly == nullptr )
        {
            size_t reassemblyBytes = 0;
            for( const Reassembly & item : _peer->reassemblies )
            {
                reassemblyBytes += item.totalSize;
            }
            if( _delivery == ESocketDelivery::ReliableOrdered )
            {
                for( size_t index = 0; index != _peer->reassemblies.size(); )
                {
                    if( _peer->reassemblies.size() < Detail::SocketReassemblyLimit && reassemblyBytes + _totalSize <= Detail::SocketReassemblyByteLimit )
                    {
                        break;
                    }
                    const Reassembly & item = _peer->reassemblies[index];
                    if( item.delivery == ESocketDelivery::ReliableOrdered )
                    {
                        ++index;
                        continue;
                    }
                    reassemblyBytes -= item.totalSize;
                    _peer->reassemblies.erase( _peer->reassemblies.begin() + static_cast<ptrdiff_t>(index) );
                }
            }
            if( _peer->reassemblies.size() >= Detail::SocketReassemblyLimit )
            {
                STATISTIC_INC_INTEGER( STATISTIC_SOCKET_PACKETS_REJECTED );
                return false;
            }
            if( reassemblyBytes + _totalSize > Detail::SocketReassemblyByteLimit )
            {
                STATISTIC_INC_INTEGER( STATISTIC_SOCKET_PACKETS_REJECTED );
                return false;
            }
            Reassembly item;
            item.messageId = _messageId;
            item.channelSequence = _channelSequence;
            item.totalSize = _totalSize;
            item.fragmentCount = _fragmentCount;
            item.channel = _channel;
            item.delivery = _delivery;
            item.createdMs = _now;
            item.fragments.resize( _fragmentCount );
            item.present.resize( _fragmentCount, 0 );
            _peer->reassemblies.emplace_back( Mengine::StdUtility::move( item ) );
            reassembly = &_peer->reassemblies.back();
        }
        if( reassembly->channelSequence != _channelSequence || reassembly->totalSize != _totalSize || reassembly->fragmentCount != _fragmentCount || reassembly->delivery != _delivery )
        {
            STATISTIC_INC_INTEGER( STATISTIC_SOCKET_PACKETS_REJECTED );
            return false;
        }
        if( reassembly->present[_fragmentIndex] == 0 )
        {
            reassembly->fragments[_fragmentIndex].resize( _payloadSize );
            if( _payloadSize != 0 )
            {
                Mengine::StdString::memcpy( reassembly->fragments[_fragmentIndex].data(), _payload, _payloadSize );
            }
            reassembly->present[_fragmentIndex] = 1;
        }

        for( uint8_t present : reassembly->present )
        {
            if( present == 0 )
            {
                return true;
            }
        }
        Data payload;
        payload.reserve( reassembly->totalSize );
        for( const Data & fragment : reassembly->fragments )
        {
            payload.insert( payload.end(), fragment.begin(), fragment.end() );
        }
        if( payload.size() != reassembly->totalSize )
        {
            STATISTIC_INC_INTEGER( STATISTIC_SOCKET_PACKETS_REJECTED );
            return false;
        }
        if( this->deliverMessage_( _peer, _channel, _delivery, _channelSequence, Mengine::StdUtility::move( payload ) ) == false )
        {
            return false;
        }
        for( size_t index = 0; index != _peer->reassemblies.size(); ++index )
        {
            if( &_peer->reassemblies[index] == reassembly )
            {
                _peer->reassemblies.erase( _peer->reassemblies.begin() + static_cast<ptrdiff_t>(index) );
                break;
            }
        }
        return true;
    }
    //////////////////////////////////////////////////////////////////////////
    bool SocketService::deliverMessage_( Peer * _peer, uint8_t _channel, ESocketDelivery _delivery, uint32_t _channelSequence, Data && _payload )
    {
        if( _delivery == ESocketDelivery::UnreliableSequenced )
        {
            if( SocketService::sequenceNewer_( _channelSequence, _peer->lastUnreliableSequence[_channel] ) == false )
            {
                return true;
            }
            _peer->lastUnreliableSequence[_channel] = _channelSequence;
            this->pushEvent_( SocketServiceEvent{ESocketServiceEventType::MessageReceived, _peer->id, _channel, 0, Mengine::StdUtility::move( _payload ), _delivery} );
            return true;
        }

        uint32_t expected = _peer->receiveChannelSequence[_channel];
        if( SocketService::sequenceNewer_( expected, _channelSequence ) == true )
        {
            return true;
        }
        if( _channelSequence != expected )
        {
            return false;
        }

        if( this->pushEvent_( SocketServiceEvent{ESocketServiceEventType::MessageReceived, _peer->id, _channel, 0, Mengine::StdUtility::move( _payload ), _delivery} ) == false )
        {
            return false;
        }
        ++_peer->receiveChannelSequence[_channel];
        return true;
    }
    //////////////////////////////////////////////////////////////////////////
    void SocketService::updatePeers_( uint64_t _now )
    {
        for( size_t peerIndex = 0; peerIndex < m_peers.size(); )
        {
            Peer & peer = m_peers[peerIndex];
            if( peer.state == EPeerState::Disconnected )
            {
                uint32_t reason = peer.disconnectReason;
                if( this->removePeer_( peerIndex, reason, true ) == false )
                {
                    ++peerIndex;
                }
                continue;
            }
            if( _now - peer.lastReceiveMs >= Detail::SocketTimeoutMs )
            {
                if( this->removePeer_( peerIndex, 1, true ) == false )
                {
                    ++peerIndex;
                }
                continue;
            }
            if( peer.state == EPeerState::Connecting && _now - peer.lastConnectSendMs >= Detail::SocketConnectRetryMs )
            {
                this->sendConnect_( &peer, _now );
            }

            for( PendingPacket & pending : peer.pending )
            {
                if( SocketService::pendingNeedsSend_( peer, pending, _now ) == false )
                {
                    continue;
                }
                bool retransmit = pending.packetSequence != 0;
                if( this->sendPending_( &peer, &pending, Detail::PacketData, _now, retransmit, false ) == false )
                {
                    break;
                }
            }

            for( size_t index = 0; index < peer.reassemblies.size(); )
            {
                if( peer.reassemblies[index].delivery == ESocketDelivery::ReliableOrdered )
                {
                    ++index;
                    continue;
                }
                if( _now - peer.reassemblies[index].createdMs >= Detail::SocketReassemblyTimeoutMs )
                {
                    peer.reassemblies.erase( peer.reassemblies.begin() + static_cast<ptrdiff_t>(index) );
                }
                else
                {
                    ++index;
                }
            }

            if( _now - peer.lastSendMs >= Detail::SocketKeepaliveMs || ( peer.ackPending == true && _now - peer.lastSendMs >= Detail::SocketAckDelayMs ) )
            {
                this->sendControl_( &peer, Detail::PacketAckOnly, 0, _now );
            }
            ++peerIndex;
        }
    }
    //////////////////////////////////////////////////////////////////////////
    bool SocketService::pushEvent_( SocketServiceEvent && _event )
    {
        MENGINE_THREAD_MUTEX_SCOPE( m_eventMutex );
        if( m_events.size() >= Detail::SocketEventLimit )
        {
            DequeSocketServiceEvent::iterator unreliable = Mengine::StdAlgorithm::find_if( m_events.begin(), m_events.end(), []( const SocketServiceEvent & _queued )
            {
                return _queued.type == ESocketServiceEventType::MessageReceived && _queued.delivery == ESocketDelivery::UnreliableSequenced;
            } );
            if( unreliable == m_events.end() )
            {
                return false;
            }
            m_events.erase( unreliable );
        }
        m_events.emplace_back( Mengine::StdUtility::move( _event ) );
        return true;
    }
    //////////////////////////////////////////////////////////////////////////
    bool SocketService::consumeBandwidth_( Peer * _peer, size_t _bytes, bool _control, uint64_t _now )
    {
        uint64_t elapsed = _now - _peer->bandwidthTimestampMs;
        _peer->bandwidthTimestampMs = _now;
        _peer->bandwidthTokens = Mengine::StdAlgorithm::min( _peer->bandwidthCapacity, _peer->bandwidthTokens + static_cast<double>(elapsed) * _peer->bandwidthBytesPerMs );
        if( _peer->bandwidthTokens < static_cast<double>(_bytes) )
        {
            if( _control == false )
            {
                return false;
            }
            _peer->bandwidthTokens = 0.0;
            return true;
        }
        _peer->bandwidthTokens -= static_cast<double>(_bytes);
        return true;
    }
    //////////////////////////////////////////////////////////////////////////
}
