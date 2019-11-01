#include "TaskPickerableMouseEnter.h"

#include "Interface/PickerInterface.h"

#include "Engine/HotSpotEventReceiverInterface.h"

#include "Kernel/FactorableUnique.h"
#include "Kernel/Eventable.h"
#include "Kernel/Assertion.h"

#include "GOAP/NodeInterface.h"

namespace Mengine
{
    //////////////////////////////////////////////////////////////////////////
    namespace Detail
    {
        class TaskPickerableMouseEnterEventReceiver
            : public HotSpotEventReceiverInterface
            , public Factorable
        {
        public:
            TaskPickerableMouseEnterEventReceiver( GOAP::NodeInterface * _node, const LambdaPickerMouseEnterEvent & _filter )
                : m_node( _node )
                , m_filter( _filter )
            {
            }

            ~TaskPickerableMouseEnterEventReceiver() override
            {
            }

        protected:
            bool onHotSpotMouseEnter( const InputMouseEnterEvent & _event ) override
            {
                bool handle = false;

                if( m_filter != nullptr )
                {
                    if( m_filter( _event, &handle ) == false )
                    {
                        return false;
                    }
                }

                m_node->complete();

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

            bool onHotSpotMouseButton( const InputMouseButtonEvent & _event ) override
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
            GOAP::NodeInterface * m_node;
            LambdaPickerMouseEnterEvent m_filter;
        };
    }
    //////////////////////////////////////////////////////////////////////////
    TaskPickerableMouseEnter::TaskPickerableMouseEnter( const PickerablePtr & _pickerable, const LambdaPickerMouseEnterEvent & _filter )
        : m_pickerable( _pickerable )
        , m_filter( _filter )
    {
    }
    //////////////////////////////////////////////////////////////////////////
    TaskPickerableMouseEnter::~TaskPickerableMouseEnter()
    {
    }
    //////////////////////////////////////////////////////////////////////////
    bool TaskPickerableMouseEnter::_onRun( GOAP::NodeInterface * _node )
    {
        PickerInterface * picker = m_pickerable->getPicker();

        Eventable * eventable = picker->getPickerEventable();

        EventationInterface * eventation = eventable->getEventation();

        EventReceiverInterfacePtr newreceiver = Helper::makeFactorableUnique<Detail::TaskPickerableMouseEnterEventReceiver>( _node, m_filter );

        EventReceiverInterfacePtr oldreceiver = eventation->addEventReceiver( EVENT_HOTSPOT_MOUSE_ENTER, newreceiver );

        MENGINE_ASSERTION_FATAL_RETURN( oldreceiver == nullptr, false, "pickerable override" );

        m_receiver = newreceiver;

        return false;
    }
    //////////////////////////////////////////////////////////////////////////
    void TaskPickerableMouseEnter::_onFinally()
    {
        PickerInterface * picker = m_pickerable->getPicker();

        Eventable * eventable = picker->getPickerEventable();

        EventationInterface * eventation = eventable->getEventation();

        EventReceiverInterfacePtr delreceiver = eventation->removeEventReceiver( EVENT_HOTSPOT_MOUSE_ENTER );

        MENGINE_ASSERTION_FATAL( m_receiver == delreceiver, "pickerable miss remove" );
        m_receiver = nullptr;
        
        m_pickerable = nullptr;
        m_filter = nullptr;
    }
    //////////////////////////////////////////////////////////////////////////
    bool TaskPickerableMouseEnter::_onSkipable() const
    {
        return false;
    }
}