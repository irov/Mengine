#   pragma once

#   include "stdex/const_string_holder.h"
#   include "stdex/stl_vector.h"

#   include "Factory/Factorable.h"

#   include "Config/Typedef.h"
#   include "Config/String.h"

namespace Menge
{
    class ConstStringHolderStringSTL
        : public stdex::const_string_holder
        , public Factorable
    {
    public:
        ConstStringHolderStringSTL();

    public:
        void setValue( const char * _value, size_t _size );

    protected:
        void _releaseString() override;
        void _destroyString() override;

    protected:
		char * m_buff;
    };
}