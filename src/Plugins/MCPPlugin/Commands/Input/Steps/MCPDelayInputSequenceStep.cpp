#include "MCPDelayInputSequenceStep.h"

#include "Kernel/TimestampHelper.h"

namespace Mengine
{
    //////////////////////////////////////////////////////////////////////////
    bool MCPDelayInputSequenceStep::execute( const MCPInputSequenceStepRequest & _request, MCPInputSequenceStepResponse * const _response )
    {
        uint32_t milliseconds = _request.step.get( "milliseconds", MENGINE_UINT32_C(0) );
        Timestamp timestamp = Helper::getSystemTimestamp();
        _response->resumeTimestamp = timestamp + milliseconds;
        _response->completed = true;

        return true;
    }
    //////////////////////////////////////////////////////////////////////////
}
