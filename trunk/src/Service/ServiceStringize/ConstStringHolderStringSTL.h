#   pragma once

#   include "Core/ConstStringHolder.h"

namespace Menge
{
    class ConstStringHolderStringSTL
        : public ConstStringHolderT<char>
    {
    public:
        ConstStringHolderStringSTL();

    public:
        void setValue( const char * _value, size_t _size );

    protected:
        void _releaseString() override;

    protected:
        String m_value;
    };
}