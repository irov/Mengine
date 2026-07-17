#pragma once

#include "Interface/ServiceInterface.h"
#include "Interface/DatagramInterface.h"

#include "Kernel/Data.h"

#include "Config/StdDef.h"
#include "Config/StdInt.h"

namespace Mengine
{
    //////////////////////////////////////////////////////////////////////////
    typedef uint32_t SocketPeerId;
    static constexpr SocketPeerId InvalidSocketPeerId = 0;
    //////////////////////////////////////////////////////////////////////////
    enum class ESocketDelivery : uint8_t
    {
        ReliableOrdered,
        UnreliableSequenced
    };
    //////////////////////////////////////////////////////////////////////////
    enum class ESocketServiceEventType : uint8_t
    {
        PeerConnected,
        PeerDisconnected,
        MessageReceived,
        TransportError
    };
    //////////////////////////////////////////////////////////////////////////
    struct SocketServiceEvent
    {
        ESocketServiceEventType type = ESocketServiceEventType::TransportError;
        SocketPeerId peerId = InvalidSocketPeerId;
        uint8_t channel = 0;
        uint32_t reason = 0;
        Data payload;
        ESocketDelivery delivery = ESocketDelivery::ReliableOrdered;
    };
    //////////////////////////////////////////////////////////////////////////
    class SocketServiceInterface
        : public ServiceInterface
    {
        SERVICE_DECLARE( "SocketService" )

    public:
        virtual bool listen( const SocketAddress & _address, uint32_t _maximumPeers ) = 0;
        virtual bool connect( const SocketAddress & _address, SocketPeerId * const _peerId ) = 0;
        virtual bool disconnect( SocketPeerId _peerId, uint32_t _reason ) = 0;
        virtual bool send( SocketPeerId _peerId, uint8_t _channel, ESocketDelivery _delivery, const void * _data, size_t _size ) = 0;
        virtual bool broadcast( uint8_t _channel, ESocketDelivery _delivery, const void * _data, size_t _size ) = 0;
        virtual bool pollEvent( SocketServiceEvent * const _event ) = 0;
        virtual void stop() = 0;
    };
    //////////////////////////////////////////////////////////////////////////
}
//////////////////////////////////////////////////////////////////////////
#define SOCKET_SERVICE()\
    ((Mengine::SocketServiceInterface *)SERVICE_GET(Mengine::SocketServiceInterface))
//////////////////////////////////////////////////////////////////////////
