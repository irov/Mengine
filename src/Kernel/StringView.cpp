#include "StringView.h"

#include "Config/StdAssert.h"
#include "Config/StdString.h"

namespace Mengine
{
    //////////////////////////////////////////////////////////////////////////
    StringView::StringView()
        : m_ptr( "" )
        , m_size( 0 )
    {
    }
    //////////////////////////////////////////////////////////////////////////
    StringView::StringView( const Char * _ptr, size_t _size )
        : m_ptr( _ptr )
        , m_size( _size )
    {
    }
    //////////////////////////////////////////////////////////////////////////
    StringView::~StringView()
    {
    }
    //////////////////////////////////////////////////////////////////////////
    void StringView::setup( const Char * _ptr, size_t _size )
    {
        m_ptr = _ptr;
        m_size = _size;
    }
    //////////////////////////////////////////////////////////////////////////
    const Char * StringView::data() const
    {
        return m_ptr;
    }
    //////////////////////////////////////////////////////////////////////////
    const Char * StringView::c_str() const
    {
        return m_ptr;
    }
    //////////////////////////////////////////////////////////////////////////
    size_t StringView::size() const
    {
        return m_size;
    }
    //////////////////////////////////////////////////////////////////////////
    bool StringView::empty() const
    {
        return m_size == 0;
    }
    //////////////////////////////////////////////////////////////////////////
    Char StringView::back() const
    {
        assert( m_size != 0 && "StringView::back empty view" );

        return m_ptr[m_size - 1];
    }
    //////////////////////////////////////////////////////////////////////////
    Char StringView::operator [] ( size_t _index ) const
    {
        assert( _index < m_size && "StringView::operator[] invalid index" );

        return m_ptr[_index];
    }
    //////////////////////////////////////////////////////////////////////////
    void StringView::remove_suffix( size_t _size )
    {
        assert( _size <= m_size && "StringView::remove_suffix invalid size" );

        m_size -= _size;
    }
    //////////////////////////////////////////////////////////////////////////
    size_t StringView::find_first_of( const Char * _characters, size_t _position ) const
    {
        if( _position >= m_size )
        {
            return npos;
        }

        for( size_t index = _position; index != m_size; ++index )
        {
            if( StdString::strchr( _characters, m_ptr[index] ) != nullptr )
            {
                return index;
            }
        }

        return npos;
    }
    //////////////////////////////////////////////////////////////////////////
    size_t StringView::rfind( const StringView & _value ) const
    {
        if( _value.m_size == 0 )
        {
            return m_size;
        }

        if( _value.m_size > m_size )
        {
            return npos;
        }

        size_t index = m_size - _value.m_size;

        for( ;; )
        {
            if( StdString::memcmp( m_ptr + index, _value.m_ptr, _value.m_size ) == 0 )
            {
                return index;
            }

            if( index == 0 )
            {
                break;
            }

            --index;
        }

        return npos;
    }
    //////////////////////////////////////////////////////////////////////////
    StringView StringView::substr( size_t _position, size_t _size ) const
    {
        assert( _position <= m_size && "StringView::substr invalid position" );

        const size_t available = m_size - _position;
        const size_t size = _size == npos || _size > available ? available : _size;

        return StringView( m_ptr + _position, size );
    }
    //////////////////////////////////////////////////////////////////////////
}
