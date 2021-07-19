#pragma once

#include "Interface/InputHandlerInterface.h"

#include "Kernel/Document.h"

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
        virtual UniqueId addGlobalHandler( const InputHandlerInterfacePtr & _handler, const DocumentPtr & _doc ) = 0;
        virtual InputHandlerInterfacePtr removeGlobalHandler( UniqueId _id ) = 0;

    public:
        typedef Lambda<void( const InputKeyEvent & _event )> LambdaKeyHandler;
        virtual UniqueId addGlobalKeyHandler( EKeyCode _code, const LambdaKeyHandler & _lambda, const DocumentPtr & _doc ) = 0;

    public:
        virtual bool enableGlobalHandler( UniqueId _id, bool _value ) = 0;
    };
    //////////////////////////////////////////////////////////////////////////
    typedef IntrusivePtr<GlobalInputHandlerInterface> GlobalInputHandlerInterfacePtr;
    //////////////////////////////////////////////////////////////////////////
}