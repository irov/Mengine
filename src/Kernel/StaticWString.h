#pragma once

#include "Config/StdString.h"
#include "Config/StdInt.h"
#include "Config/StdDef.h"

namespace Mengine
{
    namespace Detail
    {
        size_t staticWStringSize( const WChar * _buffer, size_t _capacity );
        WChar staticWStringIndex( const WChar * _buffer, size_t _capacity, size_t _index );
        void staticWStringChange( WChar * _buffer, size_t _capacity, size_t _index, WChar _ch );
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
            return N + 1;
        }

        size_t size() const
        {
            size_t capacity = this->capacity();

            size_t result = Detail::staticWStringSize( m_buffer, capacity );

            return result;
        }

        void clear()
        {
            size_t capacity = this->capacity();

            Detail::staticWStringChange( m_buffer, capacity, 0, L'\0' );
        }

    public:
        void assign( WChar _value )
        {
            size_t capacity = this->capacity();

            StdString::wcschrcpy_safe( m_buffer, _value, capacity );
        }

        void assign( const WChar * _value )
        {
            size_t capacity = this->capacity();

            StdString::wcscpy_safe( m_buffer, _value, capacity );
        }

        void assign( const WChar * _value, size_t _size )
        {
            size_t capacity = this->capacity();

            StdString::wcszcpy_safe( m_buffer, _value, _size, capacity );
        }

        void append( const WChar * _value )
        {
            size_t capacity = this->capacity();

            StdString::wcscat_safe( m_buffer, _value, capacity );
        }

        void append( const WChar * _value, size_t _size )
        {
            size_t capacity = this->capacity();

            StdString::wcszcat_safe( m_buffer, _value, _size, capacity );
        }

        void copy( WChar * _value ) const
        {
            size_t capacity = this->capacity();

            StdString::wcscpy_safe( _value, m_buffer, capacity );
        }
         
        int32_t compare( const WChar * _value ) const
        {
            size_t capacity = this->capacity();

            int32_t result = Detail::staticWStringCompare( m_buffer, capacity, _value );

            return result;
        }

        template<size_t N2>
        int32_t compare( const StaticWString<N2> & _value ) const
        {
            const WChar * value_str = _value.c_str();

            int32_t result = this->compare( value_str );

            return result;
        }

        void change( size_t _index, WChar _ch )
        {
            size_t capacity = this->capacity();

            Detail::staticWStringChange( m_buffer, capacity, _index, _ch );
        }

    public:
        WChar operator []( size_t _index ) const
        {
            size_t capacity = this->capacity();

            WChar c = Detail::staticWStringIndex( m_buffer, capacity, _index );

            return c;
        }

    protected:
        WChar m_buffer[N + 1] = {'\0'};
    };
}