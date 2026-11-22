#pragma once

#include "Config/Char.h"
#include "Config/StdDef.h"
#include "Config/StdInt.h"

namespace Mengine
{
    //////////////////////////////////////////////////////////////////////////
    struct UUIDv4
    {
        typedef Char value_type;
        static MENGINE_CONSTEXPR size_t size_data = MENGINE_UUID_STRING_COUNT;

        value_type data[UUIDv4::size_data];
    };
    //////////////////////////////////////////////////////////////////////////
    namespace Helper
    {
        bool makeUUIDv4( UUIDv4 * const _uuid, uint32_t _flags );
    }
    //////////////////////////////////////////////////////////////////////////
}
