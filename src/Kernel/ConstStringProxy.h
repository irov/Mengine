#pragma once

#include "Kernel/ConstStringHelper.h"

namespace Mengine
{
    template<ConstString::hash_type Hash>
    class ConstStringProxy
    {
    public:
        typedef ConstString::size_type size_type;
        typedef ConstString::hash_type hash_type;
        typedef ConstString::value_type value_type;

    public:
        ConstStringProxy( const value_type * _string, size_type _size )
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

        const value_type * c_str() const
        {
            const ConstString & cstr = this->c_cstr();

            return cstr.c_str();
        }

        operator const ConstString & () const
        {
            return this->c_cstr();
        }

    public:
        bool operator == ( const ConstString & _cstr ) const
        {
            const ConstString & cstr = this->c_cstr();

            return cstr == _cstr;
        }

        bool operator != ( const ConstString & _cstr ) const
        {
            const ConstString & cstr = this->c_cstr();

            return cstr == _cstr;
        }

    protected:
        const value_type * m_string;
        size_type m_size;
    };
}