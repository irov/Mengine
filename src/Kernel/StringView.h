#pragma once

#include "Config/Char.h"
#include "Config/StdDef.h"

namespace Mengine
{
    class StringView
    {
    public:
        static constexpr size_t npos = static_cast<size_t>(-1);

    public:
        StringView();
        StringView( const Char * _ptr, size_t _size );
        ~StringView();

    public:
        void setup( const Char * _ptr, size_t _size );

    public:
        const Char * data() const;
        const Char * c_str() const;
        size_t size() const;
        bool empty() const;
        Char back() const;
        Char operator [] ( size_t _index ) const;

    public:
        void remove_suffix( size_t _size );
        size_t find_first_of( const Char * _characters, size_t _position = 0 ) const;
        size_t rfind( const StringView & _value ) const;
        StringView substr( size_t _position, size_t _size = npos ) const;

    protected:
        const Char * m_ptr;
        size_t m_size;
    };
}
