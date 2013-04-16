#   pragma once

#   include "Core/ConstStringHolder.h"

namespace Menge
{
    class ConstStringHolderString256
        : public ConstStringHolder
    {
    public:
        ConstStringHolderString256();

    public:
        void setValue( const char * _value, size_t _size );

    protected:
        const char * _c_str() const override
        {
            return m_value;
        }

    protected:			
        char m_value[256];
    };
}