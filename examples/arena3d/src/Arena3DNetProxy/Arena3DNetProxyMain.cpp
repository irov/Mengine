#include "Arena3DHeadless.h"

#include "Interface/SocketSystemInterface.h"

#include "Kernel/DocumentHelper.h"
#include "Kernel/Data.h"
#include "Kernel/Deque.h"

#include "Config/Atomic.h"
#include "Config/StdAlgorithm.h"
#include "Config/StdIO.h"
#include "Config/StdLib.h"
#include "Config/StdSignal.h"
#include "Config/StdString.h"
#include "Config/StdUtility.h"

//////////////////////////////////////////////////////////////////////////
int main( int argc, char ** argv )
{
    struct DelayedDatagram
    {
        uint64_t due = 0;
        bool toServer = false;
        Mengine::SocketAddress destination;
        Mengine::Data data;
    };
    typedef Mengine::Deque<DelayedDatagram> DequeDelayedDatagram;

    static Mengine::AtomicBool running{true};

    const auto stopProxy = []( int )
    {
        running = false;
    };
    const auto argument = [argc, argv]( const char * name, const char * fallback ) -> const char *
    {
        for( int index = 1; index + 1 < argc; ++index )
        {
            if( Mengine::StdString::strcmp( argv[index], name ) == 0 )
            {
                return argv[index + 1];
            }
        }
        return fallback;
    };

    Arena3D::HeadlessEngine engine;
    if( engine.initialize() == false )
    {
        Mengine::StdIO::fprintf( stderr, "Arena3DNetProxy: Mengine headless bootstrap failed\n" );
        return EXIT_FAILURE;
    }

    const char * listenHost = argument( "--listen-host", "0.0.0.0" );
    const uint16_t listenPort = static_cast<uint16_t>(Mengine::StdLib::strtoul( argument( "--listen-port", "27961" ), nullptr, 10 ));
    const char * serverHost = argument( "--server-host", "127.0.0.1" );
    const uint16_t serverPort = static_cast<uint16_t>(Mengine::StdLib::strtoul( argument( "--server-port", "27960" ), nullptr, 10 ));
    const uint32_t latencyMs = static_cast<uint32_t>(Mengine::StdLib::strtoul( argument( "--latency-ms", "40" ), nullptr, 10 ));
    const uint32_t lossPermille = Mengine::StdAlgorithm::min<uint32_t>( 1000, static_cast<uint32_t>(Mengine::StdLib::strtoul( argument( "--loss-permille", "20" ), nullptr, 10 )) );

    Mengine::SocketAddress listenAddress, serverAddress, ephemeralAddress;
    if( engine.resolve( listenHost, listenPort, &listenAddress ) == false || engine.resolve( serverHost, serverPort, &serverAddress ) == false || engine.resolve( "0.0.0.0", 0, &ephemeralAddress ) == false )
    {
        Mengine::StdIO::fprintf( stderr, "Arena3DNetProxy: address resolution failed\n" );
        engine.finalize();
        return EXIT_FAILURE;
    }

    Mengine::DatagramInterfacePtr front = SOCKET_SYSTEM()->createDatagram( MENGINE_DOCUMENT_FUNCTION );
    Mengine::DatagramInterfacePtr back = SOCKET_SYSTEM()->createDatagram( MENGINE_DOCUMENT_FUNCTION );
    if( front == nullptr || back == nullptr || front->open( Mengine::ESocketAddressFamily::IPv4 ) != Mengine::ESocketResult::Successful ||
        back->open( Mengine::ESocketAddressFamily::IPv4 ) != Mengine::ESocketResult::Successful || front->setReuseAddress( true ) != Mengine::ESocketResult::Successful ||
        front->bind( listenAddress ) != Mengine::ESocketResult::Successful || back->bind( ephemeralAddress ) != Mengine::ESocketResult::Successful ||
        front->setBlocking( false ) != Mengine::ESocketResult::Successful || back->setBlocking( false ) != Mengine::ESocketResult::Successful )
    {
        Mengine::StdIO::fprintf( stderr, "Arena3DNetProxy: socket setup failed\n" );
        engine.finalize();
        return EXIT_FAILURE;
    }

    Mengine::StdSignal::signal( SIGINT, stopProxy );
    Mengine::StdSignal::signal( SIGTERM, stopProxy );
    Mengine::StdIO::printf( "Arena3DNetProxy %s:%u -> %s:%u latency=%ums loss=%u/1000\n", listenHost, listenPort, serverHost, serverPort, latencyMs, lossPermille );
    DequeDelayedDatagram queue;
    Mengine::SocketAddress clientAddress;
    bool hasClient = false;
    uint32_t randomState = UINT32_C(0xA3D01234);
    uint8_t buffer[1200];

    auto enqueue = [&]( bool toServer, const Mengine::SocketAddress & destination, const uint8_t * data, size_t size )
    {
        randomState = randomState * UINT32_C(1664525) + UINT32_C(1013904223);
        if( randomState % 1000u < lossPermille )
        {
            return;
        }
        DelayedDatagram packet;
        packet.due = engine.getTimeMilliseconds() + latencyMs;
        packet.toServer = toServer;
        packet.destination = destination;
        packet.data.assign( data, data + size );
        queue.emplace_back( Mengine::StdUtility::move( packet ) );
    };

    while( running == true )
    {
        for( ;; )
        {
            size_t received = 0;
            Mengine::SocketAddress source;
            const Mengine::ESocketResult result = front->receiveFrom( buffer, sizeof( buffer ), &received, &source );
            if( result == Mengine::ESocketResult::WouldBlock )
            {
                break;
            }
            if( result == Mengine::ESocketResult::Successful || result == Mengine::ESocketResult::Truncated )
            {
                clientAddress = source;
                hasClient = true;
                enqueue( true, serverAddress, buffer, received );
            }
            else
            {
                break;
            }
        }
        for( ;; )
        {
            size_t received = 0;
            Mengine::SocketAddress source;
            const Mengine::ESocketResult result = back->receiveFrom( buffer, sizeof( buffer ), &received, &source );
            if( result == Mengine::ESocketResult::WouldBlock )
            {
                break;
            }
            if( hasClient == true && ( result == Mengine::ESocketResult::Successful || result == Mengine::ESocketResult::Truncated ) )
            {
                enqueue( false, clientAddress, buffer, received );
            }
            else if( result != Mengine::ESocketResult::Successful && result != Mengine::ESocketResult::Truncated )
            {
                break;
            }
        }

        const uint64_t now = engine.getTimeMilliseconds();
        while( queue.empty() == false && queue.front().due <= now )
        {
            DelayedDatagram packet = Mengine::StdUtility::move( queue.front() );
            queue.pop_front();
            size_t sent = 0;
            (packet.toServer ? back : front)->sendTo( packet.destination, packet.data.data(), packet.data.size(), &sent );
        }
        engine.sleep( 1 );
    }

    back->close();
    front->close();
    back = nullptr;
    front = nullptr;
    engine.finalize();
    return EXIT_SUCCESS;
}
