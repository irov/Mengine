#pragma once

#include "Kernel/ConstStringHelper.h"

namespace Mengine
{
    template<ConstString::hash_type Hash>
    class ConstStringProxy
    {
    public:
        ConstStringProxy( const Char * _string, ConstString::size_type _size )
            : m_string( _string )
            , m_size( _size )
        {
        }

        ~ConstStringProxy()
        {
        }

    public:
        const ConstString & c_cstr() const
        {
            static ConstString cstr = Helper::stringizeStringHashUnique( m_string, m_size, Hash );

            return cstr;
        }

        const Char * c_str() const
        {
            const ConstString & cstr = this->c_cstr();

            return cstr.c_str();
        }

        operator const ConstString & () const
        {
            return this->c_cstr();
        }

    protected:
        const Char * m_string;
        ConstString::size_type m_size;
    };
}