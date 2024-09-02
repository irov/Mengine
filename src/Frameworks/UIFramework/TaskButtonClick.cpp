#include "TaskButtonClick.h"

#include "ButtonEventReceiverInterface.h"

#include "Kernel/Assertion.h"
#include "Kernel/FactorableUnique.h"
#include "Kernel/Eventable.h"

namespace Mengine
{
    //////////////////////////////////////////////////////////////////////////
    namespace Detail
    {
        //////////////////////////////////////////////////////////////////////////
        typedef Lambda<void()> LambdaTaskButtonClickEvent;
        //////////////////////////////////////////////////////////////////////////
        class TaskButtonClickEventReceiver
            : public ButtonEventReceiverInterface
            , public Factorable
        {
        public:
            explicit TaskButtonClickEventReceiver( const LambdaTaskButtonClickEvent & _event )
                : m_event( _event )
            {
            }

            ~TaskButtonClickEventReceiver() override
            {
            }

        protected:
            void onButtonMouseEnter() override
            {
                //Empty
            }

            void onButtonMouseLeave() override
            {
                //Empty
            }

            void onButtonMousePush() override
            {
                //Empty
            }

            void onButtonMousePressed() override
            {
                //Empty
            }

            void onButtonMouseRelease() override
            {
                //Empty
            }

            void onButtonMouseButton() override
            {
                m_event();
            }

            void onButtonMouseButtonBegin() override
            {
                //Empty
            }

            void onButtonMouseButtonEnd() override
            {
                //Empty
            }

        protected:
            LambdaTaskButtonClickEvent m_event;
        };
    }
    //////////////////////////////////////////////////////////////////////////
    TaskButtonClick::TaskButtonClick( GOAP::Allocator * _allocator, const ButtonPtr & _button )
        : GOAP::TaskInterface( _allocator )
        , m_button( _button )
    {
    }
    //////////////////////////////////////////////////////////////////////////
    TaskButtonClick::~TaskButtonClick()
    {
    }
    //////////////////////////////////////////////////////////////////////////
    bool TaskButtonClick::_onRun( GOAP::NodeInterface * _node )
    {
        auto lambda = [_node]()
        {
            _node->complete();
        };

        EventationInterface * eventation = m_button->getEventation();

        EventReceiverInterfacePtr newreceiver = Helper::makeFactorableUnique<Detail::TaskButtonClickEventReceiver>( MENGINE_DOCUMENT_FUNCTION, lambda );

        EventReceiverInterfacePtr oldreceiver = eventation->addEventReceiver( EVENT_BUTTON_MOUSE_BUTTON, newreceiver );

        MENGINE_ASSERTION_FATAL( oldreceiver == nullptr, "event EVENT_BUTTON_MOUSE_BUTTON override" );

        m_receiver = newreceiver;

        return false;
    }
    //////////////////////////////////////////////////////////////////////////
    void TaskButtonClick::_onFinally()
    {
        EventationInterface * eventation = m_button->getEventation();

        EventReceiverInterfacePtr delreceiver = eventation->removeEventReceiver( EVENT_BUTTON_MOUSE_BUTTON );

        MENGINE_ASSERTION_FATAL( delreceiver == m_receiver, "event EVENT_BUTTON_MOUSE_BUTTON override" );
        m_receiver = nullptr;

        m_button = nullptr;
    }
    //////////////////////////////////////////////////////////////////////////
    bool TaskButtonClick::_onSkipable() const
    {
        return false;
    }
    //////////////////////////////////////////////////////////////////////////
}