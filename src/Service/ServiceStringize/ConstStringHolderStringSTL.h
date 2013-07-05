#   pragma once

#   include "Core/ConstStringHolder.h"

#   include "Config/Typedef.h"
#   include "Config/String.h"

namespace Menge
{
    class ConstStringHolderStringSTL
        : public ConstStringHolder
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