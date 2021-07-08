#pragma once

#include "Config/Typedef.h"
#include "Config/Char.h"

namespace Mengine
{
    //////////////////////////////////////////////////////////////////////////
    typedef uint32_t magic_number_type;
    typedef uint32_t magic_version_type;
    //////////////////////////////////////////////////////////////////////////
    template<Char c0, Char c1, Char c2, Char c3, uint32_t version>
    struct Magic
    {
        static constexpr magic_number_type magic_number = MENGINE_FOURCC( c0, c1, c2, c3 );
        static constexpr magic_version_type magic_version = version;
    };
    //////////////////////////////////////////////////////////////////////////
    namespace Helper
    {
        MENGINE_INLINE bool magicTest4( const Char * _header, const Char * _magic )
        {
            return (_header[0] == _magic[0])
                && (_header[1] == _magic[1])
                && (_header[2] == _magic[2])
                && (_header[3] == _magic[3]);
        }
    }
    //////////////////////////////////////////////////////////////////////////
}
//////////////////////////////////////////////////////////////////////////
#define DECLARE_MAGIC_NUMBER( name, c0, c1, c2, c3, version)\
    typedef Magic<c0, c1, c2, c3, version> name;
//////////////////////////////////////////////////////////////////////////
#define GET_MAGIC_NUMBER( name ) (name::magic_number)
#define GET_MAGIC_VERSION( name ) (name::magic_version)
//////////////////////////////////////////////////////////////////////////
