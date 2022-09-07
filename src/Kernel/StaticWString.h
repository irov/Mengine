#pragma once

#include "Config/Char.h"
#include "Config/StdInt.h"
#include "Config/StdDef.h"

namespace Mengine
{
    namespace Detail
    {
        size_t staticWStringSize( const WChar * _buffer, size_t _capacity );
        WChar staticWStringIndex( const WChar * _buffer, size_t _capacity, size_t _index );
        void staticWStringChange( WChar * _buffer, size_t _capacity, size_t _index, WChar _ch );
        void staticWStringAssign( WChar * _buffer, size_t _capacity, const WChar * _value, size_t _size );
        void staticWStringAppend( WChar * _buffer, size_t _capacity, const WChar * _value, size_t _size );
        void staticWStringCopy( const WChar * _buffer, size_t _capacity, WChar * _value );
        int32_t staticWStringCompare( const WChar * _buffer, size_t _capacity, const WChar * _value );
    }

    template<size_t N>
    class StaticWString
    {
    public:
        StaticWString()
        {
        }

        ~StaticWString()
        {
        }

    public:
        WChar * data()
        {
            return m_buffer;
        }

        const WChar * c_str() const
        {
            return m_buffer;
        }

        size_t capacity() const
        {
            return N;
        }

        size_t size() const
        {
            return Detail::staticWStringSize( m_buffer, N );
        }

        void clear()
        {
            Detail::staticWStringChange( m_buffer, N, 0, L'\0');
        }

    public:
        void assign( const WChar * _value )
        {
            Detail::staticWStringAssign( m_buffer, N, _value, MENGINE_UNKNOWN_SIZE );
        }

        void assign( const WChar * _value, size_t _size )
        {
            Detail::staticWStringAssign( m_buffer, N, _value, _size );
        }

        void append( const WChar * _value )
        {
            Detail::staticWStringAppend( m_buffer, N, _value, MENGINE_UNKNOWN_SIZE );
        }

        void append( const WChar * _value, size_t _size )
        {
            Detail::staticWStringAppend( m_buffer, N, _value, _size );
        }

        void copy( WChar * _value ) const
        {
            Detail::staticWStringCopy( m_buffer, N, _value );
        }
         
        int32_t compare( const WChar * _value ) const
        {
            int32_t result = Detail::staticWStringCompare( m_buffer, N, _value );

            return result;
        }

        template<size_t N2>
        int32_t compare( const StaticWString<N2> & _value ) const
        {
            int32_t result = Detail::staticWStringCompare( m_buffer, N, _value.c_str() );

            return result;
        }

        void change( size_t _index, WChar _ch )
        {
            Detail::staticWStringChange( m_buffer, N, _index, _ch );
        }

    public:
        WChar operator []( size_t _index ) const
        {
            WChar c = Detail::staticWStringIndex( m_buffer, N, _index );

            return c;
        }

    protected:
        WChar m_buffer[N] = {'\0'};
    };
}
