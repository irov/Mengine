#pragma once

#include "Interface/ServiceInterface.h"

#include "Config/Char.h"

namespace Mengine
{
    class Win32FindPython3ServiceInterface
        : public ServiceInterface
    {
        SERVICE_DECLARE( "Win32FindPython3Service" )

    public:
        virtual void getPython3Path( Char * const _python3Path ) const = 0;
    };
}
//////////////////////////////////////////////////////////////////////////
#define WIN32_FINDPYTHON3_SERVICE()\
    ((Mengine::Win32FindPython3ServiceInterface *)SERVICE_GET(Mengine::Win32FindPython3ServiceInterface))
//////////////////////////////////////////////////////////////////////////