#pragma once

#include "Kernel/String.h"
#include "Kernel/ConstStringHolder.h"

namespace Mengine
{
    class ConstStringHolderMemory
        : public ConstStringHolder
        , public stdex::intrusive_linked<ConstStringHolderMemory>
    {
    public:
        ConstStringHolderMemory();
        ~ConstStringHolderMemory() override;

    public:
        void setValue( const ConstStringHolder::value_type * _value, ConstStringHolder::size_type _size, hash_type _hash );
    };
}