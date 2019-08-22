#pragma once

#include "Interface/InputHandlerInterface.h"

#include "Config/Lambda.h"

namespace Mengine
{
    class GlobalInputHandlerInterface
        : public InputHandlerInterface
    {
    public:
        virtual bool initialize() = 0;
        virtual void finalize() = 0;

    public:
        virtual uint32_t addGlobalHandler( const InputHandlerInterfacePtr & _handler, const Char * _doc ) = 0;
        virtual InputHandlerInterfacePtr removeGlobalHandler( uint32_t _id ) = 0;

    public:
        typedef Lambda<void( const InputKeyEvent & _event )> LambdaKeyHandler;
        virtual uint32_t addGlobalKeyHandler( EKeyCode _code, const LambdaKeyHandler & _lambda, const Char * _doc ) = 0;

    public:
        virtual bool enableGlobalHandler( uint32_t _id, bool _value ) = 0;

    public:
        virtual void update() = 0;
    };
    //////////////////////////////////////////////////////////////////////////
    typedef IntrusivePtr<GlobalInputHandlerInterface> GlobalInputHandlerInterfacePtr;
    //////////////////////////////////////////////////////////////////////////
}