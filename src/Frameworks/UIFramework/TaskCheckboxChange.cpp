#include "TaskCheckboxChange.h"

#include "CheckboxEventReceiverInterface.h"

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
    TaskCheckboxChange::TaskCheckboxChange( const CheckboxPtr & _checkbox, bool _value )
        : m_checkbox( _checkbox )
        , m_value( _value )
    {
    }
    //////////////////////////////////////////////////////////////////////////
    TaskCheckboxChange::~TaskCheckboxChange()
    {
    }
    //////////////////////////////////////////////////////////////////////////
    bool TaskCheckboxChange::_onRun()
    {
        auto lambda = [this]()
        {
            this->complete();
        };

        EventationInterface * eventation = m_checkbox->getEventation();

        eventation->addEventReceiver( EVENT_CHECKBOX_CHANGE, Helper::makeFactorableUnique<Detail::TaskCheckboxChangeEventReceiver>( lambda, m_value ) );

        return false;
    }
    //////////////////////////////////////////////////////////////////////////
    void TaskCheckboxChange::_onFinally()
    {
        EventationInterface * eventation = m_checkbox->getEventation();

        eventation->removeEventReceiver( EVENT_CHECKBOX_CHANGE );

        m_checkbox = nullptr;
    }
    //////////////////////////////////////////////////////////////////////////
    bool TaskCheckboxChange::_onSkipable() const
    {
        return false;
    }
}