#include "MCPInputAccelerometerCommand.h"

#include "Kernel/InputServiceHelper.h"
#include "Kernel/TimestampHelper.h"

namespace Mengine
{
    //////////////////////////////////////////////////////////////////////////
    MCPInputAccelerometerCommand::MCPInputAccelerometerCommand()
    {
    }
    //////////////////////////////////////////////////////////////////////////
    EMCPCommandStatus MCPInputAccelerometerCommand::execute( const MCPCommandRequest & _request, MCPCommandResponse * const _response )
    {
        float x = _request.params.get( "x", 0.f );
        float y = _request.params.get( "y", 0.f );
        float z = _request.params.get( "z", 1.f );

        Timestamp timestamp = Helper::getSystemTimestamp();
        Timestamp timestampSensor = timestamp * 1000000ULL;

        Helper::pushAccelerometerEvent( timestamp, timestampSensor, x, y, z );

        _response->result.set( "x", x );
        _response->result.set( "y", y );
        _response->result.set( "z", z );

        return EMCPCommandStatus::SUCCESS;
    }
    //////////////////////////////////////////////////////////////////////////
}
