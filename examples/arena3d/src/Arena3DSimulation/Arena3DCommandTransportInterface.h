#pragma once

#include "Arena3DServerFrame.h"

#include "Kernel/Factorable.h"
#include "Kernel/IntrusivePtr.h"

namespace Arena3D
{
    //////////////////////////////////////////////////////////////////////////
    class ICommandTransport
        : public Mengine::Factorable
    {
    public:
        virtual void sendCommand( const CommandEnvelope & _command ) = 0;
        virtual VectorCommandEnvelope takeServerCommands() = 0;
        virtual void publishServerFrame( ServerFrame _frame ) = 0;
        virtual bool takeClientFrame( ServerFrame * _frame ) = 0;
    };
    //////////////////////////////////////////////////////////////////////////
    typedef Mengine::IntrusivePtr<ICommandTransport> ICommandTransportPtr;
    //////////////////////////////////////////////////////////////////////////
}
//////////////////////////////////////////////////////////////////////////
