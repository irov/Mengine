#pragma once

#include "Arena3DNetworking.h"
#include "Arena3DCommandSession.h"

#include "Interface/SocketServiceInterface.h"

#include "Kernel/Factorable.h"

namespace Arena3D
{
    //////////////////////////////////////////////////////////////////////////
    enum class NetworkClientState : uint8_t
    {
        Disconnected,
        Connecting,
        Handshaking,
        Connected,
        Rejected
    };
    //////////////////////////////////////////////////////////////////////////
    class NetworkClient
        : public Mengine::Factorable
    {
        DECLARE_FACTORABLE( NetworkClient );

    public:
        explicit NetworkClient( Mengine::SocketServiceInterface * socketService, SimulationConfig config = SimulationConfig::defaults() );
        ~NetworkClient() override;

        bool connect( const Mengine::SocketAddress & address, Mengine::String name, NetworkClientKind kind );
        void updateNetwork();
        void tick();
        bool sendCommand( CommandType type, uint64_t sourceTimeMs, const CommandPayload & payload = {} );
        void disconnect( uint32_t reason = 0 );

        NetworkClientState state() const;
        uint32_t playerId() const;
        const Simulation & simulation() const;
        const VectorServerEvent & events() const;
        const VectorNetworkRosterEntry & roster() const;
        const Mengine::String & rejection() const;
        uint64_t reconciliationCount() const;
        size_t pendingCommandCount() const;

    private:
        void clearConnection_( NetworkClientState _state );
        bool sendHello_();
        void requestSnapshot_();
        bool sendSnapshotRequest_();
        void sendControlMessages_();
        void disconnectPeer_( Mengine::SocketPeerId _peerId, uint32_t _reason );
        bool sendPendingCommands_();
        void predictTick_();
        void predictToTick_( uint64_t tick );
        bool applyProgress_( uint64_t _acknowledged, uint64_t _completedMask, ECommandStream _stream );
        void onMessage_( uint8_t channel, const Mengine::Data & payload );

    private:
        Mengine::SocketServiceInterface * m_socketService;
        Simulation m_simulation;
        Mengine::SocketPeerId m_peerId = Mengine::InvalidSocketPeerId;
        Mengine::SocketPeerId m_disconnectPeerId = Mengine::InvalidSocketPeerId;
        uint32_t m_disconnectReason = 0;
        bool m_helloPending = false;
        bool m_snapshotRequestPending = false;
        NetworkClientState m_state = NetworkClientState::Disconnected;
        NetworkClientKind m_kind = NetworkClientKind::Human;
        Mengine::String m_name;
        Mengine::String m_rejection;
        uint32_t m_playerId = 0;
        Mengine::Array<uint64_t, 2> m_lastPredictedSequences{};
        uint64_t m_reconciliationCount = 0;
        uint32_t m_networkUpdateOrdinal = 0;
        CommandClient m_commands;
        VectorCommandEnvelope m_predictionCommands;
        VectorServerEvent m_events;
        VectorNetworkRosterEntry m_roster;
    };
    //////////////////////////////////////////////////////////////////////////
    typedef Mengine::IntrusivePtr<NetworkClient> NetworkClientPtr;
    //////////////////////////////////////////////////////////////////////////
}
//////////////////////////////////////////////////////////////////////////
