#include "Arena3DSession.h"
#include "Arena3DReplay.h"

#include "Config/StdIO.h"

#define COMMAND_CHECK( Expression ) \
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
        static bool testClientValidation()
        {
            CommandClient client;
            COMMAND_CHECK( client.reset( 1, 1 ) );
            CommandEnvelope command;
            CommandPayload invalid;
            invalid.weapon = WeaponType::Count;
            COMMAND_CHECK( client.makeCommand( CommandType::SelectWeapon, 0, 0, invalid, &command ) == false );
            invalid = {};
            invalid.button = static_cast<InputButton>(255);
            COMMAND_CHECK( client.makeCommand( CommandType::ButtonDown, 0, 0, invalid, &command ) == false );
            COMMAND_CHECK( client.makeCommand( static_cast<CommandType>(255), 0, 0, {}, &command ) == false );
            COMMAND_CHECK( client.makeCommand( CommandType::LookDelta, 0, 0, {}, nullptr ) == false );
            COMMAND_CHECK( client.getPendingCount() == 0 );
            COMMAND_CHECK( client.makeCommand( CommandType::LookDelta, 0, 0, {}, &command ) );
            COMMAND_CHECK( command.sequence == 1 );
            VectorCommandEnvelope pending = client.getPendingCommands();
            COMMAND_CHECK( pending.size() == 1 );
            COMMAND_CHECK( pending[0].type == CommandType::LookDelta );

            return true;
        }
        //////////////////////////////////////////////////////////////////////////
        static bool testLocalBackpressure()
        {
            size_t counts[] = {65, MAXIMUM_PENDING_GAMEPLAY_COMMANDS};
            for( size_t count : counts )
            {
                LocalSession session;
                session.reset( 1, 123 );
                for( size_t index = 0; index != count; ++index )
                {
                    CommandEnvelope command = session.makeCommand( CommandType::LookDelta, 0 );
                    COMMAND_CHECK( command.accepted );
                    session.submit( command );
                }
                if( count == MAXIMUM_PENDING_GAMEPLAY_COMMANDS )
                {
                    CommandEnvelope overflow = session.makeCommand( CommandType::LookDelta, 0 );
                    COMMAND_CHECK( overflow.accepted == false );
                    COMMAND_CHECK( session.submit( overflow ) == false );
                }
                for( uint32_t tick = 0; tick != 8 && session.journal().size() != count; ++tick )
                {
                    COMMAND_CHECK( session.tick() );
                }
                COMMAND_CHECK( session.journal().size() == count );
                CommandEnvelope next = session.makeCommand( CommandType::LookDelta, 0 );
                COMMAND_CHECK( next.accepted );
                COMMAND_CHECK( next.sequence == count + 1 );
                COMMAND_CHECK( session.submit( next ) );
                COMMAND_CHECK( session.tick() );
                COMMAND_CHECK( session.journal().size() == count + 1 );
            }

            return true;
        }
        //////////////////////////////////////////////////////////////////////////
        static bool testPausedGameplayBacklog()
        {
            LocalSession session;
            session.reset( 1, 123 );
            CommandEnvelope pause = session.makeCommand( CommandType::Pause, 0 );
            COMMAND_CHECK( session.submit( pause ) );
            COMMAND_CHECK( session.tick() );
            for( size_t index = 0; index != MAXIMUM_PENDING_GAMEPLAY_COMMANDS; ++index )
            {
                CommandEnvelope future = session.makeCommand( CommandType::LookDelta, 1000000 );
                COMMAND_CHECK( future.accepted );
                session.submit( future );
            }
            for( uint32_t cycle = 0; cycle != 70; ++cycle )
            {
                CommandEnvelope resume = session.makeCommand( CommandType::Resume, 1000000 );
                COMMAND_CHECK( resume.accepted );
                COMMAND_CHECK( session.submit( resume ) );
                COMMAND_CHECK( session.tick() );
                COMMAND_CHECK( session.server().state().paused == false );
                pause = session.makeCommand( CommandType::Pause, 1000000 );
                COMMAND_CHECK( pause.accepted );
                COMMAND_CHECK( session.submit( pause ) );
                COMMAND_CHECK( session.tick() );
                COMMAND_CHECK( session.server().state().paused );
            }
            CommandEnvelope restart = session.makeCommand( CommandType::Restart, 1000000 );
            COMMAND_CHECK( restart.accepted );
            COMMAND_CHECK( session.submit( restart ) );
            COMMAND_CHECK( session.tick() );
            COMMAND_CHECK( session.server().state().matchId == 2 );
            CommandEnvelope fresh = session.makeCommand( CommandType::LookDelta, 0 );
            COMMAND_CHECK( fresh.sequence == 1 );
            COMMAND_CHECK( session.submit( fresh ) );
            COMMAND_CHECK( session.tick() );

            return true;
        }
        //////////////////////////////////////////////////////////////////////////
        static bool testLocalSession()
        {
            {
                LocalSession direct;
                direct.reset( 1, 123 );
                CommandEnvelope pending;
                pending.matchId = 1;
                pending.playerId = 1;
                pending.sequence = 1;
                pending.type = CommandType::Pause;
                direct.submit( pending );
                direct.reset( 1, 456 );
                if( direct.server().state().seed != 123 )
                {
                    testFailed( __LINE__ );
                    return false;
                }
                if( direct.tick() == false )
                {
                    testFailed( __LINE__ );
                    return false;
                }
                if( direct.server().state().paused == false )
                {
                    testFailed( __LINE__ );
                    return false;
                }
            }
            LocalSession session;
            session.reset( 1, 123 );
            CommandEnvelope pause = session.makeCommand( CommandType::Pause, 0 );
            session.submit( pause );
            session.submit( pause );
            session.reset( 1, 456 );
            if( session.tick() == false )
            {
                testFailed( __LINE__ );
                return false;
            }
            if( session.server().state().paused == false )
            {
                testFailed( __LINE__ );
                return false;
            }
            if( session.journal().size() != 1 )
            {
                testFailed( __LINE__ );
                return false;
            }
            StateChecksum pausedChecksum = session.server().checksum();
            session.reset( 1, 456 );
            if( session.server().checksum() != pausedChecksum )
            {
                testFailed( __LINE__ );
                return false;
            }
            if( session.server().state().seed != 123 )
            {
                testFailed( __LINE__ );
                return false;
            }
            if( session.journal().size() != 1 )
            {
                testFailed( __LINE__ );
                return false;
            }
            CommandEnvelope resume = session.makeCommand( CommandType::Resume, 100000 );
            if( resume.sequence != 2 )
            {
                testFailed( __LINE__ );
                return false;
            }
            session.submit( resume );
            if( session.tick() == false )
            {
                testFailed( __LINE__ );
                return false;
            }
            if( session.server().state().paused == true )
            {
                testFailed( __LINE__ );
                return false;
            }
            CommandEnvelope restart = session.makeCommand( CommandType::Restart, 100000 );
            session.submit( restart );
            if( session.tick() == false )
            {
                testFailed( __LINE__ );
                return false;
            }
            if( session.server().state().matchId != 2 )
            {
                testFailed( __LINE__ );
                return false;
            }
            // A delayed command from the old session is discarded, and the
            // current local session remains usable.
            session.submit( pause );
            if( session.tick() == false )
            {
                testFailed( __LINE__ );
                return false;
            }
            CommandEnvelope next = session.makeCommand( CommandType::LookDelta, 0 );
            if( next.matchId != 2 )
            {
                testFailed( __LINE__ );
                return false;
            }
            if( next.sequence != 1 )
            {
                testFailed( __LINE__ );
                return false;
            }
            session.submit( next );
            if( session.tick() == false )
            {
                testFailed( __LINE__ );
                return false;
            }

            return true;
        }
        //////////////////////////////////////////////////////////////////////////
        static bool testAdapterAndReplay( const Mengine::String & _path )
        {
            Simulation simulation;
            simulation.reset( 1, 123 );
            simulation.findPlayer( 1 )->yaw = 0;
            CommandServer server( &simulation );
            if( server.reset() == false )
            {
                testFailed( __LINE__ );
                return false;
            }
            CommandClient client;
            if( client.reset( 1, 1 ) == false )
            {
                testFailed( __LINE__ );
                return false;
            }
            CommandPayload payload;
            payload.lookYaw = 17;
            CommandEnvelope future;
            if( client.makeCommand( CommandType::LookDelta, 100, 0, payload, &future ) == false )
            {
                testFailed( __LINE__ );
                return false;
            }
            if( server.submit( future ) != ACKORD_OK )
            {
                testFailed( __LINE__ );
                return false;
            }
            CommandEnvelope immediate;
            if( client.makeCommand( CommandType::FireUp, 0, 0, {}, &immediate ) == false )
            {
                testFailed( __LINE__ );
                return false;
            }
            if( server.submit( immediate ) != ACKORD_OK )
            {
                testFailed( __LINE__ );
                return false;
            }
            if( client.reset( 1, 1 ) == false )
            {
                testFailed( __LINE__ );
                return false;
            }
            if( server.reset() == false )
            {
                testFailed( __LINE__ );
                return false;
            }
            if( client.getPendingCount() != 2 )
            {
                testFailed( __LINE__ );
                return false;
            }
            if( server.tick() == false )
            {
                testFailed( __LINE__ );
                return false;
            }
            if( server.getConfirmedSequence( 1 ) != 0 )
            {
                testFailed( __LINE__ );
                return false;
            }
            if( server.getCompletedMask( 1 ) != 2 )
            {
                testFailed( __LINE__ );
                return false;
            }
            ackord_journal_entry entry;
            if( server.getNextJournalEntry( 0, &entry ) == false )
            {
                testFailed( __LINE__ );
                return false;
            }
            if( entry.confirmation.sequence != 2 )
            {
                testFailed( __LINE__ );
                return false;
            }
            if( entry.confirmation.status != 1 )
            {
                testFailed( __LINE__ );
                return false;
            }
            if( client.confirm( 2, entry.confirmation.status ) == false )
            {
                testFailed( __LINE__ );
                return false;
            }
            if( client.getPendingCount() != 1 )
            {
                testFailed( __LINE__ );
                return false;
            }
            if( client.reset( 1, 1 ) == false )
            {
                testFailed( __LINE__ );
                return false;
            }
            if( server.reset() == false )
            {
                testFailed( __LINE__ );
                return false;
            }
            if( client.getPendingCount() != 1 )
            {
                testFailed( __LINE__ );
                return false;
            }
            if( server.getCompletedMask( 1 ) != 2 )
            {
                testFailed( __LINE__ );
                return false;
            }
            if( server.submit( immediate ) != ACKORD_DUPLICATE )
            {
                testFailed( __LINE__ );
                return false;
            }
            while( simulation.state().tick <= 10 )
            {
                if( server.tick() == false )
                {
                    testFailed( __LINE__ );
                    return false;
                }
            }
            if( server.getConfirmedSequence( 1 ) != 2 )
            {
                testFailed( __LINE__ );
                return false;
            }
            if( simulation.state().players[0].yaw != 17 )
            {
                testFailed( __LINE__ );
                return false;
            }
            if( client.confirmProgress( 2, 0 ) == false )
            {
                testFailed( __LINE__ );
                return false;
            }
            if( client.getPendingCount() != 0 )
            {
                testFailed( __LINE__ );
                return false;
            }
            uint64_t last = 0;
            while( server.getNextJournalEntry( last, &entry ) == true )
            {
                last = entry.index;
            }
            server.trimJournal( last );
            if( server.submit( future ) != ACKORD_STALE )
            {
                testFailed( __LINE__ );
                return false;
            }

            // Replay covers the unchanged simulation serialization and the journal
            // exported from the C-backed local session, including rejected inputs.
            LocalSession session;
            session.reset( 1, 123 );
            for( uint64_t tick = 0; tick != 80; ++tick )
            {
                CommandPayload look;
                look.lookYaw = 3;
                CommandEnvelope command = session.makeCommand( CommandType::LookDelta, tick * TickMilliseconds, look );
                session.submit( command );
                if( session.tick() == false )
                {
                    testFailed( __LINE__ );
                    return false;
                }
            }
            const Simulation & serverSimulation = session.server();
            const VectorCommandEnvelope & commandJournal = session.journal();
            const VectorServerEvent & eventJournal = session.eventJournal();
            const VectorReplayBatch & batches = session.batchJournal();
            ReplayRecord record = makeReplayRecord( serverSimulation, 123, commandJournal, eventJournal, batches );
            if( writeReplay( _path, record ) == false )
            {
                testFailed( __LINE__ );
                return false;
            }
            ReplayRecord decoded;
            if( readReplay( _path, &decoded ) == false )
            {
                testFailed( __LINE__ );
                return false;
            }
            Simulation replayed;
            if( runReplay( decoded, &replayed ) == false )
            {
                testFailed( __LINE__ );
                return false;
            }

            return true;
        }
        //////////////////////////////////////////////////////////////////////////
        static bool testScheduledOrder( const Mengine::String & _path )
        {
            LocalSession session;
            session.reset( 1, 123 );
            for( uint32_t tick = 0; tick != 10; ++tick )
            {
                if( session.tick() == false )
                {
                    testFailed( __LINE__ );
                    return false;
                }
            }
            CommandEnvelope down = session.makeCommand( CommandType::ButtonDown, 9 * TickMilliseconds );
            CommandEnvelope up = session.makeCommand( CommandType::ButtonUp, 8 * TickMilliseconds );
            session.submit( down );
            session.submit( up );
            if( session.tick() == false )
            {
                testFailed( __LINE__ );
                return false;
            }
            const VectorCommandEnvelope & commands = session.journal();
            if( commands.size() != 2 )
            {
                testFailed( __LINE__ );
                return false;
            }
            if( commands[0].sequence != up.sequence )
            {
                testFailed( __LINE__ );
                return false;
            }
            if( commands[1].sequence != down.sequence )
            {
                testFailed( __LINE__ );
                return false;
            }
            const Simulation & simulation = session.server();
            const PlayerState * player = simulation.findPlayer( 1 );
            if( player->inputMask != 1 )
            {
                testFailed( __LINE__ );
                return false;
            }
            const VectorServerEvent & events = session.eventJournal();
            const VectorReplayBatch & batches = session.batchJournal();
            ReplayRecord record = makeReplayRecord( simulation, 123, commands, events, batches );
            if( writeReplay( _path, record ) == false )
            {
                testFailed( __LINE__ );
                return false;
            }
            ReplayRecord decoded;
            if( readReplay( _path, &decoded ) == false )
            {
                testFailed( __LINE__ );
                return false;
            }
            Simulation replayed;
            if( runReplay( decoded, &replayed ) == false )
            {
                testFailed( __LINE__ );
                return false;
            }

            return true;
        }
        //////////////////////////////////////////////////////////////////////////
        static bool testSessionControlOrder()
        {
            uint64_t sourceTimes[] = {50, 100000};
            for( uint64_t sourceTime : sourceTimes )
            {
                LocalSession session;
                session.reset( 1, 123 );
                for( uint32_t tick = 0; tick != 5; ++tick )
                {
                    if( session.tick() == false )
                    {
                        testFailed( __LINE__ );
                        return false;
                    }
                }
                CommandEnvelope pause = session.makeCommand( CommandType::Pause, sourceTime );
                CommandEnvelope resume = session.makeCommand( CommandType::Resume, sourceTime );
                session.submit( pause );
                session.submit( resume );
                if( session.tick() == false )
                {
                    testFailed( __LINE__ );
                    return false;
                }
                if( session.server().state().paused == true )
                {
                    testFailed( __LINE__ );
                    return false;
                }
                if( session.server().state().tick != 6 )
                {
                    testFailed( __LINE__ );
                    return false;
                }
                const VectorCommandEnvelope & commands = session.journal();
                if( commands.size() != 2 )
                {
                    testFailed( __LINE__ );
                    return false;
                }
                if( commands[0].sequence != pause.sequence )
                {
                    testFailed( __LINE__ );
                    return false;
                }
                if( commands[1].sequence != resume.sequence )
                {
                    testFailed( __LINE__ );
                    return false;
                }
            }

            return true;
        }
        //////////////////////////////////////////////////////////////////////////
        static bool testSessionControlRetries()
        {
            bool sameBatchValues[] = {true, false};
            for( bool sameBatch : sameBatchValues )
            {
                Simulation simulation;
                simulation.reset( 1, 123 );
                CommandServer server( &simulation );
                if( server.reset() == false )
                {
                    testFailed( __LINE__ );
                    return false;
                }
                for( uint32_t tick = 0; tick != 5; ++tick )
                {
                    if( server.tick() == false )
                    {
                        testFailed( __LINE__ );
                        return false;
                    }
                }
                CommandEnvelope pause;
                pause.matchId = 1;
                pause.playerId = 1;
                pause.sequence = 1;
                pause.sourceTimeMs = 100000;
                pause.type = CommandType::Pause;
                CommandEnvelope resume = pause;
                resume.sequence = 2;
                resume.type = CommandType::Resume;
                if( server.submit( pause ) != ACKORD_OK )
                {
                    testFailed( __LINE__ );
                    return false;
                }
                if( server.submit( pause ) != ACKORD_DUPLICATE )
                {
                    testFailed( __LINE__ );
                    return false;
                }
                CommandEnvelope conflict = pause;
                conflict.payload.lookYaw = 1;
                if( server.submit( conflict ) != ACKORD_CONFLICT )
                {
                    testFailed( __LINE__ );
                    return false;
                }
                if( sameBatch == false )
                {
                    if( server.tick() == false )
                    {
                        testFailed( __LINE__ );
                        return false;
                    }
                    if( simulation.state().paused == false )
                    {
                        testFailed( __LINE__ );
                        return false;
                    }
                }
                if( server.submit( resume ) != ACKORD_OK )
                {
                    testFailed( __LINE__ );
                    return false;
                }
                if( server.submit( resume ) != ACKORD_DUPLICATE )
                {
                    testFailed( __LINE__ );
                    return false;
                }
                if( server.tick() == false )
                {
                    testFailed( __LINE__ );
                    return false;
                }
                if( simulation.state().paused == true )
                {
                    testFailed( __LINE__ );
                    return false;
                }
                if( simulation.state().tick != 6 )
                {
                    testFailed( __LINE__ );
                    return false;
                }
                if( server.submit( pause ) != ACKORD_DUPLICATE )
                {
                    testFailed( __LINE__ );
                    return false;
                }
                if( server.submit( resume ) != ACKORD_DUPLICATE )
                {
                    testFailed( __LINE__ );
                    return false;
                }
                if( server.submit( conflict ) != ACKORD_CONFLICT )
                {
                    testFailed( __LINE__ );
                    return false;
                }
                conflict = resume;
                ++conflict.sourceTimeMs;
                if( server.submit( conflict ) != ACKORD_CONFLICT )
                {
                    testFailed( __LINE__ );
                    return false;
                }
                ackord_journal_entry entry;
                uint64_t index = 0;
                while( server.getNextJournalEntry( index, &entry ) == true )
                {
                    if( entry.confirmation.status != 0 )
                    {
                        testFailed( __LINE__ );
                        return false;
                    }
                    index = entry.index;
                }
                if( index != 2 )
                {
                    testFailed( __LINE__ );
                    return false;
                }
                server.trimJournal( index );
                if( server.submit( pause ) != ACKORD_STALE )
                {
                    testFailed( __LINE__ );
                    return false;
                }
                if( server.submit( resume ) != ACKORD_STALE )
                {
                    testFailed( __LINE__ );
                    return false;
                }
            }

            return true;
        }
        //////////////////////////////////////////////////////////////////////////
        static bool testPlayerCommandOrder()
        {
            Simulation simulation;
            simulation.reset( 1, 123 );
            COMMAND_CHECK( simulation.addPlayer( UINT32_MAX ) == UINT32_MAX );
            COMMAND_CHECK( simulation.addPlayer( 2 ) == 2 );
            CommandServer server( &simulation );
            COMMAND_CHECK( server.reset() );
            CommandEnvelope command;
            command.matchId = 1;
            command.playerId = UINT32_MAX;
            command.sequence = 1;
            command.type = CommandType::Pause;
            COMMAND_CHECK( server.submit( command ) == ACKORD_OK );
            command.playerId = 2;
            COMMAND_CHECK( server.submit( command ) == ACKORD_OK );
            command.type = CommandType::JoinPlayer;
            COMMAND_CHECK( server.submit( command ) == ACKORD_OK );
            COMMAND_CHECK( server.tick() );
            const VectorCommandEnvelope & commands = server.getCommands();
            COMMAND_CHECK( commands.size() == 3 );
            COMMAND_CHECK( commands[0].playerId == 2 && commands[0].type == CommandType::JoinPlayer );
            COMMAND_CHECK( commands[1].playerId == 2 && commands[1].type == CommandType::Pause );
            COMMAND_CHECK( commands[2].playerId == UINT32_MAX && commands[2].type == CommandType::Pause );
            ackord_journal_entry entry;
            uint64_t cursor = 0;
            size_t count = 0;
            while( server.getNextJournalEntry( cursor, &entry ) == true )
            {
                bool rejected = entry.command.client == UINT32_MAX || entry.command.type == static_cast<uint32_t>(CommandType::JoinPlayer);
                int32_t expectedStatus = rejected == true ? 1 : 0;
                COMMAND_CHECK( entry.confirmation.status == expectedStatus );
                COMMAND_CHECK( entry.confirmation.client == entry.command.client );
                cursor = entry.index;
                ++count;
            }
            COMMAND_CHECK( count == 3 );
            COMMAND_CHECK( server.getConfirmedSequence( 2, ECommandStream::Gameplay ) == 1 );
            COMMAND_CHECK( server.getConfirmedSequence( 2, ECommandStream::Control ) == 1 );
            COMMAND_CHECK( server.getConfirmedSequence( UINT32_MAX, ECommandStream::Control ) == 1 );

            return true;
        }
        //////////////////////////////////////////////////////////////////////////
        static bool testPlayerOrderAfterRestart()
        {
            Simulation simulation;
            simulation.reset( 1, 123 );
            COMMAND_CHECK( simulation.addPlayer() == 2 );
            COMMAND_CHECK( simulation.addPlayer() == 3 );
            CommandServer server( &simulation );
            COMMAND_CHECK( server.reset() );
            server.unregisterPlayer( 1 );
            COMMAND_CHECK( simulation.removePlayer( 1 ) );
            COMMAND_CHECK( simulation.addPlayer() == 4 );
            COMMAND_CHECK( server.registerPlayer( 4 ) );
            for( uint32_t phase = 0; phase != 2; ++phase )
            {
                COMMAND_CHECK( simulation.state().players[0].id == 4 );
                COMMAND_CHECK( simulation.state().players[1].id == 2 );
                CommandEnvelope command;
                command.matchId = simulation.state().matchId;
                command.playerId = 4;
                command.sequence = 1;
                command.type = CommandType::Pause;
                COMMAND_CHECK( server.submit( command ) == ACKORD_OK );
                command.playerId = 2;
                COMMAND_CHECK( server.submit( command ) == ACKORD_OK );
                COMMAND_CHECK( server.tick() );
                const VectorCommandEnvelope & commands = server.getCommands();
                COMMAND_CHECK( commands.size() == 2 );
                COMMAND_CHECK( commands[0].playerId == 2 && commands[1].playerId == 4 );
                ackord_journal_entry entry;
                uint64_t cursor = 0;
                size_t count = 0;
                while( server.getNextJournalEntry( cursor, &entry ) == true )
                {
                    int32_t expectedStatus = entry.command.client == 2 ? 0 : 1;
                    COMMAND_CHECK( entry.confirmation.status == expectedStatus );
                    cursor = entry.index;
                    ++count;
                }
                COMMAND_CHECK( count == 2 );
                server.trimJournal( cursor );
                if( phase == 1 )
                {
                    break;
                }
                command.sequence = 2;
                command.type = CommandType::Resume;
                COMMAND_CHECK( server.submit( command ) == ACKORD_OK );
                COMMAND_CHECK( server.tick() );
                command.sequence = 3;
                command.type = CommandType::Restart;
                COMMAND_CHECK( server.submit( command ) == ACKORD_OK );
                COMMAND_CHECK( server.tick() );
                COMMAND_CHECK( simulation.state().matchId == 2 );
                COMMAND_CHECK( server.reset() );
            }

            return true;
        }
        //////////////////////////////////////////////////////////////////////////
        static bool testPlayerIdExhaustion()
        {
            Simulation simulation;
            simulation.reset( 1, 123 );
            COMMAND_CHECK( simulation.addPlayer( UINT32_MAX ) == UINT32_MAX );
            COMMAND_CHECK( simulation.state().nextPlayerId == 0 );
            Mengine::Data snapshot = simulation.snapshot();
            COMMAND_CHECK( simulation.addPlayer() == 0 );
            COMMAND_CHECK( simulation.snapshot() == snapshot );
            COMMAND_CHECK( simulation.addPlayer( 2 ) == 2 );
            COMMAND_CHECK( simulation.state().nextPlayerId == 0 );
            COMMAND_CHECK( simulation.addPlayer() == 0 );
            COMMAND_CHECK( simulation.findPlayer( 0 ) == nullptr );
            Simulation restored;
            COMMAND_CHECK( restored.restoreSnapshot( simulation.snapshot() ) );
            COMMAND_CHECK( restored.checksum() == simulation.checksum() );
            COMMAND_CHECK( restored.addPlayer() == 0 );
            simulation.reset( 2, 456 );
            COMMAND_CHECK( simulation.addPlayer( UINT32_MAX - 1 ) == UINT32_MAX - 1 );
            COMMAND_CHECK( simulation.addPlayer() == UINT32_MAX );
            COMMAND_CHECK( simulation.state().nextPlayerId == 0 );
            COMMAND_CHECK( simulation.addPlayer() == 0 );
            COMMAND_CHECK( simulation.findPlayer( 0 ) == nullptr );

            return true;
        }
        //////////////////////////////////////////////////////////////////////////
        static bool testRejectedCommandEvents( const Mengine::String & _path )
        {
            size_t playerCounts[] = {9, MaximumPlayers};
            for( size_t playerCount : playerCounts )
            {
                Simulation simulation;
                simulation.reset( 1, 123 );
                CommandServer server( &simulation );
                COMMAND_CHECK( server.reset() );
                VectorCommandEnvelope journal;
                VectorServerEvent events;
                VectorReplayBatch batches;
                auto recordBatch = [&]()
                {
                    const VectorCommandEnvelope & batch = server.getCommands();
                    const VectorServerEvent & batchEvents = simulation.events();
                    journal.insert( journal.end(), batch.begin(), batch.end() );
                    events.insert( events.end(), batchEvents.begin(), batchEvents.end() );
                    uint32_t commandCount = static_cast<uint32_t>(batch.size());
                    uint32_t eventCount = static_cast<uint32_t>(batchEvents.size());
                    batches.push_back( {commandCount, eventCount} );
                };
                for( uint32_t playerId = 2; playerId <= playerCount; ++playerId )
                {
                    COMMAND_CHECK( server.registerPlayer( playerId ) );
                    CommandEnvelope command;
                    command.matchId = 1;
                    command.playerId = playerId;
                    command.sequence = 1;
                    command.type = CommandType::JoinPlayer;
                    COMMAND_CHECK( server.submit( command ) == ACKORD_OK );
                }
                COMMAND_CHECK( server.tick() );
                recordBatch();
                server.trimJournal( UINT64_MAX );
                uint64_t sourceTime = simulation.state().tick * TickMilliseconds;
                size_t commandCount = 0;
                for( uint32_t playerId = 1; playerId <= playerCount; ++playerId )
                {
                    uint64_t previousSequence = playerId == 1 ? 0 : 1;
                    for( uint64_t sequence = 1; sequence <= 64; ++sequence )
                    {
                        CommandEnvelope command;
                        command.matchId = 1;
                        command.playerId = playerId;
                        command.sourceTimeMs = sourceTime;
                        command.sequence = previousSequence + sequence;
                        command.type = CommandType::ButtonUp;
                        COMMAND_CHECK( server.submit( command ) == ACKORD_OK );
                        command.sequence = sequence;
                        command.type = CommandType::Resume;
                        COMMAND_CHECK( server.submit( command ) == ACKORD_OK );
                        commandCount += 2;
                    }
                }
                COMMAND_CHECK( server.tick() );
                COMMAND_CHECK( server.getCommands().size() == commandCount );
                size_t rejectedEvents = 0;
                const VectorServerEvent & batchEvents = simulation.events();
                for( const ServerEvent & event : batchEvents )
                {
                    if( event.type == EventType::CommandRejected )
                    {
                        ++rejectedEvents;
                    }
                }
                COMMAND_CHECK( rejectedEvents == commandCount );
                ackord_journal_entry entry;
                uint64_t cursor = 0;
                size_t confirmed = 0;
                while( server.getNextJournalEntry( cursor, &entry ) == true )
                {
                    COMMAND_CHECK( entry.confirmation.status == 1 );
                    cursor = entry.index;
                    ++confirmed;
                }
                COMMAND_CHECK( confirmed == commandCount );
                recordBatch();
                ReplayRecord replay = makeReplayRecord( simulation, 123, journal, events, batches );
                COMMAND_CHECK( writeReplay( _path, replay ) );
                ReplayRecord decoded;
                COMMAND_CHECK( readReplay( _path, &decoded ) );
                Simulation playback;
                COMMAND_CHECK( runReplay( decoded, &playback ) );
                COMMAND_CHECK( playback.checksum() == simulation.checksum() );
            }

            return true;
        }
        //////////////////////////////////////////////////////////////////////////
        static bool testPlayersAndDisconnect()
        {
            Simulation simulation;
            simulation.reset( 1, 123 );
            uint32_t second = simulation.addPlayer();
            simulation.findPlayer( second )->yaw = 0;
            CommandServer server( &simulation );
            if( server.reset() == false )
            {
                testFailed( __LINE__ );
                return false;
            }
            CommandEnvelope command;
            command.matchId = 1;
            command.playerId = second;
            command.sequence = 1;
            command.type = CommandType::LookDelta;
            command.payload.lookYaw = 100;
            if( server.submit( command ) != ACKORD_OK )
            {
                testFailed( __LINE__ );
                return false;
            }
            if( server.tick() == false )
            {
                testFailed( __LINE__ );
                return false;
            }
            if( simulation.findPlayer( second )->yaw != 100 )
            {
                testFailed( __LINE__ );
                return false;
            }
            command.sequence = 2;
            command.sourceTimeMs = 1000;
            if( server.submit( command ) != ACKORD_OK )
            {
                testFailed( __LINE__ );
                return false;
            }
            server.unregisterPlayer( second );
            if( server.registerPlayer( second ) == true )
            {
                testFailed( __LINE__ );
                return false;
            }
            if( server.submit( command ) != ACKORD_RETIRED )
            {
                testFailed( __LINE__ );
                return false;
            }
            simulation.removePlayer( second );
            while( simulation.state().tick <= 100 )
            {
                if( server.tick() == false )
                {
                    testFailed( __LINE__ );
                    return false;
                }
            }
            if( server.getCommands().empty() == false )
            {
                testFailed( __LINE__ );
                return false;
            }
            simulation.reset( 2, 456 );
            if( server.reset() == false )
            {
                testFailed( __LINE__ );
                return false;
            }
            if( server.submit( command ) != ACKORD_WRONG_SESSION )
            {
                testFailed( __LINE__ );
                return false;
            }

            return true;
        }
        //////////////////////////////////////////////////////////////////////////
    }
}

int main( int _argc, char ** _argv )
{
    if( _argc != 2 )
    {
        return 2;
    }
    if( Arena3D::Detail::testLocalSession() == false )
    {
        return 3;
    }
    if( Arena3D::Detail::testClientValidation() == false )
    {
        return 9;
    }
    if( Arena3D::Detail::testLocalBackpressure() == false )
    {
        return 10;
    }
    if( Arena3D::Detail::testPausedGameplayBacklog() == false )
    {
        return 11;
    }
    Mengine::String path( _argv[1] );
    if( Arena3D::Detail::testAdapterAndReplay( path ) == false )
    {
        return 4;
    }
    if( Arena3D::Detail::testRejectedCommandEvents( path ) == false )
    {
        return 15;
    }
    if( Arena3D::Detail::testPlayersAndDisconnect() == false )
    {
        return 5;
    }
    if( Arena3D::Detail::testPlayerCommandOrder() == false )
    {
        return 12;
    }
    if( Arena3D::Detail::testPlayerOrderAfterRestart() == false )
    {
        return 13;
    }
    if( Arena3D::Detail::testPlayerIdExhaustion() == false )
    {
        return 14;
    }
    if( Arena3D::Detail::testScheduledOrder( path ) == false )
    {
        return 6;
    }
    if( Arena3D::Detail::testSessionControlOrder() == false )
    {
        return 7;
    }
    if( Arena3D::Detail::testSessionControlRetries() == false )
    {
        return 8;
    }
    Mengine::StdIO::printf( "Ackord Arena3D: local, pause/resume, restart, replay, results, gaps and disconnect passed\n" );

    return 0;
}
