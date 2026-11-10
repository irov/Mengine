#include "Arena3DSession.h"

#include "Config/StdIO.h"
//////////////////////////////////////////////////////////////////////////
namespace Arena3D
{
    //////////////////////////////////////////////////////////////////////////
    namespace Detail
    {
        //////////////////////////////////////////////////////////////////////////
        static bool advanceSession( LocalSession * _session, uint32_t _count )
        {
            for( uint32_t index = 0; index != _count; ++index )
            {
                if( _session->tick() == false )
                {
                    return false;
                }
            }

            return true;
        }
        //////////////////////////////////////////////////////////////////////////
        static bool submitCommand( LocalSession * _session, CommandType _type )
        {
            uint64_t sourceTime = _session->server().state().tick * TickMilliseconds;
            CommandEnvelope command = _session->makeCommand( _type, sourceTime );
            _session->submit( command );
            if( _session->tick() == false )
            {
                return false;
            }

            return true;
        }
        //////////////////////////////////////////////////////////////////////////
        static bool verifyRoundtrip( const Mengine::String & _path, const ReplayRecord & _record )
        {
            if( writeReplay( _path, _record ) == false )
            {
                return false;
            }
            ReplayRecord decoded;
            if( readReplay( _path, &decoded ) == false )
            {
                return false;
            }
            Simulation simulation;
            if( runReplay( decoded, &simulation ) == false )
            {
                return false;
            }

            return true;
        }
        //////////////////////////////////////////////////////////////////////////
        static ReplayRecord sessionReplay( const LocalSession & _session )
        {
            const Simulation & simulation = _session.server();
            const VectorCommandEnvelope & commands = _session.journal();
            const VectorServerEvent & events = _session.eventJournal();
            const VectorReplayBatch & batches = _session.batchJournal();
            ReplayRecord record = makeReplayRecord( simulation, 123, commands, events, batches );

            return record;
        }
        //////////////////////////////////////////////////////////////////////////
        static bool testEmptyRecording( const Mengine::String & _path )
        {
            LocalSession session;
            session.reset( 1, 123 );
            ReplayRecord record = sessionReplay( session );
            if( verifyRoundtrip( _path, record ) == false )
            {
                return false;
            }

            return true;
        }
        //////////////////////////////////////////////////////////////////////////
        static bool testTerminalPause( const Mengine::String & _path, uint32_t _precedingTicks )
        {
            LocalSession session;
            session.reset( 1, 123 );
            if( advanceSession( &session, _precedingTicks ) == false )
            {
                return false;
            }
            if( submitCommand( &session, CommandType::Pause ) == false )
            {
                return false;
            }
            if( advanceSession( &session, 2 ) == false )
            {
                return false;
            }
            ReplayRecord record = sessionReplay( session );
            if( verifyRoundtrip( _path, record ) == false )
            {
                return false;
            }

            return true;
        }
        //////////////////////////////////////////////////////////////////////////
        static bool testPausedBatches( const Mengine::String & _path )
        {
            LocalSession session;
            session.reset( 1, 123 );
            if( advanceSession( &session, 5 ) == false )
            {
                return false;
            }
            if( submitCommand( &session, CommandType::Pause ) == false )
            {
                return false;
            }
            ReplayCheckpoint paused;
            paused.batchIndex = session.batchJournal().size();
            paused.tick = session.server().state().tick;
            paused.checksum = session.server().checksum();
            paused.snapshot = session.server().snapshot();
            if( advanceSession( &session, 1 ) == false )
            {
                return false;
            }
            if( submitCommand( &session, CommandType::FireUp ) == false )
            {
                return false;
            }
            if( submitCommand( &session, CommandType::FireUp ) == false )
            {
                return false;
            }
            if( submitCommand( &session, CommandType::Resume ) == false )
            {
                return false;
            }
            if( advanceSession( &session, 5 ) == false )
            {
                return false;
            }
            ReplayRecord record = sessionReplay( session );
            record.checkpoints.insert( record.checkpoints.begin(), paused );
            if( verifyRoundtrip( _path, record ) == false )
            {
                return false;
            }

            return true;
        }
        //////////////////////////////////////////////////////////////////////////
        static bool testNormalRecording( const Mengine::String & _path )
        {
            LocalSession session;
            session.reset( 1, 123 );
            for( uint32_t index = 0; index != 20; ++index )
            {
                if( submitCommand( &session, CommandType::LookDelta ) == false )
                {
                    return false;
                }
            }
            ReplayRecord record = sessionReplay( session );
            if( verifyRoundtrip( _path, record ) == false )
            {
                return false;
            }

            return true;
        }
        //////////////////////////////////////////////////////////////////////////
        static bool testRestartRecording( const Mengine::String & _path )
        {
            LocalSession session;
            session.reset( 1, 123 );
            if( advanceSession( &session, 5 ) == false )
            {
                return false;
            }
            if( submitCommand( &session, CommandType::Restart ) == false )
            {
                return false;
            }
            if( submitCommand( &session, CommandType::LookDelta ) == false )
            {
                return false;
            }
            if( advanceSession( &session, 5 ) == false )
            {
                return false;
            }
            ReplayRecord record = sessionReplay( session );
            if( verifyRoundtrip( _path, record ) == false )
            {
                return false;
            }
            ++record.batches.front().commandCount;
            Simulation invalid;
            if( runReplay( record, &invalid ) == true )
            {
                return false;
            }
            if( writeReplay( _path, record ) == true )
            {
                return false;
            }

            return true;
        }
        //////////////////////////////////////////////////////////////////////////
    }
    //////////////////////////////////////////////////////////////////////////
}
//////////////////////////////////////////////////////////////////////////
int main( int _argc, char ** _argv )
{
    if( _argc != 2 )
    {
        return 1;
    }
    Mengine::String path = _argv[1];
    if( Arena3D::Detail::testEmptyRecording( path ) == false )
    {
        Mengine::StdIO::fprintf( stderr, "Empty replay failed\n" );
        return 1;
    }
    if( Arena3D::Detail::testTerminalPause( path, 0 ) == false )
    {
        Mengine::StdIO::fprintf( stderr, "Initial paused replay failed\n" );
        return 1;
    }
    if( Arena3D::Detail::testTerminalPause( path, 5 ) == false )
    {
        Mengine::StdIO::fprintf( stderr, "Terminal paused replay failed\n" );
        return 1;
    }
    if( Arena3D::Detail::testPausedBatches( path ) == false )
    {
        Mengine::StdIO::fprintf( stderr, "Paused batch replay failed\n" );
        return 1;
    }
    if( Arena3D::Detail::testNormalRecording( path ) == false )
    {
        Mengine::StdIO::fprintf( stderr, "Normal replay failed\n" );
        return 1;
    }
    if( Arena3D::Detail::testRestartRecording( path ) == false )
    {
        Mengine::StdIO::fprintf( stderr, "Restart replay failed\n" );
        return 1;
    }

    return 0;
}
