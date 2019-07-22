#include "TaskGlobalMouseMove.h"

#include "Kernel/GlobalInputHandlerHelper.h"
#include "Kernel/Document.h"
#include "Kernel/Logger.h"

namespace Mengine
{
    //////////////////////////////////////////////////////////////////////////
    TaskGlobalMouseMove::TaskGlobalMouseMove( const LambdaMouseMoveEvent & _filter )
        : m_filter( _filter )
        , m_id( 0 )
    {
    }
    //////////////////////////////////////////////////////////////////////////
    TaskGlobalMouseMove::~TaskGlobalMouseMove()
    {
    }
    //////////////////////////////////////////////////////////////////////////
    bool TaskGlobalMouseMove::_onRun()
    {
        auto lambda = [this]( const InputMouseMoveEvent & _event )
        {
            if( m_filter( _event ) == false )
            {
                return;
            }

            this->complete();
        };
                
        uint32_t id = Helper::addGlobalMouseMoveEvent( lambda, MENGINE_DOCUMENT_FUNCTION );

        if( id == 0 )
        {
            LOGGER_ERROR( "invalid add global mouse move event" );

            return true;
        }

        m_id = id;

        return false;
    }
    //////////////////////////////////////////////////////////////////////////
    void TaskGlobalMouseMove::_onFinally()
    {
        if( m_id != 0 )
        {
            Helper::removeGlobalHandler( m_id );
            m_id = 0;
        }

        m_filter = nullptr;
    }
}