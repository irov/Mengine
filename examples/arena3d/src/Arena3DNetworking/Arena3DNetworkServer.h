#pragma once

#include "Arena3DNetworking.h"
#include "Arena3DCommandSession.h"

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
        struct Peer
        {
            Mengine::SocketPeerId peerId = Mengine::InvalidSocketPeerId;
            uint32_t playerId = 0;
            bool handshaken = false;
            bool welcomePending = false;
            bool rosterPending = false;
            bool rejectPending = false;
            Mengine::Data rejection;
            NetworkClientKind kind = NetworkClientKind::Human;
            Mengine::String name;
        };
        typedef Mengine::Vector<Peer> VectorPeer;

        void onConnected_( Mengine::SocketPeerId peerId );
        void onDisconnected_( Mengine::SocketPeerId peerId );
        void onMessage_( Mengine::SocketPeerId peerId, uint8_t channel, const Mengine::Data & payload );
        bool sendWelcome_( Peer * _peer );
        void sendReject_( Mengine::SocketPeerId peerId, uint32_t reason, const char * message );
        void queueRoster_();
        bool sendRoster_( Peer * _peer );
        void sendControlMessages_();
        void sendStateFrames_();
        void sendConfirmations_();
        Peer * findPeer_( Mengine::SocketPeerId peerId );

    private:
        Mengine::SocketServiceInterface * m_socketService;
        Simulation m_simulation;
        VectorPeer m_peers;
        VectorNetworkRosterEntry m_roster;
        CommandServer m_commands;
        VectorServerEvent m_eventHistory;
        uint32_t m_maximumPlayers = MaximumPlayers;
        uint32_t m_stateFrameTicks = 0;
    };
    //////////////////////////////////////////////////////////////////////////
}
//////////////////////////////////////////////////////////////////////////
