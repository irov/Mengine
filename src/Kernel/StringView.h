#pragma once

#include "Config/Char.h"
#include "Config/StdDef.h"

namespace Mengine
{
    class StringView
    {
    public:
        StringView();
        StringView( const Char * _ptr, size_t _size );
        ~StringView();

    public:
        void setup( const Char * _ptr, size_t _size );

    public:
        const Char * c_str() const;
        size_t size() const;

    protected:
        const Char * m_ptr;
        size_t m_size;
    };
}