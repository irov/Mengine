#pragma once

#include "Arena3DCommandTransportInterface.h"

#include "Kernel/Deque.h"
#include "Kernel/IntrusivePtr.h"

namespace Arena3D
{
    //////////////////////////////////////////////////////////////////////////
    typedef Mengine::Deque<ServerFrame> DequeServerFrame;
    //////////////////////////////////////////////////////////////////////////
    class LocalCommandTransport
        : public ICommandTransport
    {
        DECLARE_FACTORABLE( LocalCommandTransport );

    public:
        void clear();

        void sendCommand( const CommandEnvelope & _command ) override;
        VectorCommandEnvelope takeServerCommands() override;
        void publishServerFrame( ServerFrame _frame ) override;
        bool takeClientFrame( ServerFrame * _frame ) override;

    private:
        VectorCommandEnvelope m_toServer;
        DequeServerFrame m_toClient;
    };
    //////////////////////////////////////////////////////////////////////////
    typedef Mengine::IntrusivePtr<LocalCommandTransport> LocalCommandTransportPtr;
    //////////////////////////////////////////////////////////////////////////
}
//////////////////////////////////////////////////////////////////////////
