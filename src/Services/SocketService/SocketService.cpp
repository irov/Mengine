#include "SocketService.h"

#include "Interface/SocketSystemInterface.h"
#include "Interface/ThreadSystemInterface.h"
#include "Interface/TimeSystemInterface.h"

#include "Kernel/DocumentHelper.h"
#include "Kernel/StatisticHelper.h"
#include "Kernel/ThreadHelper.h"
#include "Kernel/ThreadMutexHelper.h"
#include "Kernel/ThreadMutexScope.h"

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
        static constexpr uint16_t SocketVersion = 1;
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
        static constexpr double SocketBandwidthBytesPerMs = 32.0;
        static constexpr double SocketBandwidthCapacity = 32768.0;
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
        static void writeU16( uint8_t * _buffer, size_t _offset, uint16_t _value )
        {
            _buffer[_offset + 0] = static_cast<uint8_t>(_value);
            _buffer[_offset + 1] = static_cast<uint8_t>(_value >> 8);
        }
        //////////////////////////////////////////////////////////////////////////
        static void writeU32( uint8_t * _buffer, size_t _offset, uint32_t _value )
        {
            _buffer[_offset + 0] = static_cast<uint8_t>(_value);
            _buffer[_offset + 1] = static_cast<uint8_t>(_value >> 8);
            _buffer[_offset + 2] = static_cast<uint8_t>(_value >> 16);
            _buffer[_offset + 3] = static_cast<uint8_t>(_value >> 24);
        }
        //////////////////////////////////////////////////////////////////////////
        static uint16_t readU16( const uint8_t * _buffer, size_t _offset )
        {
            return static_cast<uint16_t>(_buffer[_offset + 0]) |
                static_cast<uint16_t>(static_cast<uint16_t>(_buffer[_offset + 1]) << 8);
        }
        //////////////////////////////////////////////////////////////////////////
        static uint32_t readU32( const uint8_t * _buffer, size_t _offset )
        {
            return static_cast<uint32_t>(_buffer[_offset + 0]) |
                (static_cast<uint32_t>(_buffer[_offset + 1]) << 8) |
                (static_cast<uint32_t>(_buffer[_offset + 2]) << 16) |
                (static_cast<uint32_t>(_buffer[_offset + 3]) << 24);
        }
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
        : m_nextPeerId( 1 )
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

        m_stopping = false;
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
        SocketPeerId peerId = m_nextPeerId.fetch_add( 1 );
        if( peerId == InvalidSocketPeerId )
        {
            peerId = m_nextPeerId.fetch_add( 1 );
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
        if( m_commandMutex == nullptr )
        {
            return;
        }
        Command command;
        command.type = ECommandType::Stop;
        this->enqueueCommand_( Mengine::StdUtility::move( command ) );
    }
    //////////////////////////////////////////////////////////////////////////
    bool SocketService::enqueueCommand_( Command && _command )
    {
        MENGINE_THREAD_MUTEX_SCOPE( m_commandMutex );
        if( m_commands.size() >= Detail::SocketCommandLimit )
        {
            auto unreliable = Mengine::StdAlgorithm::find_if( m_commands.begin(), m_commands.end(), []( const Command & _queued )
            {
                return (_queued.type == ECommandType::Send || _queued.type == ECommandType::Broadcast) && _queued.delivery == ESocketDelivery::UnreliableSequenced;
            } );
            if( unreliable == m_commands.end() )
            {
                return false;
            }
            m_commands.erase( unreliable );
        }
        m_commands.emplace_back( Mengine::StdUtility::move( _command ) );
        return true;
    }
    //////////////////////////////////////////////////////////////////////////
    bool SocketService::process_( const ThreadIdentityRunnerInterfacePtr & _runner )
    {
        if( _runner->isCancel() == true )
        {
            return false;
        }
        uint64_t now = static_cast<uint64_t>(TIME_SYSTEM()->getPlatformTimestamp());
        this->processCommands_( now );
        if( m_stopping == true )
        {
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

        while( commands.empty() == false )
        {
            Command command = Mengine::StdUtility::move( commands.front() );
            commands.pop_front();
            this->processCommand_( Mengine::StdUtility::move( command ), _now );
            if( m_stopping == true )
            {
                break;
            }
        }
    }
    //////////////////////////////////////////////////////////////////////////
    void SocketService::processCommand_( Command && _command, uint64_t _now )
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
            if( peer != nullptr && peer->state == EPeerState::Connected )
            {
                this->queueMessage_( peer, _command.channel, _command.delivery, _command.payload, _now );
            }
            break;
        }
        case ECommandType::Broadcast:
            for( Peer & peer : m_peers )
            {
                if( peer.state == EPeerState::Connected )
                {
                    this->queueMessage_( &peer, _command.channel, _command.delivery, _command.payload, _now );
                }
            }
            break;
        case ECommandType::Stop:
            m_stopping = true;
            this->closeEndpoint_();
            break;
        }
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
        if( Detail::isLoopbackAddress( _address ) == true )
        {
            peer.bandwidthBytesPerMs = Detail::SocketLoopbackBandwidthBytesPerMs;
            peer.bandwidthCapacity = Detail::SocketLoopbackBandwidthCapacity;
            peer.bandwidthTokens = peer.bandwidthCapacity;
        }
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
            STATISTIC_DEC_INTEGER( STATISTIC_SOCKET_PEER_COUNT );
            STATISTIC_DEL_INTEGER( STATISTIC_SOCKET_PENDING_RELIABLE_PACKETS, peer.pending.size() );
            STATISTIC_DEL_INTEGER( STATISTIC_SOCKET_PEER_RTT_MS, peer.rttMs );
        }

        m_peers.clear();
    }
    //////////////////////////////////////////////////////////////////////////
    void SocketService::removePeer_( size_t _index, uint32_t _reason, bool _notify )
    {
        const Peer & peer = m_peers[_index];
        SocketPeerId peerId = peer.id;
        if( _notify == true )
        {
            this->pushEvent_( SocketServiceEvent{ESocketServiceEventType::PeerDisconnected, peerId, 0, _reason, {}} );
        }

        STATISTIC_DEC_INTEGER( STATISTIC_SOCKET_PEER_COUNT );
        STATISTIC_DEL_INTEGER( STATISTIC_SOCKET_PENDING_RELIABLE_PACKETS, peer.pending.size() );
        STATISTIC_DEL_INTEGER( STATISTIC_SOCKET_PEER_RTT_MS, peer.rttMs );

        m_peers.erase( m_peers.begin() + static_cast<ptrdiff_t>(_index) );
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
    void SocketService::queueMessage_( Peer * _peer, uint8_t _channel, ESocketDelivery _delivery, const Data & _payload, uint64_t _now )
    {
        if( _channel >= 4 || _payload.size() > Detail::SocketMessageSize )
        {
            return;
        }
        size_t fragmentCountValue = Mengine::StdAlgorithm::max<size_t>( 1, (_payload.size() + Detail::SocketPayloadSize - 1) / Detail::SocketPayloadSize );
        if( fragmentCountValue > 64 )
        {
            return;
        }
        if( _delivery == ESocketDelivery::ReliableOrdered && _peer->pending.size() + fragmentCountValue > Detail::SocketPendingLimit )
        {
            return;
        }
        const bool reserveUnreliableMessage = _delivery == ESocketDelivery::UnreliableSequenced;
        if( reserveUnreliableMessage == true )
        {
            const size_t messageBytes = _payload.size() + fragmentCountValue * Detail::SocketHeaderSize;
            if( this->consumeBandwidth_( _peer, messageBytes, false, _now ) == false )
            {
                return;
            }
        }
        uint32_t messageId = _peer->nextMessageId++;
        uint32_t channelSequence = _peer->sendChannelSequence[_channel]++;

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
                this->sendPending_( _peer, &pending, Detail::PacketData, _now, false, false );
                _peer->pending.emplace_back( Mengine::StdUtility::move( pending ) );
                STATISTIC_INC_INTEGER( STATISTIC_SOCKET_PENDING_RELIABLE_PACKETS );
            }
            else
            {
                this->sendPending_( _peer, &pending, Detail::PacketData, _now, false, true );
            }
        }
    }
    //////////////////////////////////////////////////////////////////////////
    bool SocketService::sendPending_( Peer * _peer, PendingPacket * _pending, uint16_t _flags, uint64_t _now, bool _retransmit, bool _bandwidthReserved )
    {
        uint32_t packetSequence = 0;
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
    bool SocketService::sendPacket_( Peer * _peer, uint16_t _flags, uint32_t _messageId, uint32_t _channelSequence, uint32_t _totalSize, uint16_t _fragmentIndex, uint16_t _fragmentCount, uint8_t _channel, ESocketDelivery _delivery, const uint8_t * _payload, size_t _payloadSize, uint32_t * const _packetSequence, uint64_t _now, bool _bandwidthReserved )
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
        uint32_t sequence = _peer->nextPacketSequence++;
        if( sequence == 0 )
        {
            sequence = _peer->nextPacketSequence++;
        }
        Detail::writeU32( packet.data(), 0, Detail::SocketMagic );
        Detail::writeU16( packet.data(), 4, Detail::SocketVersion );
        Detail::writeU16( packet.data(), 6, _flags );
        Detail::writeU32( packet.data(), 8, _peer->connectionId );
        Detail::writeU32( packet.data(), 12, sequence );
        Detail::writeU32( packet.data(), 16, _peer->receiveInitialized == true ? _peer->receiveSequence : 0 );
        Detail::writeU32( packet.data(), 20, _peer->receiveMask );
        Detail::writeU32( packet.data(), 24, _messageId );
        Detail::writeU32( packet.data(), 28, _channelSequence );
        Detail::writeU32( packet.data(), 32, _totalSize );
        Detail::writeU16( packet.data(), 36, static_cast<uint16_t>(_payloadSize) );
        Detail::writeU16( packet.data(), 38, _fragmentIndex );
        Detail::writeU16( packet.data(), 40, _fragmentCount );
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
        _peer->ackPending = false;
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
        if( _size < Detail::SocketHeaderSize || Detail::readU32( _packet, 0 ) != Detail::SocketMagic || Detail::readU16( _packet, 4 ) != Detail::SocketVersion )
        {
            return;
        }
        uint16_t flags = Detail::readU16( _packet, 6 );
        uint32_t connectionId = Detail::readU32( _packet, 8 );
        uint32_t sequence = Detail::readU32( _packet, 12 );
        uint32_t ack = Detail::readU32( _packet, 16 );
        uint32_t ackBits = Detail::readU32( _packet, 20 );
        uint32_t messageId = Detail::readU32( _packet, 24 );
        uint32_t channelSequence = Detail::readU32( _packet, 28 );
        uint32_t totalSize = Detail::readU32( _packet, 32 );
        uint16_t payloadSize = Detail::readU16( _packet, 36 );
        uint16_t fragmentIndex = Detail::readU16( _packet, 38 );
        uint16_t fragmentCount = Detail::readU16( _packet, 40 );
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
            SocketPeerId peerId = m_nextPeerId.fetch_add( 1 );
            if( peerId == InvalidSocketPeerId )
            {
                peerId = m_nextPeerId.fetch_add( 1 );
            }
            peer = this->createPeer_( peerId, _address, connectionId, EPeerState::Connected, _now );
            if( peer == nullptr )
            {
                return;
            }
            this->pushEvent_( SocketServiceEvent{ESocketServiceEventType::PeerConnected, peer->id, 0, 0, {}} );
        }

        peer->lastReceiveMs = _now;
        STATISTIC_ADD_INTEGER( STATISTIC_SOCKET_BYTES_RECEIVED, _size );
        STATISTIC_INC_INTEGER( STATISTIC_SOCKET_PACKETS_RECEIVED );
        this->acknowledge_( peer, ack, ackBits, _now );
        bool fresh = this->markReceived_( peer, sequence );
        if( fresh == true && ( flags & Detail::PacketData ) != 0 )
        {
            peer->ackPending = true;
        }

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
                peer->state = EPeerState::Connected;
                this->pushEvent_( SocketServiceEvent{ESocketServiceEventType::PeerConnected, peer->id, 0, 0, {}} );
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

        if( (flags & Detail::PacketData) != 0 && peer->state == EPeerState::Connected )
        {
            this->receiveFragment_( peer, messageId, channelSequence, totalSize, fragmentIndex, fragmentCount, channel, static_cast<ESocketDelivery>(deliveryValue), _packet + Detail::SocketHeaderSize, payloadSize, _now );
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
    void SocketService::receiveFragment_( Peer * _peer, uint32_t _messageId, uint32_t _channelSequence, uint32_t _totalSize, uint16_t _fragmentIndex, uint16_t _fragmentCount, uint8_t _channel, ESocketDelivery _delivery, const uint8_t * _payload, size_t _payloadSize, uint64_t _now )
    {
        if( _totalSize > Detail::SocketMessageSize || _fragmentCount == 0 || _fragmentCount > 64 || _fragmentIndex >= _fragmentCount )
        {
            STATISTIC_INC_INTEGER( STATISTIC_SOCKET_PACKETS_REJECTED );
            return;
        }
        Reassembly * reassembly = nullptr;
        for( Reassembly & item : _peer->reassemblies )
        {
            if( item.messageId == _messageId && item.channel == _channel )
            {
                reassembly = &item;
                break;
            }
        }
        if( reassembly == nullptr )
        {
            if( _peer->reassemblies.size() >= Detail::SocketReassemblyLimit )
            {
                STATISTIC_INC_INTEGER( STATISTIC_SOCKET_PACKETS_REJECTED );
                return;
            }
            size_t reassemblyBytes = 0;
            for( const Reassembly & item : _peer->reassemblies )
            {
                reassemblyBytes += item.totalSize;
            }
            if( reassemblyBytes + _totalSize > Detail::SocketReassemblyByteLimit )
            {
                STATISTIC_INC_INTEGER( STATISTIC_SOCKET_PACKETS_REJECTED );
                return;
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
            return;
        }
        if( reassembly->present[_fragmentIndex] != 0 )
        {
            return;
        }
        reassembly->fragments[_fragmentIndex].resize( _payloadSize );
        if( _payloadSize != 0 )
        {
            Mengine::StdString::memcpy( reassembly->fragments[_fragmentIndex].data(), _payload, _payloadSize );
        }
        reassembly->present[_fragmentIndex] = 1;

        for( uint8_t present : reassembly->present )
        {
            if( present == 0 )
            {
                return;
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
            return;
        }
        uint32_t sequence = reassembly->channelSequence;
        uint8_t channel = reassembly->channel;
        ESocketDelivery delivery = reassembly->delivery;
        for( size_t index = 0; index != _peer->reassemblies.size(); ++index )
        {
            if( &_peer->reassemblies[index] == reassembly )
            {
                _peer->reassemblies.erase( _peer->reassemblies.begin() + static_cast<ptrdiff_t>(index) );
                break;
            }
        }
        this->deliverMessage_( _peer, channel, delivery, sequence, Mengine::StdUtility::move( payload ) );
    }
    //////////////////////////////////////////////////////////////////////////
    void SocketService::deliverMessage_( Peer * _peer, uint8_t _channel, ESocketDelivery _delivery, uint32_t _channelSequence, Data && _payload )
    {
        if( _delivery == ESocketDelivery::UnreliableSequenced )
        {
            if( SocketService::sequenceNewer_( _channelSequence, _peer->lastUnreliableSequence[_channel] ) == false )
            {
                return;
            }
            _peer->lastUnreliableSequence[_channel] = _channelSequence;
            this->pushEvent_( SocketServiceEvent{ESocketServiceEventType::MessageReceived, _peer->id, _channel, 0, Mengine::StdUtility::move( _payload ), _delivery} );
            return;
        }

        uint32_t expected = _peer->receiveChannelSequence[_channel];
        if( SocketService::sequenceNewer_( expected, _channelSequence ) == true )
        {
            return;
        }
        if( _channelSequence != expected )
        {
            if( _peer->ordered[_channel].size() < 64 )
            {
                _peer->ordered[_channel].emplace( _channelSequence, Mengine::StdUtility::move( _payload ) );
            }
            return;
        }

        this->pushEvent_( SocketServiceEvent{ESocketServiceEventType::MessageReceived, _peer->id, _channel, 0, Mengine::StdUtility::move( _payload ), _delivery} );
        ++_peer->receiveChannelSequence[_channel];
        for( ;; )
        {
            auto it = _peer->ordered[_channel].find( _peer->receiveChannelSequence[_channel] );
            if( it == _peer->ordered[_channel].end() )
            {
                break;
            }
            Data next = Mengine::StdUtility::move( it->second );
            _peer->ordered[_channel].erase( it );
            this->pushEvent_( SocketServiceEvent{ESocketServiceEventType::MessageReceived, _peer->id, _channel, 0, Mengine::StdUtility::move( next ), _delivery} );
            ++_peer->receiveChannelSequence[_channel];
        }
    }
    //////////////////////////////////////////////////////////////////////////
    void SocketService::updatePeers_( uint64_t _now )
    {
        for( size_t peerIndex = 0; peerIndex < m_peers.size(); )
        {
            Peer & peer = m_peers[peerIndex];
            if( _now - peer.lastReceiveMs >= Detail::SocketTimeoutMs )
            {
                this->removePeer_( peerIndex, 1, true );
                continue;
            }
            if( peer.state == EPeerState::Connecting && _now - peer.lastConnectSendMs >= Detail::SocketConnectRetryMs )
            {
                this->sendConnect_( &peer, _now );
            }

            for( PendingPacket & pending : peer.pending )
            {
                uint32_t baseRto = peer.rttMs == 0 ? 100 : Mengine::StdAlgorithm::max<uint32_t>( 100, Mengine::StdAlgorithm::min<uint32_t>( 1000, peer.rttMs * 2 ) );
                uint32_t shift = Mengine::StdAlgorithm::min<uint32_t>( pending.retryCount, 3 );
                uint32_t rto = Mengine::StdAlgorithm::min<uint32_t>( 1000, baseRto << shift );
                if( pending.packetSequence == 0 || _now - pending.lastSendMs >= rto )
                {
                    this->sendPending_( &peer, &pending, Detail::PacketData, _now, pending.packetSequence != 0, false );
                }
            }

            for( size_t index = 0; index < peer.reassemblies.size(); )
            {
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
    void SocketService::pushEvent_( SocketServiceEvent && _event )
    {
        MENGINE_THREAD_MUTEX_SCOPE( m_eventMutex );
        if( m_events.size() >= Detail::SocketEventLimit )
        {
            auto unreliable = Mengine::StdAlgorithm::find_if( m_events.begin(), m_events.end(), []( const SocketServiceEvent & _queued )
            {
                return _queued.type == ESocketServiceEventType::MessageReceived && _queued.delivery == ESocketDelivery::UnreliableSequenced;
            } );
            if( unreliable == m_events.end() )
            {
                return;
            }
            m_events.erase( unreliable );
        }
        m_events.emplace_back( Mengine::StdUtility::move( _event ) );
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
