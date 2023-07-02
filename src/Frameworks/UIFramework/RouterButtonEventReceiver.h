#pragma once

#include "Kernel/Factorable.h"
#include "Kernel/ConstString.h"
#include "Kernel/Hashtable.h"

#include "ButtonEventReceiverInterface.h"
#include "Button.h"

namespace Mengine
{
    class RouterButtonEventReceiver
        : public ButtonEventReceiverInterface
        , public Factorable
    {
    public:
        RouterButtonEventReceiver();
        ~RouterButtonEventReceiver() override;

    public:
        void addButtonEventReceiver( const ConstString & _state, const ButtonEventReceiverInterfacePtr & _button );
        ButtonEventReceiverInterfacePtr removeButtonEventReceiver( const ConstString & _state );

    public:
        void setState( const ConstString & _state );

    protected:
        void onButtonMouseEnter() override;
        void onButtonMouseLeave() override;
        void onButtonMousePush() override;
        void onButtonMousePressed() override;
        void onButtonMouseRelease() override;
        void onButtonMouseButton() override;
        void onButtonMouseButtonBegin() override;
        void onButtonMouseButtonEnd() override;

    protected:
        ConstString m_state;

        typedef Hashtable<ConstString, ButtonEventReceiverInterfacePtr> HashtableButtonEventReceiverInterfacePtrs;
        HashtableButtonEventReceiverInterfacePtrs m_receivers;
    };
    //////////////////////////////////////////////////////////////////////////
    typedef IntrusivePtr<RouterButtonEventReceiver> RouterButtonEventReceiverPtr;
    //////////////////////////////////////////////////////////////////////////
    namespace Helper
    {
        RouterButtonEventReceiverPtr makeRouterButtonEventReceiver( const ButtonPtr & _button, const DocumentInterfacePtr & _doc );
    }
    //////////////////////////////////////////////////////////////////////////
}