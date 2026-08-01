#include "MCPRuntimeWaitCondition.h"

#include "Interface/ApplicationInterface.h"

namespace Mengine
{
    //////////////////////////////////////////////////////////////////////////
    bool MCPRuntimeWaitCondition::evaluate( const MCPWaitConditionRequest & _request, MCPWaitConditionResponse * const _response )
    {
        bool currentUpdateFrozen = APPLICATION_SERVICE()->isUpdateFrozen();
        bool currentRenderFrozen = APPLICATION_SERVICE()->isRenderFrozen();
        bool expectedUpdateFrozen = _request.condition.get( "updateFrozen", currentUpdateFrozen );
        bool expectedRenderFrozen = _request.condition.get( "renderFrozen", currentRenderFrozen );
        _response->satisfied = currentUpdateFrozen == expectedUpdateFrozen && currentRenderFrozen == expectedRenderFrozen;
        _response->details.set( "updateFrozen", currentUpdateFrozen );
        _response->details.set( "renderFrozen", currentRenderFrozen );

        return true;
    }
    //////////////////////////////////////////////////////////////////////////
}
