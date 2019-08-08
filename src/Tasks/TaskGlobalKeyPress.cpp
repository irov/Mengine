#include "TaskGlobalKeyPress.h"

#include "Kernel/GlobalInputHandlerHelper.h"
#include "Kernel/Document.h"
#include "Kernel/Logger.h"

namespace Mengine
{
    //////////////////////////////////////////////////////////////////////////
    TaskGlobalKeyPress::TaskGlobalKeyPress( EKeyCode _code, bool _isDown, const LambdaInputKeyEvent & _filter )
        : m_code( _code )
        , m_isDown( _isDown )
        , m_filter( _filter )
        , m_id( 0 )
    {
    }
    //////////////////////////////////////////////////////////////////////////
    TaskGlobalKeyPress::~TaskGlobalKeyPress()
    {
    }
    //////////////////////////////////////////////////////////////////////////
    bool TaskGlobalKeyPress::_onRun()
    {
        auto lambda = [this]( const InputKeyEvent & _event ) -> bool
        {
            if( this->m_filter != nullptr )
            {
                if( this->m_filter( _event ) == true )
                {
                    this->complete();
                }
            }
            else
            {
                this->complete();
            }

            return false;
        };
        
        uint32_t id = Helper::addGlobalKeyHandler( m_code, m_isDown, lambda, MENGINE_DOCUMENT_FUNCTION );

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