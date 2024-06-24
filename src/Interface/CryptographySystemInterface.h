#pragma once

#include "Interface/ServiceInterface.h"

#include "Kernel/ConstString.h"

#include "Config/Char.h"
#include "Config/StdInt.h"

namespace Mengine
{
    class CryptographySystemInterface
        : public ServiceInterface
    {
        SERVICE_DECLARE( "CryptographySystem" )

    public:
        virtual bool generateRandomSeed( uint64_t * const _seed ) const = 0;
        virtual bool generateRandomHexadecimal( uint32_t _length, Char * const _hexadecimal, bool _lowercase ) const = 0;
    };
}
//////////////////////////////////////////////////////////////////////////
#define CRYPTOGRAPHY_SYSTEM()\
    ((Mengine::CryptographySystemInterface *)SERVICE_GET(Mengine::CryptographySystemInterface))
//////////////////////////////////////////////////////////////////////////
