#include "TaskGlobalKeyPress.h"

#include "Kernel/GlobalInputHandlerHelper.h"
#include "Kernel/DocumentHelper.h"
#include "Kernel/Logger.h"

#include "GOAP/NodeInterface.h"

namespace Mengine
{
    //////////////////////////////////////////////////////////////////////////
    TaskGlobalKeyPress::TaskGlobalKeyPress( GOAP::Allocator * _allocator, EKeyCode _code, bool _isDown, const LambdaInputKeyEvent & _filter, const DocumentPtr & _doc )
        : TaskInterface( _allocator )
        , m_code( _code )
        , m_isDown( _isDown )
        , m_filter( _filter )
#if MENGINE_DOCUMENT_ENABLE
        , m_doc( _doc )
#endif
        , m_id( 0 )
    {
        MENGINE_UNUSED( _doc );
    }
    //////////////////////////////////////////////////////////////////////////
    TaskGlobalKeyPress::~TaskGlobalKeyPress()
    {
    }
    //////////////////////////////////////////////////////////////////////////
    bool TaskGlobalKeyPress::_onRun( GOAP::NodeInterface * _node )
    {
        auto lambda = [this, _node]( const InputKeyEvent & _event )
        {
            if( _event.isSpecial == true )
            {
                return false;
            }

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

        uint32_t id = Helper::addGlobalKeyHandler( m_code, m_isDown, lambda, MENGINE_DOCUMENT_VALUE( m_doc, nullptr ) );

        if( id == 0 )
        {
            LOGGER_ERROR( "invalid add global key event" );

            return true;
        }

        m_id = id;

        return false;
    }
    //////////////////////////////////////////////////////////////////////////
    void TaskGlobalKeyPress::_onFinally()
    {
        if( m_id != 0 )
        {
            Helper::removeGlobalHandler( m_id );
            m_id = 0;
        }

        m_filter = nullptr;
    }
}