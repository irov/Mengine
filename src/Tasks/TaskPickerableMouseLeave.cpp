#include "TaskPickerableMouseLeave.h"

#include "Interface/PickerInterface.h"

#include "Engine/HotSpotEventReceiverInterface.h"

#include "Kernel/FactorableUnique.h"
#include "Kernel/Eventable.h"

namespace Mengine
{
    //////////////////////////////////////////////////////////////////////////
    namespace Detail
    {
        class TaskPickerableMouseLeaveEventReceiver
            : public HotSpotEventReceiverInterface
            , public Factorable
        {
        public:
            TaskPickerableMouseLeaveEventReceiver( const LambdaPickerMouseLeaveEvent & _filter )
                : m_filter( _filter )
            {
            }

            ~TaskPickerableMouseLeaveEventReceiver() override
            {
            }

        protected:
            void onHotSpotMouseLeave( const InputMouseLeaveEvent & _event ) override
            {
                m_filter( _event );
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

            bool onHotSpotMouseButton( const InputMouseButtonEvent & _event ) override
            {
                MENGINE_UNUSED( _event );

                //Empty

                return false;
            }

            bool onHotSpotMouseEnter( const InputMouseEnterEvent & _event ) override
            {
                MENGINE_UNUSED( _event );

                //Empty

                return false;
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
            LambdaPickerMouseLeaveEvent m_filter;
        };
    }
    //////////////////////////////////////////////////////////////////////////
    TaskPickerableMouseLeave::TaskPickerableMouseLeave( const PickerablePtr & _pickerable, const LambdaPickerMouseLeaveEvent & _filter )
        : m_pickerable( _pickerable )
        , m_filter( _filter )
    {
    }
    //////////////////////////////////////////////////////////////////////////
    TaskPickerableMouseLeave::~TaskPickerableMouseLeave()
    {
    }
    //////////////////////////////////////////////////////////////////////////
    bool TaskPickerableMouseLeave::_onRun()
    {
        auto lambda = [this]( const InputMouseLeaveEvent & _event )
        {
            if( m_filter != nullptr )
            {
                m_filter( _event );
            }

            this->complete();
        };

        PickerInterface * picker = m_pickerable->getPicker();

        Eventable * eventable = picker->getPickerEventable();

        EventationInterface * eventation = eventable->getEventation();

        eventation->addEventReceiver( EVENT_HOTSPOT_MOUSE_LEAVE, Helper::makeFactorableUnique<Detail::TaskPickerableMouseLeaveEventReceiver>( lambda ) );

        return false;
    }
    //////////////////////////////////////////////////////////////////////////
    void TaskPickerableMouseLeave::_onFinally()
    {
        PickerInterface * picker = m_pickerable->getPicker();

        Eventable * eventable = picker->getPickerEventable();

        EventationInterface * eventation = eventable->getEventation();

        eventation->removeEventReceiver( EVENT_HOTSPOT_MOUSE_LEAVE );
        
        m_pickerable = nullptr;
        m_filter = nullptr;
    }
    //////////////////////////////////////////////////////////////////////////
    bool TaskPickerableMouseLeave::_onSkipable() const
    {
        return false;
    }
}