#   pragma once

#   include "Core/ConstStringHolder.h"

#   include <memory.h>

namespace Menge
{
    template<size_t I_Size>
    class ConstStringHolderStringSize
        : public ConstStringHolder
    {
    public:
        void setValue( const char * _value, size_t _size )
        {
            memcpy( m_value, _value, _size );
            m_value[_size] = 0;

            this->setup( m_value, _size );
        }

    protected:
        const char * _c_str() const override
        {
            return m_value;
        }

    protected:			
        char m_value[I_Size];
    };
}