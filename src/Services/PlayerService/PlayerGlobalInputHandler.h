#pragma once

#include "Interface/GlobalInputHandlerInterface.h"

#include "Kernel/Factorable.h"

#include "Config/Vector.h"
#include "Config/String.h"

namespace Mengine
{
    //////////////////////////////////////////////////////////////////////////
    class PlayerGlobalInputHandler
        : public GlobalInputHandlerInterface
        , public Factorable
    {
    public:
        PlayerGlobalInputHandler();
        ~PlayerGlobalInputHandler() override;

    public:
        bool initialize() override;
        void finalize() override;

    public:
        uint32_t addGlobalHandler( const InputHandlerInterfacePtr & _handler, const Char * _doc ) override;
        InputHandlerInterfacePtr removeGlobalHandler( uint32_t _id ) override;

    public:
        uint32_t addGlobalKeyHandler( KeyCode _code, const LambdaKeyHandler & _lambda, const Char * _doc ) override;

    public:
        bool enableGlobalHandler( uint32_t _id, bool _value ) override;

    public:
        void update() override;
        void clear() override;

    public:
        bool handleKeyEvent( const InputKeyEvent & _event ) override;
        bool handleTextEvent( const InputTextEvent & _event ) override;

    public:
        bool handleMouseButtonEvent( const InputMouseButtonEvent & _event ) override;
        bool handleMouseButtonEventBegin( const InputMouseButtonEvent & _event ) override;
        bool handleMouseButtonEventEnd( const InputMouseButtonEvent & _event ) override;
        bool handleMouseMove( const InputMouseMoveEvent & _event ) override;
        bool handleMouseWheel( const InputMouseWheelEvent & _event ) override;

    protected:
        struct GlobalHandlerDesc
        {
            uint32_t id;

            InputHandlerInterfacePtr handler;
            bool enable;
            bool dead;
#ifdef MENGINE_DEBUG
            String doc;
#endif
        };

        typedef Vector<GlobalHandlerDesc> VectorGlobalHandler;
        VectorGlobalHandler m_handlers;
        VectorGlobalHandler m_handlersAdd;
    };
}
