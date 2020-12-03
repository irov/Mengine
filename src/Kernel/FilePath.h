#pragma once

#include "Kernel/ConstString.h"

namespace Mengine
{
    //////////////////////////////////////////////////////////////////////////
    class FilePath
        : public ConstString
    {
    public:
        FilePath() noexcept
        {
        }

        explicit FilePath( const ConstString & _cst )
            : ConstString( _cst )
        {
        }

    public:
        static const FilePath & none();
    };
    //////////////////////////////////////////////////////////////////////////
    template<>
    struct Hashgen<FilePath>
    {
        HashType operator()( const FilePath & _value ) const
        {
            return _value.hash();
        }
    };
    //////////////////////////////////////////////////////////////////////////
}