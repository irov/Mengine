#include "MCPDebuggerWaitCondition.h"

#include "../../MCPCommandInterface.h"

namespace Mengine
{
    //////////////////////////////////////////////////////////////////////////
    MCPDebuggerWaitCondition::MCPDebuggerWaitCondition( MCPCommandHostInterface * _host )
        : m_host( _host )
    {
    }
    //////////////////////////////////////////////////////////////////////////
    bool MCPDebuggerWaitCondition::evaluate( const MCPWaitConditionRequest & _request, MCPWaitConditionResponse * const _response )
    {
        bool expected = _request.condition.get( "paused", true );
        bool paused = m_host->isMCPDebuggerPaused();
        _response->satisfied = paused == expected;
        _response->details.set( "paused", paused );

        return true;
    }
    //////////////////////////////////////////////////////////////////////////
}
