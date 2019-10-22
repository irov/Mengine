#include "TaskGlobalMouseButton.h"

#include "Kernel/GlobalInputHandlerHelper.h"
#include "Kernel/Document.h"
#include "Kernel/Logger.h"

#include "GOAP/NodeInterface.h"

namespace Mengine
{
    //////////////////////////////////////////////////////////////////////////
    TaskGlobalMouseButton::TaskGlobalMouseButton( EMouseCode _code, bool _isDown, const LambdaInputMouseButtonEvent & _filter )
        : m_code( _code )
        , m_isDown( _isDown )
        , m_filter( _filter )
        , m_id( 0 )
    {
    }
    //////////////////////////////////////////////////////////////////////////
    TaskGlobalMouseButton::~TaskGlobalMouseButton()
    {
    }
    //////////////////////////////////////////////////////////////////////////
    bool TaskGlobalMouseButton::_onRun( GOAP::NodeInterface * _node )
    {
        auto lambda = [this, _node]( const InputMouseButtonEvent & _event )
        {
            if( m_filter != nullptr )
            {
                if( m_filter( _event ) == true )
                {
                    _node->complete();
                }
            }
            else
            {
                _node->complete();
            }

            return false;
        };

        uint32_t id = Helper::addGlobalMouseButtonEvent( m_code, m_isDown, lambda, MENGINE_DOCUMENT_FUNCTION );

        if( id == 0 )
        {
            LOGGER_ERROR( "invalid add global mouse button event" );

            return true;
        }

        m_id = id;

        return false;
    }
    //////////////////////////////////////////////////////////////////////////
    void TaskGlobalMouseButton::_onFinally()
    {
        if( m_id != 0 )
        {
            Helper::removeGlobalHandler( m_id );
            m_id = 0;
        }

        m_filter = nullptr;
    }
}