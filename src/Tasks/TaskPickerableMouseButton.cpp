#include "TaskPickerableMouseButton.h"

#include "Engine/HotSpotEventReceiverInterface.h"

#include "Kernel/FactorableUnique.h"

namespace Mengine
{
    //////////////////////////////////////////////////////////////////////////
    namespace Detail
    {
        class TaskPickerableMouseButtonEventReceiver
            : public HotSpotEventReceiverInterface
            , public Factorable
        {
        public:
            TaskPickerableMouseButtonEventReceiver( EMouseCode _code, bool _isDown, const LambdaPickerMouseButtonEvent & _filter )
                : m_code( _code )
                , m_isDown( _isDown )
                , m_filter( _filter )
            {
            }

            ~TaskPickerableMouseButtonEventReceiver() override
            {
            }

        protected:
            bool onHotSpotMouseButton( const InputMouseButtonEvent & _event ) override
            {
                bool handle = m_filter( _event );

                return handle;
            }

        protected:
            void onHotSpotActivate() override
            {
                //Empty
            }

            void onHotSpotDeactivate() override
            {
                //Empty
            }

            bool onHotSpotMouseEnter( const InputMouseEnterEvent & _event ) override
            {
                MENGINE_UNUSED( _event );

                //Empty

                return false;
            }

            void onHotSpotMouseLeave( const InputMouseLeaveEvent & _event ) override
            {
                MENGINE_UNUSED( _event );

                //Empty
            }

            bool onHotSpotKey( const InputKeyEvent & _event ) override
            {
                MENGINE_UNUSED( _event );

                //Empty

                return false;
            }

            bool onHotSpotText( const InputTextEvent & _event ) override
            {
                MENGINE_UNUSED( _event );

                //Empty

                return false;
            }

            bool onHotSpotMouseButtonBegin( const InputMouseButtonEvent & _event ) override
            {
                MENGINE_UNUSED( _event );

                //Empty

                return false;
            }

            bool onHotSpotMouseButtonEnd( const InputMouseButtonEvent & _event ) override
            {
                MENGINE_UNUSED( _event );

                //Empty

                return false;
            }

            bool onHotSpotMouseMove( const InputMouseMoveEvent & _event ) override
            {
                MENGINE_UNUSED( _event );

                //Empty

                return false;
            }

            bool onHotSpotMouseWheel( const InputMouseWheelEvent & _event ) override
            {
                MENGINE_UNUSED( _event );

                //Empty

                return false;
            }

            void onHotSpotMouseOverDestroy() override
            {
                //Empty
            }

            void onHotSpotMouseButtonBegin( uint32_t _enumerator, bool _isEnd ) override
            {
                MENGINE_UNUSED( _enumerator );
                MENGINE_UNUSED( _isEnd );

                //Empty
            }

            void onHotSpotMouseButtonEnd( uint32_t _enumerator, bool _isEnd ) override
            {
                MENGINE_UNUSED( _enumerator );
                MENGINE_UNUSED( _isEnd );

                //Empty
            }

        protected:
            EMouseCode m_code;
            bool m_isDown;
            LambdaPickerMouseButtonEvent m_filter;
        };
    }
    //////////////////////////////////////////////////////////////////////////
    TaskPickerableMouseButton::TaskPickerableMouseButton( const PickerablePtr & _pickerable, EMouseCode _code, bool _isDown, const LambdaPickerMouseButtonEvent & _filter )
        : m_pickerable( _pickerable )
        , m_code( _code )
        , m_isDown( _isDown )
        , m_filter( _filter )
    {
    }
    //////////////////////////////////////////////////////////////////////////
    TaskPickerableMouseButton::~TaskPickerableMouseButton()
    {
    }
    //////////////////////////////////////////////////////////////////////////
    bool TaskPickerableMouseButton::_onRun()
    {
        auto lambda = [this]( const InputMouseButtonEvent & _event )
        {
            if( _event.code != m_code )
            {
                return false;
            }

            if( _event.isDown != m_isDown )
            {
                return false;
            }

            bool handle = false;

            if( m_filter != nullptr )
            {
                handle = m_filter( _event );
            }

            this->complete();
            
            return handle;
        };

        PickerInterface * picker = m_pickerable->getPicker();

        Eventable * eventable = picker->getPickerEventable();

        EventationInterface * eventation = eventable->getEventation();

        eventation->addEventReceiver( EVENT_HOTSPOT_MOUSE_BUTTON, Helper::makeFactorableUnique<Detail::TaskPickerableMouseButtonEventReceiver>( m_code, m_isDown, lambda ) );

        return false;
    }
    //////////////////////////////////////////////////////////////////////////
    void TaskPickerableMouseButton::_onFinally()
    {
        PickerInterface * picker = m_pickerable->getPicker();

        Eventable * eventable = picker->getPickerEventable();

        EventationInterface * eventation = eventable->getEventation();

        eventation->removeEventReceiver( EVENT_HOTSPOT_MOUSE_BUTTON );
        m_pickerable = nullptr;

        m_filter = nullptr;
    }
    //////////////////////////////////////////////////////////////////////////
    bool TaskPickerableMouseButton::_onSkipable() const
    {
        return false;
    }
}