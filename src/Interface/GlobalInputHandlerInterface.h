#pragma once

#include "Interface/InputHandlerInterface.h"
#include "Interface/DocumentInterface.h"

#include "Config/Lambda.h"
#include "Config/UniqueId.h"

namespace Mengine
{
    //////////////////////////////////////////////////////////////////////////
    class GlobalInputHandlerInterface
        : public InputHandlerInterface
    {
    public:
        virtual bool initialize() = 0;
        virtual void finalize() = 0;

    public:
        virtual UniqueId addGlobalHandler( const InputHandlerInterfacePtr & _handler, const DocumentInterfacePtr & _doc ) = 0;
        virtual InputHandlerInterfacePtr removeGlobalHandler( UniqueId _id ) = 0;

    public:
        virtual bool enableGlobalHandler( UniqueId _id, bool _value ) = 0;

    public:
        virtual void update() = 0;
    };
    //////////////////////////////////////////////////////////////////////////
    typedef IntrusivePtr<GlobalInputHandlerInterface> GlobalInputHandlerInterfacePtr;
    //////////////////////////////////////////////////////////////////////////
}