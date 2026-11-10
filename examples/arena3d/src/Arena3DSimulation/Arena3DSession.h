#pragma once

#include "Arena3DCommandSession.h"
#include "Arena3DReplay.h"

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
        bool submit( const CommandEnvelope & _command );
        bool tick();

        const Simulation & server() const;
        const Simulation & client() const;
        const VectorCommandEnvelope & journal() const;
        const VectorServerEvent & eventJournal() const;
        const VectorReplayBatch & batchJournal() const;
        uint64_t reconciliationCount() const;

    private:
        SimulationConfig m_config;
        Simulation m_server;
        Simulation m_client;
        CommandServer m_commandServer;
        CommandClient m_commandClient;
        VectorCommandEnvelope m_journal;
        VectorServerEvent m_eventJournal;
        VectorReplayBatch m_batchJournal;
        uint64_t m_reconciliationCount = 0;
        bool m_hasActivity = false;
        bool m_retryPending = false;
        bool m_synchronized = true;
    };
    //////////////////////////////////////////////////////////////////////////
}
//////////////////////////////////////////////////////////////////////////
