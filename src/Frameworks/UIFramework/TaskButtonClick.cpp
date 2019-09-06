#include "TaskButtonClick.h"

#include "ButtonEventReceiverInterface.h"

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
    TaskButtonClick::TaskButtonClick( const ButtonPtr & _button )
        : m_button( _button )        
    {
    }
    //////////////////////////////////////////////////////////////////////////
    TaskButtonClick::~TaskButtonClick()
    {
    }
    //////////////////////////////////////////////////////////////////////////
    bool TaskButtonClick::_onRun()
    {
        auto lambda = [this]()
        {
            this->complete();
        };

        EventationInterface * eventation = m_button->getEventation();

        eventation->addEventReceiver( EVENT_BUTTON_MOUSE_BUTTON, Helper::makeFactorableUnique<Detail::TaskButtonClickEventReceiver>( lambda ) );

        return false;
    }
    //////////////////////////////////////////////////////////////////////////
    void TaskButtonClick::_onFinally()
    {
        EventationInterface * eventation = m_button->getEventation();

        eventation->removeEventReceiver( EVENT_BUTTON_MOUSE_BUTTON );

        m_button = nullptr;
    }
    //////////////////////////////////////////////////////////////////////////
    bool TaskButtonClick::_onSkipable() const
    {
        return false;
    }
}