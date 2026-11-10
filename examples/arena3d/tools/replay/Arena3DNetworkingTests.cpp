#include "Arena3DNetworkClient.h"
#include "Arena3DNetworkServer.h"

#include "Config/StdIO.h"
#include "Config/StdUtility.h"

#define NETWORK_CHECK( Expression ) \
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
        static void testFailed( int32_t _line )
        {
            Mengine::StdIO::fprintf( stderr, "Integration check failed at %s:%d\n", __FILE__, _line );
        }

        //////////////////////////////////////////////////////////////////////////
        class TestSocket;
        //////////////////////////////////////////////////////////////////////////
        struct TestPacketDesc
        {
            TestSocket * destination;
            Mengine::SocketServiceEvent event;
            uint64_t due;
            uint64_t ordinal;
        };
        //////////////////////////////////////////////////////////////////////////
        struct TestBus
        {
            Mengine::Vector<TestPacketDesc> packets;
            uint64_t now = 0;
            uint64_t ordinal = 0;
            bool loss = false;
            const NetworkServer * server = nullptr;
            Mengine::SocketPeerId pausedFramePeer = Mengine::InvalidSocketPeerId;
            uint32_t pausedFrameDrops = 0;
            uint32_t confirmationLimit = UINT32_MAX;
            uint32_t confirmationsThisStep = 0;
            uint32_t commandSendFailures = 0;
            Mengine::Array<uint32_t, 9> messageSendFailures{};
            Mengine::Array<uint32_t, 9> acceptedMessages{};
            Mengine::Array<uint32_t, 4> acceptedRosters{};
            Mengine::SocketPeerId failingMessagePeer = Mengine::InvalidSocketPeerId;
            uint32_t disconnectFailures = 0;
            uint32_t acceptedDisconnects = 0;
            Mengine::Vector<uint64_t> confirmations;

            void advance();
        };
        //////////////////////////////////////////////////////////////////////////
        class TestSocket
            : public Mengine::SocketServiceInterface
        {
        public:
            TestSocket( TestBus * _bus, uint32_t _id, TestSocket * _server = nullptr )
                : m_bus( _bus )
                , m_id( _id )
                , m_server( _server )
            {
                uint32_t identity = 100 + m_id;
                this->setUniqueIdentity( identity );
            }

            struct ConnectionDesc
            {
                uint32_t peer;
                uint32_t remotePeer;
                TestSocket * socket;
                uint64_t received[4]{};
            };

            bool listen( const Mengine::SocketAddress &, uint32_t ) override
            {
                return true;
            }

            bool connect( const Mengine::SocketAddress &, Mengine::SocketPeerId * const _peer ) override
            {
                if( m_server == nullptr )
                {
                    return false;
                }
                uint32_t peerId = m_nextPeerId++;
                uint32_t serverPeerId = m_server->m_nextPeerId++;
                *_peer = peerId;
                m_connections.push_back( {peerId, serverPeerId, m_server, {}} );
                m_server->m_connections.push_back( {serverPeerId, peerId, this, {}} );
                Mengine::SocketServiceEvent event;
                event.type = Mengine::ESocketServiceEventType::PeerConnected;
                event.peerId = peerId;
                m_events.emplace_back( event );
                event.peerId = serverPeerId;
                m_server->m_events.emplace_back( event );
                return true;
            }

            bool disconnect( Mengine::SocketPeerId _peer, uint32_t _reason ) override
            {
                if( m_bus->disconnectFailures != 0 )
                {
                    --m_bus->disconnectFailures;
                    return false;
                }
                for( ConnectionDesc & connection : m_connections )
                {
                    if( connection.peer != _peer )
                    {
                        continue;
                    }
                    Mengine::SocketServiceEvent event;
                    event.type = Mengine::ESocketServiceEventType::PeerDisconnected;
                    event.peerId = connection.remotePeer;
                    event.reason = _reason;
                    connection.socket->m_events.emplace_back( event );
                    event.peerId = _peer;
                    m_events.emplace_back( event );
                    ++m_bus->acceptedDisconnects;
                    return true;
                }
                return false;
            }

            bool send( Mengine::SocketPeerId _peer, uint8_t _channel, Mengine::ESocketDelivery _delivery, const void * _data, size_t _size ) override
            {
                for( ConnectionDesc & connection : m_connections )
                {
                    if( connection.peer != _peer )
                    {
                        continue;
                    }
                    const uint8_t * bytes = static_cast<const uint8_t *>(_data);
                    uint8_t message = _size >= 9 ? bytes[8] : 0;
                    if( message < m_bus->messageSendFailures.size() )
                    {
                        if( m_bus->failingMessagePeer == Mengine::InvalidSocketPeerId || m_bus->failingMessagePeer == _peer )
                        {
                            if( m_bus->messageSendFailures[message] != 0 )
                            {
                                --m_bus->messageSendFailures[message];
                                return false;
                            }
                        }
                    }
                    if( m_server != nullptr && _channel == 1 && m_bus->commandSendFailures != 0 )
                    {
                        --m_bus->commandSendFailures;
                        return false;
                    }
                    if( m_server == nullptr && _channel == 2 && _peer == m_bus->pausedFramePeer && m_bus->pausedFrameDrops != 0 )
                    {
                        if( m_bus->server->simulation().state().paused == true )
                        {
                            --m_bus->pausedFrameDrops;
                            return true;
                        }
                    }
                    if( m_server == nullptr && _channel == 1 && m_bus->confirmationLimit != UINT32_MAX )
                    {
                        if( m_bus->confirmationsThisStep >= m_bus->confirmationLimit )
                        {
                            return false;
                        }
                        CanonicalReader reader( _data, _size );
                        constexpr size_t headerSize = sizeof( uint32_t ) * 2 + sizeof( uint8_t );
                        uint8_t header[headerSize];
                        if( reader.readBytes( header, headerSize ) == false )
                        {
                            return false;
                        }
                        uint64_t matchId = 0;
                        if( reader.readU64( &matchId ) == false )
                        {
                            return false;
                        }
                        uint8_t stream = 0;
                        if( reader.readU8( &stream ) == false )
                        {
                            return false;
                        }
                        uint64_t sequence = 0;
                        if( reader.readU64( &sequence ) == false )
                        {
                            return false;
                        }
                        m_bus->confirmations.emplace_back( sequence );
                        ++m_bus->confirmationsThisStep;
                    }
                    if( message < m_bus->acceptedMessages.size() )
                    {
                        ++m_bus->acceptedMessages[message];
                    }
                    if( message == static_cast<uint8_t>(NetworkMessage::Roster) && _peer < m_bus->acceptedRosters.size() )
                    {
                        ++m_bus->acceptedRosters[_peer];
                    }
                    uint64_t ordinal = ++m_bus->ordinal;
                    bool unreliable = _delivery == Mengine::ESocketDelivery::UnreliableSequenced;
                    if( m_bus->loss == true && unreliable == true && ordinal % 5 == 0 )
                    {
                        return true;
                    }
                    Mengine::SocketServiceEvent event;
                    event.type = Mengine::ESocketServiceEventType::MessageReceived;
                    event.peerId = connection.remotePeer;
                    event.channel = _channel;
                    event.delivery = _delivery;
                    event.payload.assign( bytes, bytes + _size );
                    uint64_t delay = m_bus->loss == true && unreliable == true ? ordinal % 6 : 2;
                    m_bus->packets.push_back( {connection.socket, Mengine::StdUtility::move( event ), m_bus->now + delay, ordinal} );
                    return true;
                }
                return false;
            }

            bool broadcast( uint8_t _channel, Mengine::ESocketDelivery _delivery, const void * _data, size_t _size ) override
            {
                for( const ConnectionDesc & connection : m_connections )
                {
                    if( this->send( connection.peer, _channel, _delivery, _data, _size ) == false )
                    {
                        return false;
                    }
                }
                return true;
            }

            bool pollEvent( Mengine::SocketServiceEvent * const _event ) override
            {
                if( m_events.empty() == true )
                {
                    return false;
                }
                *_event = Mengine::StdUtility::move( m_events.front() );
                m_events.erase( m_events.begin() );
                return true;
            }

            void enqueueEvent( const Mengine::SocketServiceEvent & _event )
            {
                m_events.emplace_back( _event );
            }

            uint32_t getPeerId() const
            {
                return m_nextPeerId - 1;
            }

            void deliver( TestPacketDesc _packet )
            {
                for( ConnectionDesc & connection : m_connections )
                {
                    if( connection.peer != _packet.event.peerId )
                    {
                        continue;
                    }
                    if( _packet.event.delivery == Mengine::ESocketDelivery::UnreliableSequenced )
                    {
                        uint64_t & received = connection.received[_packet.event.channel];
                        if( received >= _packet.ordinal )
                        {
                            return;
                        }
                        received = _packet.ordinal;
                    }
                    m_events.emplace_back( Mengine::StdUtility::move( _packet.event ) );
                    return;
                }
            }

            void stop() override
            {
                // Empty
            }

            bool isAvailableService() const override
            {
                return true;
            }

            bool isInitializeService() const override
            {
                return true;
            }

            bool isStopService() const override
            {
                return false;
            }

            const Mengine::ServiceRequiredList & requiredServices() const override
            {
                static Mengine::ServiceRequiredList required;
                return required;
            }

            bool registerService( const Mengine::ServiceInterfacePtr & ) override
            {
                return true;
            }

            void unregisterService( const Mengine::ServiceInterfacePtr & ) override
            {
                // Empty
            }

            bool availableService() override
            {
                return true;
            }

            bool initializeService( Mengine::ServiceProviderInterface * ) override
            {
                return true;
            }

            void finalizeService() override
            {
                // Empty
            }

            void replaceService() override
            {
                // Empty
            }

            bool runService() override
            {
                return true;
            }

            void stopService() override
            {
                // Empty
            }

        private:
            TestBus * m_bus;
            uint32_t m_id;
            TestSocket * m_server;
            Mengine::Vector<ConnectionDesc> m_connections;
            Mengine::Vector<Mengine::SocketServiceEvent> m_events;
            uint32_t m_nextPeerId = 1;
        };
        //////////////////////////////////////////////////////////////////////////
        void TestBus::advance()
        {
            ++now;
            confirmationsThisStep = 0;
            for( size_t index = 0; index != packets.size(); )
            {
                if( packets[index].due > now )
                {
                    ++index;
                    continue;
                }
                TestPacketDesc packet = Mengine::StdUtility::move( packets[index] );
                packets.erase( packets.begin() + static_cast<ptrdiff_t>( index ) );
                TestSocket * destination = packet.destination;
                destination->deliver( Mengine::StdUtility::move( packet ) );
            }
        }
        //////////////////////////////////////////////////////////////////////////
        static void requestSnapshot( TestSocket * _socket )
        {
            CanonicalWriter request;
            request.writeU32( NetworkWireMagic );
            request.writeU32( NetworkProtocolVersion );
            request.writeU8( static_cast<uint8_t>( NetworkMessage::SnapshotRequest ) );
            const Mengine::Data & data = request.data();
            const void * bytes = data.data();
            size_t size = data.size();
            uint32_t peerId = _socket->getPeerId();
            _socket->send( peerId, 3, Mengine::ESocketDelivery::ReliableOrdered, bytes, size );
        }
        //////////////////////////////////////////////////////////////////////////
        static bool testMatch( bool _loss )
        {
            TestBus bus;
            bus.loss = _loss;
            TestSocket serverSocket( &bus, 0 );
            TestSocket firstSocket( &bus, 1, &serverSocket );
            TestSocket secondSocket( &bus, 2, &serverSocket );
            NetworkServer server( &serverSocket );
            NetworkClient first( &firstSocket );
            NetworkClient second( &secondSocket );
            first.setUniqueIdentity( 200 );
            second.setUniqueIdentity( 201 );
            Mengine::SocketAddress address;
            if( server.listen( address ) == false )
            {
                testFailed( __LINE__ );
                return false;
            }
            if( first.connect( address, "first", NetworkClientKind::Human ) == false )
            {
                testFailed( __LINE__ );
                return false;
            }
            if( second.connect( address, "second", NetworkClientKind::Bot ) == false )
            {
                testFailed( __LINE__ );
                return false;
            }
            bool paused = false;
            bool resumed = false;
            bool restarted = false;
            for( uint64_t step = 0; step != 600; ++step )
            {
                bus.advance();
                server.tick();
                first.updateNetwork();
                second.updateNetwork();
                if( first.state() != NetworkClientState::Connected || second.state() != NetworkClientState::Connected )
                {
                    continue;
                }
                uint64_t time = first.simulation().state().tick * TickMilliseconds;
                if( step >= 20 && step < 60 )
                {
                    CommandPayload look;
                    look.lookYaw = 5;
                    first.sendCommand( CommandType::LookDelta, time, look );
                    second.sendCommand( CommandType::LookDelta, time, look );
                }
                if( step == 47 )
                {
                    requestSnapshot( &firstSocket );
                }
                if( step == 80 )
                {
                    first.sendCommand( CommandType::Pause, time );
                }
                if( step >= 90 && step < 100 )
                {
                    first.sendCommand( CommandType::LookDelta, time );
                }
                if( step == 110 )
                {
                    paused = server.simulation().state().paused && first.simulation().state().paused && second.simulation().state().paused;
                    first.sendCommand( CommandType::Resume, time );
                }
                if( step == 135 )
                {
                    resumed = server.simulation().state().paused == false;
                }
                if( step == 150 )
                {
                    first.sendCommand( CommandType::LookDelta, 100000 );
                    first.sendCommand( CommandType::Restart, time );
                }
                if( step == 180 )
                {
                    restarted = first.simulation().state().matchId == 2 && second.simulation().state().matchId == 2;
                    first.sendCommand( CommandType::FireDown, time );
                }
                if( step == 190 )
                {
                    first.sendCommand( CommandType::FireUp, time );
                }
                first.tick();
                second.tick();
            }
            // End at a published state frame, then drain the transport without
            // advancing either client prediction or authoritative simulation.
            while( server.simulation().state().tick % NetworkStateFrameTicks != 0 )
            {
                bus.advance();
                server.tick();
                first.updateNetwork();
                second.updateNetwork();
            }
            // The final unreliable StateFrame can itself be lost. Request a
            // reliable baseline before comparing stopped simulations.
            requestSnapshot( &firstSocket );
            requestSnapshot( &secondSocket );
            for( uint32_t index = 0; index != 30; ++index )
            {
                bus.advance();
                server.updateNetwork();
                first.updateNetwork();
                second.updateNetwork();
            }
            bool equal = first.simulation().checksum() == server.simulation().checksum() && second.simulation().checksum() == server.simulation().checksum();
            Mengine::StdIO::printf( "network loss=%d pause=%d resume=%d restart=%d equal=%d pending=%zu/%zu ticks=%llu/%llu/%llu\n", _loss, paused, resumed, restarted, equal,
                first.pendingCommandCount(), second.pendingCommandCount(), static_cast<unsigned long long>( server.simulation().state().tick ),
                static_cast<unsigned long long>( first.simulation().state().tick ), static_cast<unsigned long long>( second.simulation().state().tick ) );
            if( paused == false )
            {
                testFailed( __LINE__ );
                return false;
            }
            if( resumed == false )
            {
                testFailed( __LINE__ );
                return false;
            }
            if( restarted == false )
            {
                testFailed( __LINE__ );
                return false;
            }
            if( equal == false )
            {
                testFailed( __LINE__ );
                return false;
            }
            if( first.pendingCommandCount() != 0 )
            {
                testFailed( __LINE__ );
                return false;
            }
            if( second.pendingCommandCount() != 0 )
            {
                testFailed( __LINE__ );
                return false;
            }
            first.disconnect();
            server.updateNetwork();
            if( server.roster().size() != 1 )
            {
                testFailed( __LINE__ );
                return false;
            }

            return true;
        }
        //////////////////////////////////////////////////////////////////////////
        static bool testLostPauseFrame()
        {
            TestBus bus;
            TestSocket serverSocket( &bus, 0 );
            TestSocket firstSocket( &bus, 1, &serverSocket );
            TestSocket observerSocket( &bus, 2, &serverSocket );
            NetworkServer server( &serverSocket );
            NetworkClient first( &firstSocket );
            NetworkClient observer( &observerSocket );
            first.setUniqueIdentity( 200 );
            observer.setUniqueIdentity( 201 );
            Mengine::SocketAddress address;
            if( server.listen( address ) == false )
            {
                testFailed( __LINE__ );
                return false;
            }
            if( first.connect( address, "first", NetworkClientKind::Human ) == false )
            {
                testFailed( __LINE__ );
                return false;
            }
            if( observer.connect( address, "observer", NetworkClientKind::Human ) == false )
            {
                testFailed( __LINE__ );
                return false;
            }
            auto advance = [&]()
            {
                bus.advance();
                server.tick();
                first.updateNetwork();
                observer.updateNetwork();
                first.tick();
                observer.tick();
            };
            for( uint32_t step = 0; step != 60; ++step )
            {
                advance();
            }
            bus.server = &server;
            bus.pausedFramePeer = 2;
            bus.pausedFrameDrops = 1;
            uint64_t time = first.simulation().state().tick * TickMilliseconds;
            if( first.sendCommand( CommandType::Pause, time ) == false )
            {
                testFailed( __LINE__ );
                return false;
            }
            for( uint32_t step = 0; step != 100; ++step )
            {
                advance();
            }
            if( bus.pausedFrameDrops != 0 )
            {
                testFailed( __LINE__ );
                return false;
            }
            if( server.simulation().state().paused == false )
            {
                testFailed( __LINE__ );
                return false;
            }
            StateChecksum checksum = server.simulation().checksum();
            if( first.simulation().checksum() != checksum )
            {
                testFailed( __LINE__ );
                return false;
            }
            if( observer.simulation().checksum() != checksum )
            {
                testFailed( __LINE__ );
                return false;
            }

            return true;
        }
        //////////////////////////////////////////////////////////////////////////
        static bool testConfirmationBackpressure()
        {
            TestBus bus;
            TestSocket serverSocket( &bus, 0 );
            TestSocket clientSocket( &bus, 1, &serverSocket );
            NetworkServer server( &serverSocket );
            NetworkClient client( &clientSocket );
            client.setUniqueIdentity( 202 );
            Mengine::SocketAddress address;
            if( server.listen( address ) == false )
            {
                testFailed( __LINE__ );
                return false;
            }
            if( client.connect( address, "client", NetworkClientKind::Human ) == false )
            {
                testFailed( __LINE__ );
                return false;
            }
            auto advance = [&]()
            {
                bus.advance();
                server.tick();
                client.updateNetwork();
                client.tick();
            };
            for( uint32_t step = 0; step != 60; ++step )
            {
                advance();
            }
            bus.confirmationLimit = 2;
            uint64_t time = client.simulation().state().tick * TickMilliseconds;
            for( uint32_t index = 0; index != 3; ++index )
            {
                if( client.sendCommand( CommandType::LookDelta, time ) == false )
                {
                    testFailed( __LINE__ );
                    return false;
                }
            }
            for( uint32_t step = 0; step != 100; ++step )
            {
                advance();
            }
            if( bus.confirmations.size() != 3 )
            {
                testFailed( __LINE__ );
                return false;
            }
            for( uint64_t index = 0; index != 3; ++index )
            {
                if( bus.confirmations[index] != index + 1 )
                {
                    testFailed( __LINE__ );
                    return false;
                }
            }
            if( client.pendingCommandCount() != 0 )
            {
                testFailed( __LINE__ );
                return false;
            }

            return true;
        }
        //////////////////////////////////////////////////////////////////////////
        static bool testCommandSendBackpressure()
        {
            TestBus bus;
            TestSocket serverSocket( &bus, 0 );
            TestSocket clientSocket( &bus, 1, &serverSocket );
            NetworkServer server( &serverSocket );
            NetworkClient client( &clientSocket );
            client.setUniqueIdentity( 202 );
            Mengine::SocketAddress address;
            if( server.listen( address ) == false )
            {
                return false;
            }
            if( client.connect( address, "send-retry", NetworkClientKind::Human ) == false )
            {
                return false;
            }
            auto advance = [&]()
            {
                bus.advance();
                server.tick();
                client.updateNetwork();
                client.tick();
            };
            for( uint32_t update = 0; update != 60; ++update )
            {
                advance();
            }
            uint32_t playerId = client.playerId();
            const PlayerState * before = server.simulation().findPlayer( playerId );
            kf_angle16_t previousYaw = before->yaw;
            CommandPayload payload;
            payload.lookYaw = 7;
            bus.commandSendFailures = 1;
            uint64_t time = client.simulation().state().tick * TickMilliseconds;
            if( client.sendCommand( CommandType::LookDelta, time, payload ) == false )
            {
                return false;
            }
            if( bus.commandSendFailures != 0 || client.pendingCommandCount() != 1 )
            {
                return false;
            }
            for( uint32_t update = 0; update != 40; ++update )
            {
                advance();
            }
            const PlayerState * after = server.simulation().findPlayer( playerId );
            kf_angle16_t expectedYaw = kf_angle16_add( previousYaw, payload.lookYaw );
            if( after->yaw != expectedYaw )
            {
                return false;
            }
            if( client.pendingCommandCount() != 0 )
            {
                return false;
            }

            return true;
        }
        //////////////////////////////////////////////////////////////////////////
        static Mengine::Data makeProgressMessage( const Simulation & _simulation, NetworkMessage _message, uint32_t _playerId, uint64_t _gameplayProgress, uint64_t _controlProgress )
        {
            Mengine::Data snapshot = _simulation.snapshot();
            Mengine::Data compressed;
            for( size_t offset = 0; offset != snapshot.size(); )
            {
                size_t remaining = snapshot.size() - offset;
                size_t count = remaining < 128 ? remaining : 128;
                compressed.push_back( static_cast<uint8_t>(count - 1) );
                compressed.insert( compressed.end(), snapshot.begin() + static_cast<ptrdiff_t>(offset), snapshot.begin() + static_cast<ptrdiff_t>(offset + count) );
                offset += count;
            }
            CanonicalWriter writer;
            writer.writeU32( NetworkWireMagic );
            writer.writeU32( NetworkProtocolVersion );
            writer.writeU8( static_cast<uint8_t>(_message) );
            if( _message == NetworkMessage::Welcome )
            {
                writer.writeU32( _playerId );
            }
            writer.writeU64( _simulation.state().matchId );
            if( _message == NetworkMessage::Welcome )
            {
                writer.writeU64( _simulation.state().seed );
            }
            writer.writeU64( _simulation.state().tick );
            writer.writeU64( _gameplayProgress );
            writer.writeU64( 0 );
            writer.writeU64( _controlProgress );
            writer.writeU64( 0 );
            if( _message == NetworkMessage::StateFrame )
            {
                uint32_t checksum = _simulation.checksum().full;
                writer.writeU32( checksum );
                writer.writeU32( 0 );
            }
            writer.writeU32( static_cast<uint32_t>(snapshot.size()) );
            writer.writeU32( static_cast<uint32_t>(compressed.size()) );
            writer.writeBytes( compressed.data(), compressed.size() );
            Mengine::Data data = writer.take();

            return data;
        }
        //////////////////////////////////////////////////////////////////////////
        static void deliverProgressMessage( TestBus * _bus, TestSocket * _socket, NetworkMessage _message, Mengine::Data _data )
        {
            TestPacketDesc packet;
            packet.destination = _socket;
            packet.due = _bus->now;
            packet.ordinal = ++_bus->ordinal;
            packet.event.type = Mengine::ESocketServiceEventType::MessageReceived;
            packet.event.peerId = 1;
            packet.event.channel = _message == NetworkMessage::Welcome ? 3 : 2;
            packet.event.delivery = _message == NetworkMessage::Welcome ? Mengine::ESocketDelivery::ReliableOrdered : Mengine::ESocketDelivery::UnreliableSequenced;
            packet.event.payload = Mengine::StdUtility::move( _data );
            _socket->deliver( Mengine::StdUtility::move( packet ) );
        }
        //////////////////////////////////////////////////////////////////////////
        static bool testProgressValidation( NetworkMessage _message, bool _invalidGameplay, bool _freshSession )
        {
            TestBus bus;
            TestSocket serverSocket( &bus, 0 );
            TestSocket clientSocket( &bus, 1, &serverSocket );
            NetworkServer server( &serverSocket );
            NetworkClient client( &clientSocket );
            client.setUniqueIdentity( 202 );
            Mengine::SocketAddress address;
            if( server.listen( address ) == false )
            {
                return false;
            }
            if( client.connect( address, "progress", NetworkClientKind::Human ) == false )
            {
                return false;
            }
            for( uint32_t update = 0; update != 60; ++update )
            {
                bus.advance();
                server.tick();
                client.updateNetwork();
                client.tick();
            }
            CommandPayload look;
            look.lookYaw = 7;
            if( client.sendCommand( CommandType::LookDelta, 1000000, look ) == false )
            {
                return false;
            }
            if( client.sendCommand( CommandType::Pause, 1000000 ) == false )
            {
                return false;
            }
            client.tick();
            StateChecksum before = client.simulation().checksum();
            Simulation baseline = server.simulation();
            if( _freshSession == true )
            {
                baseline.reset( 2, 456 );
            }
            uint64_t gameplayProgress = _invalidGameplay == true ? 2 : 1;
            uint64_t controlProgress = _invalidGameplay == true ? 1 : 2;
            if( _freshSession == true )
            {
                gameplayProgress = 0;
                controlProgress = 1;
            }
            uint32_t playerId = client.playerId();
            Mengine::Data invalid = makeProgressMessage( baseline, _message, playerId, gameplayProgress, controlProgress );
            deliverProgressMessage( &bus, &clientSocket, _message, Mengine::StdUtility::move( invalid ) );
            client.updateNetwork();
            if( client.pendingCommandCount() != 2 )
            {
                testFailed( __LINE__ );
                return false;
            }
            if( client.simulation().checksum() != before )
            {
                testFailed( __LINE__ );
                return false;
            }
            const Simulation & authoritative = server.simulation();
            Mengine::Data valid = makeProgressMessage( authoritative, NetworkMessage::StateFrame, playerId, 0, 0 );
            deliverProgressMessage( &bus, &clientSocket, NetworkMessage::StateFrame, Mengine::StdUtility::move( valid ) );
            client.updateNetwork();
            client.tick();
            if( client.simulation().state().paused == false )
            {
                testFailed( __LINE__ );
                return false;
            }
            const PlayerState * expected = authoritative.findPlayer( playerId );
            const PlayerState * actual = client.simulation().findPlayer( playerId );
            kf_angle16_t yaw = kf_angle16_add( expected->yaw, look.lookYaw );
            if( actual->yaw != yaw )
            {
                testFailed( __LINE__ );
                return false;
            }
            if( client.pendingCommandCount() != 2 )
            {
                testFailed( __LINE__ );
                return false;
            }

            return true;
        }
        //////////////////////////////////////////////////////////////////////////
        static bool testPausedGameplayBacklog()
        {
            TestBus bus;
            TestSocket serverSocket( &bus, 0 );
            TestSocket clientSocket( &bus, 1, &serverSocket );
            NetworkServer server( &serverSocket );
            NetworkClient client( &clientSocket );
            client.setUniqueIdentity( 202 );
            Mengine::SocketAddress address;
            if( server.listen( address ) == false )
            {
                return false;
            }
            if( client.connect( address, "backlog", NetworkClientKind::Human ) == false )
            {
                return false;
            }
            auto advance = [&]()
            {
                bus.advance();
                server.tick();
                client.updateNetwork();
                client.tick();
            };
            for( uint32_t update = 0; update != 60; ++update )
            {
                advance();
            }
            if( client.sendCommand( CommandType::Pause, 1000000 ) == false )
            {
                return false;
            }
            for( uint32_t update = 0; update != 40; ++update )
            {
                advance();
            }
            for( size_t index = 0; index != MAXIMUM_PENDING_GAMEPLAY_COMMANDS; ++index )
            {
                if( client.sendCommand( CommandType::LookDelta, 1000000 ) == false )
                {
                    return false;
                }
            }
            client.tick();
            for( uint32_t cycle = 0; cycle != 70; ++cycle )
            {
                if( client.sendCommand( CommandType::Resume, 1000000 ) == false )
                {
                    return false;
                }
                client.tick();
                if( client.simulation().state().paused == true )
                {
                    return false;
                }
                for( uint32_t update = 0; update != 15; ++update )
                {
                    advance();
                }
                if( server.simulation().state().paused == true )
                {
                    return false;
                }
                if( client.pendingCommandCount() != MAXIMUM_PENDING_GAMEPLAY_COMMANDS )
                {
                    return false;
                }
                if( client.sendCommand( CommandType::Pause, 1000000 ) == false )
                {
                    return false;
                }
                for( uint32_t update = 0; update != 15; ++update )
                {
                    advance();
                }
                if( server.simulation().state().paused == false )
                {
                    return false;
                }
            }
            if( client.sendCommand( CommandType::Restart, 1000000 ) == false )
            {
                return false;
            }
            for( uint32_t update = 0; update != 30; ++update )
            {
                advance();
            }
            if( server.simulation().state().matchId != 2 )
            {
                return false;
            }
            if( client.pendingCommandCount() != 0 )
            {
                return false;
            }

            return true;
        }
        //////////////////////////////////////////////////////////////////////////
        static Mengine::SocketServiceEvent makeRejectEvent( uint32_t _peerId, const char * _message )
        {
            CanonicalWriter writer;
            writer.writeU32( NetworkWireMagic );
            writer.writeU32( NetworkProtocolVersion );
            writer.writeU8( static_cast<uint8_t>(NetworkMessage::Reject) );
            writer.writeU32( 73 );
            writer.writeString( _message );
            Mengine::SocketServiceEvent event;
            event.type = Mengine::ESocketServiceEventType::MessageReceived;
            event.peerId = _peerId;
            event.channel = 0;
            event.delivery = Mengine::ESocketDelivery::ReliableOrdered;
            event.payload = writer.data();

            return event;
        }
        //////////////////////////////////////////////////////////////////////////
        static bool testTerminalEvents( uint32_t _mode )
        {
            TestBus bus;
            TestSocket serverSocket( &bus, 0 );
            TestSocket clientSocket( &bus, 1, &serverSocket );
            NetworkServer server( &serverSocket );
            NetworkClient client( &clientSocket );
            client.setUniqueIdentity( 204 );
            Mengine::SocketAddress address;
            if( server.listen( address ) == false )
            {
                testFailed( __LINE__ );
                return false;
            }
            if( client.connect( address, "terminal", NetworkClientKind::Human ) == false )
            {
                testFailed( __LINE__ );
                return false;
            }
            auto advance = [&]()
            {
                bus.advance();
                server.updateNetwork();
                client.updateNetwork();
            };
            for( uint32_t update = 0; update != 12; ++update )
            {
                advance();
            }
            if( client.state() != NetworkClientState::Connected )
            {
                testFailed( __LINE__ );
                return false;
            }
            requestSnapshot( &clientSocket );
            advance();
            advance();
            Mengine::SocketServiceEvent welcome;
            bool foundWelcome = false;
            for( const TestPacketDesc & packet : bus.packets )
            {
                if( packet.destination != &clientSocket )
                {
                    continue;
                }
                if( packet.event.channel != 3 )
                {
                    continue;
                }
                welcome = packet.event;
                foundWelcome = true;
                break;
            }
            if( foundWelcome == false )
            {
                testFailed( __LINE__ );
                return false;
            }
            if( client.sendCommand( CommandType::LookDelta, 100000 ) == false )
            {
                testFailed( __LINE__ );
                return false;
            }
            if( client.pendingCommandCount() != 1 )
            {
                testFailed( __LINE__ );
                return false;
            }
            uint32_t previousPeer = clientSocket.getPeerId();
            Mengine::SocketServiceEvent terminal;
            terminal.peerId = previousPeer;
            if( _mode == 0 )
            {
                client.disconnect();
            }
            else if( _mode == 1 )
            {
                terminal.type = Mengine::ESocketServiceEventType::PeerDisconnected;
                clientSocket.enqueueEvent( terminal );
            }
            else if( _mode == 2 )
            {
                terminal.type = Mengine::ESocketServiceEventType::TransportError;
                clientSocket.enqueueEvent( terminal );
            }
            else
            {
                clientSocket.enqueueEvent( makeRejectEvent( previousPeer, "first rejection" ) );
            }
            Mengine::SocketServiceEvent connected;
            connected.type = Mengine::ESocketServiceEventType::PeerConnected;
            connected.peerId = previousPeer;
            clientSocket.enqueueEvent( connected );
            clientSocket.enqueueEvent( welcome );
            clientSocket.enqueueEvent( makeRejectEvent( previousPeer, "late rejection" ) );
            terminal.type = Mengine::ESocketServiceEventType::PeerDisconnected;
            terminal.peerId = previousPeer;
            clientSocket.enqueueEvent( terminal );
            client.updateNetwork();
            NetworkClientState expectedState = _mode == 3 ? NetworkClientState::Rejected : NetworkClientState::Disconnected;
            if( client.state() != expectedState )
            {
                testFailed( __LINE__ );
                return false;
            }
            if( client.pendingCommandCount() != 0 )
            {
                testFailed( __LINE__ );
                return false;
            }
            if( client.playerId() != 0 )
            {
                testFailed( __LINE__ );
                return false;
            }
            if( client.roster().empty() == false )
            {
                testFailed( __LINE__ );
                return false;
            }
            if( client.events().empty() == false )
            {
                testFailed( __LINE__ );
                return false;
            }
            Mengine::String expectedRejection = _mode == 3 ? "first rejection" : "";
            if( client.rejection() != expectedRejection )
            {
                testFailed( __LINE__ );
                return false;
            }
            if( client.sendCommand( CommandType::LookDelta, 0 ) == true )
            {
                testFailed( __LINE__ );
                return false;
            }
            if( client.connect( address, "reconnected", NetworkClientKind::Human ) == false )
            {
                testFailed( __LINE__ );
                return false;
            }
            clientSocket.enqueueEvent( connected );
            clientSocket.enqueueEvent( welcome );
            clientSocket.enqueueEvent( makeRejectEvent( previousPeer, "stale rejection" ) );
            clientSocket.enqueueEvent( terminal );
            terminal.type = Mengine::ESocketServiceEventType::TransportError;
            terminal.peerId = Mengine::InvalidSocketPeerId;
            clientSocket.enqueueEvent( terminal );
            client.updateNetwork();
            if( client.state() != NetworkClientState::Handshaking )
            {
                testFailed( __LINE__ );
                return false;
            }
            if( client.rejection().empty() == false )
            {
                testFailed( __LINE__ );
                return false;
            }
            for( uint32_t update = 0; update != 12; ++update )
            {
                advance();
            }
            if( client.state() != NetworkClientState::Connected )
            {
                testFailed( __LINE__ );
                return false;
            }
            connected.peerId = clientSocket.getPeerId();
            clientSocket.enqueueEvent( connected );
            client.updateNetwork();
            if( client.state() != NetworkClientState::Connected )
            {
                testFailed( __LINE__ );
                return false;
            }

            return true;
        }
        //////////////////////////////////////////////////////////////////////////
        static bool testNewServerIdentityReuse()
        {
            TestBus bus;
            bus.confirmationLimit = 64;
            TestSocket serverSocket( &bus, 0 );
            TestSocket clientSocket( &bus, 1, &serverSocket );
            NetworkServer previousServer( &serverSocket );
            NetworkClient client( &clientSocket );
            client.setUniqueIdentity( 205 );
            Mengine::SocketAddress address;
            if( previousServer.listen( address ) == false )
            {
                testFailed( __LINE__ );
                return false;
            }
            if( client.connect( address, "old server", NetworkClientKind::Human ) == false )
            {
                testFailed( __LINE__ );
                return false;
            }
            for( uint32_t update = 0; update != 12; ++update )
            {
                bus.advance();
                previousServer.updateNetwork();
                client.updateNetwork();
            }
            if( client.state() != NetworkClientState::Connected )
            {
                testFailed( __LINE__ );
                return false;
            }
            uint32_t previousPlayer = client.playerId();
            uint64_t previousMatch = client.simulation().state().matchId;
            if( client.sendCommand( CommandType::LookDelta, 100000 ) == false )
            {
                testFailed( __LINE__ );
                return false;
            }
            client.disconnect();
            if( client.pendingCommandCount() != 0 )
            {
                testFailed( __LINE__ );
                return false;
            }
            for( uint32_t update = 0; update != 8; ++update )
            {
                bus.advance();
                previousServer.updateNetwork();
                client.updateNetwork();
            }
            NetworkServer server( &serverSocket );
            if( server.listen( address ) == false )
            {
                testFailed( __LINE__ );
                return false;
            }
            if( client.connect( address, "new server", NetworkClientKind::Human ) == false )
            {
                testFailed( __LINE__ );
                return false;
            }
            for( uint32_t update = 0; update != 12; ++update )
            {
                bus.advance();
                server.tick();
                client.updateNetwork();
            }
            if( client.state() != NetworkClientState::Connected )
            {
                testFailed( __LINE__ );
                return false;
            }
            if( client.playerId() != previousPlayer )
            {
                testFailed( __LINE__ );
                return false;
            }
            if( client.simulation().state().matchId != previousMatch )
            {
                testFailed( __LINE__ );
                return false;
            }
            CommandPayload payload;
            payload.lookYaw = 7;
            if( client.sendCommand( CommandType::LookDelta, 0, payload ) == false )
            {
                testFailed( __LINE__ );
                return false;
            }
            for( uint32_t update = 0; update != 12; ++update )
            {
                bus.advance();
                server.tick();
                client.updateNetwork();
            }
            if( client.pendingCommandCount() != 0 )
            {
                testFailed( __LINE__ );
                return false;
            }
            if( bus.confirmations.size() != 1 )
            {
                testFailed( __LINE__ );
                return false;
            }
            if( bus.confirmations[0] != 1 )
            {
                testFailed( __LINE__ );
                return false;
            }

            return true;
        }
        //////////////////////////////////////////////////////////////////////////
        static bool testControlSendBackpressure( NetworkMessage _message )
        {
            TestBus bus;
            size_t messageIndex = static_cast<size_t>(_message);
            bus.messageSendFailures[messageIndex] = 4;
            TestSocket serverSocket( &bus, 0 );
            TestSocket clientSocket( &bus, 1, &serverSocket );
            NetworkServer server( &serverSocket );
            NetworkClient client( &clientSocket );
            client.setUniqueIdentity( 206 );
            Mengine::SocketAddress address;
            NETWORK_CHECK( server.listen( address ) );
            NETWORK_CHECK( client.connect( address, "control retry", NetworkClientKind::Human ) );
            for( uint32_t update = 0; update != 64; ++update )
            {
                bus.advance();
                server.updateNetwork();
                client.updateNetwork();
            }
            NETWORK_CHECK( bus.messageSendFailures[messageIndex] == 0 );
            NETWORK_CHECK( client.state() == NetworkClientState::Connected );
            NETWORK_CHECK( server.roster().size() == 1 );
            NETWORK_CHECK( client.roster().size() == 1 );
            NETWORK_CHECK( bus.acceptedMessages[static_cast<size_t>(NetworkMessage::Hello)] == 1 );
            NETWORK_CHECK( bus.acceptedMessages[static_cast<size_t>(NetworkMessage::Welcome)] == 1 );
            NETWORK_CHECK( bus.acceptedMessages[static_cast<size_t>(NetworkMessage::Roster)] == 1 );
            NETWORK_CHECK( client.pendingCommandCount() == 0 );

            return true;
        }
        //////////////////////////////////////////////////////////////////////////
        static bool testRestartWelcomeBackpressure()
        {
            TestBus bus;
            bus.confirmationLimit = 64;
            TestSocket serverSocket( &bus, 0 );
            TestSocket clientSocket( &bus, 1, &serverSocket );
            NetworkServer server( &serverSocket );
            NetworkClient client( &clientSocket );
            client.setUniqueIdentity( 207 );
            Mengine::SocketAddress address;
            NETWORK_CHECK( server.listen( address ) );
            NETWORK_CHECK( client.connect( address, "restart retry", NetworkClientKind::Human ) );
            auto advance = [&]()
            {
                bus.advance();
                server.tick();
                client.updateNetwork();
            };
            for( uint32_t update = 0; update != 12; ++update )
            {
                advance();
            }
            NETWORK_CHECK( client.state() == NetworkClientState::Connected );
            bus.messageSendFailures[static_cast<size_t>(NetworkMessage::Welcome)] = 5;
            uint64_t sourceTime = client.simulation().state().tick * TickMilliseconds;
            NETWORK_CHECK( client.sendCommand( CommandType::Restart, sourceTime ) );
            client.tick();
            for( uint32_t update = 0; update != 40; ++update )
            {
                advance();
            }
            NETWORK_CHECK( client.state() == NetworkClientState::Connected );
            NETWORK_CHECK( client.simulation().state().matchId == 2 );
            NETWORK_CHECK( server.simulation().state().matchId == 2 );
            NETWORK_CHECK( bus.messageSendFailures[static_cast<size_t>(NetworkMessage::Welcome)] == 0 );
            NETWORK_CHECK( bus.acceptedMessages[static_cast<size_t>(NetworkMessage::Welcome)] == 2 );
            uint32_t playerId = client.playerId();
            const PlayerState * before = server.simulation().findPlayer( playerId );
            kf_angle16_t expectedYaw = kf_angle16_add( before->yaw, 1000 );
            CommandPayload look;
            look.lookYaw = 1000;
            sourceTime = client.simulation().state().tick * TickMilliseconds;
            NETWORK_CHECK( client.sendCommand( CommandType::LookDelta, sourceTime, look ) );
            for( uint32_t update = 0; update != 40; ++update )
            {
                advance();
            }
            const PlayerState * after = server.simulation().findPlayer( playerId );
            NETWORK_CHECK( after->yaw == expectedYaw );
            NETWORK_CHECK( client.pendingCommandCount() == 0 );
            NETWORK_CHECK( server.roster().size() == 1 );
            NETWORK_CHECK( bus.acceptedMessages[static_cast<size_t>(NetworkMessage::Hello)] == 1 );
            NETWORK_CHECK( bus.acceptedMessages[static_cast<size_t>(NetworkMessage::Welcome)] == 2 );
            NETWORK_CHECK( bus.confirmations.size() == 2 );

            return true;
        }
        //////////////////////////////////////////////////////////////////////////
        static bool testSnapshotBackpressure()
        {
            TestBus bus;
            TestSocket serverSocket( &bus, 0 );
            TestSocket clientSocket( &bus, 1, &serverSocket );
            NetworkServer server( &serverSocket );
            NetworkClient client( &clientSocket );
            client.setUniqueIdentity( 208 );
            Mengine::SocketAddress address;
            NETWORK_CHECK( server.listen( address ) );
            NETWORK_CHECK( client.connect( address, "snapshot retry", NetworkClientKind::Human ) );
            auto advance = [&]()
            {
                bus.advance();
                server.updateNetwork();
                client.updateNetwork();
            };
            for( uint32_t update = 0; update != 12; ++update )
            {
                advance();
            }
            NETWORK_CHECK( client.state() == NetworkClientState::Connected );
            bus.messageSendFailures[static_cast<size_t>(NetworkMessage::SnapshotRequest)] = 3;
            bus.messageSendFailures[static_cast<size_t>(NetworkMessage::Welcome)] = 4;
            const Simulation & authoritative = server.simulation();
            uint32_t playerId = client.playerId();
            for( uint32_t index = 0; index != 3; ++index )
            {
                Mengine::Data invalid = makeProgressMessage( authoritative, NetworkMessage::StateFrame, playerId, 0, 0 );
                invalid.back() ^= 1;
                deliverProgressMessage( &bus, &clientSocket, NetworkMessage::StateFrame, Mengine::StdUtility::move( invalid ) );
            }
            client.updateNetwork();
            NETWORK_CHECK( bus.messageSendFailures[static_cast<size_t>(NetworkMessage::SnapshotRequest)] == 2 );
            for( uint32_t update = 0; update != 32; ++update )
            {
                advance();
            }
            NETWORK_CHECK( client.state() == NetworkClientState::Connected );
            NETWORK_CHECK( bus.messageSendFailures[static_cast<size_t>(NetworkMessage::SnapshotRequest)] == 0 );
            NETWORK_CHECK( bus.messageSendFailures[static_cast<size_t>(NetworkMessage::Welcome)] == 0 );
            NETWORK_CHECK( bus.acceptedMessages[static_cast<size_t>(NetworkMessage::SnapshotRequest)] == 1 );
            NETWORK_CHECK( bus.acceptedMessages[static_cast<size_t>(NetworkMessage::Welcome)] == 2 );
            NETWORK_CHECK( server.roster().size() == 1 );
            NETWORK_CHECK( client.pendingCommandCount() == 0 );

            return true;
        }
        //////////////////////////////////////////////////////////////////////////
        static bool testRosterFanoutBackpressure()
        {
            TestBus bus;
            TestSocket serverSocket( &bus, 0 );
            TestSocket firstSocket( &bus, 1, &serverSocket );
            TestSocket secondSocket( &bus, 2, &serverSocket );
            NetworkServer server( &serverSocket );
            NetworkClient first( &firstSocket );
            NetworkClient second( &secondSocket );
            first.setUniqueIdentity( 209 );
            second.setUniqueIdentity( 210 );
            Mengine::SocketAddress address;
            NETWORK_CHECK( server.listen( address ) );
            NETWORK_CHECK( first.connect( address, "first roster", NetworkClientKind::Human ) );
            auto advance = [&]()
            {
                bus.advance();
                server.updateNetwork();
                first.updateNetwork();
                second.updateNetwork();
            };
            for( uint32_t update = 0; update != 12; ++update )
            {
                advance();
            }
            bus.failingMessagePeer = 2;
            bus.messageSendFailures[static_cast<size_t>(NetworkMessage::Roster)] = 4;
            NETWORK_CHECK( second.connect( address, "second roster", NetworkClientKind::Human ) );
            for( uint32_t update = 0; update != 32; ++update )
            {
                advance();
            }
            NETWORK_CHECK( first.roster().size() == 2 );
            NETWORK_CHECK( second.roster().size() == 2 );
            NETWORK_CHECK( bus.acceptedRosters[1] == 2 );
            NETWORK_CHECK( bus.acceptedRosters[2] == 1 );
            NETWORK_CHECK( bus.messageSendFailures[static_cast<size_t>(NetworkMessage::Roster)] == 0 );
            second.disconnect();
            bus.failingMessagePeer = 1;
            bus.messageSendFailures[static_cast<size_t>(NetworkMessage::Roster)] = 3;
            for( uint32_t update = 0; update != 32; ++update )
            {
                advance();
            }
            NETWORK_CHECK( first.roster().size() == 1 );
            NETWORK_CHECK( server.roster().size() == 1 );
            NETWORK_CHECK( bus.acceptedRosters[1] == 3 );

            return true;
        }
        //////////////////////////////////////////////////////////////////////////
        static bool testRejectBackpressure()
        {
            TestBus bus;
            bus.messageSendFailures[static_cast<size_t>(NetworkMessage::Reject)] = 4;
            TestSocket serverSocket( &bus, 0 );
            TestSocket clientSocket( &bus, 1, &serverSocket );
            NetworkServer server( &serverSocket );
            NetworkClient client( &clientSocket );
            client.setUniqueIdentity( 211 );
            Mengine::SocketAddress address;
            NETWORK_CHECK( server.listen( address ) );
            NETWORK_CHECK( client.connect( address, "reject retry", NetworkClientKind::Human ) );
            uint8_t invalid[9]{};
            uint32_t peerId = clientSocket.getPeerId();
            NETWORK_CHECK( clientSocket.send( peerId, 0, Mengine::ESocketDelivery::ReliableOrdered, invalid, sizeof( invalid ) ) );
            for( uint32_t update = 0; update != 32; ++update )
            {
                bus.advance();
                server.updateNetwork();
                client.updateNetwork();
            }
            NETWORK_CHECK( client.state() == NetworkClientState::Rejected );
            NETWORK_CHECK( client.rejection() == "invalid wire header" );
            NETWORK_CHECK( server.roster().empty() );
            NETWORK_CHECK( bus.acceptedMessages[static_cast<size_t>(NetworkMessage::Reject)] == 1 );
            NETWORK_CHECK( bus.acceptedMessages[static_cast<size_t>(NetworkMessage::Welcome)] == 0 );
            NETWORK_CHECK( bus.messageSendFailures[static_cast<size_t>(NetworkMessage::Reject)] == 0 );

            return true;
        }
        //////////////////////////////////////////////////////////////////////////
        static bool testDisconnectBackpressure()
        {
            TestBus bus;
            TestSocket serverSocket( &bus, 0 );
            TestSocket clientSocket( &bus, 1, &serverSocket );
            NetworkServer server( &serverSocket );
            NetworkClient client( &clientSocket );
            client.setUniqueIdentity( 212 );
            Mengine::SocketAddress address;
            NETWORK_CHECK( server.listen( address ) );
            NETWORK_CHECK( client.connect( address, "disconnect retry", NetworkClientKind::Human ) );
            auto advance = [&]()
            {
                bus.advance();
                server.updateNetwork();
                client.updateNetwork();
            };
            for( uint32_t update = 0; update != 12; ++update )
            {
                advance();
            }
            NETWORK_CHECK( client.state() == NetworkClientState::Connected );
            NETWORK_CHECK( client.sendCommand( CommandType::LookDelta, 1000000 ) );
            bus.disconnectFailures = 3;
            client.disconnect();
            NETWORK_CHECK( client.state() == NetworkClientState::Disconnected );
            NETWORK_CHECK( client.pendingCommandCount() == 0 );
            NETWORK_CHECK( client.connect( address, "too early", NetworkClientKind::Human ) == false );
            for( uint32_t update = 0; update != 12; ++update )
            {
                advance();
            }
            NETWORK_CHECK( bus.disconnectFailures == 0 );
            NETWORK_CHECK( bus.acceptedDisconnects == 1 );
            NETWORK_CHECK( server.roster().empty() );
            NETWORK_CHECK( client.connect( address, "after close", NetworkClientKind::Human ) );
            for( uint32_t update = 0; update != 12; ++update )
            {
                advance();
            }
            NETWORK_CHECK( client.state() == NetworkClientState::Connected );
            NETWORK_CHECK( server.roster().size() == 1 );
            NETWORK_CHECK( client.pendingCommandCount() == 0 );

            return true;
        }
        //////////////////////////////////////////////////////////////////////////
        static bool testReconnects()
        {
            TestBus bus;
            TestSocket serverSocket( &bus, 0 );
            TestSocket clientSocket( &bus, 1, &serverSocket );
            NetworkServer server( &serverSocket );
            NetworkClient client( &clientSocket );
            client.setUniqueIdentity( 202 );
            Mengine::SocketAddress address;
            if( server.listen( address ) == false )
            {
                testFailed( __LINE__ );
                return false;
            }
            uint32_t previousPlayer = 0;
            for( size_t cycle = 0; cycle != MaximumPlayers + 4; ++cycle )
            {
                if( client.connect( address, "reconnect", NetworkClientKind::Human ) == false )
                {
                    testFailed( __LINE__ );
                    return false;
                }
                for( uint32_t update = 0; update != 12; ++update )
                {
                    bus.advance();
                    server.updateNetwork();
                    client.updateNetwork();
                }
                if( client.state() != NetworkClientState::Connected )
                {
                    testFailed( __LINE__ );
                    return false;
                }
                if( client.playerId() <= previousPlayer )
                {
                    testFailed( __LINE__ );
                    return false;
                }
                if( client.pendingCommandCount() != 0 )
                {
                    testFailed( __LINE__ );
                    return false;
                }
                previousPlayer = client.playerId();
                client.sendCommand( CommandType::LookDelta, 100000 );
                for( uint32_t update = 0; update != 8; ++update )
                {
                    bus.advance();
                    server.updateNetwork();
                    client.updateNetwork();
                }
                if( client.pendingCommandCount() != 1 )
                {
                    testFailed( __LINE__ );
                    return false;
                }
                client.disconnect();
                for( uint32_t update = 0; update != 8; ++update )
                {
                    bus.advance();
                    server.updateNetwork();
                    client.updateNetwork();
                }
                if( server.roster().empty() == false )
                {
                    testFailed( __LINE__ );
                    return false;
                }
                if( server.simulation().findPlayer( previousPlayer ) != nullptr )
                {
                    testFailed( __LINE__ );
                    return false;
                }
            }
            if( bus.packets.empty() == false )
            {
                testFailed( __LINE__ );
                return false;
            }

            return true;
        }
    }
}

int main()
{
    Arena3D::NetworkMessage retryMessages[] = {Arena3D::NetworkMessage::Hello, Arena3D::NetworkMessage::Welcome, Arena3D::NetworkMessage::Roster};
    for( Arena3D::NetworkMessage message : retryMessages )
    {
        if( Arena3D::Detail::testControlSendBackpressure( message ) == false )
        {
            return 17;
        }
    }
    if( Arena3D::Detail::testRestartWelcomeBackpressure() == false )
    {
        return 18;
    }
    if( Arena3D::Detail::testSnapshotBackpressure() == false )
    {
        return 19;
    }
    if( Arena3D::Detail::testRosterFanoutBackpressure() == false )
    {
        return 20;
    }
    if( Arena3D::Detail::testRejectBackpressure() == false )
    {
        return 21;
    }
    if( Arena3D::Detail::testDisconnectBackpressure() == false )
    {
        return 22;
    }
    if( Arena3D::Detail::testMatch( false ) == false )
    {
        return 1;
    }
    if( Arena3D::Detail::testMatch( true ) == false )
    {
        return 2;
    }
    if( Arena3D::Detail::testCommandSendBackpressure() == false )
    {
        return 11;
    }
    if( Arena3D::Detail::testProgressValidation( Arena3D::NetworkMessage::StateFrame, false, false ) == false )
    {
        return 7;
    }
    if( Arena3D::Detail::testProgressValidation( Arena3D::NetworkMessage::StateFrame, true, false ) == false )
    {
        return 8;
    }
    if( Arena3D::Detail::testProgressValidation( Arena3D::NetworkMessage::Welcome, false, false ) == false )
    {
        return 9;
    }
    if( Arena3D::Detail::testProgressValidation( Arena3D::NetworkMessage::Welcome, false, true ) == false )
    {
        return 10;
    }
    if( Arena3D::Detail::testPausedGameplayBacklog() == false )
    {
        Mengine::StdIO::fprintf( stderr, "Paused gameplay backlog blocked session controls\n" );
        return 6;
    }
    if( Arena3D::Detail::testReconnects() == false )
    {
        return 3;
    }
    for( uint32_t mode = 0; mode != 4; ++mode )
    {
        if( Arena3D::Detail::testTerminalEvents( mode ) == false )
        {
            return 12 + mode;
        }
    }
    if( Arena3D::Detail::testNewServerIdentityReuse() == false )
    {
        return 16;
    }
    if( Arena3D::Detail::testLostPauseFrame() == false )
    {
        return 4;
    }
    if( Arena3D::Detail::testConfirmationBackpressure() == false )
    {
        return 5;
    }

    return 0;
}
