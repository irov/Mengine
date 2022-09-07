#pragma once

#include "Config/Char.h"
#include "Config/StdArg.h"
#include "Config/StdInt.h"
#include "Config/StdDef.h"

namespace Mengine
{
    namespace Detail
    {
        size_t staticStringSize( const Char * _buffer, size_t _capacity );
        Char staticStringIndex( const Char * _buffer, size_t _capacity, size_t _index );
        void staticStringChange( Char * _buffer, size_t _capacity, size_t _index, Char _ch );
        void staticStringAssign( Char * _buffer, size_t _capacity, const Char * _value, size_t _size );
        void staticStringAppend( Char * _buffer, size_t _capacity, const Char * _value, size_t _size );
        void staticStringFormat( Char * _buffer, size_t _capacity, const Char * _format, MENGINE_VA_LIST_TYPE _args );
        void staticStringCopy( const Char * _buffer, size_t _capacity, Char * _value );
        int32_t staticStringCompare( const Char * _buffer, size_t _capacity, const Char * _value );
    }

    template<size_t N>
    class StaticString
    {
    public:
        typedef Char value_type;

    public:
        StaticString()
        {
        }

        StaticString( const Char * _value )
        {
            this->assign( _value );
        }

        ~StaticString()
        {
        }

    public:
        Char * data()
        {
            return m_buffer;
        }

        const Char * c_str() const
        {
            return m_buffer;
        }

        size_t size() const
        {
            size_t result = Detail::staticStringSize( m_buffer, N );

            return result;
        }

        size_t capacity() const
        {
            return N;
        }

        void clear()
        {
            Detail::staticStringChange( m_buffer, N, 0, '\0' );
        }

    public:
        void assign( const Char * _value )
        {
            Detail::staticStringAssign( m_buffer, N, _value, MENGINE_UNKNOWN_SIZE );
        }

        void assign( const Char * _value, size_t _size )
        {
            Detail::staticStringAssign( m_buffer, N, _value, _size );
        }

        template<size_t N2>
        void assign( const StaticString<N2> & _value )
        {
            const Char * value_str = _value.c_str();
            size_t value_size = _value.size();
            Detail::staticStringAssign( m_buffer, N, value_str, value_size );
        }

        void append( Char _value )
        {
            Detail::staticStringAppend( m_buffer, N, &_value, 1 );
        }

        void append( const Char * _value )
        {
            Detail::staticStringAppend( m_buffer, N, _value, MENGINE_UNKNOWN_SIZE );
        }

        void append( const Char * _value, size_t _size )
        {
            Detail::staticStringAppend( m_buffer, N, _value, _size );
        }

        void copy( Char * _value ) const
        {
            Detail::staticStringCopy( m_buffer, N, _value );
        }

        void format( const Char * _format, ... )
        {
            MENGINE_VA_LIST_TYPE args;
            MENGINE_VA_LIST_START( args, _format );

            Detail::staticStringFormat( m_buffer, N, _format, args );

            MENGINE_VA_LIST_END( args );
        }

        int32_t compare( const Char * _value ) const
        {
            int32_t result = Detail::staticStringCompare( m_buffer, N, _value );

            return result;
        }

        template<size_t N2>
        int32_t compare( const StaticString<N2> & _value ) const
        {
            const Char * value_str = _value.c_str();

            int32_t result = this->compare( value_str );

            return result;
        }

        void change( size_t _index, Char _ch )
        {
            Detail::staticStringChange( m_buffer, N, _index, _ch );
        }

    public:
        Char operator []( size_t _index ) const
        {
            Char c = Detail::staticStringIndex( m_buffer, N, _index );

            return c;
        }

    protected:
        Char m_buffer[N] = {'\0'};
    };
}
