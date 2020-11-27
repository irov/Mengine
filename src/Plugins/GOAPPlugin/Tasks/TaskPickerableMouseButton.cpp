#include "TaskPickerableMouseButton.h"

#include "Interface/PickerInterface.h"

#include "Engine/HotSpotEventReceiverInterface.h"

#include "Kernel/FactorableUnique.h"
#include "Kernel/DocumentHelper.h"
#include "Kernel/Eventable.h"
#include "Kernel/Assertion.h"

#include "GOAP/NodeInterface.h"

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
            TaskPickerableMouseButtonEventReceiver( GOAP::NodeInterface * _node, EMouseCode _code, bool _isDown, bool _isPressed, const LambdaPickerMouseButtonEvent & _filter )
                : m_node(_node)
                , m_code( _code )
                , m_isDown( _isDown )
                , m_isPressed( _isPressed )
                , m_filter( _filter )
            {
            }

            ~TaskPickerableMouseButtonEventReceiver() override
            {
            }

        protected:
            bool onHotSpotMouseButton( const InputMouseButtonEvent & _event ) override
            {
                if( _event.code != m_code )
                {
                    return false;
                }

                if( _event.isDown != m_isDown )
                {
                    return false;
                }

                if( _event.isDown != m_isPressed )
                {
                    return false;
                }

                bool handle = true;

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
            GOAP::NodeInterface * m_node;
            EMouseCode m_code;
            bool m_isDown;
            bool m_isPressed;
            LambdaPickerMouseButtonEvent m_filter;
        };
    }
    //////////////////////////////////////////////////////////////////////////
    TaskPickerableMouseButton::TaskPickerableMouseButton( GOAP::Allocator * _allocator, const PickerablePtr & _pickerable, EMouseCode _code, bool _isDown, bool _isPressed, const LambdaPickerMouseButtonEvent & _filter, const DocumentPtr & _doc )
        : GOAP::TaskInterface( _allocator )
        , m_pickerable( _pickerable )
        , m_code( _code )
        , m_isDown( _isDown )
        , m_isPressed( _isPressed )
#if MENGINE_DOCUMENT_ENABLE
        , m_doc( _doc )
#endif
        , m_filter( _filter )
    {
        MENGINE_UNUSED( _doc );
    }
    //////////////////////////////////////////////////////////////////////////
    TaskPickerableMouseButton::~TaskPickerableMouseButton()
    {
    }
    //////////////////////////////////////////////////////////////////////////
    bool TaskPickerableMouseButton::_onRun( GOAP::NodeInterface * _node )
    {
        PickerInterface * picker = m_pickerable->getPicker();

        Eventable * eventable = picker->getPickerEventable();

        EventationInterface * eventation = eventable->getEventation();

        EventReceiverInterfacePtr newreceiver = Helper::makeFactorableUnique<Detail::TaskPickerableMouseButtonEventReceiver>( MENGINE_DOCUMENT_VALUE( m_doc, nullptr ), _node, m_code, m_isDown, m_isPressed, m_filter );

        EventReceiverInterfacePtr oldreceiver = eventation->addEventReceiver( EVENT_HOTSPOT_MOUSE_BUTTON, newreceiver );

        MENGINE_ASSERTION_FATAL( oldreceiver == nullptr, "event EVENT_HOTSPOT_MOUSE_BUTTON override" );

        m_receiver = newreceiver;

        return false;
    }
    //////////////////////////////////////////////////////////////////////////
    void TaskPickerableMouseButton::_onFinally()
    {
        PickerInterface * picker = m_pickerable->getPicker();

        Eventable * eventable = picker->getPickerEventable();

        EventationInterface * eventation = eventable->getEventation();

        EventReceiverInterfacePtr delreceiver = eventation->removeEventReceiver( EVENT_HOTSPOT_MOUSE_BUTTON );

        MENGINE_ASSERTION_FATAL( m_receiver == delreceiver, "event EVENT_HOTSPOT_MOUSE_BUTTON miss remove" );
        m_receiver = nullptr;

        m_pickerable = nullptr;
        m_filter = nullptr;
    }
    //////////////////////////////////////////////////////////////////////////
    bool TaskPickerableMouseButton::_onSkipable() const
    {
        return false;
    }
}