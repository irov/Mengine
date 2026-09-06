#pragma once

#include "Interface/ServiceInterface.h"

#include "Config/Char.h"

namespace Mengine
{
    //////////////////////////////////////////////////////////////////////////
    class FindPython3ServiceInterface
        : public ServiceInterface
    {
        SERVICE_DECLARE( "FindPython3Service" )

    public:
        virtual void getPython3ExecutablePath( Char * const _path ) const = 0;
    };
    //////////////////////////////////////////////////////////////////////////
}
//////////////////////////////////////////////////////////////////////////
#define FINDPYTHON3_SERVICE()\
    ((Mengine::FindPython3ServiceInterface *)SERVICE_GET(Mengine::FindPython3ServiceInterface))
//////////////////////////////////////////////////////////////////////////
