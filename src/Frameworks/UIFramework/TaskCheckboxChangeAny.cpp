#include "TaskCheckboxChangeAny.h"

#include "CheckboxEventReceiverInterface.h"

#include "Kernel/Assertion.h"
#include "Kernel/FactorableUnique.h"
#include "Kernel/Eventable.h"

#include "GOAP/NodeInterface.h"

namespace Mengine
{
    //////////////////////////////////////////////////////////////////////////
    namespace Detail
    {
        //////////////////////////////////////////////////////////////////////////
        typedef Lambda<void()> LambdaTaskCheckboxChangeAnyEvent;
        //////////////////////////////////////////////////////////////////////////
        class TaskCheckboxChangeAnyEventReceiver
            : public CheckboxEventReceiverInterface
            , public Factorable
        {
        public:
            explicit TaskCheckboxChangeAnyEventReceiver( const LambdaTaskCheckboxChangeAnyEvent & _event )
                : m_event( _event )
            {
            }

            ~TaskCheckboxChangeAnyEventReceiver() override
            {
            }

        protected:
            void onCheckboxMouseEnter( bool _value ) override
            {
                MENGINE_UNUSED( _value );

                //Empty
            }

            void onCheckboxMouseLeave( bool _value ) override
            {
                MENGINE_UNUSED( _value );

                //Empty
            }

            void onCheckboxMousePush( bool _value ) override
            {
                MENGINE_UNUSED( _value );

                //Empty
            }

            void onCheckboxMousePressed( bool _value ) override
            {
                MENGINE_UNUSED( _value );

                //Empty
            }

            void onCheckboxMouseRelease( bool _value ) override
            {
                MENGINE_UNUSED( _value );

                //Empty
            }

            void onCheckboxChange( bool _value ) override
            {
                MENGINE_UNUSED( _value );

                m_event();
            }

        protected:
            LambdaTaskCheckboxChangeAnyEvent m_event;
        };
    }
    //////////////////////////////////////////////////////////////////////////
    TaskCheckboxChangeAny::TaskCheckboxChangeAny( GOAP::Allocator * _allocator, const CheckboxPtr & _checkbox )
        : GOAP::TaskInterface( _allocator )
        , m_checkbox( _checkbox )
    {
    }
    //////////////////////////////////////////////////////////////////////////
    TaskCheckboxChangeAny::~TaskCheckboxChangeAny()
    {
    }
    //////////////////////////////////////////////////////////////////////////
    bool TaskCheckboxChangeAny::_onRun( GOAP::NodeInterface * _node )
    {
        auto lambda = [_node]()
        {
            _node->complete();
        };

        EventationInterface * eventation = m_checkbox->getEventation();

        EventReceiverInterfacePtr newreceiver = Helper::makeFactorableUnique<Detail::TaskCheckboxChangeAnyEventReceiver>( MENGINE_DOCUMENT_FUNCTION, lambda );

        EventReceiverInterfacePtr oldreceiver = eventation->addEventReceiver( EVENT_CHECKBOX_CHANGE, newreceiver );

        MENGINE_ASSERTION_FATAL( oldreceiver == nullptr, "event EVENT_CHECKBOX_CHANGE override" );

        m_receiver = newreceiver;

        return false;
    }
    //////////////////////////////////////////////////////////////////////////
    void TaskCheckboxChangeAny::_onFinally()
    {
        EventationInterface * eventation = m_checkbox->getEventation();

        EventReceiverInterfacePtr delreceiver = eventation->removeEventReceiver( EVENT_CHECKBOX_CHANGE );

        MENGINE_ASSERTION_FATAL( delreceiver == m_receiver, "event EVENT_BUTTON_MOUSE_BUTTON override" );
        m_receiver = nullptr;

        m_checkbox = nullptr;
    }
    //////////////////////////////////////////////////////////////////////////
    bool TaskCheckboxChangeAny::_onSkipable() const
    {
        return false;
    }
    //////////////////////////////////////////////////////////////////////////
}