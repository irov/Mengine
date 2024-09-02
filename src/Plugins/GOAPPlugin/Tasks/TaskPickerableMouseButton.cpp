#include "TaskPickerableMouseButton.h"

#include "Interface/PickerInterface.h"

#include "Kernel/BaseHotSpotEventReceiver.h"

#include "Kernel/FactorableUnique.h"
#include "Kernel/Eventable.h"
#include "Kernel/Assertion.h"

#include "GOAP/NodeInterface.h"

namespace Mengine
{
    //////////////////////////////////////////////////////////////////////////
    namespace Detail
    {
        class TaskPickerableMouseButtonEventReceiver
            : public BaseHotSpotEventReceiver
            , public Factorable
        {
        public:
            TaskPickerableMouseButtonEventReceiver( GOAP::NodeInterface * _node, EMouseButtonCode _button, bool _isDown, bool _isPressed, const LambdaPickerMouseButtonEvent & _filter )
                : m_node( _node )
                , m_button( _button )
                , m_isDown( _isDown )
                , m_isPressed( _isPressed )
                , m_filter( _filter )
            {
            }

            ~TaskPickerableMouseButtonEventReceiver() override
            {
            }

        protected:
            bool onHotSpotMouseButton( const RenderContext * _context, const InputMouseButtonEvent & _event ) override
            {
                if( _event.button != m_button )
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

                if( _event.special.isAlt == true )
                {
                    return false;
                }

                if( _event.special.isShift == true )
                {
                    return false;
                }                

                if( _event.special.isControl == true )
                {
                    return false;
                }

                bool handle = true;

                if( m_filter != nullptr )
                {
                    if( m_filter( _context, _event, &handle ) == false )
                    {
                        return false;
                    }
                }

                m_node->complete();

                return handle;
            }

        protected:
            GOAP::NodeInterface * m_node;
            EMouseButtonCode m_button;
            bool m_isDown;
            bool m_isPressed;
            LambdaPickerMouseButtonEvent m_filter;
        };
    }
    //////////////////////////////////////////////////////////////////////////
    TaskPickerableMouseButton::TaskPickerableMouseButton( GOAP::Allocator * _allocator, const PickerablePtr & _pickerable, EMouseButtonCode _button, bool _isDown, bool _isPressed, const LambdaPickerMouseButtonEvent & _filter, const DocumentInterfacePtr & _doc )
        : GOAP::TaskInterface( _allocator )
        , m_pickerable( _pickerable )
        , m_button( _button )
        , m_isDown( _isDown )
        , m_isPressed( _isPressed )
#if defined(MENGINE_DOCUMENT_ENABLE)
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

        EventReceiverInterfacePtr newreceiver = Helper::makeFactorableUnique<Detail::TaskPickerableMouseButtonEventReceiver>( MENGINE_DOCUMENT_VALUE( m_doc, nullptr ), _node, m_button, m_isDown, m_isPressed, m_filter );

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

#if defined(MENGINE_DOCUMENT_ENABLE)
        m_doc = nullptr;
#endif
    }
    //////////////////////////////////////////////////////////////////////////
    bool TaskPickerableMouseButton::_onSkipable() const
    {
        return false;
    }
    //////////////////////////////////////////////////////////////////////////
}