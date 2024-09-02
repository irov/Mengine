#include "TaskGlobalMouseButton.h"

#include "Kernel/GlobalInputHandlerHelper.h"
#include "Kernel/Logger.h"

#include "GOAP/NodeInterface.h"

namespace Mengine
{
    //////////////////////////////////////////////////////////////////////////
    TaskGlobalMouseButton::TaskGlobalMouseButton( GOAP::Allocator * _allocator, EMouseButtonCode _button, bool _isDown, const LambdaInputMouseButtonFilter & _filter, const LambdaInputMouseButtonComplete & _cb, const DocumentInterfacePtr & _doc )
        : GOAP::TaskInterface( _allocator )
        , m_button( _button )
        , m_isDown( _isDown )
        , m_filter( _filter )
        , m_cb( _cb )
#if defined(MENGINE_DOCUMENT_ENABLE)
        , m_doc( _doc )
#endif
        , m_id( INVALID_UNIQUE_ID )
    {
        MENGINE_UNUSED( _doc );
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
                    if( m_cb != nullptr )
                    {
                        m_cb( _event );
                    }

                    _node->complete();
                }
            }
            else
            {
                if( m_cb != nullptr )
                {
                    m_cb( _event );
                }

                _node->complete();
            }

            return false;
        };

        uint32_t id = Helper::addGlobalMouseButtonHandler( m_button, m_isDown, lambda, MENGINE_DOCUMENT_VALUE( m_doc, nullptr ) );

        if( id == INVALID_UNIQUE_ID )
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
        if( m_id != INVALID_UNIQUE_ID )
        {
            Helper::removeGlobalHandler( m_id );
            m_id = INVALID_UNIQUE_ID;
        }

        m_filter = nullptr;
        m_cb = nullptr;

#if defined(MENGINE_DOCUMENT_ENABLE)
        m_doc = nullptr;
#endif
    }
    //////////////////////////////////////////////////////////////////////////
}