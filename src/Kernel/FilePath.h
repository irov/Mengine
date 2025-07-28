#pragma once

#include "Kernel/ConstString.h"

namespace Mengine
{
    //////////////////////////////////////////////////////////////////////////
    class FilePath
        : public ConstString
    {
    public:
        MENGINE_CONSTEXPR FilePath() MENGINE_NOEXCEPT
        {
        }

        explicit FilePath( const ConstString & _cst ) MENGINE_NOEXCEPT
            : ConstString( _cst )
        {
        }

    public:
        static MENGINE_INLINE const FilePath & none() MENGINE_NOEXCEPT
        {
            static const FilePath s_none;

            return s_none;
        }
    };
    //////////////////////////////////////////////////////////////////////////
    bool operator == ( const FilePath & _left, const FilePath::value_type * _right ) MENGINE_NOEXCEPT;
    //////////////////////////////////////////////////////////////////////////
    bool operator == ( const FilePath::value_type * _left, const FilePath & _right ) MENGINE_NOEXCEPT;
    //////////////////////////////////////////////////////////////////////////
    bool operator != ( const FilePath & _left, const FilePath::value_type * _right ) MENGINE_NOEXCEPT;
    //////////////////////////////////////////////////////////////////////////
    bool operator != ( const FilePath::value_type * _left, const FilePath & _right ) MENGINE_NOEXCEPT;
    //////////////////////////////////////////////////////////////////////////
    template<>
    struct Hashgen<FilePath>
    {
        MENGINE_CONSTEXPR HashType operator()( const FilePath & _value ) const MENGINE_NOEXCEPT
        {
            return _value.hash();
        }
    };
    //////////////////////////////////////////////////////////////////////////
}