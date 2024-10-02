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
        virtual void getPython3ExecutablePathW( WChar * const _path ) const = 0;
        virtual void getPython3LibraryPathW( WChar * const _path ) const = 0;
        virtual void getPython3DllPathW( WChar * const _path ) const = 0;

        virtual void getPython3ExecutablePathA( Char * const _path ) const = 0;
        virtual void getPython3LibraryPathA( Char * const _path ) const = 0;
        virtual void getPython3DllPathA( Char * const _path ) const = 0;
    };
}
//////////////////////////////////////////////////////////////////////////
#define WIN32_FINDPYTHON3_SERVICE()\
    ((Mengine::Win32FindPython3ServiceInterface *)SERVICE_GET(Mengine::Win32FindPython3ServiceInterface))
//////////////////////////////////////////////////////////////////////////