#include "Arena3DReplay.h"
#include "Arena3DSession.h"

#include "Config/StdIO.h"

//////////////////////////////////////////////////////////////////////////
int main( int argc, char ** argv )
{
    if( argc != 2 )
    {
        Mengine::StdIO::fprintf( stderr, "Usage: Arena3DReplayTool <match.a3r>\n" );
        return 2;
    }
    Arena3D::ReplayRecord replay;
    if( Arena3D::readReplay( Mengine::String( argv[1] ), &replay ) == false )
    {
        Mengine::StdIO::fprintf( stderr, "Unable to read compatible Arena3D replay\n" );
        return 3;
    }
    Arena3D::Simulation simulation;
    Arena3D::StateChecksum checksum;
    if( Arena3D::runReplay( replay, &simulation, &checksum ) == false )
    {
        Mengine::StdIO::fprintf( stderr, "Replay desync at or before tick %llu", static_cast<unsigned long long>(simulation.state().tick) );
        if( replay.checkpoints.empty() == false && replay.checkpoints.back().snapshot.empty() == false )
        {
            Arena3D::Simulation expected;
            if( expected.restoreSnapshot( replay.checkpoints.back().snapshot ) == true )
            {
                const Mengine::String field = Arena3D::describeFirstDifference( expected.state(), simulation.state() );
                if( field.empty() == false ) Mengine::StdIO::fprintf( stderr, ", first differing field: %s", field.c_str() );
            }
        }
        Mengine::StdIO::fprintf( stderr, "\n" );
        return 4;
    }
    Mengine::StdIO::printf( "Replay OK: tick=%llu crc=0x%08x\n", static_cast<unsigned long long>(simulation.state().tick), checksum.full );
    return 0;
}
