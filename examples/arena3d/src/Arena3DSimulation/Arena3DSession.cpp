#include "Arena3DSession.h"

#include <algorithm>
#include <utility>

namespace Arena3D
{
    void LocalCommandTransport::sendCommand( const CommandEnvelope & command )
    {
        m_toServer.emplace_back( command );
    }

    std::vector<CommandEnvelope> LocalCommandTransport::takeServerCommands()
    {
        std::vector<CommandEnvelope> result;
        result.swap( m_toServer );
        return result;
    }

    void LocalCommandTransport::publishServerFrame( ServerFrame frame )
    {
        m_toClient.emplace_back( std::move( frame ) );
    }

    bool LocalCommandTransport::takeClientFrame( ServerFrame * frame )
    {
        if( m_toClient.empty() == true ) return false;
        *frame = std::move( m_toClient.front() );
        m_toClient.pop_front();
        return true;
    }

    LocalSession::LocalSession( SimulationConfig config )
        : m_config( std::move( config ) )
        , m_server( m_config )
        , m_client( m_config )
    {
        m_scheduled.reserve( 256 );
        m_journal.reserve( 4096 );
        m_eventJournal.reserve( 4096 );
    }

    void LocalSession::reset( uint64_t matchId, uint64_t seed )
    {
        m_server.reset( matchId, seed );
        m_client.reset( matchId, seed );
        m_transport = LocalCommandTransport{};
        m_scheduled.clear();
        m_journal.clear();
        m_eventJournal.clear();
        m_nextSequence = 1;
        m_lastAcceptedSequence = 0;
        m_reconciliationCount = 0;
        m_synchronized = true;
    }

    CommandEnvelope LocalSession::makeCommand( CommandType type, uint64_t sourceTimeMs, const CommandPayload & payload )
    {
        CommandEnvelope command;
        command.matchId = m_server.state().matchId;
        command.playerId = m_server.state().player.id;
        command.sequence = m_nextSequence++;
        command.sourceTimeMs = sourceTimeMs;
        command.type = type;
        command.payload = payload;
        return command;
    }

    void LocalSession::submit( const CommandEnvelope & command )
    {
        m_transport.sendCommand( command );
    }

    void LocalSession::receiveCommands_()
    {
        std::vector<CommandEnvelope> incoming = m_transport.takeServerCommands();
        std::stable_sort( incoming.begin(), incoming.end(), []( const CommandEnvelope & left, const CommandEnvelope & right )
        {
            if( left.playerId != right.playerId ) return left.playerId < right.playerId;
            return left.sequence < right.sequence;
        } );
        for( CommandEnvelope & command : incoming )
        {
            command.accepted = command.matchId == m_server.state().matchId && command.playerId == m_server.state().player.id &&
                command.sequence > m_lastAcceptedSequence;
            if( command.accepted == true ) m_lastAcceptedSequence = command.sequence;

            const uint64_t requestedTick = (command.sourceTimeMs + TickMilliseconds - 1u) / TickMilliseconds;
            const bool immediateAdmin = m_server.state().paused == true &&
                (command.type == CommandType::Resume || command.type == CommandType::Restart);
            const uint64_t earliestTick = m_server.state().tick;
            command.appliedTick = immediateAdmin ? earliestTick : std::max( requestedTick, earliestTick );
            command.tickCorrected = command.appliedTick != requestedTick;
            m_scheduled.emplace_back( command );
        }
        std::stable_sort( m_scheduled.begin(), m_scheduled.end(), []( const CommandEnvelope & left, const CommandEnvelope & right )
        {
            if( left.appliedTick != right.appliedTick ) return left.appliedTick < right.appliedTick;
            if( left.playerId != right.playerId ) return left.playerId < right.playerId;
            return left.sequence < right.sequence;
        } );
    }

    std::vector<CommandEnvelope> LocalSession::commandsForCurrentTick_()
    {
        std::vector<CommandEnvelope> result;
        const uint64_t tickValue = m_server.state().tick;
        size_t count = 0;
        while( count < m_scheduled.size() && m_scheduled[count].appliedTick <= tickValue )
        {
            result.emplace_back( m_scheduled[count] );
            m_journal.emplace_back( m_scheduled[count] );
            ++count;
        }
        if( count != 0 ) m_scheduled.erase( m_scheduled.begin(), m_scheduled.begin() + static_cast<std::ptrdiff_t>(count) );
        return result;
    }

    bool LocalSession::tick()
    {
        this->receiveCommands_();
        std::vector<CommandEnvelope> commands = this->commandsForCurrentTick_();
        if( m_server.step( commands ) == false ) return false;

        ServerFrame frame;
        frame.tick = m_server.state().tick;
        frame.acknowledgedSequence = m_lastAcceptedSequence;
        frame.checksum = m_server.checksum();
        frame.commands = commands;
        frame.events = m_server.events();
        m_eventJournal.insert( m_eventJournal.end(), frame.events.begin(), frame.events.end() );
        if( frame.tick % 100u == 0u ) frame.snapshot = m_server.snapshot();
        m_transport.publishServerFrame( std::move( frame ) );
        this->consumeClientFrames_();
        return m_synchronized;
    }

    void LocalSession::consumeClientFrames_()
    {
        ServerFrame frame;
        while( m_transport.takeClientFrame( &frame ) == true )
        {
            if( m_client.step( frame.commands ) == false )
            {
                m_synchronized = false;
                return;
            }
            if( m_client.checksum() != frame.checksum )
            {
                std::vector<uint8_t> authoritative = frame.snapshot.empty() == false ? frame.snapshot : m_server.snapshot();
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

    bool LocalSession::advanceTicks( uint64_t count )
    {
        for( uint64_t index = 0; index != count; ++index ) if( this->tick() == false ) return false;
        return true;
    }

    const Simulation & LocalSession::server() const { return m_server; }
    const Simulation & LocalSession::client() const { return m_client; }
    const std::vector<CommandEnvelope> & LocalSession::journal() const { return m_journal; }
    const std::vector<ServerEvent> & LocalSession::eventJournal() const { return m_eventJournal; }
    bool LocalSession::synchronized() const { return m_synchronized; }
    uint64_t LocalSession::reconciliationCount() const { return m_reconciliationCount; }
    void LocalSession::injectClientFaultForTest( int64_t rawDelta ) { m_client.injectFaultForTest( rawDelta ); }
}
