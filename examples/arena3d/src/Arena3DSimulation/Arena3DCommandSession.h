#pragma once

#include "Arena3DSimulation.h"

#include "Kernel/Array.h"

#include <ackord/ackord.hpp>

namespace Arena3D
{
    //////////////////////////////////////////////////////////////////////////
    enum class ECommandStream : uint8_t
    {
        Gameplay,
        Control
    };
    //////////////////////////////////////////////////////////////////////////
    static const size_t MAXIMUM_PENDING_GAMEPLAY_COMMANDS = 256;
    static const size_t MAXIMUM_PENDING_CONTROL_COMMANDS = 64;
    //////////////////////////////////////////////////////////////////////////
    ECommandStream getCommandStream( CommandType _type );
    //////////////////////////////////////////////////////////////////////////
    class CommandClient
    {
    public:
        CommandClient();
        ~CommandClient();

        CommandClient( const CommandClient & ) = delete;
        CommandClient & operator=( const CommandClient & ) = delete;

        bool isValid() const;
        bool clear();
        bool reset( uint64_t _matchId, uint32_t _playerId );
        bool makeCommand( CommandType _type, uint64_t _sourceTimeMs, uint64_t _predictedTick, const CommandPayload & _payload, CommandEnvelope * const _command );
        VectorCommandEnvelope getPendingCommands( size_t _maximumCount = SIZE_MAX ) const;
        size_t getPendingCount() const;
        uint64_t getMatchId() const;
        bool confirm( uint64_t _sequence, int32_t _status, ECommandStream _stream = ECommandStream::Gameplay );
        bool validateProgress( uint64_t _sequence, uint64_t _completedMask, ECommandStream _stream = ECommandStream::Gameplay ) const;
        bool confirmProgress( uint64_t _sequence, uint64_t _completedMask, ECommandStream _stream = ECommandStream::Gameplay );

    private:
        Mengine::Array<ackord_client *, 2> m_clients{};
        uint64_t m_matchId = 0;
        uint32_t m_playerId = 0;
    };
    //////////////////////////////////////////////////////////////////////////
    class CommandServer
    {
    public:
        explicit CommandServer( Simulation * _simulation );
        ~CommandServer();

        CommandServer( const CommandServer & ) = delete;
        CommandServer & operator=( const CommandServer & ) = delete;

        bool reset();
        bool registerPlayer( uint32_t _playerId );
        void unregisterPlayer( uint32_t _playerId );
        ackord_status submit( const CommandEnvelope & _command );
        bool tick();
        uint64_t getConfirmedSequence( uint32_t _playerId, ECommandStream _stream = ECommandStream::Gameplay ) const;
        uint64_t getCompletedMask( uint32_t _playerId, ECommandStream _stream = ECommandStream::Gameplay ) const;
        const VectorCommandEnvelope & getCommands() const;
        bool getNextJournalEntry( uint64_t _afterIndex, ackord_journal_entry * const _entry ) const;
        void trimJournal( uint64_t _throughIndex );

    private:
        struct PlayerStreamsDesc
        {
            uint32_t playerId = 0;
            uint32_t gameplayId = 0;
            uint32_t controlId = 0;
            bool active = false;
        };

        static ackord_status process_( void * _user, const ackord_command * _commands, ackord_outcome * _outcomes, size_t _count ) noexcept;
        uint32_t getStreamId_( uint32_t _playerId, ECommandStream _stream ) const;

    private:
        Simulation * m_simulation;
        ackord_server * m_server = nullptr;
        Mengine::Vector<PlayerStreamsDesc> m_players;
        Mengine::Vector<size_t> m_commandIndices;
        VectorCommandEnvelope m_commands;
    };
    //////////////////////////////////////////////////////////////////////////
}
