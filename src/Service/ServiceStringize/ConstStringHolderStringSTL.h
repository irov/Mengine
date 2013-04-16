#   pragma once

#   include "Core/ConstStringHolder.h"

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
        const char * _c_str() const override
        {
            const char * str = m_value.c_str();

            return str;
        }

    protected:
        void releaseString() override;

    protected:			
        String m_value;
    };
}