#include "TaskGlobalMouseWheel.h"

#include "Kernel/GlobalInputHandlerHelper.h"
#include "Kernel/DocumentHelper.h"
#include "Kernel/Logger.h"

#include "GOAP/NodeInterface.h"

namespace Mengine
{
    //////////////////////////////////////////////////////////////////////////
    TaskGlobalMouseWheel::TaskGlobalMouseWheel( GOAP::Allocator * _allocator, const LambdaInputMouseWheelEvent & _filter, const DocumentInterfacePtr & _doc )
        : GOAP::TaskInterface( _allocator )
        , m_filter( _filter )
#if defined(MENGINE_DOCUMENT_ENABLE)
        , m_doc( _doc )
#endif
        , m_id( INVALID_UNIQUE_ID )
    {
        MENGINE_UNUSED( _doc );
    }
    //////////////////////////////////////////////////////////////////////////
    TaskGlobalMouseWheel::~TaskGlobalMouseWheel()
    {
    }
    //////////////////////////////////////////////////////////////////////////
    bool TaskGlobalMouseWheel::_onRun( GOAP::NodeInterface * _node )
    {
        auto lambda = [this, _node]( const InputMouseWheelEvent & _event )
        {
            if( m_filter( _event ) == false )
            {
                return;
            }

            _node->complete();
        };

        UniqueId id = Helper::addGlobalMouseWheelEvent( lambda, MENGINE_DOCUMENT_VALUE( m_doc, nullptr ) );

        if( id == INVALID_UNIQUE_ID )
        {
            LOGGER_ERROR( "invalid add global mouse wheel event" );

            return true;
        }

        m_id = id;

        return false;
    }
    //////////////////////////////////////////////////////////////////////////
    void TaskGlobalMouseWheel::_onFinally()
    {
        if( m_id != INVALID_UNIQUE_ID )
        {
            Helper::removeGlobalHandler( m_id );
            m_id = INVALID_UNIQUE_ID;
        }

        m_filter = nullptr;

#if defined(MENGINE_DOCUMENT_ENABLE)
        m_doc = nullptr;
#endif
    }
    //////////////////////////////////////////////////////////////////////////
}