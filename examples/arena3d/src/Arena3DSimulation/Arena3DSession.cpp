#include "Arena3DSession.h"

#include "Kernel/FactorableUnique.h"

#include "Config/StdAlgorithm.h"
#include "Config/StdUtility.h"

//////////////////////////////////////////////////////////////////////////
namespace Arena3D
{
    //////////////////////////////////////////////////////////////////////////
    LocalSession::LocalSession( SimulationConfig config )
        : m_config( Mengine::StdUtility::move( config ) )
        , m_server( m_config )
        , m_client( m_config )
        , m_transport( Mengine::Helper::makeFactorableUnique<LocalCommandTransport>( Mengine::DocumentInterfacePtr::none() ) )
    {
        m_scheduled.reserve( 256 );
        m_journal.reserve( 4096 );
        m_eventJournal.reserve( 4096 );
    }
    //////////////////////////////////////////////////////////////////////////
    void LocalSession::reset( uint64_t matchId, uint64_t seed )
    {
        m_server.reset( matchId, seed );
        m_client.reset( matchId, seed );
        m_transport->clear();
        m_scheduled.clear();
        m_journal.clear();
        m_eventJournal.clear();
        m_nextSequence = 1;
        m_lastAcceptedSequence = 0;
        m_reconciliationCount = 0;
        m_synchronized = true;
    }
    //////////////////////////////////////////////////////////////////////////
    CommandEnvelope LocalSession::makeCommand( CommandType type, uint64_t sourceTimeMs, const CommandPayload & payload )
    {
        CommandEnvelope command;
        command.matchId = m_server.state().matchId;
        command.playerId = m_server.state().players[0].id;
        command.sequence = m_nextSequence++;
        command.sourceTimeMs = sourceTimeMs;
        command.type = type;
        command.payload = payload;
        return command;
    }
    //////////////////////////////////////////////////////////////////////////
    void LocalSession::submit( const CommandEnvelope & command )
    {
        m_transport->sendCommand( command );
    }
    //////////////////////////////////////////////////////////////////////////
    void LocalSession::receiveCommands_()
    {
        VectorCommandEnvelope incoming = m_transport->takeServerCommands();
        Mengine::StdAlgorithm::stable_sort( incoming.begin(), incoming.end(),
                                            []( const CommandEnvelope & left, const CommandEnvelope & right )
                                            {
                                                if( left.playerId != right.playerId )
                                                {
                                                    return left.playerId < right.playerId;
                                                }
                                                return left.sequence < right.sequence;
                                            } );
        for( CommandEnvelope & command : incoming )
        {
            command.accepted = command.matchId == m_server.state().matchId && m_server.findPlayer( command.playerId ) != nullptr &&
                command.sequence > m_lastAcceptedSequence;
            if( command.accepted == true )
            {
                m_lastAcceptedSequence = command.sequence;
            }

            const uint64_t requestedTick = (command.sourceTimeMs + TickMilliseconds - 1u) / TickMilliseconds;
            const bool immediateAdmin = m_server.state().paused == true &&
                (command.type == CommandType::Resume || command.type == CommandType::Restart);
            const uint64_t earliestTick = m_server.state().tick;
            command.appliedTick = immediateAdmin ? earliestTick : Mengine::StdAlgorithm::max( requestedTick, earliestTick );
            command.tickCorrected = command.appliedTick != requestedTick;
            m_scheduled.emplace_back( command );
        }
        Mengine::StdAlgorithm::stable_sort( m_scheduled.begin(), m_scheduled.end(),
                                            []( const CommandEnvelope & left, const CommandEnvelope & right )
                                            {
                                                if( left.appliedTick != right.appliedTick )
                                                {
                                                    return left.appliedTick < right.appliedTick;
                                                }
                                                if( left.playerId != right.playerId )
                                                {
                                                    return left.playerId < right.playerId;
                                                }
                                                return left.sequence < right.sequence;
                                            } );
    }
    //////////////////////////////////////////////////////////////////////////
    VectorCommandEnvelope LocalSession::commandsForCurrentTick_()
    {
        VectorCommandEnvelope result;
        const uint64_t tickValue = m_server.state().tick;
        size_t count = 0;
        while( count < m_scheduled.size() && m_scheduled[count].appliedTick <= tickValue )
        {
            result.emplace_back( m_scheduled[count] );
            m_journal.emplace_back( m_scheduled[count] );
            ++count;
        }
        if( count != 0 )
        {
            m_scheduled.erase( m_scheduled.begin(), m_scheduled.begin() + static_cast<ptrdiff_t>(count) );
        }
        return result;
    }
    //////////////////////////////////////////////////////////////////////////
    bool LocalSession::tick()
    {
        this->receiveCommands_();
        VectorCommandEnvelope commands = this->commandsForCurrentTick_();
        m_server.step( commands );

        ServerFrame frame;
        frame.tick = m_server.state().tick;
        frame.acknowledgedSequence = m_lastAcceptedSequence;
        frame.checksum = m_server.checksum();
        frame.commands = commands;
        frame.events = m_server.events();
        m_eventJournal.insert( m_eventJournal.end(), frame.events.begin(), frame.events.end() );
        if( frame.tick % 100u == 0u )
        {
            frame.snapshot = m_server.snapshot();
        }
        m_transport->publishServerFrame( Mengine::StdUtility::move( frame ) );
        this->consumeClientFrames_();
        return m_synchronized;
    }
    //////////////////////////////////////////////////////////////////////////
    void LocalSession::consumeClientFrames_()
    {
        ServerFrame frame;
        while( m_transport->takeClientFrame( &frame ) == true )
        {
            m_client.step( frame.commands );
            if( m_client.checksum() != frame.checksum )
            {
                Mengine::Data authoritative = frame.snapshot.empty() == false ? frame.snapshot : m_server.snapshot();
                if( m_client.restoreSnapshot( authoritative ) == false || m_client.checksum() != frame.checksum )
                {
                    m_synchronized = false;
                    return;
                }
                ++m_reconciliationCount;
            }
        }
        m_synchronized = m_client.checksum() == m_server.checksum();
    }
    //////////////////////////////////////////////////////////////////////////
    const Simulation & LocalSession::server() const
    {
        return m_server;
    }
    //////////////////////////////////////////////////////////////////////////
    const Simulation & LocalSession::client() const
    {
        return m_client;
    }
    //////////////////////////////////////////////////////////////////////////
    const VectorCommandEnvelope & LocalSession::journal() const
    {
        return m_journal;
    }
    //////////////////////////////////////////////////////////////////////////
    const VectorServerEvent & LocalSession::eventJournal() const
    {
        return m_eventJournal;
    }
    //////////////////////////////////////////////////////////////////////////
    uint64_t LocalSession::reconciliationCount() const
    {
        return m_reconciliationCount;
    }
    //////////////////////////////////////////////////////////////////////////
}
