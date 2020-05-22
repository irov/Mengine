#pragma once

#include "Kernel/ConstStringHelper.h"

namespace Mengine
{
    template<ConstString::hash_type Hash>
    class ConstStringProxy
    {
    public:
        ConstStringProxy( const Char * _string, ConstStringHolder::size_type _size )
            : m_string( _string )
            , m_size( _size )
        {
        }

    public:
        const ConstString & c_cstr() const
        {
            static ConstString cstr = Helper::stringizeStringHashUnique( m_string, m_size, Hash );

            return cstr;
        }

        operator const ConstString & () const
        {
            return this->c_cstr();
        }

    protected:
        const Char * m_string;
        ConstStringHolder::size_type m_size;
    };
}