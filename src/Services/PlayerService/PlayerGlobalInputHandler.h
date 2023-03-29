#pragma once

#include "Interface/GlobalInputHandlerInterface.h"

#include "Kernel/Factorable.h"
#include "Kernel/Vector.h"
#include "Kernel/String.h"

#include "Config/UniqueId.h"

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
        UniqueId addGlobalHandler( const InputHandlerInterfacePtr & _handler, const DocumentPtr & _doc ) override;
        InputHandlerInterfacePtr removeGlobalHandler( UniqueId _id ) override;

    public:
        UniqueId addGlobalKeyHandler( EKeyCode _code, const LambdaKeyHandler & _lambda, const DocumentPtr & _doc ) override;

    public:
        bool enableGlobalHandler( UniqueId _id, bool _value ) override;

    public:
        void update() override;

    public:
        bool handleKeyEvent( const InputKeyEvent & _event ) override;
        bool handleTextEvent( const InputTextEvent & _event ) override;

    public:
        bool handleMouseButtonEvent( const InputMouseButtonEvent & _event ) override;
        bool handleMouseButtonEventBegin( const InputMouseButtonEvent & _event ) override;
        bool handleMouseButtonEventEnd( const InputMouseButtonEvent & _event ) override;
        bool handleMouseMove( const InputMouseMoveEvent & _event ) override;
        bool handleMouseWheel( const InputMouseWheelEvent & _event ) override;

    public:
        bool handleMouseEnter( const InputMouseEnterEvent & _event ) override;
        void handleMouseLeave( const InputMouseLeaveEvent & _event ) override;

    protected:
        struct GlobalHandlerDesc
        {
            UniqueId id;

            InputHandlerInterfacePtr handler;
            bool enable;
            bool dead;

#if defined(MENGINE_DOCUMENT_ENABLE)
            DocumentPtr doc;
#endif
        };

        typedef Vector<GlobalHandlerDesc> VectorGlobalHandler;
        VectorGlobalHandler m_handlers;
        VectorGlobalHandler m_handlersAdd;
    };
}
