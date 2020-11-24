#include "TaskGlobalMouseWheel.h"

#include "Kernel/GlobalInputHandlerHelper.h"
#include "Kernel/DocumentHelper.h"
#include "Kernel/Logger.h"

#include "GOAP/NodeInterface.h"

namespace Mengine
{
    //////////////////////////////////////////////////////////////////////////
    TaskGlobalMouseWheel::TaskGlobalMouseWheel( const LambdaInputMouseWheelEvent & _filter, const DocumentPtr & _doc )
        : m_filter( _filter )
#if MENGINE_DOCUMENT_ENABLE
        , m_doc( _doc )
#endif
        , m_id( 0 )
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

        uint32_t id = Helper::addGlobalMouseWheelEvent( lambda, MENGINE_DOCUMENT_VALUE( m_doc, nullptr ) );

        if( id == 0 )
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
        if( m_id != 0 )
        {
            Helper::removeGlobalHandler( m_id );
            m_id = 0;
        }

        m_filter = nullptr;
    }
}