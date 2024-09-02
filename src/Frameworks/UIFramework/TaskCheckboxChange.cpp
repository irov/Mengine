#include "TaskCheckboxChange.h"

#include "CheckboxEventReceiverInterface.h"

#include "Kernel/Assertion.h"
#include "Kernel/FactorableUnique.h"
#include "Kernel/Eventable.h"

namespace Mengine
{
    //////////////////////////////////////////////////////////////////////////
    namespace Detail
    {
        //////////////////////////////////////////////////////////////////////////
        typedef Lambda<void()> LambdaTaskCheckboxChangeEvent;
        //////////////////////////////////////////////////////////////////////////
        class TaskCheckboxChangeEventReceiver
            : public CheckboxEventReceiverInterface
            , public Factorable
        {
        public:
            explicit TaskCheckboxChangeEventReceiver( const LambdaTaskCheckboxChangeEvent & _event, bool _value )
                : m_event( _event )
                , m_value( _value )
            {
            }

            ~TaskCheckboxChangeEventReceiver() override
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
                if( m_value != _value )
                {
                    return;
                }

                m_event();
            }

        protected:
            LambdaTaskCheckboxChangeEvent m_event;
            bool m_value;
        };
    }
    //////////////////////////////////////////////////////////////////////////
    TaskCheckboxChange::TaskCheckboxChange( GOAP::Allocator * _allocator, const CheckboxPtr & _checkbox, bool _value )
        : GOAP::TaskInterface( _allocator )
        , m_checkbox( _checkbox )
        , m_value( _value )
    {
    }
    //////////////////////////////////////////////////////////////////////////
    TaskCheckboxChange::~TaskCheckboxChange()
    {
    }
    //////////////////////////////////////////////////////////////////////////
    bool TaskCheckboxChange::_onRun( GOAP::NodeInterface * _node )
    {
        auto lambda = [_node]()
        {
            _node->complete();
        };

        EventationInterface * eventation = m_checkbox->getEventation();

        EventReceiverInterfacePtr newreceiver = Helper::makeFactorableUnique<Detail::TaskCheckboxChangeEventReceiver>( MENGINE_DOCUMENT_FUNCTION, lambda, m_value );

        EventReceiverInterfacePtr oldreceiver = eventation->addEventReceiver( EVENT_CHECKBOX_CHANGE, newreceiver );

        MENGINE_ASSERTION_FATAL( oldreceiver == nullptr, "event EVENT_CHECKBOX_CHANGE override" );

        m_receiver = newreceiver;

        return false;
    }
    //////////////////////////////////////////////////////////////////////////
    void TaskCheckboxChange::_onFinally()
    {
        EventationInterface * eventation = m_checkbox->getEventation();

        EventReceiverInterfacePtr delreceiver = eventation->removeEventReceiver( EVENT_CHECKBOX_CHANGE );

        MENGINE_ASSERTION_FATAL( delreceiver == m_receiver, "event EVENT_BUTTON_MOUSE_BUTTON override" );
        m_receiver = nullptr;

        m_checkbox = nullptr;
    }
    //////////////////////////////////////////////////////////////////////////
    bool TaskCheckboxChange::_onSkipable() const
    {
        return false;
    }
    //////////////////////////////////////////////////////////////////////////
}