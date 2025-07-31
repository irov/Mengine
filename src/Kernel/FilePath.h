#pragma once

#include "Kernel/ConstString.h"

namespace Mengine
{
    //////////////////////////////////////////////////////////////////////////
    class FilePath
        : public ConstString
    {
    public:
        MENGINE_CONSTEXPR FilePath()
        {
        }

        explicit FilePath( const ConstString & _cst )
            : ConstString( _cst )
        {
        }

    public:
        static MENGINE_INLINE const FilePath & none()
        {
            static const FilePath s_none;

            return s_none;
        }
    };
    //////////////////////////////////////////////////////////////////////////
    bool operator == ( const FilePath & _left, const FilePath::value_type * _right );
    //////////////////////////////////////////////////////////////////////////
    bool operator == ( const FilePath::value_type * _left, const FilePath & _right );
    //////////////////////////////////////////////////////////////////////////
    bool operator != ( const FilePath & _left, const FilePath::value_type * _right );
    //////////////////////////////////////////////////////////////////////////
    bool operator != ( const FilePath::value_type * _left, const FilePath & _right );
    //////////////////////////////////////////////////////////////////////////
    template<>
    struct Hashgen<FilePath>
    {
        MENGINE_CONSTEXPR HashType operator()( const FilePath & _value ) const
        {
            return _value.hash();
        }
    };
    //////////////////////////////////////////////////////////////////////////
}