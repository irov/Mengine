#include "Arena3DHeadless.h"
#include "Arena3DNetworkServer.h"

#include "Interface/SocketServiceInterface.h"

#include "Config/Atomic.h"
#include "Config/StdIO.h"
#include "Config/StdLib.h"
#include "Config/StdSignal.h"
#include "Config/StdString.h"

//////////////////////////////////////////////////////////////////////////
int main( int argc, char ** argv )
{
    static Mengine::AtomicBool running{true};

    const auto stopServer = []( int )
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
        Mengine::StdIO::fprintf( stderr, "Arena3DServer: Mengine headless bootstrap failed\n" );
        return EXIT_FAILURE;
    }

    const char * host = argument( "--host", "0.0.0.0" );
    const uint16_t port = static_cast<uint16_t>(Mengine::StdLib::strtoul( argument( "--port", "27960" ), nullptr, 10 ));
    const uint32_t maximumPlayers = static_cast<uint32_t>(Mengine::StdLib::strtoul( argument( "--max-players", "16" ), nullptr, 10 ));
    const uint64_t maximumTicks = Mengine::StdLib::strtoull( argument( "--ticks", "0" ), nullptr, 10 );
    Mengine::SocketAddress address;
    if( engine.resolve( host, port, &address ) == false )
    {
        Mengine::StdIO::fprintf( stderr, "Arena3DServer: cannot resolve %s:%u\n", host, port );
        engine.finalize();
        return EXIT_FAILURE;
    }

    Arena3D::NetworkServer server( SOCKET_SERVICE() );
    if( server.listen( address, maximumPlayers ) == false )
    {
        Mengine::StdIO::fprintf( stderr, "Arena3DServer: listen failed\n" );
        engine.finalize();
        return EXIT_FAILURE;
    }

    Mengine::StdSignal::signal( SIGINT, stopServer );
    Mengine::StdSignal::signal( SIGTERM, stopServer );
    Mengine::StdIO::printf( "Arena3DServer listening on %s:%u, max players %u\n", host, port, maximumPlayers );
    uint64_t nextTick = engine.getTimeMilliseconds();
    uint64_t shots = 0;
    uint64_t damageEvents = 0;
    uint64_t deaths = 0;
    uint64_t respawns = 0;
    while( running == true && (maximumTicks == 0 || server.simulation().state().tick < maximumTicks) )
    {
        nextTick += Arena3D::TickMilliseconds;
        server.tick();
        for( const Arena3D::ServerEvent & event : server.simulation().events() )
        {
            shots += event.type == Arena3D::EventType::Shot ? 1u : 0u;
            damageEvents += event.type == Arena3D::EventType::Damage ? 1u : 0u;
            deaths += event.type == Arena3D::EventType::PlayerDeath ? 1u : 0u;
            respawns += event.type == Arena3D::EventType::PlayerRespawned ? 1u : 0u;
        }
        if( server.simulation().state().tick % 500u == 0u )
        {
            Mengine::StdIO::printf( "tick=%llu players=%zu crc=%08x\n", static_cast<unsigned long long>(server.simulation().state().tick), server.roster().size(), server.simulation().checksum().full );
        }
        const uint64_t now = engine.getTimeMilliseconds();
        if( nextTick > now )
        {
            engine.sleep( nextTick - now );
        }
    }

    Mengine::StdIO::printf( "Arena3DServer summary shots=%llu damage=%llu deaths=%llu respawns=%llu final_crc=%08x\n",
        static_cast<unsigned long long>(shots), static_cast<unsigned long long>(damageEvents),
        static_cast<unsigned long long>(deaths), static_cast<unsigned long long>(respawns), server.simulation().checksum().full );

    engine.finalize();
    return EXIT_SUCCESS;
}
