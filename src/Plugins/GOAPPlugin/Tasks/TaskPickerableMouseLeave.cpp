#include "TaskPickerableMouseLeave.h"

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
        class TaskPickerableMouseLeaveEventReceiver
            : public BaseHotSpotEventReceiver
            , public Factorable
        {
        public:
            TaskPickerableMouseLeaveEventReceiver( GOAP::NodeInterface * _node, const LambdaPickerMouseLeaveEvent & _filter )
                : m_node( _node )
                , m_filter( _filter )
            {
            }

            ~TaskPickerableMouseLeaveEventReceiver() override
            {
            }

        protected:
            void onHotSpotMouseLeave( const RenderContext * _context, const InputMouseLeaveEvent & _event ) override
            {
                if( m_filter != nullptr )
                {
                    if( m_filter( _context, _event ) == false )
                    {
                        return;
                    }
                }

                m_node->complete();
            }

        protected:
            GOAP::NodeInterface * m_node;

            LambdaPickerMouseLeaveEvent m_filter;
        };
    }
    //////////////////////////////////////////////////////////////////////////
    TaskPickerableMouseLeave::TaskPickerableMouseLeave( GOAP::Allocator * _allocator, const PickerablePtr & _pickerable, const LambdaPickerMouseLeaveEvent & _filter, const DocumentInterfacePtr & _doc )
        : GOAP::TaskInterface( _allocator )
        , m_pickerable( _pickerable )
        , m_filter( _filter )
#if defined(MENGINE_DOCUMENT_ENABLE)
        , m_doc( _doc )
#endif
    {
        MENGINE_UNUSED( _doc );
    }
    //////////////////////////////////////////////////////////////////////////
    TaskPickerableMouseLeave::~TaskPickerableMouseLeave()
    {
    }
    //////////////////////////////////////////////////////////////////////////
    bool TaskPickerableMouseLeave::_onRun( GOAP::NodeInterface * _node )
    {
        PickerInterface * picker = m_pickerable->getPicker();

        Eventable * eventable = picker->getPickerEventable();

        EventationInterface * eventation = eventable->getEventation();

        EventReceiverInterfacePtr newreceiver = Helper::makeFactorableUnique<Detail::TaskPickerableMouseLeaveEventReceiver>( MENGINE_DOCUMENT_VALUE( m_doc, nullptr ), _node, m_filter );

        EventReceiverInterfacePtr oldreceiver = eventation->addEventReceiver( EVENT_HOTSPOT_MOUSE_LEAVE, newreceiver );

        MENGINE_ASSERTION_FATAL( oldreceiver == nullptr, "pickerable override" );

        m_receiver = newreceiver;

        return false;
    }
    //////////////////////////////////////////////////////////////////////////
    void TaskPickerableMouseLeave::_onFinally()
    {
        PickerInterface * picker = m_pickerable->getPicker();

        Eventable * eventable = picker->getPickerEventable();

        EventationInterface * eventation = eventable->getEventation();

        EventReceiverInterfacePtr delreceiver = eventation->removeEventReceiver( EVENT_HOTSPOT_MOUSE_LEAVE );

        MENGINE_ASSERTION_FATAL( m_receiver == delreceiver, "event EVENT_HOTSPOT_MOUSE_LEAVE miss remove" );
        m_receiver = nullptr;
        
        m_pickerable = nullptr;
        m_filter = nullptr;

#if defined(MENGINE_DOCUMENT_ENABLE)
        m_doc = nullptr;
#endif
    }
    //////////////////////////////////////////////////////////////////////////
    bool TaskPickerableMouseLeave::_onSkipable() const
    {
        return false;
    }
    //////////////////////////////////////////////////////////////////////////
}
