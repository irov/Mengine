#pragma once

#include "Config/Config.h"

namespace Mengine
{
    //////////////////////////////////////////////////////////////////////////
    template<class T>
    class Range
    {
    public:
        Range( T * _begin, T * _end )
            : m_begin( _begin )
            , m_end( _end )
        {
        }

    public:
        T * begin()
        {
            return m_begin;
        }

        T * end()
        {
            return m_end;
        }

    protected:
        T * m_begin;
        T * m_end;
    };
    //////////////////////////////////////////////////////////////////////////
    namespace Helper
    {
        template<class T>
        Range<T> makeRange( T * _ptr, size_t _size )
        {
            return Range<T>( _ptr, _ptr + _size );
        }
    }
    //////////////////////////////////////////////////////////////////////////
}