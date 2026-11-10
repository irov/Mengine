#include "Arena3DSession.h"

#include "Config/StdUtility.h"

namespace Arena3D
{
    //////////////////////////////////////////////////////////////////////////
    LocalSession::LocalSession( SimulationConfig config )
        : m_config( Mengine::StdUtility::move( config ) )
        , m_server( m_config )
        , m_client( m_config )
        , m_commandServer( &m_server )
    {
        const SimulationState & state = m_server.state();
        uint64_t matchId = state.matchId;
        uint64_t seed = state.seed;
        this->reset( matchId, seed );
    }
    //////////////////////////////////////////////////////////////////////////
    void LocalSession::reset( uint64_t matchId, uint64_t seed )
    {
        // A live session keeps its state and queues under the same ID.
        if( matchId == m_commandClient.getMatchId() && m_hasActivity == true )
        {
            return;
        }
        m_server.reset( matchId, seed );
        m_client.reset( matchId, seed );
        m_journal.clear();
        m_eventJournal.clear();
        m_batchJournal.clear();
        m_reconciliationCount = 0;
        m_hasActivity = false;
        m_retryPending = false;
        m_synchronized = m_commandServer.reset();
        uint32_t playerId = m_server.state().players[0].id;
        if( m_commandClient.reset( matchId, playerId ) == false )
        {
            m_synchronized = false;
        }
    }
    //////////////////////////////////////////////////////////////////////////
    CommandEnvelope LocalSession::makeCommand( CommandType type, uint64_t sourceTimeMs, const CommandPayload & payload )
    {
        CommandEnvelope command;
        uint64_t tick = m_server.state().tick;
        if( m_commandClient.makeCommand( type, sourceTimeMs, tick, payload, &command ) == false )
        {
            command.accepted = false;
            return command;
        }
        m_hasActivity = true;

        return command;
    }
    //////////////////////////////////////////////////////////////////////////
    bool LocalSession::submit( const CommandEnvelope & _command )
    {
        if( _command.accepted == false )
        {
            return false;
        }
        m_hasActivity = true;
        ackord_status status = m_commandServer.submit( _command );
        if( status == ACKORD_OK || status == ACKORD_DUPLICATE || status == ACKORD_STALE || status == ACKORD_WRONG_SESSION )
        {
            return true;
        }
        if( status == ACKORD_FULL || status == ACKORD_OUT_OF_WINDOW )
        {
            m_retryPending = true;
            return false;
        }
        m_synchronized = false;

        return false;
    }
    //////////////////////////////////////////////////////////////////////////
    bool LocalSession::tick()
    {
        if( m_synchronized == false )
        {
            return false;
        }
        m_hasActivity = true;
        if( m_retryPending == true )
        {
            m_retryPending = false;
            VectorCommandEnvelope pending = m_commandClient.getPendingCommands();
            for( const CommandEnvelope & command : pending )
            {
                this->submit( command );
                if( m_synchronized == false )
                {
                    return false;
                }
            }
        }
        uint64_t previousMatch = m_server.state().matchId;
        if( m_commandServer.tick() == false )
        {
            return false;
        }
        const VectorCommandEnvelope & commands = m_commandServer.getCommands();
        m_journal.insert( m_journal.end(), commands.begin(), commands.end() );
        const VectorServerEvent & events = m_server.events();
        m_eventJournal.insert( m_eventJournal.end(), events.begin(), events.end() );
        uint32_t commandCount = static_cast<uint32_t>(commands.size());
        uint32_t eventCount = static_cast<uint32_t>(events.size());
        m_batchJournal.push_back( {commandCount, eventCount} );
        m_client.step( commands );
        StateChecksum checksum = m_server.checksum();
        StateChecksum clientChecksum = m_client.checksum();
        if( clientChecksum != checksum )
        {
            Mengine::Data snapshot = m_server.snapshot();
            if( m_client.restoreSnapshot( snapshot ) == false )
            {
                m_synchronized = false;
                return false;
            }
            ++m_reconciliationCount;
            clientChecksum = m_client.checksum();
        }
        ackord_journal_entry entry;
        uint64_t index = 0;
        while( m_commandServer.getNextJournalEntry( index, &entry ) == true )
        {
            ECommandStream stream = getCommandStream( static_cast<CommandType>(entry.command.type) );
            m_commandClient.confirm( entry.confirmation.sequence, entry.confirmation.status, stream );
            index = entry.index;
        }
        m_commandServer.trimJournal( index );
        if( m_server.state().matchId != previousMatch )
        {
            m_retryPending = false;
            if( m_commandServer.reset() == false )
            {
                return false;
            }
            uint64_t matchId = m_server.state().matchId;
            uint32_t playerId = m_server.state().players[0].id;
            if( m_commandClient.reset( matchId, playerId ) == false )
            {
                return false;
            }
        }
        m_synchronized = clientChecksum == checksum;

        return m_synchronized;
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
    const VectorReplayBatch & LocalSession::batchJournal() const
    {
        return m_batchJournal;
    }
    //////////////////////////////////////////////////////////////////////////
    uint64_t LocalSession::reconciliationCount() const
    {
        return m_reconciliationCount;
    }
    //////////////////////////////////////////////////////////////////////////
}
