#pragma once

#include "Arena3DSimulation.h"

#include <cstdint>
#include <deque>
#include <vector>

namespace Arena3D
{
    struct ServerFrame
    {
        uint64_t tick = 0;
        uint64_t acknowledgedSequence = 0;
        StateChecksum checksum;
        std::vector<CommandEnvelope> commands;
        std::vector<ServerEvent> events;
        std::vector<uint8_t> snapshot;
    };

    class ICommandTransport
    {
    public:
        virtual ~ICommandTransport() = default;
        virtual void sendCommand( const CommandEnvelope & command ) = 0;
        virtual std::vector<CommandEnvelope> takeServerCommands() = 0;
        virtual void publishServerFrame( ServerFrame frame ) = 0;
        virtual bool takeClientFrame( ServerFrame * frame ) = 0;
    };

    class LocalCommandTransport final
        : public ICommandTransport
    {
    public:
        void sendCommand( const CommandEnvelope & command ) override;
        std::vector<CommandEnvelope> takeServerCommands() override;
        void publishServerFrame( ServerFrame frame ) override;
        bool takeClientFrame( ServerFrame * frame ) override;

    private:
        std::vector<CommandEnvelope> m_toServer;
        std::deque<ServerFrame> m_toClient;
    };

    class LocalSession
    {
    public:
        explicit LocalSession( SimulationConfig config = SimulationConfig::defaults() );

        void reset( uint64_t matchId, uint64_t seed );
        CommandEnvelope makeCommand( CommandType type, uint64_t sourceTimeMs, const CommandPayload & payload = {} );
        void submit( const CommandEnvelope & command );
        bool tick();
        bool advanceTicks( uint64_t count );

        const Simulation & server() const;
        const Simulation & client() const;
        const std::vector<CommandEnvelope> & journal() const;
        const std::vector<ServerEvent> & eventJournal() const;
        bool synchronized() const;
        uint64_t reconciliationCount() const;
        void injectClientFaultForTest( int64_t rawDelta );

    private:
        void receiveCommands_();
        std::vector<CommandEnvelope> commandsForCurrentTick_();
        void consumeClientFrames_();

    private:
        SimulationConfig m_config;
        Simulation m_server;
        Simulation m_client;
        LocalCommandTransport m_transport;
        std::vector<CommandEnvelope> m_scheduled;
        std::vector<CommandEnvelope> m_journal;
        std::vector<ServerEvent> m_eventJournal;
        uint64_t m_nextSequence = 1;
        uint64_t m_lastAcceptedSequence = 0;
        uint64_t m_reconciliationCount = 0;
        bool m_synchronized = true;
    };
}
