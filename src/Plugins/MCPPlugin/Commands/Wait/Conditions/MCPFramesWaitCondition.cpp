#include "MCPFramesWaitCondition.h"

namespace Mengine
{
    //////////////////////////////////////////////////////////////////////////
    bool MCPFramesWaitCondition::evaluate( const MCPWaitConditionRequest & _request, MCPWaitConditionResponse * const _response )
    {
        uint32_t frames = _request.condition.get( "frames", MENGINE_UINT32_C(1) );
        uint64_t framesElapsed = _request.currentGeneration - _request.startGeneration;
        _response->satisfied = framesElapsed >= frames;
        _response->details.set( "framesElapsed", (int64_t)framesElapsed );

        return true;
    }
    //////////////////////////////////////////////////////////////////////////
}
