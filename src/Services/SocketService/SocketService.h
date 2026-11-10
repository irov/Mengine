#pragma once

#include "Interface/SocketServiceInterface.h"
#include "Interface/ThreadIdentityInterface.h"
#include "Interface/ThreadMutexInterface.h"

#include "Kernel/Array.h"
#include "Kernel/Deque.h"
#include "Kernel/ServiceBase.h"
#include "Kernel/Vector.h"

#include "Config/Atomic.h"

namespace Mengine
{
    //////////////////////////////////////////////////////////////////////////
    class SocketService
        : public ServiceBase<SocketServiceInterface>
    {
        DECLARE_FACTORABLE( SocketService );

    public:
        SocketService();
        ~SocketService() override;

    protected:
        const ServiceRequiredList & requiredServices() const override;
        bool _initializeService() override;
        void _finalizeService() override;

    protected:
        bool listen( const SocketAddress & _address, uint32_t _maximumPeers ) override;
        bool connect( const SocketAddress & _address, SocketPeerId * const _peerId ) override;
        bool disconnect( SocketPeerId _peerId, uint32_t _reason ) override;
        bool send( SocketPeerId _peerId, uint8_t _channel, ESocketDelivery _delivery, const void * _data, size_t _size ) override;
        bool broadcast( uint8_t _channel, ESocketDelivery _delivery, const void * _data, size_t _size ) override;
        bool pollEvent( SocketServiceEvent * const _event ) override;
        void stop() override;

    protected:
        //////////////////////////////////////////////////////////////////////////
        enum class ECommandType : uint8_t
        {
            Listen,
            Connect,
            Disconnect,
            Send,
            Broadcast,
            Stop
        };
        //////////////////////////////////////////////////////////////////////////
        enum class EPeerState : uint8_t
        {
            Connecting,
            Connected,
            Disconnected
        };
        typedef Vector<SocketPeerId> VectorSocketPeerId;
        //////////////////////////////////////////////////////////////////////////
        struct Command
        {
            ECommandType type = ECommandType::Stop;
            SocketPeerId peerId = InvalidSocketPeerId;
            SocketAddress address;
            uint32_t value = 0;
            uint8_t channel = 0;
            ESocketDelivery delivery = ESocketDelivery::UnreliableSequenced;
            Data payload;
            VectorSocketPeerId recipients;
            bool recipientsInitialized = false;
        };
        //////////////////////////////////////////////////////////////////////////
        struct PendingPacket
        {
            uint32_t packetSequence = 0;
            uint32_t messageId = 0;
            uint32_t channelSequence = 0;
            uint32_t totalSize = 0;
            uint16_t fragmentIndex = 0;
            uint16_t fragmentCount = 0;
            uint8_t channel = 0;
            ESocketDelivery delivery = ESocketDelivery::ReliableOrdered;
            uint64_t lastSendMs = 0;
            uint32_t retryCount = 0;
            Data payload;
        };
        //////////////////////////////////////////////////////////////////////////
        typedef Vector<Data> VectorData;
        //////////////////////////////////////////////////////////////////////////
        struct Reassembly
        {
            uint32_t messageId = 0;
            uint32_t channelSequence = 0;
            uint32_t totalSize = 0;
            uint16_t fragmentCount = 0;
            uint8_t channel = 0;
            ESocketDelivery delivery = ESocketDelivery::UnreliableSequenced;
            uint64_t createdMs = 0;
            VectorData fragments;
            Data present;
        };
        //////////////////////////////////////////////////////////////////////////
        typedef Array<uint32_t, 4> ArrayChannelSequence;
        typedef Vector<PendingPacket> VectorPendingPacket;
        typedef Vector<Reassembly> VectorReassembly;
        //////////////////////////////////////////////////////////////////////////
        struct Peer
        {
            SocketPeerId id = InvalidSocketPeerId;
            SocketAddress address;
            EPeerState state = EPeerState::Connecting;
            uint32_t disconnectReason = 0;
            uint32_t connectionId = 0;
            uint32_t nextPacketSequence = 1;
            uint32_t nextMessageId = 1;
            bool receiveInitialized = false;
            bool ackPending = false;
            uint32_t receiveSequence = 0;
            uint32_t receiveMask = 0;
            ArrayChannelSequence sendReliableChannelSequence{{1, 1, 1, 1}};
            ArrayChannelSequence sendUnreliableChannelSequence{{1, 1, 1, 1}};
            ArrayChannelSequence receiveChannelSequence{{1, 1, 1, 1}};
            ArrayChannelSequence lastUnreliableSequence{{0, 0, 0, 0}};
            VectorPendingPacket pending;
            VectorReassembly reassemblies;
            uint8_t blockedChannels = 0;
            uint64_t createdMs = 0;
            uint64_t lastReceiveMs = 0;
            uint64_t lastSendMs = 0;
            uint64_t lastConnectSendMs = 0;
            uint32_t rttMs = 0;
            double bandwidthTokens = 0.0;
            double bandwidthBytesPerMs = 0.0;
            double bandwidthCapacity = 0.0;
            uint64_t bandwidthTimestampMs = 0;
        };
        //////////////////////////////////////////////////////////////////////////
        typedef Deque<Command> DequeCommand;
        typedef Deque<SocketServiceEvent> DequeSocketServiceEvent;
        typedef Vector<Peer> VectorPeer;

    private:
        static bool sequenceNewer_( uint32_t _left, uint32_t _right );
        static bool sequenceAcknowledged_( uint32_t _sequence, uint32_t _ack, uint32_t _ackBits );
        static bool sequenceMarkReceived_( uint32_t _sequence, bool * const _initialized, uint32_t * const _latest, uint32_t * const _mask );
        static bool pendingNeedsSend_( const Peer & _peer, const PendingPacket & _pending, uint64_t _now );

    protected:
        SocketPeerId allocatePeerId_();
        bool enqueueCommand_( Command && _command );
        bool process_( const ThreadIdentityRunnerInterfacePtr & _runner );
        void processCommands_( uint64_t _now );
        bool processCommand_( Command & _command, uint64_t _now );
        void receivePackets_( uint64_t _now );
        void processPacket_( const SocketAddress & _address, const uint8_t * _packet, size_t _size, uint64_t _now );
        void updatePeers_( uint64_t _now );
        void openEndpoint_( const SocketAddress & _address, bool _server, uint32_t _maximumPeers );
        void closeEndpoint_();
        Peer * findPeer_( SocketPeerId _peerId );
        Peer * findPeer_( const SocketAddress & _address, uint32_t _connectionId );
        Peer * createPeer_( SocketPeerId _peerId, const SocketAddress & _address, uint32_t _connectionId, EPeerState _state, uint64_t _now );
        void clearPeers_();
        bool removePeer_( size_t _index, uint32_t _reason, bool _notify );
        void sendConnect_( Peer * _peer, uint64_t _now );
        void sendAccept_( Peer * _peer, uint64_t _now );
        void sendDisconnect_( Peer * _peer, uint32_t _reason, uint64_t _now );
        bool queueMessage_( Peer * _peer, uint8_t _channel, ESocketDelivery _delivery, const Data & _payload, uint64_t _now );
        bool sendPending_( Peer * _peer, PendingPacket * _pending, uint16_t _flags, uint64_t _now, bool _retransmit, bool _bandwidthReserved );
        bool sendControl_( Peer * _peer, uint16_t _flags, uint32_t _reason, uint64_t _now );
        void sendAcknowledgement_( Peer * _peer, uint32_t _sequence, uint64_t _now );
        bool sendPacket_( Peer * _peer, uint16_t _flags, uint32_t _messageId, uint32_t _channelSequence, uint32_t _totalSize, uint16_t _fragmentIndex, uint16_t _fragmentCount, uint8_t _channel, ESocketDelivery _delivery, const uint8_t * _payload, size_t _payloadSize, uint32_t * const _packetSequence, uint64_t _now, bool _bandwidthReserved, uint32_t _ackSequence = 0 );
        void acknowledge_( Peer * _peer, uint32_t _ack, uint32_t _ackBits, uint64_t _now );
        bool markReceived_( Peer * _peer, uint32_t _sequence );
        bool receiveFragment_( Peer * _peer, uint32_t _messageId, uint32_t _channelSequence, uint32_t _totalSize, uint16_t _fragmentIndex, uint16_t _fragmentCount, uint8_t _channel, ESocketDelivery _delivery, const uint8_t * _payload, size_t _payloadSize, uint64_t _now );
        bool deliverMessage_( Peer * _peer, uint8_t _channel, ESocketDelivery _delivery, uint32_t _channelSequence, Data && _payload );
        bool pushEvent_( SocketServiceEvent && _event );
        bool consumeBandwidth_( Peer * _peer, size_t _bytes, bool _control, uint64_t _now );

    protected:
        DatagramInterfacePtr m_socket;
        ThreadIdentityInterfacePtr m_thread;
        ThreadMutexInterfacePtr m_commandMutex;
        ThreadMutexInterfacePtr m_eventMutex;
        DequeCommand m_commands;
        DequeSocketServiceEvent m_events;
        VectorPeer m_peers;
        size_t m_commandCount;
        Mengine::AtomicUInt32 m_nextPeerId;
        Mengine::AtomicUInt32 m_nextConnectionId;
        uint32_t m_maximumPeers;
        bool m_server;
        Mengine::AtomicBool m_stopping;
    };
    //////////////////////////////////////////////////////////////////////////
}
//////////////////////////////////////////////////////////////////////////
