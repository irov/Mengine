#pragma once

#include "Kernel/MemoryCopy.h"
#include "Kernel/StringLength.h"

#include "Config/Char.h"
#include "Config/StdInt.h"

namespace Mengine
{
    template<class T, uint32_t Size>
    class ArrayTString
    {
    public:
        typedef T value_type;
        typedef uint32_t size_type;

    public:
        ArrayTString()
            : m_pos( 0 )
        {
        }

        ~ArrayTString()
        {
        }

    public:
        const value_type * c_str() const
        {
            return m_buffer;
        }

        size_type size() const
        {
            return m_pos;
        }

        bool empty() const
        {
            return m_pos == 0;
        }

    public:
        void clear()
        {
            m_pos = 0;
        }

    public:
        void append( const value_type * _value, size_type _size )
        {
            if( m_pos + _size >= Size )
            {
                return;
            }

            Helper::memoryCopy( m_buffer, (size_t)m_pos * sizeof( value_type ), _value, 0, (size_t)_size * sizeof( value_type ) );

            m_pos += _size;
            m_buffer[m_pos] = Helper::stringTerminalChar<T>();
        }

        void append( value_type * _value )
        {
            size_type size = (size_type)Helper::stringLength( _value );

            this->append( _value, size );
        }

        void append( const value_type * _value )
        {
            size_type size = (size_type)Helper::stringLength( _value );

            this->append( _value, size );
        }

        void append( value_type _ch )
        {
            if( m_pos + 1 >= Size )
            {
                return;
            }

            m_buffer[m_pos] = _ch;
            m_pos += 1;
            m_buffer[m_pos] = Helper::stringTerminalChar<T>();
        }

        template<class StringView>
        void append( const StringView & _value )
        {
            const value_type * value_str = _value.c_str();
            size_type value_size = (size_type)_value.size();

            this->append( value_str, value_size );
        }

    public:
        void assign( const value_type * _value, size_type _size )
        {
            if( _size >= Size )
            {
                return;
            }

            Helper::memoryCopy( m_buffer, 0, _value, 0, (size_t)_size * sizeof( value_type ) );

            m_pos = _size;
            m_buffer[m_pos] = Helper::stringTerminalChar<T>();
        }

        void assign( value_type * _value )
        {
            size_type size = (size_type)Helper::stringLength( _value );

            this->assign( _value, size );
        }

        void assign( const value_type * _value )
        {
            size_type size = (size_type)Helper::stringLength( _value );

            this->assign( _value, size );
        }

        void assign( value_type _ch )
        {
            if( 1 >= Size )
            {
                return;
            }

            m_buffer[0] = _ch;
            m_pos = 1;
            m_buffer[m_pos] = Helper::stringTerminalChar<T>();
        }

        template<class StringView>
        void assign( const StringView & _value )
        {
            const value_type * value_str = _value.c_str();
            size_type value_size = (size_type)_value.size();

            this->assign( value_str, value_size );
        }

    public:
        void replace_last( const value_type * _value )
        {
            size_type size = (size_type)Helper::stringLength( _value );

            if( size > m_pos )
            {
                return;
            }

            Helper::memoryCopy( m_buffer, (size_t)(m_pos - size) * sizeof( value_type ), _value, 0, size * sizeof( value_type ) );
        }

        void cut_before_last_of( value_type _ch )
        {
            value_type * ch_pos = Helper::stringFindLastOccurrenceChar( m_buffer, _ch );

            if( ch_pos == nullptr )
            {
                return;
            }

            ptrdiff_t d = ch_pos - m_buffer;

            m_pos = (size_type)d;
        }

    public:
        void operator = ( const value_type * _value )
        {
            this->clear();
            this->append( _value );
        }

        ArrayTString<T, Size> & operator += ( value_type * _value )
        {
            this->append( _value );

            return *this;
        }

        ArrayTString<T, Size> & operator += ( const value_type * _value )
        {
            this->append( _value );

            return *this;
        }

        ArrayTString<T, Size> & operator += ( value_type _value )
        {
            this->append( _value );

            return *this;
        }

        template<class StringView>
        void operator = ( const StringView & _value )
        {
            this->clear();
            this->append( _value );
        }

        template<class StringView>
        ArrayTString<T, Size> & operator += ( const StringView & _value )
        {
            this->append( _value );

            return *this;
        }

    protected:
        value_type m_buffer[Size] = {Helper::stringTerminalChar<T>()};
        size_type m_pos;
    };
    //////////////////////////////////////////////////////////////////////////
    template<class T, uint32_t SizeL, uint32_t SizeR>
    ArrayTString<T, (SizeL > SizeR ? SizeL : SizeR)> operator + ( const ArrayTString<T, SizeL> & _left, const ArrayTString<T, SizeR> & _right )
    {
        ArrayTString<T, (SizeL > SizeR ? SizeL : SizeR)> str;

        str += _left;
        str += _right;

        return str;
    }
    //////////////////////////////////////////////////////////////////////////
    template<class T, uint32_t Size>
    ArrayTString<T, Size> operator + ( const ArrayTString<T, Size> & _left, const Char * _right )
    {
        ArrayTString<T, Size> str;

        str += _left;
        str += _right;

        return str;
    }
    //////////////////////////////////////////////////////////////////////////
    template<class T, uint32_t Size>
    ArrayTString<T, Size> operator + ( const Char * _left, const ArrayTString<T, Size> & _right )
    {
        ArrayTString<T, Size> str;

        str += _left;
        str += _right;

        return str;
    }
    //////////////////////////////////////////////////////////////////////////
    template<class T, uint32_t Size, class R>
    ArrayTString<T, Size> operator + ( const ArrayTString<T, Size> & _left, const R & _right )
    {
        ArrayTString<T, Size> str;

        str += _left;
        str += _right.c_str();

        return str;
    }
    //////////////////////////////////////////////////////////////////////////
    template<class T, uint32_t Size, class R>
    ArrayTString<T, Size> operator + ( const R & _left, const ArrayTString<T, Size> & _right )
    {
        ArrayTString<T, Size> str;

        str += _left.c_str();
        str += _right;

        return str;
    }
    //////////////////////////////////////////////////////////////////////////
}
