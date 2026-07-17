#pragma once

#include "Arena3DNetworking.h"

#include "Interface/SocketServiceInterface.h"

namespace Arena3D
{
    //////////////////////////////////////////////////////////////////////////
    class NetworkServer
    {
    public:
        explicit NetworkServer( Mengine::SocketServiceInterface * socketService, SimulationConfig config = SimulationConfig::defaults() );
        ~NetworkServer();

        bool listen( const Mengine::SocketAddress & address, uint32_t maximumPlayers = MaximumPlayers );
        void updateNetwork();
        void tick();

        const Simulation & simulation() const;
        const VectorNetworkRosterEntry & roster() const;

    private:
        //////////////////////////////////////////////////////////////////////////
        struct Peer
        {
            Mengine::SocketPeerId peerId = Mengine::InvalidSocketPeerId;
            uint32_t playerId = 0;
            uint64_t lastReceivedSequence = 0;
            uint64_t lastAppliedSequence = 0;
            bool handshaken = false;
            NetworkClientKind kind = NetworkClientKind::Human;
            Mengine::String name;
        };
        //////////////////////////////////////////////////////////////////////////
        typedef Mengine::Vector<Peer> VectorPeer;

        void onConnected_( Mengine::SocketPeerId peerId );
        void onDisconnected_( Mengine::SocketPeerId peerId );
        void onMessage_( Mengine::SocketPeerId peerId, uint8_t channel, const Mengine::Data & payload );
        void sendWelcome_( Peer * peer );
        void sendReject_( Mengine::SocketPeerId peerId, uint32_t reason, const char * message );
        void sendRoster_();
        void sendStateFrames_();
        Peer * findPeer_( Mengine::SocketPeerId peerId );

    private:
        Mengine::SocketServiceInterface * m_socketService;
        Simulation m_simulation;
        VectorPeer m_peers;
        VectorNetworkRosterEntry m_roster;
        VectorCommandEnvelope m_scheduled;
        VectorServerEvent m_eventHistory;
        uint32_t m_maximumPlayers = MaximumPlayers;
    };
    //////////////////////////////////////////////////////////////////////////
}
//////////////////////////////////////////////////////////////////////////
