#include "Services/SocketService/SocketService.h"

#include "Kernel/WriteHelper.h"

#include "Config/StdIO.h"
#include "Config/StdString.h"
#include "Config/StdUtility.h"
//////////////////////////////////////////////////////////////////////////
namespace Arena3D
{
    //////////////////////////////////////////////////////////////////////////
    namespace Detail
    {
        //////////////////////////////////////////////////////////////////////////
        class TestReceiveSocketService
            : public Mengine::SocketService
        {
        public:
            explicit TestReceiveSocketService( bool _connected = true )
            {
                this->setUniqueIdentity( 1 );
                Peer peer;
                peer.id = 1;
                peer.connectionId = 1;
                peer.state = _connected == true ? EPeerState::Connected : EPeerState::Connecting;
                peer.address.family = Mengine::ESocketAddressFamily::IPv4;
                peer.address.address[0] = 127;
                peer.address.address[3] = 1;
                peer.address.port = 5000;
                m_peers.emplace_back( Mengine::StdUtility::move( peer ) );
            }

            ~TestReceiveSocketService() override
            {
            }

        public:
            using SocketService::pollEvent;

            void receiveFragment( uint32_t _packetSequence, uint32_t _channelSequence, uint8_t _channel, uint16_t _fragmentIndex, uint16_t _fragmentCount, uint32_t _totalSize, const Mengine::Data & _payload, Mengine::ESocketDelivery _delivery, uint64_t _now = 0 )
            {
                Mengine::Data packet( 44 + _payload.size(), 0 );
                Mengine::Helper::writeUint32( packet.data(), 0x53474E4D );
                Mengine::Helper::writeUint16( packet.data() + 4, 2 );
                Mengine::Helper::writeUint16( packet.data() + 6, 16 );
                Mengine::Helper::writeUint32( packet.data() + 8, 1 );
                Mengine::Helper::writeUint32( packet.data() + 12, _packetSequence );
                Mengine::Helper::writeUint32( packet.data() + 24, _channelSequence );
                Mengine::Helper::writeUint32( packet.data() + 28, _channelSequence );
                Mengine::Helper::writeUint32( packet.data() + 32, _totalSize );
                uint16_t payloadSize = static_cast<uint16_t>(_payload.size());
                Mengine::Helper::writeUint16( packet.data() + 36, payloadSize );
                Mengine::Helper::writeUint16( packet.data() + 38, _fragmentIndex );
                Mengine::Helper::writeUint16( packet.data() + 40, _fragmentCount );
                packet[42] = _channel;
                packet[43] = static_cast<uint8_t>(_delivery);
                if( _payload.empty() == false )
                {
                    Mengine::StdString::memcpy( packet.data() + 44, _payload.data(), _payload.size() );
                }
                this->processPacket_( m_peers[0].address, packet.data(), packet.size(), _now );
            }

            void receiveValue( uint32_t _packetSequence, uint32_t _channelSequence, uint32_t _value )
            {
                Mengine::Data payload( sizeof( _value ) );
                Mengine::StdString::memcpy( payload.data(), &_value, sizeof( _value ) );
                this->receiveFragment( _packetSequence, _channelSequence, 1, 0, 1, sizeof( _value ), payload, Mengine::ESocketDelivery::ReliableOrdered );
            }

            void prepareServer( uint32_t _maximumPeers = 1 )
            {
                m_peers.clear();
                m_server = true;
                m_maximumPeers = _maximumPeers;
            }

            void setNextPeerId( Mengine::SocketPeerId _peerId )
            {
                m_nextPeerId.store( _peerId );
            }

            void receiveControl( uint32_t _packetSequence, uint16_t _flags, uint32_t _reason = 0, uint16_t _version = 2, uint32_t _connectionId = 1 )
            {
                Mengine::Data packet( 44, 0 );
                Mengine::Helper::writeUint32( packet.data(), 0x53474E4D );
                Mengine::Helper::writeUint16( packet.data() + 4, _version );
                Mengine::Helper::writeUint16( packet.data() + 6, _flags );
                Mengine::Helper::writeUint32( packet.data() + 8, _connectionId );
                Mengine::Helper::writeUint32( packet.data() + 12, _packetSequence );
                Mengine::Helper::writeUint32( packet.data() + 24, _reason );
                Mengine::SocketAddress address;
                address.family = Mengine::ESocketAddressFamily::IPv4;
                address.address[0] = 127;
                address.address[3] = 1;
                address.port = 5000;
                this->processPacket_( address, packet.data(), packet.size(), 0 );
            }

            size_t getPeerCount() const
            {
                size_t count = m_peers.size();

                return count;
            }

            bool isConnected() const
            {
                if( m_peers.empty() == true )
                {
                    return false;
                }
                if( m_peers[0].state != EPeerState::Connected )
                {
                    return false;
                }

                return true;
            }

            bool wasPacketReceived( uint32_t _sequence ) const
            {
                const Peer & peer = m_peers[0];
                if( peer.receiveInitialized == false )
                {
                    return false;
                }
                if( peer.receiveSequence == _sequence )
                {
                    return true;
                }
                uint32_t distance = peer.receiveSequence - _sequence;
                if( distance == 0 || distance > 32 )
                {
                    return false;
                }
                if( (peer.receiveMask & (UINT32_C(1) << (distance - 1))) == 0 )
                {
                    return false;
                }

                return true;
            }

            uint32_t getExpectedSequence( uint8_t _channel ) const
            {
                return m_peers[0].receiveChannelSequence[_channel];
            }

            size_t getReassemblyCount() const
            {
                size_t count = m_peers[0].reassemblies.size();

                return count;
            }

            size_t getReassemblyBytes() const
            {
                size_t bytes = 0;
                for( const Reassembly & item : m_peers[0].reassemblies )
                {
                    bytes += item.totalSize;
                }

                return bytes;
            }

            void fillEvents()
            {
                for( size_t index = 0; index != 1024; ++index )
                {
                    this->pushEvent_( {Mengine::ESocketServiceEventType::MessageReceived, 1, 1, 0, {}, Mengine::ESocketDelivery::ReliableOrdered} );
                }
            }

            void updatePeers( uint64_t _now )
            {
                this->updatePeers_( _now );
            }
        };
        //////////////////////////////////////////////////////////////////////////
    }
    //////////////////////////////////////////////////////////////////////////
}
//////////////////////////////////////////////////////////////////////////
#define RECEIVE_CHECK( Expression ) \
    do \
    { \
        if( (Expression) == false ) \
        { \
            Mengine::StdIO::fprintf( stderr, "%s:%d: %s\n", __FILE__, __LINE__, #Expression ); \
            return false; \
        } \
    } while( false )
//////////////////////////////////////////////////////////////////////////
namespace Arena3D
{
    //////////////////////////////////////////////////////////////////////////
    namespace Detail
    {
        //////////////////////////////////////////////////////////////////////////
        static bool testFutureReceiveRetry()
        {
            TestReceiveSocketService service;
            for( uint32_t sequence = 2; sequence <= 100; ++sequence )
            {
                service.receiveValue( sequence, sequence, sequence );
                RECEIVE_CHECK( service.wasPacketReceived( sequence ) == false );
            }
            RECEIVE_CHECK( service.getReassemblyCount() == 0 );
            service.receiveValue( 101, 1, 1 );
            RECEIVE_CHECK( service.wasPacketReceived( 101 ) );
            for( uint32_t sequence = 2; sequence <= 100; ++sequence )
            {
                uint32_t packetSequence = sequence + 100;
                service.receiveValue( packetSequence, sequence, sequence );
                RECEIVE_CHECK( service.wasPacketReceived( packetSequence ) );
            }
            for( uint32_t sequence = 1; sequence <= 100; ++sequence )
            {
                Mengine::SocketServiceEvent event;
                RECEIVE_CHECK( service.pollEvent( &event ) );
                RECEIVE_CHECK( event.payload.size() == sizeof( sequence ) );
                uint32_t value = 0;
                Mengine::StdString::memcpy( &value, event.payload.data(), sizeof( value ) );
                RECEIVE_CHECK( value == sequence );
            }
            service.receiveValue( 201, 1, 1 );
            RECEIVE_CHECK( service.wasPacketReceived( 201 ) );
            Mengine::SocketServiceEvent duplicate;
            RECEIVE_CHECK( service.pollEvent( &duplicate ) == false );

            return true;
        }
        //////////////////////////////////////////////////////////////////////////
        static bool testEventBackpressure()
        {
            TestReceiveSocketService service;
            service.fillEvents();
            service.receiveValue( 1, 1, 42 );
            RECEIVE_CHECK( service.wasPacketReceived( 1 ) == false );
            RECEIVE_CHECK( service.getExpectedSequence( 1 ) == 1 );
            RECEIVE_CHECK( service.getReassemblyCount() == 1 );
            Mengine::SocketServiceEvent event;
            RECEIVE_CHECK( service.pollEvent( &event ) );
            service.receiveValue( 2, 1, 42 );
            RECEIVE_CHECK( service.wasPacketReceived( 2 ) );
            RECEIVE_CHECK( service.getExpectedSequence( 1 ) == 2 );
            RECEIVE_CHECK( service.getReassemblyCount() == 0 );
            for( size_t index = 0; index != 1023; ++index )
            {
                RECEIVE_CHECK( service.pollEvent( &event ) );
                RECEIVE_CHECK( event.payload.empty() );
            }
            RECEIVE_CHECK( service.pollEvent( &event ) );
            RECEIVE_CHECK( event.payload.size() == sizeof(uint32_t) );
            uint32_t value = 0;
            Mengine::StdString::memcpy( &value, event.payload.data(), sizeof( value ) );
            RECEIVE_CHECK( value == 42 );
            RECEIVE_CHECK( service.pollEvent( &event ) == false );

            return true;
        }
        //////////////////////////////////////////////////////////////////////////
        static bool testReliableFragmentLifetime()
        {
            TestReceiveSocketService service;
            Mengine::Data first( 1156, 7 );
            service.receiveFragment( 1, 1, 1, 0, 2, 1157, first, Mengine::ESocketDelivery::ReliableOrdered );
            RECEIVE_CHECK( service.wasPacketReceived( 1 ) );
            service.updatePeers( 2500 );
            RECEIVE_CHECK( service.getReassemblyCount() == 1 );
            service.receiveFragment( 1, 1, 1, 0, 2, 1157, first, Mengine::ESocketDelivery::ReliableOrdered, 2501 );
            RECEIVE_CHECK( service.getReassemblyCount() == 1 );
            Mengine::Data last( 1, 9 );
            service.receiveFragment( 2, 1, 1, 1, 2, 1157, last, Mengine::ESocketDelivery::ReliableOrdered, 2502 );
            RECEIVE_CHECK( service.wasPacketReceived( 2 ) );
            Mengine::SocketServiceEvent event;
            RECEIVE_CHECK( service.pollEvent( &event ) );
            RECEIVE_CHECK( event.payload.size() == 1157 );
            RECEIVE_CHECK( event.payload.front() == 7 );
            RECEIVE_CHECK( event.payload.back() == 9 );
            service.receiveFragment( 2, 1, 1, 1, 2, 1157, last, Mengine::ESocketDelivery::ReliableOrdered, 2503 );
            RECEIVE_CHECK( service.getReassemblyCount() == 0 );
            RECEIVE_CHECK( service.pollEvent( &event ) == false );

            return true;
        }
        //////////////////////////////////////////////////////////////////////////
        static bool testUnreliableReassemblyPressure()
        {
            TestReceiveSocketService service;
            Mengine::Data fragment( 1156, 5 );
            for( uint32_t index = 1; index <= 64; ++index )
            {
                service.receiveFragment( index, index, 2, 0, 4, 4096, fragment, Mengine::ESocketDelivery::UnreliableSequenced );
            }
            RECEIVE_CHECK( service.getReassemblyCount() == 64 );
            RECEIVE_CHECK( service.getReassemblyBytes() == 256 * 1024 );
            uint32_t packetSequence = 65;
            for( uint16_t index = 0; index != 57; ++index )
            {
                size_t remaining = 65536 - static_cast<size_t>(index) * 1156;
                size_t payloadSize = remaining < 1156 ? remaining : 1156;
                Mengine::Data payload( payloadSize, 9 );
                service.receiveFragment( packetSequence, 1, 1, index, 57, 65536, payload, Mengine::ESocketDelivery::ReliableOrdered );
                RECEIVE_CHECK( service.wasPacketReceived( packetSequence ) );
                RECEIVE_CHECK( service.getReassemblyCount() <= 64 );
                RECEIVE_CHECK( service.getReassemblyBytes() <= 256 * 1024 );
                ++packetSequence;
            }
            Mengine::SocketServiceEvent event;
            RECEIVE_CHECK( service.pollEvent( &event ) );
            RECEIVE_CHECK( event.payload.size() == 65536 );
            RECEIVE_CHECK( event.payload.front() == 9 );
            RECEIVE_CHECK( event.payload.back() == 9 );

            return true;
        }
        //////////////////////////////////////////////////////////////////////////
        static bool testFourReliableAssemblies()
        {
            TestReceiveSocketService service;
            Mengine::Data fragment( 1156, 5 );
            for( uint8_t channel = 0; channel != 4; ++channel )
            {
                uint32_t packetSequence = static_cast<uint32_t>(channel) + 1;
                service.receiveFragment( packetSequence, 1, channel, 0, 57, 65536, fragment, Mengine::ESocketDelivery::ReliableOrdered );
                RECEIVE_CHECK( service.wasPacketReceived( packetSequence ) );
            }
            RECEIVE_CHECK( service.getReassemblyCount() == 4 );
            RECEIVE_CHECK( service.getReassemblyBytes() == 256 * 1024 );
            service.receiveFragment( 5, 2, 1, 0, 57, 65536, fragment, Mengine::ESocketDelivery::ReliableOrdered );
            RECEIVE_CHECK( service.wasPacketReceived( 5 ) == false );
            service.receiveFragment( 6, 2, 1, 0, 57, 65536, fragment, Mengine::ESocketDelivery::UnreliableSequenced );
            RECEIVE_CHECK( service.getReassemblyCount() == 4 );
            RECEIVE_CHECK( service.getReassemblyBytes() == 256 * 1024 );
            service.updatePeers( 2500 );
            RECEIVE_CHECK( service.getReassemblyCount() == 4 );

            return true;
        }
        //////////////////////////////////////////////////////////////////////////
        static bool testConnectBackpressure()
        {
            {
                TestReceiveSocketService server;
                server.prepareServer();
                server.receiveControl( 1, 1, 0, 1 );
                RECEIVE_CHECK( server.getPeerCount() == 0 );
                server.fillEvents();
                server.receiveControl( 1, 1 );
                RECEIVE_CHECK( server.getPeerCount() == 0 );
                Mengine::SocketServiceEvent event;
                RECEIVE_CHECK( server.pollEvent( &event ) );
                server.receiveControl( 2, 1 );
                RECEIVE_CHECK( server.getPeerCount() == 1 );
                for( size_t index = 0; index != 1023; ++index )
                {
                    RECEIVE_CHECK( server.pollEvent( &event ) );
                }
                RECEIVE_CHECK( server.pollEvent( &event ) );
                RECEIVE_CHECK( event.type == Mengine::ESocketServiceEventType::PeerConnected );
                RECEIVE_CHECK( server.pollEvent( &event ) == false );
            }
            {
                TestReceiveSocketService client( false );
                client.fillEvents();
                client.receiveControl( 1, 2 );
                RECEIVE_CHECK( client.isConnected() == false );
                Mengine::SocketServiceEvent event;
                RECEIVE_CHECK( client.pollEvent( &event ) );
                client.receiveControl( 2, 2 );
                RECEIVE_CHECK( client.isConnected() );
                for( size_t index = 0; index != 1023; ++index )
                {
                    RECEIVE_CHECK( client.pollEvent( &event ) );
                }
                RECEIVE_CHECK( client.pollEvent( &event ) );
                RECEIVE_CHECK( event.type == Mengine::ESocketServiceEventType::PeerConnected );
                RECEIVE_CHECK( client.pollEvent( &event ) == false );
            }

            return true;
        }
        //////////////////////////////////////////////////////////////////////////
        static bool testServerPeerIdExhaustion()
        {
            TestReceiveSocketService service;
            service.prepareServer( 4 );
            service.receiveControl( 1, 1 );
            Mengine::SocketServiceEvent event;
            RECEIVE_CHECK( service.pollEvent( &event ) );
            RECEIVE_CHECK( event.type == Mengine::ESocketServiceEventType::PeerConnected && event.peerId == 1 );
            service.setNextPeerId( UINT32_MAX );
            service.receiveControl( 1, 1, 0, 2, 2 );
            RECEIVE_CHECK( service.pollEvent( &event ) );
            RECEIVE_CHECK( event.type == Mengine::ESocketServiceEventType::PeerConnected && event.peerId == UINT32_MAX );
            for( uint32_t connection = 3; connection <= 6; ++connection )
            {
                service.receiveControl( 1, 1, 0, 2, connection );
                RECEIVE_CHECK( service.getPeerCount() == 2 );
                RECEIVE_CHECK( service.pollEvent( &event ) == false );
            }
            service.receiveValue( 2, 1, 73 );
            RECEIVE_CHECK( service.pollEvent( &event ) );
            RECEIVE_CHECK( event.type == Mengine::ESocketServiceEventType::MessageReceived && event.peerId == 1 );
            uint32_t value;
            Mengine::StdString::memcpy( &value, event.payload.data(), sizeof( value ) );
            RECEIVE_CHECK( value == 73 );
            RECEIVE_CHECK( service.pollEvent( &event ) == false );

            return true;
        }
        //////////////////////////////////////////////////////////////////////////
        static bool testDisconnectBackpressure()
        {
            TestReceiveSocketService service;
            service.fillEvents();
            service.receiveControl( 1, 4, 73 );
            RECEIVE_CHECK( service.isConnected() == false );
            RECEIVE_CHECK( service.getPeerCount() == 1 );
            service.updatePeers( 1 );
            RECEIVE_CHECK( service.getPeerCount() == 1 );
            Mengine::SocketServiceEvent event;
            RECEIVE_CHECK( service.pollEvent( &event ) );
            service.updatePeers( 2 );
            RECEIVE_CHECK( service.getPeerCount() == 0 );
            for( size_t index = 0; index != 1023; ++index )
            {
                RECEIVE_CHECK( service.pollEvent( &event ) );
            }
            RECEIVE_CHECK( service.pollEvent( &event ) );
            RECEIVE_CHECK( event.type == Mengine::ESocketServiceEventType::PeerDisconnected );
            RECEIVE_CHECK( event.reason == 73 );
            RECEIVE_CHECK( service.pollEvent( &event ) == false );

            return true;
        }
        //////////////////////////////////////////////////////////////////////////
    }
    //////////////////////////////////////////////////////////////////////////
    bool runSocketReceiveTests()
    {
        if( Detail::testFutureReceiveRetry() == false )
        {
            return false;
        }
        if( Detail::testEventBackpressure() == false )
        {
            return false;
        }
        if( Detail::testReliableFragmentLifetime() == false )
        {
            return false;
        }
        if( Detail::testUnreliableReassemblyPressure() == false )
        {
            return false;
        }
        if( Detail::testFourReliableAssemblies() == false )
        {
            return false;
        }
        if( Detail::testConnectBackpressure() == false )
        {
            return false;
        }
        if( Detail::testServerPeerIdExhaustion() == false )
        {
            return false;
        }
        if( Detail::testDisconnectBackpressure() == false )
        {
            return false;
        }

        return true;
    }
    //////////////////////////////////////////////////////////////////////////
}
