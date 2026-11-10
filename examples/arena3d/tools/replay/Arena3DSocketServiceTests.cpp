#include "Services/SocketService/SocketService.h"

#include "Kernel/AssertionObservable.h"
#include "Kernel/ReadHelper.h"
#include "Kernel/ThreadHelper.h"
#include "Kernel/ThreadMutexHelper.h"

#include "Config/StdIO.h"
#include "Config/StdLib.h"
#include "Config/StdString.h"
#include "Config/StdUtility.h"
//////////////////////////////////////////////////////////////////////////
namespace Mengine
{
    //////////////////////////////////////////////////////////////////////////
    namespace Helper
    {
#if defined(MENGINE_ASSERTION_DEBUG_ENABLE)
        //////////////////////////////////////////////////////////////////////////
        bool assertionHasObserver( Observable * _observer )
        {
            MENGINE_UNUSED( _observer );

            return false;
        }
#endif
        //////////////////////////////////////////////////////////////////////////
        ThreadMutexInterfacePtr createThreadMutex( const DocumentInterfacePtr & _doc )
        {
            MENGINE_UNUSED( _doc );

            StdIO::fprintf( stderr, "Unexpected mutex bootstrap in SocketService queue tests\n" );
            StdLib::exit( EXIT_FAILURE );
        }
        //////////////////////////////////////////////////////////////////////////
        ThreadIdentityInterfacePtr createThreadIdentity( const ThreadDescription & _description, EThreadPriority _priority, const LambdaThreadRunner & _lambda, Timestamp _sleep, const DocumentInterfacePtr & _doc )
        {
            MENGINE_UNUSED( _description );
            MENGINE_UNUSED( _priority );
            MENGINE_UNUSED( _lambda );
            MENGINE_UNUSED( _sleep );
            MENGINE_UNUSED( _doc );

            StdIO::fprintf( stderr, "Unexpected thread bootstrap in SocketService queue tests\n" );
            StdLib::exit( EXIT_FAILURE );
        }
        //////////////////////////////////////////////////////////////////////////
    }
    //////////////////////////////////////////////////////////////////////////
}
//////////////////////////////////////////////////////////////////////////
namespace Arena3D
{
    bool runSocketReceiveTests();

    //////////////////////////////////////////////////////////////////////////
    namespace Detail
    {
        //////////////////////////////////////////////////////////////////////////
        struct TestDatagramPacketDesc
        {
            Mengine::Data bytes;
            uint64_t due;
        };
        //////////////////////////////////////////////////////////////////////////
        class TestDatagram
            : public Mengine::DatagramInterface
        {
        public:
            TestDatagram()
            {
                this->setUniqueIdentity( 2 );
            }

            ~TestDatagram() override
            {
            }

        public:
            uint32_t incref() override
            {
                uint32_t count = m_references.increfReferenceCount();

                return count;
            }

            void decref() override
            {
                m_references.decrefReferenceCount();
            }

            uint32_t getrefcount() const override
            {
                uint32_t count = m_references.getReferenceCount();

                return count;
            }

            Mengine::ESocketResult open( Mengine::ESocketAddressFamily _family ) override
            {
                MENGINE_UNUSED( _family );

                return Mengine::ESocketResult::Successful;
            }

            Mengine::ESocketResult bind( const Mengine::SocketAddress & _address ) override
            {
                MENGINE_UNUSED( _address );

                return Mengine::ESocketResult::Successful;
            }

            Mengine::ESocketResult connect( const Mengine::SocketAddress & _address ) override
            {
                MENGINE_UNUSED( _address );

                return Mengine::ESocketResult::Successful;
            }

            Mengine::ESocketResult setBlocking( bool _blocking ) override
            {
                MENGINE_UNUSED( _blocking );

                return Mengine::ESocketResult::Successful;
            }

            Mengine::ESocketResult setReuseAddress( bool _reuse ) override
            {
                MENGINE_UNUSED( _reuse );

                return Mengine::ESocketResult::Successful;
            }

            Mengine::ESocketResult setSendBufferSize( uint32_t _size ) override
            {
                MENGINE_UNUSED( _size );

                return Mengine::ESocketResult::Successful;
            }

            Mengine::ESocketResult setReceiveBufferSize( uint32_t _size ) override
            {
                MENGINE_UNUSED( _size );

                return Mengine::ESocketResult::Successful;
            }

            Mengine::ESocketResult sendTo( const Mengine::SocketAddress & _address, const void * _data, size_t _size, size_t * const _sent ) override
            {
                MENGINE_UNUSED( _address );

                TestDatagramPacketDesc packet;
                packet.bytes.resize( _size );
                Mengine::StdString::memcpy( packet.bytes.data(), _data, _size );
                packet.due = now + delay;
                packets.emplace_back( Mengine::StdUtility::move( packet ) );
                *_sent = _size;

                return Mengine::ESocketResult::Successful;
            }

            Mengine::ESocketResult send( const void * _data, size_t _size, size_t * const _sent ) override
            {
                MENGINE_UNUSED( _data );
                MENGINE_UNUSED( _size );
                MENGINE_UNUSED( _sent );

                return Mengine::ESocketResult::Failure;
            }

            Mengine::ESocketResult receiveFrom( void * const _data, size_t _capacity, size_t * const _received, Mengine::SocketAddress * const _address ) override
            {
                MENGINE_UNUSED( _data );
                MENGINE_UNUSED( _capacity );
                MENGINE_UNUSED( _received );
                MENGINE_UNUSED( _address );

                return Mengine::ESocketResult::WouldBlock;
            }

            Mengine::ESocketResult receive( void * const _data, size_t _capacity, size_t * const _received ) override
            {
                MENGINE_UNUSED( _data );
                MENGINE_UNUSED( _capacity );
                MENGINE_UNUSED( _received );

                return Mengine::ESocketResult::WouldBlock;
            }

            Mengine::ESocketResult waitReadable( uint32_t _timeoutMs ) override
            {
                MENGINE_UNUSED( _timeoutMs );

                return Mengine::ESocketResult::Timeout;
            }

            Mengine::ESocketResult waitWritable( uint32_t _timeoutMs ) override
            {
                MENGINE_UNUSED( _timeoutMs );

                return Mengine::ESocketResult::Successful;
            }

            Mengine::ESocketResult getLocalAddress( Mengine::SocketAddress * const _address ) const override
            {
                MENGINE_UNUSED( _address );

                return Mengine::ESocketResult::Failure;
            }

            void close() override
            {
                //Empty
            }

        public:
            uint64_t now = 0;
            uint64_t delay = 0;
            Mengine::Vector<TestDatagramPacketDesc> packets;

        private:
            Mengine::ReferenceCounter m_references;
        };
        //////////////////////////////////////////////////////////////////////////
        class TestSocketService
            : public Mengine::SocketService
        {
        public:
            TestSocketService()
            {
                this->setUniqueIdentity( 1 );
            }

            ~TestSocketService() override
            {
            }

        public:
            using SocketService::connect;
            using SocketService::send;
            using SocketService::broadcast;
            using SocketService::disconnect;
            using SocketService::pollEvent;
            using SocketService::stop;

            void addPeer( Mengine::SocketPeerId _id, bool _connected = true, bool _loopback = true )
            {
                m_maximumPeers = 1024;
                Mengine::SocketAddress address;
                address.family = Mengine::ESocketAddressFamily::IPv4;
                address.address[0] = _loopback == true ? 127 : 192;
                address.address[2] = _loopback == true ? 0 : 2;
                address.address[3] = 1;
                address.port = 5000;
                EPeerState state = _connected == true ? EPeerState::Connected : EPeerState::Connecting;
                this->createPeer_( _id, address, _id, state, 0 );
            }

            void connectPeer( Mengine::SocketPeerId _id )
            {
                Peer * peer = this->findPeer_( _id );
                peer->state = EPeerState::Connected;
            }

            void setPendingCount( Mengine::SocketPeerId _id, size_t _count )
            {
                Peer * peer = this->findPeer_( _id );
                peer->pending.resize( _count );
            }

            size_t getPendingCount( Mengine::SocketPeerId _id )
            {
                Peer * peer = this->findPeer_( _id );
                size_t count = peer->pending.size();

                return count;
            }

            uint32_t getPendingValue( Mengine::SocketPeerId _id, size_t _index )
            {
                Peer * peer = this->findPeer_( _id );
                const Mengine::Data & payload = peer->pending[_index].payload;
                uint32_t value = 0;
                Mengine::StdString::memcpy( &value, payload.data(), sizeof( value ) );

                return value;
            }

            uint32_t getPendingSequence( Mengine::SocketPeerId _id, size_t _index )
            {
                Peer * peer = this->findPeer_( _id );
                uint32_t sequence = peer->pending[_index].channelSequence;

                return sequence;
            }

            uint32_t getPendingPacketId( Mengine::SocketPeerId _id, size_t _index )
            {
                Peer * peer = this->findPeer_( _id );
                uint32_t sequence = peer->pending[_index].packetSequence;

                return sequence;
            }

            void setBandwidthTokens( Mengine::SocketPeerId _id, double _tokens )
            {
                Peer * peer = this->findPeer_( _id );
                peer->bandwidthTokens = _tokens;
            }

            void acknowledgePackets( uint32_t _ack, uint32_t _ackBits )
            {
                this->acknowledge_( &m_peers[0], _ack, _ackBits, 0 );
            }

            size_t getCommandCount() const
            {
                return m_commandCount;
            }

            size_t getPeerCount() const
            {
                size_t count = m_peers.size();

                return count;
            }

            void processCommands( uint64_t _now = 0 )
            {
                this->processCommands_( _now );
            }

            void setDatagram( TestDatagram * _datagram )
            {
                m_socket = Mengine::DatagramInterfacePtr( _datagram );
            }

            void receivePacket( const Mengine::Data & _packet, uint64_t _now = 0 )
            {
                const Mengine::SocketAddress & address = m_peers[0].address;
                this->processPacket_( address, _packet.data(), _packet.size(), _now );
            }

            void updatePeers( uint64_t _now )
            {
                this->updatePeers_( _now );
            }

            void setPacketSequence( uint32_t _sequence )
            {
                m_peers[0].nextPacketSequence = _sequence;
            }

            void setNextPeerId( Mengine::SocketPeerId _peerId )
            {
                m_nextPeerId.store( _peerId );
            }

            void setReliableSequence( uint8_t _channel, uint32_t _sequence )
            {
                m_peers[0].sendReliableChannelSequence[_channel] = _sequence;
                m_peers[0].receiveChannelSequence[_channel] = _sequence;
            }

            bool processOnce( const Mengine::ThreadIdentityRunnerInterfacePtr & _runner )
            {
                if( this->process_( _runner ) == false )
                {
                    return false;
                }

                return true;
            }
        };
        //////////////////////////////////////////////////////////////////////////
        class TestRunner
            : public Mengine::ThreadIdentityRunnerInterface
        {
        public:
            TestRunner()
            {
            }

            ~TestRunner() override
            {
            }

        public:
            uint32_t incref() override
            {
                uint32_t count = m_references.increfReferenceCount();

                return count;
            }

            void decref() override
            {
                m_references.decrefReferenceCount();
            }

            uint32_t getrefcount() const override
            {
                uint32_t count = m_references.getReferenceCount();

                return count;
            }

            bool run() override
            {
                return true;
            }

            void cancel() override
            {
                //Empty
            }

            void sleep() override
            {
                //Empty
            }

            bool isCancel() const override
            {
                return false;
            }

        private:
            Mengine::ReferenceCounter m_references;
        };
        //////////////////////////////////////////////////////////////////////////
    }
    //////////////////////////////////////////////////////////////////////////
}
//////////////////////////////////////////////////////////////////////////
#define CHECK( Expression ) \
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
        static void deliverPackets( TestDatagram * _datagram, TestSocketService * _receiver, uint64_t _now )
        {
            for( size_t index = 0; index != _datagram->packets.size(); )
            {
                if( _datagram->packets[index].due > _now )
                {
                    ++index;
                    continue;
                }
                Mengine::Data packet = Mengine::StdUtility::move( _datagram->packets[index].bytes );
                _datagram->packets.erase( _datagram->packets.begin() + static_cast<ptrdiff_t>(index) );
                _receiver->receivePacket( packet, _now );
            }
        }
        //////////////////////////////////////////////////////////////////////////
        static bool takeValue( TestSocketService * _receiver, uint32_t _value, Mengine::ESocketDelivery _delivery )
        {
            Mengine::SocketServiceEvent event;
            CHECK( _receiver->pollEvent( &event ) );
            CHECK( event.type == Mengine::ESocketServiceEventType::MessageReceived );
            CHECK( event.delivery == _delivery );
            CHECK( event.payload.size() == sizeof( _value ) );
            uint32_t value = 0;
            Mengine::StdString::memcpy( &value, event.payload.data(), sizeof( value ) );
            CHECK( value == _value );

            return true;
        }
        //////////////////////////////////////////////////////////////////////////
        static bool testDelayedAcknowledgement()
        {
            TestDatagram outbound;
            TestDatagram inbound;
            outbound.delay = 750;
            inbound.delay = 750;
            TestSocketService sender;
            TestSocketService receiver;
            sender.addPeer( 1 );
            receiver.addPeer( 1 );
            sender.setDatagram( &outbound );
            receiver.setDatagram( &inbound );
            uint32_t value = 101;
            CHECK( sender.send( 1, 1, Mengine::ESocketDelivery::ReliableOrdered, &value, sizeof( value ) ) );
            sender.processCommands();
            for( uint64_t now = 0; now <= 4000; now += 10 )
            {
                outbound.now = now;
                inbound.now = now;
                deliverPackets( &outbound, &receiver, now );
                deliverPackets( &inbound, &sender, now );
                sender.updatePeers( now );
                receiver.updatePeers( now );
                CHECK( sender.getPendingCount( 1 ) <= 1 );
                CHECK( outbound.packets.size() < 16 );
                CHECK( inbound.packets.size() < 16 );
            }
            CHECK( sender.getPendingCount( 1 ) == 0 );
            CHECK( takeValue( &receiver, value, Mengine::ESocketDelivery::ReliableOrdered ) );
            Mengine::SocketServiceEvent event;
            CHECK( receiver.pollEvent( &event ) == false );

            return true;
        }
        //////////////////////////////////////////////////////////////////////////
        static bool testOldRetryAcknowledgement()
        {
            TestDatagram outbound;
            TestDatagram inbound;
            TestSocketService sender;
            TestSocketService receiver;
            sender.addPeer( 1 );
            receiver.addPeer( 1 );
            sender.setDatagram( &outbound );
            receiver.setDatagram( &inbound );
            uint32_t value = 111;
            CHECK( sender.send( 1, 1, Mengine::ESocketDelivery::ReliableOrdered, &value, sizeof( value ) ) );
            sender.processCommands();
            deliverPackets( &outbound, &receiver, 0 );
            receiver.updatePeers( 20 );
            inbound.packets.clear();
            for( uint32_t index = 0; index != 80; ++index )
            {
                CHECK( sender.send( 1, 1, Mengine::ESocketDelivery::UnreliableSequenced, &index, sizeof( index ) ) );
            }
            sender.processCommands();
            deliverPackets( &outbound, &receiver, 30 );
            receiver.updatePeers( 40 );
            deliverPackets( &inbound, &sender, 40 );
            CHECK( sender.getPendingCount( 1 ) == 1 );
            uint32_t finalValue = 112;
            CHECK( sender.send( 1, 1, Mengine::ESocketDelivery::UnreliableSequenced, &finalValue, sizeof( finalValue ) ) );
            sender.processCommands();
            deliverPackets( &outbound, &receiver, 50 );
            sender.updatePeers( 100 );
            CHECK( outbound.packets.size() == 1 );
            uint32_t retrySequence;
            Mengine::Helper::readUint32( outbound.packets[0].bytes.data() + 12, &retrySequence );
            CHECK( retrySequence == 1 );
            deliverPackets( &outbound, &receiver, 100 );
            CHECK( inbound.packets.size() == 2 );
            uint32_t exactAck;
            Mengine::Helper::readUint32( inbound.packets[0].bytes.data() + 16, &exactAck );
            CHECK( exactAck == 1 );
            uint32_t currentAck;
            Mengine::Helper::readUint32( inbound.packets[1].bytes.data() + 16, &currentAck );
            CHECK( currentAck == 82 );
            inbound.packets.erase( inbound.packets.begin() );
            deliverPackets( &inbound, &sender, 100 );
            CHECK( sender.getPendingCount( 1 ) == 1 );
            sender.updatePeers( 300 );
            deliverPackets( &outbound, &receiver, 300 );
            deliverPackets( &inbound, &sender, 300 );
            CHECK( sender.getPendingCount( 1 ) == 0 );
            CHECK( takeValue( &receiver, value, Mengine::ESocketDelivery::ReliableOrdered ) );
            for( uint32_t index = 0; index != 80; ++index )
            {
                CHECK( takeValue( &receiver, index, Mengine::ESocketDelivery::UnreliableSequenced ) );
            }
            CHECK( takeValue( &receiver, finalValue, Mengine::ESocketDelivery::UnreliableSequenced ) );
            Mengine::SocketServiceEvent event;
            CHECK( receiver.pollEvent( &event ) == false );

            return true;
        }
        //////////////////////////////////////////////////////////////////////////
        static bool testReorderedOldPackets()
        {
            TestDatagram outbound;
            TestDatagram inbound;
            TestSocketService sender;
            TestSocketService receiver;
            sender.addPeer( 1 );
            receiver.addPeer( 1 );
            sender.setDatagram( &outbound );
            receiver.setDatagram( &inbound );
            uint32_t value = 121;
            CHECK( sender.send( 1, 1, Mengine::ESocketDelivery::ReliableOrdered, &value, sizeof( value ) ) );
            value = 122;
            CHECK( sender.send( 1, 1, Mengine::ESocketDelivery::ReliableOrdered, &value, sizeof( value ) ) );
            sender.processCommands();
            Mengine::Data first = outbound.packets[0].bytes;
            Mengine::Data second = outbound.packets[1].bytes;
            receiver.receivePacket( second );
            CHECK( inbound.packets.empty() );
            outbound.packets.clear();
            for( uint32_t index = 0; index != 80; ++index )
            {
                CHECK( sender.send( 1, 2, Mengine::ESocketDelivery::UnreliableSequenced, &index, sizeof( index ) ) );
            }
            sender.processCommands();
            deliverPackets( &outbound, &receiver, 0 );
            receiver.receivePacket( second );
            CHECK( inbound.packets.empty() );
            receiver.receivePacket( first );
            receiver.receivePacket( second );
            deliverPackets( &inbound, &sender, 0 );
            CHECK( sender.getPendingCount( 1 ) == 0 );
            for( uint32_t index = 0; index != 80; ++index )
            {
                CHECK( takeValue( &receiver, index, Mengine::ESocketDelivery::UnreliableSequenced ) );
            }
            CHECK( takeValue( &receiver, 121, Mengine::ESocketDelivery::ReliableOrdered ) );
            CHECK( takeValue( &receiver, 122, Mengine::ESocketDelivery::ReliableOrdered ) );

            return true;
        }
        //////////////////////////////////////////////////////////////////////////
        static bool testMixedDeliveries()
        {
            TestDatagram firstDatagram;
            TestDatagram secondDatagram;
            TestSocketService first;
            TestSocketService second;
            first.addPeer( 1 );
            second.addPeer( 1 );
            first.setDatagram( &firstDatagram );
            second.setDatagram( &secondDatagram );
            for( uint32_t index = 0; index != 4; ++index )
            {
                Mengine::ESocketDelivery firstDelivery = index % 2 == 0 ? Mengine::ESocketDelivery::ReliableOrdered : Mengine::ESocketDelivery::UnreliableSequenced;
                Mengine::ESocketDelivery secondDelivery = index % 2 == 0 ? Mengine::ESocketDelivery::UnreliableSequenced : Mengine::ESocketDelivery::ReliableOrdered;
                CHECK( first.send( 1, 1, firstDelivery, &index, sizeof( index ) ) );
                CHECK( second.send( 1, 1, secondDelivery, &index, sizeof( index ) ) );
            }
            first.processCommands();
            second.processCommands();
            deliverPackets( &firstDatagram, &second, 0 );
            deliverPackets( &secondDatagram, &first, 0 );
            first.updatePeers( 20 );
            second.updatePeers( 20 );
            deliverPackets( &firstDatagram, &second, 20 );
            deliverPackets( &secondDatagram, &first, 20 );
            CHECK( first.getPendingCount( 1 ) == 0 );
            CHECK( second.getPendingCount( 1 ) == 0 );
            for( uint32_t index = 0; index != 4; ++index )
            {
                Mengine::ESocketDelivery firstDelivery = index % 2 == 0 ? Mengine::ESocketDelivery::UnreliableSequenced : Mengine::ESocketDelivery::ReliableOrdered;
                Mengine::ESocketDelivery secondDelivery = index % 2 == 0 ? Mengine::ESocketDelivery::ReliableOrdered : Mengine::ESocketDelivery::UnreliableSequenced;
                CHECK( takeValue( &first, index, firstDelivery ) );
                CHECK( takeValue( &second, index, secondDelivery ) );
            }

            return true;
        }
        //////////////////////////////////////////////////////////////////////////
        static bool testSequenceWrap()
        {
            TestDatagram outbound;
            TestDatagram inbound;
            TestSocketService sender;
            TestSocketService receiver;
            sender.addPeer( 1 );
            receiver.addPeer( 1 );
            sender.setDatagram( &outbound );
            receiver.setDatagram( &inbound );
            sender.setPacketSequence( UINT32_MAX );
            sender.setReliableSequence( 1, UINT32_MAX );
            receiver.setReliableSequence( 1, UINT32_MAX );
            uint32_t value = 131;
            CHECK( sender.send( 1, 1, Mengine::ESocketDelivery::ReliableOrdered, &value, sizeof( value ) ) );
            sender.processCommands();
            sender.setPacketSequence( UINT32_MAX );
            value = 132;
            CHECK( sender.send( 1, 1, Mengine::ESocketDelivery::ReliableOrdered, &value, sizeof( value ) ) );
            sender.processCommands();
            CHECK( outbound.packets.size() == 2 );
            uint32_t secondSequence;
            Mengine::Helper::readUint32( outbound.packets[1].bytes.data() + 12, &secondSequence );
            CHECK( secondSequence == 1 );
            receiver.receivePacket( outbound.packets[1].bytes );
            receiver.receivePacket( outbound.packets[0].bytes );
            receiver.receivePacket( outbound.packets[1].bytes );
            outbound.packets.clear();
            receiver.updatePeers( 20 );
            deliverPackets( &inbound, &sender, 20 );
            CHECK( sender.getPendingCount( 1 ) == 0 );
            CHECK( takeValue( &receiver, 131, Mengine::ESocketDelivery::ReliableOrdered ) );
            CHECK( takeValue( &receiver, 132, Mengine::ESocketDelivery::ReliableOrdered ) );

            return true;
        }
        //////////////////////////////////////////////////////////////////////////
        static bool testReliableRetention()
        {
            TestSocketService service;
            service.addPeer( 1 );
            for( uint32_t value = 1; value <= 513; ++value )
            {
                CHECK( service.send( 1, 1, Mengine::ESocketDelivery::ReliableOrdered, &value, sizeof( value ) ) );
            }
            service.processCommands();
            CHECK( service.getPendingCount( 1 ) == 512 );
            CHECK( service.getCommandCount() == 1 );
            service.setPendingCount( 1, 0 );
            service.processCommands();
            CHECK( service.getPendingCount( 1 ) == 1 );
            CHECK( service.getPendingValue( 1, 0 ) == 513 );
            CHECK( service.getCommandCount() == 0 );

            return true;
        }
        //////////////////////////////////////////////////////////////////////////
        static bool testChannelOrdering()
        {
            TestSocketService service;
            service.addPeer( 1 );
            service.setPendingCount( 1, 511 );
            Mengine::Data fragmented( 1157, 0 );
            CHECK( service.send( 1, 1, Mengine::ESocketDelivery::ReliableOrdered, fragmented.data(), fragmented.size() ) );
            uint32_t value = 42;
            CHECK( service.send( 1, 1, Mengine::ESocketDelivery::ReliableOrdered, &value, sizeof( value ) ) );
            value = 43;
            CHECK( service.send( 1, 2, Mengine::ESocketDelivery::ReliableOrdered, &value, sizeof( value ) ) );
            service.processCommands();
            CHECK( service.getPendingCount( 1 ) == 512 );
            CHECK( service.getPendingValue( 1, 511 ) == 43 );
            CHECK( service.getCommandCount() == 2 );
            service.setPendingCount( 1, 0 );
            service.processCommands();
            CHECK( service.getPendingCount( 1 ) == 3 );
            CHECK( service.getPendingSequence( 1, 0 ) == 1 );
            CHECK( service.getPendingSequence( 1, 1 ) == 1 );
            CHECK( service.getPendingSequence( 1, 2 ) == 2 );
            CHECK( service.getPendingValue( 1, 2 ) == 42 );
            CHECK( service.getCommandCount() == 0 );

            return true;
        }
        //////////////////////////////////////////////////////////////////////////
        static bool testBroadcastRetry()
        {
            TestSocketService service;
            service.addPeer( 1 );
            service.addPeer( 2 );
            service.setPendingCount( 1, 512 );
            uint32_t value = 51;
            CHECK( service.broadcast( 1, Mengine::ESocketDelivery::ReliableOrdered, &value, sizeof( value ) ) );
            value = 52;
            CHECK( service.send( 2, 1, Mengine::ESocketDelivery::ReliableOrdered, &value, sizeof( value ) ) );
            service.processCommands();
            CHECK( service.getCommandCount() == 1 );
            CHECK( service.getPendingCount( 2 ) == 2 );
            CHECK( service.getPendingValue( 2, 0 ) == 51 );
            CHECK( service.getPendingValue( 2, 1 ) == 52 );
            service.addPeer( 3 );
            service.setPendingCount( 1, 0 );
            value = 53;
            CHECK( service.send( 1, 1, Mengine::ESocketDelivery::ReliableOrdered, &value, sizeof( value ) ) );
            service.processCommands();
            CHECK( service.getCommandCount() == 0 );
            CHECK( service.getPendingCount( 1 ) == 2 );
            CHECK( service.getPendingValue( 1, 0 ) == 51 );
            CHECK( service.getPendingValue( 1, 1 ) == 53 );
            CHECK( service.getPendingCount( 2 ) == 2 );
            CHECK( service.getPendingCount( 3 ) == 0 );

            return true;
        }
        //////////////////////////////////////////////////////////////////////////
        static bool testConnectingPeer()
        {
            TestSocketService service;
            service.addPeer( 1, false );
            uint32_t value = 61;
            CHECK( service.send( 1, 1, Mengine::ESocketDelivery::ReliableOrdered, &value, sizeof( value ) ) );
            service.processCommands();
            CHECK( service.getPendingCount( 1 ) == 0 );
            CHECK( service.getCommandCount() == 1 );
            service.connectPeer( 1 );
            service.processCommands();
            CHECK( service.getPendingCount( 1 ) == 1 );
            CHECK( service.getPendingValue( 1, 0 ) == 61 );
            CHECK( service.getCommandCount() == 0 );

            return true;
        }
        //////////////////////////////////////////////////////////////////////////
        static bool testBoundedQueueAndDisconnect()
        {
            TestSocketService service;
            service.addPeer( 1 );
            service.setPendingCount( 1, 512 );
            uint32_t value = 71;
            for( size_t index = 0; index != 1023; ++index )
            {
                CHECK( service.send( 1, 1, Mengine::ESocketDelivery::ReliableOrdered, &value, sizeof( value ) ) );
            }
            service.processCommands();
            CHECK( service.getCommandCount() == 1023 );
            CHECK( service.send( 1, 1, Mengine::ESocketDelivery::ReliableOrdered, &value, sizeof( value ) ) == false );
            CHECK( service.disconnect( 1, 0 ) );
            CHECK( service.getCommandCount() == 1024 );
            CHECK( service.disconnect( 2, 0 ) == false );
            service.processCommands();
            CHECK( service.getPeerCount() == 0 );
            service.processCommands();
            CHECK( service.getCommandCount() == 0 );

            return true;
        }
        //////////////////////////////////////////////////////////////////////////
        static bool testStopAtCapacity()
        {
            TestSocketService service;
            service.addPeer( 1 );
            service.setPendingCount( 1, 512 );
            uint32_t value = 81;
            for( size_t index = 0; index != 1023; ++index )
            {
                CHECK( service.send( 1, 1, Mengine::ESocketDelivery::ReliableOrdered, &value, sizeof( value ) ) );
            }
            CHECK( service.disconnect( 2, 0 ) );
            CHECK( service.getCommandCount() == 1024 );
            service.stop();
            TestRunner testRunner;
            Mengine::ThreadIdentityRunnerInterfacePtr runner( &testRunner );
            CHECK( service.processOnce( runner ) == false );
            CHECK( service.send( 1, 1, Mengine::ESocketDelivery::ReliableOrdered, &value, sizeof( value ) ) == false );

            return true;
        }
        //////////////////////////////////////////////////////////////////////////
        static bool testMaximumUnreliableMessage()
        {
            TestDatagram outbound;
            TestSocketService sender;
            TestSocketService receiver;
            sender.addPeer( 1, true, false );
            receiver.addPeer( 1, true, false );
            sender.setDatagram( &outbound );
            size_t maximumMessageSize = 65536;
            Mengine::Data payload( maximumMessageSize, 17 );
            const void * bytes = payload.data();
            CHECK( sender.send( 1, 2, Mengine::ESocketDelivery::UnreliableSequenced, bytes, maximumMessageSize ) );
            sender.processCommands();
            CHECK( outbound.packets.size() > 1 );
            deliverPackets( &outbound, &receiver, 0 );
            Mengine::SocketServiceEvent event;
            CHECK( receiver.pollEvent( &event ) );
            CHECK( event.delivery == Mengine::ESocketDelivery::UnreliableSequenced );
            CHECK( event.payload == payload );
            CHECK( sender.send( 1, 2, Mengine::ESocketDelivery::UnreliableSequenced, bytes, maximumMessageSize ) );
            sender.processCommands();
            CHECK( outbound.packets.empty() );
            CHECK( sender.send( 1, 2, Mengine::ESocketDelivery::UnreliableSequenced, bytes, maximumMessageSize ) );
            sender.processCommands( 2000 );
            CHECK( outbound.packets.empty() );
            outbound.now = 2200;
            CHECK( sender.send( 1, 2, Mengine::ESocketDelivery::UnreliableSequenced, bytes, maximumMessageSize ) );
            sender.processCommands( 2200 );
            CHECK( outbound.packets.size() > 1 );
            deliverPackets( &outbound, &receiver, 2200 );
            CHECK( receiver.pollEvent( &event ) );
            CHECK( event.delivery == Mengine::ESocketDelivery::UnreliableSequenced );
            CHECK( event.payload == payload );
            CHECK( receiver.pollEvent( &event ) == false );
            CHECK( sender.getPendingCount( 1 ) == 0 );

            return true;
        }
        //////////////////////////////////////////////////////////////////////////
        static bool testReliableBandwidthPriority()
        {
            TestDatagram outbound;
            TestDatagram inbound;
            TestSocketService sender;
            TestSocketService receiver;
            sender.addPeer( 1, true, false );
            receiver.addPeer( 1, true, false );
            sender.setDatagram( &outbound );
            receiver.setDatagram( &inbound );
            sender.setBandwidthTokens( 1, 0.0 );
            Mengine::Data reliable( 1156, 19 );
            const void * reliableBytes = reliable.data();
            size_t reliableSize = reliable.size();
            CHECK( sender.send( 1, 1, Mengine::ESocketDelivery::ReliableOrdered, reliableBytes, reliableSize ) );
            sender.processCommands();
            CHECK( sender.getPendingCount( 1 ) == 1 );
            CHECK( sender.getPendingPacketId( 1, 0 ) == 0 );
            Mengine::Data unreliable( 276, 23 );
            const void * unreliableBytes = unreliable.data();
            size_t unreliableSize = unreliable.size();
            bool unreliableWhileAwaitingAck = false;
            uint32_t droppedPacketId = 0;
            bool reliableRetried = false;
            for( uint64_t now = 10; now <= 10000; now += 10 )
            {
                outbound.now = now;
                inbound.now = now;
                CHECK( sender.send( 1, 2, Mengine::ESocketDelivery::UnreliableSequenced, unreliableBytes, unreliableSize ) );
                sender.processCommands( now );
                if( sender.getPendingCount( 1 ) != 0 )
                {
                    if( sender.getPendingPacketId( 1, 0 ) != 0 )
                    {
                        for( const TestDatagramPacketDesc & packet : outbound.packets )
                        {
                            if( packet.bytes[43] == static_cast<uint8_t>(Mengine::ESocketDelivery::UnreliableSequenced) )
                            {
                                unreliableWhileAwaitingAck = true;
                            }
                        }
                    }
                }
                for( size_t index = 0; index != outbound.packets.size(); ++index )
                {
                    const TestDatagramPacketDesc & packet = outbound.packets[index];
                    if( packet.bytes.size() != reliableSize + 44 )
                    {
                        continue;
                    }
                    uint32_t packetId = 0;
                    Mengine::Helper::readUint32( packet.bytes.data() + 12, &packetId );
                    if( droppedPacketId == 0 )
                    {
                        droppedPacketId = packetId;
                        outbound.packets.erase( outbound.packets.begin() + static_cast<ptrdiff_t>(index) );
                        break;
                    }
                    CHECK( packetId == droppedPacketId );
                    reliableRetried = true;
                }
                deliverPackets( &outbound, &receiver, now );
                deliverPackets( &inbound, &sender, now );
                sender.updatePeers( now );
                receiver.updatePeers( now );
            }
            CHECK( sender.getPeerCount() == 1 );
            CHECK( receiver.getPeerCount() == 1 );
            CHECK( sender.getPendingCount( 1 ) == 0 );
            CHECK( unreliableWhileAwaitingAck );
            CHECK( droppedPacketId != 0 );
            CHECK( reliableRetried );
            size_t reliableCount = 0;
            size_t unreliableCount = 0;
            Mengine::SocketServiceEvent event;
            while( receiver.pollEvent( &event ) == true )
            {
                if( event.delivery == Mengine::ESocketDelivery::ReliableOrdered )
                {
                    CHECK( event.payload == reliable );
                    ++reliableCount;
                    continue;
                }
                CHECK( event.payload == unreliable );
                ++unreliableCount;
            }
            CHECK( reliableCount == 1 );
            CHECK( unreliableCount != 0 );

            return true;
        }
        //////////////////////////////////////////////////////////////////////////
        static bool testReliableCreditOrder( bool _retransmit )
        {
            TestDatagram outbound;
            TestDatagram inbound;
            TestSocketService sender;
            TestSocketService receiver;
            sender.addPeer( 1, true, false );
            receiver.addPeer( 1, true, false );
            sender.setDatagram( &outbound );
            receiver.setDatagram( &inbound );
            sender.setBandwidthTokens( 1, _retransmit == true ? 1200.0 : 0.0 );
            Mengine::Data original( 1156, 19 );
            const void * originalBytes = original.data();
            size_t originalSize = original.size();
            CHECK( sender.send( 1, 1, Mengine::ESocketDelivery::ReliableOrdered, originalBytes, originalSize ) );
            sender.processCommands();
            uint32_t originalPacketId = sender.getPendingPacketId( 1, 0 );
            CHECK( (originalPacketId != 0) == _retransmit );
            outbound.packets.clear();
            Mengine::Data following( 276, 23 );
            const void * followingBytes = following.data();
            size_t followingSize = following.size();
            size_t originalCount = 0;
            size_t followingCount = 0;
            uint64_t originalDeliveredMs = 0;
            for( uint64_t now = 10; now <= 2000; now += 10 )
            {
                outbound.now = now;
                inbound.now = now;
                if( now <= 1000 )
                {
                    CHECK( sender.send( 1, 2, Mengine::ESocketDelivery::ReliableOrdered, followingBytes, followingSize ) );
                }
                sender.processCommands( now );
                for( const TestDatagramPacketDesc & packet : outbound.packets )
                {
                    if( packet.bytes.size() != originalSize + 44 )
                    {
                        continue;
                    }
                    uint32_t packetId;
                    Mengine::Helper::readUint32( packet.bytes.data() + 12, &packetId );
                    if( originalPacketId == 0 )
                    {
                        originalPacketId = packetId;
                    }
                    CHECK( packetId == originalPacketId );
                }
                deliverPackets( &outbound, &receiver, now );
                deliverPackets( &inbound, &sender, now );
                sender.updatePeers( now );
                receiver.updatePeers( now );
                Mengine::SocketServiceEvent event;
                while( receiver.pollEvent( &event ) == true )
                {
                    CHECK( event.type == Mengine::ESocketServiceEventType::MessageReceived );
                    CHECK( event.delivery == Mengine::ESocketDelivery::ReliableOrdered );
                    if( event.channel == 1 )
                    {
                        CHECK( event.payload == original );
                        ++originalCount;
                        originalDeliveredMs = now;
                        continue;
                    }
                    CHECK( event.channel == 2 && event.payload == following );
                    ++followingCount;
                }
                if( now == 1000 )
                {
                    CHECK( originalCount == 1 );
                    CHECK( originalDeliveredMs <= 200 );
                }
            }
            CHECK( originalCount == 1 && followingCount == 100 );
            CHECK( originalPacketId != 0 );
            CHECK( sender.getPendingCount( 1 ) == 0 );
            CHECK( sender.getCommandCount() == 0 );

            return true;
        }
        //////////////////////////////////////////////////////////////////////////
        static bool testReliableFragmentCreditOrder()
        {
            TestDatagram outbound;
            TestSocketService sender;
            TestSocketService receiver;
            sender.addPeer( 1, true, false );
            receiver.addPeer( 1, true, false );
            sender.setDatagram( &outbound );
            sender.setBandwidthTokens( 1, 88.0 );
            Mengine::Data payload( 1200, 31 );
            const void * bytes = payload.data();
            size_t size = payload.size();
            CHECK( sender.send( 1, 1, Mengine::ESocketDelivery::ReliableOrdered, bytes, size ) );
            sender.processCommands();
            CHECK( sender.getPendingCount( 1 ) == 2 );
            CHECK( outbound.packets.empty() );
            sender.updatePeers( 1 );
            CHECK( outbound.packets.empty() );
            sender.updatePeers( 40 );
            CHECK( outbound.packets.size() == 2 );
            for( size_t index = 0; index != 2; ++index )
            {
                uint16_t fragmentIndex;
                Mengine::Helper::readUint16( outbound.packets[index].bytes.data() + 38, &fragmentIndex );
                CHECK( fragmentIndex == index );
            }
            deliverPackets( &outbound, &receiver, 40 );
            Mengine::SocketServiceEvent event;
            CHECK( receiver.pollEvent( &event ) );
            CHECK( event.payload == payload );
            CHECK( receiver.pollEvent( &event ) == false );

            return true;
        }
        //////////////////////////////////////////////////////////////////////////
        static bool testPeerIdExhaustion()
        {
            TestSocketService service;
            service.setNextPeerId( UINT32_MAX - 1 );
            Mengine::SocketAddress address;
            CHECK( service.connect( address, nullptr ) == false );
            Mengine::SocketPeerId peerId = Mengine::InvalidSocketPeerId;
            CHECK( service.connect( address, &peerId ) );
            CHECK( peerId == UINT32_MAX - 1 );
            CHECK( service.connect( address, &peerId ) );
            CHECK( peerId == UINT32_MAX );
            for( size_t attempt = 0; attempt != 4; ++attempt )
            {
                peerId = 73;
                CHECK( service.connect( address, &peerId ) == false );
                CHECK( peerId == 73 );
            }
            CHECK( service.getCommandCount() == 2 );

            return true;
        }
        //////////////////////////////////////////////////////////////////////////
        static bool testUnsentAcknowledgement()
        {
            TestDatagram outbound;
            TestSocketService sender;
            sender.addPeer( 1 );
            sender.setDatagram( &outbound );
            sender.setBandwidthTokens( 1, 0.0 );
            uint32_t value = 151;
            CHECK( sender.send( 1, 1, Mengine::ESocketDelivery::ReliableOrdered, &value, sizeof( value ) ) );
            sender.processCommands();
            CHECK( outbound.packets.empty() );
            CHECK( sender.getPendingCount( 1 ) == 1 );
            CHECK( sender.getPendingPacketId( 1, 0 ) == 0 );
            sender.acknowledgePackets( 1, 1 );
            CHECK( sender.getPendingCount( 1 ) == 1 );
            sender.acknowledgePackets( 0, UINT32_MAX );
            CHECK( sender.getPendingCount( 1 ) == 1 );
            sender.updatePeers( 100 );
            uint32_t packetId = sender.getPendingPacketId( 1, 0 );
            CHECK( packetId != 0 );
            sender.acknowledgePackets( packetId, 0 );
            CHECK( sender.getPendingCount( 1 ) == 0 );

            return true;
        }
        //////////////////////////////////////////////////////////////////////////
        static bool testUnreliableAdmission()
        {
            TestSocketService service;
            service.addPeer( 1 );
            uint32_t value = 91;
            CHECK( service.send( 1, 1, Mengine::ESocketDelivery::UnreliableSequenced, &value, sizeof( value ) ) );
            service.processCommands();
            CHECK( service.getPendingCount( 1 ) == 0 );
            CHECK( service.getCommandCount() == 0 );

            return true;
        }
        //////////////////////////////////////////////////////////////////////////
    }
    //////////////////////////////////////////////////////////////////////////
}
//////////////////////////////////////////////////////////////////////////
int main()
{
    if( Arena3D::Detail::testDelayedAcknowledgement() == false )
    {
        return EXIT_FAILURE;
    }
    if( Arena3D::Detail::testOldRetryAcknowledgement() == false )
    {
        return EXIT_FAILURE;
    }
    if( Arena3D::Detail::testReorderedOldPackets() == false )
    {
        return EXIT_FAILURE;
    }
    if( Arena3D::Detail::testMixedDeliveries() == false )
    {
        return EXIT_FAILURE;
    }
    if( Arena3D::Detail::testSequenceWrap() == false )
    {
        return EXIT_FAILURE;
    }
    if( Arena3D::Detail::testReliableRetention() == false )
    {
        return EXIT_FAILURE;
    }
    if( Arena3D::Detail::testChannelOrdering() == false )
    {
        return EXIT_FAILURE;
    }
    if( Arena3D::Detail::testBroadcastRetry() == false )
    {
        return EXIT_FAILURE;
    }
    if( Arena3D::Detail::testConnectingPeer() == false )
    {
        return EXIT_FAILURE;
    }
    if( Arena3D::Detail::testBoundedQueueAndDisconnect() == false )
    {
        return EXIT_FAILURE;
    }
    if( Arena3D::Detail::testStopAtCapacity() == false )
    {
        return EXIT_FAILURE;
    }
    if( Arena3D::Detail::testUnreliableAdmission() == false )
    {
        return EXIT_FAILURE;
    }
    if( Arena3D::Detail::testUnsentAcknowledgement() == false )
    {
        return EXIT_FAILURE;
    }
    if( Arena3D::Detail::testMaximumUnreliableMessage() == false )
    {
        return EXIT_FAILURE;
    }
    if( Arena3D::Detail::testReliableBandwidthPriority() == false )
    {
        return EXIT_FAILURE;
    }
    if( Arena3D::Detail::testReliableCreditOrder( false ) == false )
    {
        return EXIT_FAILURE;
    }
    if( Arena3D::Detail::testReliableCreditOrder( true ) == false )
    {
        return EXIT_FAILURE;
    }
    if( Arena3D::Detail::testReliableFragmentCreditOrder() == false )
    {
        return EXIT_FAILURE;
    }
    if( Arena3D::Detail::testPeerIdExhaustion() == false )
    {
        return EXIT_FAILURE;
    }
    if( Arena3D::runSocketReceiveTests() == false )
    {
        return EXIT_FAILURE;
    }
    Mengine::StdIO::printf( "SocketService reliable queue, ordering, broadcasts and lifecycle passed\n" );

    return EXIT_SUCCESS;
}
