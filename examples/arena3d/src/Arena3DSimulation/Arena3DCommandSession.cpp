#include "Arena3DCommandSession.h"

#include "Kernel/AllocatorHelper.h"

#include "Config/StdAlgorithm.h"
//////////////////////////////////////////////////////////////////////////
namespace Arena3D
{
    //////////////////////////////////////////////////////////////////////////
    namespace Detail
    {
        //////////////////////////////////////////////////////////////////////////
        static const uint32_t MAX_COMMAND_BYTES = 128;
        static const uint32_t COMMAND_SEQUENCE_WINDOW = 64;
        static const size_t MAX_SERVER_RECORDS = 4096;
        static const size_t MAX_SESSION_CLIENTS = 4096;
        //////////////////////////////////////////////////////////////////////////
        static void * allocateAckord( void * _user, size_t _size )
        {
            MENGINE_UNUSED( _user );

            void * memory = Mengine::Helper::allocateMemory( _size, "ackord" );

            return memory;
        }
        //////////////////////////////////////////////////////////////////////////
        static void deallocateAckord( void * _user, void * _memory )
        {
            MENGINE_UNUSED( _user );

            Mengine::Helper::deallocateMemory( _memory, "ackord" );
        }
        //////////////////////////////////////////////////////////////////////////
        static ackord_config commandConfig( size_t _maximumRecords )
        {
            ackord_config config{};
            config.max_clients = MaximumPlayers;
            config.max_records = _maximumRecords;
            config.max_payload_size = MAX_COMMAND_BYTES;
            config.max_result_size = 0;
            config.sequence_window = COMMAND_SEQUENCE_WINDOW;
            config.allocator = {allocateAckord, deallocateAckord, nullptr};

            return config;
        }
        //////////////////////////////////////////////////////////////////////////
        static Mengine::Data encodeCommand( const CommandEnvelope & _command )
        {
            CommandEnvelope canonical = _command;
            canonical.sequence = 0;
            canonical.appliedTick = 0;
            canonical.tickCorrected = false;
            canonical.accepted = true;
            CanonicalWriter writer;
            writer.reserve( MAX_COMMAND_BYTES );
            serializeCommand( &writer, canonical );
            Mengine::Data data = writer.take();

            return data;
        }
        //////////////////////////////////////////////////////////////////////////
        static bool decodeCommand( const ackord_command & _source, CommandEnvelope * const _command )
        {
            CanonicalReader reader( _source.payload, _source.payload_size );
            if( deserializeCommand( &reader, _command ) == false )
            {
                return false;
            }
            if( reader.remaining() != 0 )
            {
                return false;
            }

            _command->matchId = _source.session;
            _command->playerId = _source.client;
            _command->sequence = _source.sequence;
            _command->appliedTick = _source.step;

            return true;
        }
        //////////////////////////////////////////////////////////////////////////
    }
    //////////////////////////////////////////////////////////////////////////
    ECommandStream getCommandStream( CommandType _type )
    {
        if( _type == CommandType::Pause || _type == CommandType::Resume || _type == CommandType::Restart )
        {
            return ECommandStream::Control;
        }

        return ECommandStream::Gameplay;
    }
    //////////////////////////////////////////////////////////////////////////
    CommandClient::CommandClient()
    {
        ackord_config config = Detail::commandConfig( MAXIMUM_PENDING_GAMEPLAY_COMMANDS );
        ackord_client_create( &config, &m_clients[0] );
        config.max_records = MAXIMUM_PENDING_CONTROL_COMMANDS;
        ackord_client_create( &config, &m_clients[1] );
    }
    //////////////////////////////////////////////////////////////////////////
    CommandClient::~CommandClient()
    {
        for( ackord_client * client : m_clients )
        {
            ackord_client_destroy( client );
        }
    }
    //////////////////////////////////////////////////////////////////////////
    bool CommandClient::isValid() const
    {
        if( m_clients[0] == nullptr )
        {
            return false;
        }
        if( m_clients[1] == nullptr )
        {
            return false;
        }

        return true;
    }
    //////////////////////////////////////////////////////////////////////////
    bool CommandClient::clear()
    {
        if( this->isValid() == false )
        {
            return false;
        }
        for( ackord_client * client : m_clients )
        {
            if( ackord_client_clear( client ) != ACKORD_OK )
            {
                return false;
            }
        }
        m_matchId = 0;
        m_playerId = 0;

        return true;
    }
    //////////////////////////////////////////////////////////////////////////
    bool CommandClient::reset( uint64_t _matchId, uint32_t _playerId )
    {
        if( this->isValid() == false )
        {
            return false;
        }
        for( ackord_client * client : m_clients )
        {
            ackord_status status = ackord_client_reset( client, _matchId, _playerId );
            if( status != ACKORD_OK && status != ACKORD_DUPLICATE )
            {
                return false;
            }
        }
        m_matchId = _matchId;
        m_playerId = _playerId;

        return true;
    }
    //////////////////////////////////////////////////////////////////////////
    bool CommandClient::makeCommand( CommandType _type, uint64_t _sourceTimeMs, uint64_t _predictedTick, const CommandPayload & _payload, CommandEnvelope * const _command )
    {
        if( _command == nullptr )
        {
            return false;
        }
        if( _type > CommandType::LeavePlayer )
        {
            return false;
        }
        if( _payload.button > InputButton::Crouch )
        {
            return false;
        }
        if( _payload.weapon >= WeaponType::Count )
        {
            return false;
        }
        CommandEnvelope command;
        command.matchId = m_matchId;
        command.playerId = m_playerId;
        command.sourceTimeMs = _sourceTimeMs;
        command.type = _type;
        command.payload = _payload;
        Mengine::Data data = Detail::encodeCommand( command );
        const void * bytes = data.data();
        size_t dataSize = data.size();
        if( dataSize > UINT32_MAX )
        {
            return false;
        }
        uint32_t size = static_cast<uint32_t>( dataSize );
        ECommandStream stream = getCommandStream( _type );
        ackord_client * client = m_clients[static_cast<size_t>(stream)];
        ackord_command queued;
        if( ackord_client_submit( client, _predictedTick, static_cast<uint32_t>( _type ), bytes, size, &queued ) != ACKORD_OK )
        {
            return false;
        }
        command.sequence = queued.sequence;
        command.appliedTick = queued.step;
        *_command = command;

        return true;
    }
    //////////////////////////////////////////////////////////////////////////
    VectorCommandEnvelope CommandClient::getPendingCommands( size_t _maximumCount ) const
    {
        VectorCommandEnvelope commands;
        size_t pendingCount = this->getPendingCount();
        size_t count = pendingCount < _maximumCount ? pendingCount : _maximumCount;
        commands.reserve( count );
        size_t streamIndices[] = {1, 0};
        for( size_t streamIndex : streamIndices )
        {
            uint64_t sequence = 0;
            ackord_command queued;
            while( commands.size() != count && ackord_client_pending_next( m_clients[streamIndex], sequence, &queued ) == ACKORD_OK )
            {
                CommandEnvelope command;
                if( Detail::decodeCommand( queued, &command ) == false )
                {
                    break;
                }
                commands.emplace_back( command );
                sequence = queued.sequence;
            }
        }

        return commands;
    }
    //////////////////////////////////////////////////////////////////////////
    size_t CommandClient::getPendingCount() const
    {
        size_t count = 0;
        for( ackord_client * client : m_clients )
        {
            count += ackord_client_pending_count( client );
        }

        return count;
    }
    //////////////////////////////////////////////////////////////////////////
    uint64_t CommandClient::getMatchId() const
    {
        return m_matchId;
    }
    //////////////////////////////////////////////////////////////////////////
    bool CommandClient::confirm( uint64_t _sequence, int32_t _status, ECommandStream _stream )
    {
        if( _stream > ECommandStream::Control )
        {
            return false;
        }
        ackord_client * client = m_clients[static_cast<size_t>(_stream)];
        ackord_confirmation confirmation{m_matchId, m_playerId, _sequence, 0, _status, nullptr, 0};
        ackord_status status = ackord_client_confirm( client, &confirmation );
        if( status != ACKORD_OK && status != ACKORD_DUPLICATE )
        {
            return false;
        }

        return true;
    }
    //////////////////////////////////////////////////////////////////////////
    bool CommandClient::validateProgress( uint64_t _sequence, uint64_t _completedMask, ECommandStream _stream ) const
    {
        if( _stream > ECommandStream::Control )
        {
            return false;
        }
        const ackord_client * client = m_clients[static_cast<size_t>(_stream)];
        ackord_status status = ackord_client_validate_progress( client, m_matchId, m_playerId, _sequence, _completedMask );
        if( status != ACKORD_OK )
        {
            return false;
        }

        return true;
    }
    //////////////////////////////////////////////////////////////////////////
    bool CommandClient::confirmProgress( uint64_t _sequence, uint64_t _completedMask, ECommandStream _stream )
    {
        if( _stream > ECommandStream::Control )
        {
            return false;
        }
        ackord_client * client = m_clients[static_cast<size_t>(_stream)];
        ackord_status status = ackord_client_confirm_progress( client, m_matchId, m_playerId, _sequence, _completedMask );
        if( status != ACKORD_OK )
        {
            return false;
        }

        return true;
    }
    //////////////////////////////////////////////////////////////////////////
    CommandServer::CommandServer( Simulation * _simulation )
        : m_simulation( _simulation )
    {
        ackord_config config = Detail::commandConfig( Detail::MAX_SERVER_RECORDS );
        config.max_clients = Detail::MAX_SESSION_CLIENTS * 2;
        m_players.reserve( Detail::MAX_SESSION_CLIENTS );
        ackord_server_create( &config, &m_server );
    }
    //////////////////////////////////////////////////////////////////////////
    CommandServer::~CommandServer()
    {
        ackord_server_destroy( m_server );
    }
    //////////////////////////////////////////////////////////////////////////
    bool CommandServer::reset()
    {
        uint64_t matchId = m_simulation->state().matchId;
        ackord_status status = ackord_server_reset( m_server, matchId );
        if( status == ACKORD_DUPLICATE )
        {
            return true;
        }
        if( status != ACKORD_OK )
        {
            return false;
        }
        m_commands.clear();
        m_players.clear();
        const SimulationState & state = m_simulation->state();
        for( const PlayerState & player : state.players )
        {
            if( player.active == false )
            {
                continue;
            }
            if( this->registerPlayer( player.id ) == false )
            {
                return false;
            }
        }

        return true;
    }
    //////////////////////////////////////////////////////////////////////////
    bool CommandServer::registerPlayer( uint32_t _playerId )
    {
        if( _playerId == 0 )
        {
            return false;
        }
        for( const PlayerStreamsDesc & player : m_players )
        {
            if( player.playerId == _playerId )
            {
                if( player.active == false )
                {
                    return false;
                }

                return true;
            }
        }
        if( m_players.size() == Detail::MAX_SESSION_CLIENTS )
        {
            return false;
        }
        PlayerStreamsDesc player;
        player.playerId = _playerId;
        player.gameplayId = static_cast<uint32_t>(m_players.size()) * 2 + 1;
        player.controlId = player.gameplayId + 1;
        ackord_status status = ackord_server_register( m_server, player.gameplayId );
        if( status != ACKORD_OK )
        {
            return false;
        }
        status = ackord_server_register( m_server, player.controlId );
        if( status != ACKORD_OK )
        {
            ackord_server_unregister( m_server, player.gameplayId );
            m_players.emplace_back( player );
            return false;
        }
        player.active = true;
        m_players.emplace_back( player );

        return true;
    }
    //////////////////////////////////////////////////////////////////////////
    void CommandServer::unregisterPlayer( uint32_t _playerId )
    {
        for( PlayerStreamsDesc & player : m_players )
        {
            if( player.playerId != _playerId )
            {
                continue;
            }
            ackord_server_unregister( m_server, player.gameplayId );
            ackord_server_unregister( m_server, player.controlId );
            player.active = false;
            return;
        }
    }
    //////////////////////////////////////////////////////////////////////////
    uint32_t CommandServer::getStreamId_( uint32_t _playerId, ECommandStream _stream ) const
    {
        if( _stream > ECommandStream::Control )
        {
            return 0;
        }
        for( const PlayerStreamsDesc & player : m_players )
        {
            if( player.playerId == _playerId )
            {
                return _stream == ECommandStream::Control ? player.controlId : player.gameplayId;
            }
        }

        return 0;
    }
    //////////////////////////////////////////////////////////////////////////
    ackord_status CommandServer::submit( const CommandEnvelope & _command )
    {
        if( _command.matchId != m_simulation->state().matchId )
        {
            return ACKORD_WRONG_SESSION;
        }
        ECommandStream stream = getCommandStream( _command.type );
        uint32_t streamId = this->getStreamId_( _command.playerId, stream );
        if( streamId == 0 )
        {
            return ACKORD_NOT_FOUND;
        }
        uint64_t requestedTick = _command.sourceTimeMs / TickMilliseconds + ( _command.sourceTimeMs % TickMilliseconds != 0u ? 1u : 0u );
        // Session controls share the authoritative clock, including while paused.
        if( stream == ECommandStream::Control )
        {
            requestedTick = m_simulation->state().tick;
            ackord_command_view retained;
            if( ackord_server_lookup( m_server, _command.matchId, streamId, _command.sequence, &retained ) == ACKORD_OK )
            {
                requestedTick = retained.command.step;
            }
        }
        Mengine::Data data = Detail::encodeCommand( _command );
        const void * bytes = data.data();
        size_t dataSize = data.size();
        if( dataSize > UINT32_MAX )
        {
            return ACKORD_INVALID;
        }
        uint32_t size = static_cast<uint32_t>( dataSize );
        ackord_command command{ _command.matchId, streamId, _command.sequence, requestedTick,
            static_cast<uint32_t>( _command.type ), bytes, size };
        ackord_status status = ackord_server_submit( m_server, &command );

        return status;
    }
    //////////////////////////////////////////////////////////////////////////
    bool CommandServer::tick()
    {
        m_commands.clear();
        uint64_t tick = m_simulation->state().tick;
        if( ackord_server_process_checked( m_server, tick, process_, this ) != ACKORD_OK )
        {
            m_commands.clear();
            return false;
        }

        return true;
    }
    //////////////////////////////////////////////////////////////////////////
    ackord_status CommandServer::process_( void * _user, const ackord_command * _commands, ackord_outcome * _outcomes, size_t _count ) noexcept
    {
        try
        {
            CommandServer * server = static_cast<CommandServer *>( _user );
            uint64_t tick = server->m_simulation->state().tick;
            Mengine::Vector<size_t> & commandIndices = server->m_commandIndices;
            commandIndices.resize( _count );
            for( size_t index = 0; index != _count; ++index )
            {
                commandIndices[index] = index;
            }
            Mengine::StdAlgorithm::sort( commandIndices.begin(), commandIndices.end(), [server, _commands]( size_t _left, size_t _right )
            {
                const ackord_command & left = _commands[_left];
                const ackord_command & right = _commands[_right];
                if( left.step != right.step )
                {
                    return left.step < right.step;
                }
                uint32_t leftPlayerId = server->m_players[(left.client - 1) / 2].playerId;
                uint32_t rightPlayerId = server->m_players[(right.client - 1) / 2].playerId;
                if( leftPlayerId != rightPlayerId )
                {
                    return leftPlayerId < rightPlayerId;
                }
                if( left.client != right.client )
                {
                    return left.client < right.client;
                }

                return left.sequence < right.sequence;
            } );
            for( size_t index : commandIndices )
            {
                CommandEnvelope command;
                bool decoded = Detail::decodeCommand( _commands[index], &command );
                command.accepted = decoded;
                size_t playerIndex = (_commands[index].client - 1) / 2;
                command.playerId = server->m_players[playerIndex].playerId;
                command.appliedTick = tick;
                uint64_t requestedTick = command.sourceTimeMs / TickMilliseconds + ( command.sourceTimeMs % TickMilliseconds != 0u ? 1u : 0u );
                command.tickCorrected = tick != requestedTick;
                server->m_commands.emplace_back( command );
            }
            VectorCommandResult results;
            server->m_simulation->step( server->m_commands, &results );
            if( results.size() != _count )
            {
                return ACKORD_HANDLER_FAILED;
            }
            for( size_t index = 0; index != _count; ++index )
            {
                size_t outcomeIndex = commandIndices[index];
                _outcomes[outcomeIndex].status = results[index].status;
            }
        }
        catch( ... )
        {
            return ACKORD_HANDLER_FAILED;
        }

        return ACKORD_OK;
    }
    //////////////////////////////////////////////////////////////////////////
    uint64_t CommandServer::getConfirmedSequence( uint32_t _playerId, ECommandStream _stream ) const
    {
        uint32_t streamId = this->getStreamId_( _playerId, _stream );
        uint64_t sequence = 0;
        ackord_server_confirmed( m_server, streamId, &sequence );

        return sequence;
    }
    //////////////////////////////////////////////////////////////////////////
    uint64_t CommandServer::getCompletedMask( uint32_t _playerId, ECommandStream _stream ) const
    {
        uint32_t streamId = this->getStreamId_( _playerId, _stream );
        uint64_t mask = 0;
        ackord_server_completed_mask( m_server, streamId, &mask );

        return mask;
    }
    //////////////////////////////////////////////////////////////////////////
    const VectorCommandEnvelope & CommandServer::getCommands() const
    {
        return m_commands;
    }
    //////////////////////////////////////////////////////////////////////////
    bool CommandServer::getNextJournalEntry( uint64_t _afterIndex, ackord_journal_entry * const _entry ) const
    {
        ackord_status status = ackord_server_journal_next( m_server, _afterIndex, _entry );
        if( status != ACKORD_OK )
        {
            return false;
        }

        size_t playerIndex = (_entry->command.client - 1) / 2;
        uint32_t playerId = m_players[playerIndex].playerId;
        _entry->command.client = playerId;
        _entry->confirmation.client = playerId;

        return true;
    }
    //////////////////////////////////////////////////////////////////////////
    void CommandServer::trimJournal( uint64_t _throughIndex )
    {
        ackord_server_journal_trim( m_server, _throughIndex );
    }
    //////////////////////////////////////////////////////////////////////////
}
