#pragma once

#include "Arena3DLocalCommandTransport.h"

#include "Kernel/Vector.h"

#include "Config/StdInt.h"

namespace Arena3D
{
    //////////////////////////////////////////////////////////////////////////
    class LocalSession
    {
    public:
        explicit LocalSession( SimulationConfig config = SimulationConfig::defaults() );

        void reset( uint64_t matchId, uint64_t seed );
        CommandEnvelope makeCommand( CommandType type, uint64_t sourceTimeMs, const CommandPayload & payload = {} );
        void submit( const CommandEnvelope & command );
        bool tick();

        const Simulation & server() const;
        const Simulation & client() const;
        const VectorCommandEnvelope & journal() const;
        const VectorServerEvent & eventJournal() const;
        uint64_t reconciliationCount() const;

    private:
        void receiveCommands_();
        VectorCommandEnvelope commandsForCurrentTick_();
        void consumeClientFrames_();

    private:
        SimulationConfig m_config;
        Simulation m_server;
        Simulation m_client;
        LocalCommandTransportPtr m_transport;
        VectorCommandEnvelope m_scheduled;
        VectorCommandEnvelope m_journal;
        VectorServerEvent m_eventJournal;
        uint64_t m_nextSequence = 1;
        uint64_t m_lastAcceptedSequence = 0;
        uint64_t m_reconciliationCount = 0;
        bool m_synchronized = true;
    };
    //////////////////////////////////////////////////////////////////////////
}
//////////////////////////////////////////////////////////////////////////
