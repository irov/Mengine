#include "MCPFramesInputSequenceStep.h"

namespace Mengine
{
    //////////////////////////////////////////////////////////////////////////
    bool MCPFramesInputSequenceStep::execute( const MCPInputSequenceStepRequest & _request, MCPInputSequenceStepResponse * const _response )
    {
        uint32_t frames = _request.step.get( "frames", MENGINE_UINT32_C(1) );
        _response->resumeGeneration = _request.currentGeneration + frames;
        _response->completed = true;

        return true;
    }
    //////////////////////////////////////////////////////////////////////////
}
