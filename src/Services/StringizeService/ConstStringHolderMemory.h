#pragma once

#include "Kernel/String.h"
#include "Kernel/ConstStringHolder.h"
#include "Kernel/IntrusiveLinked.h"

namespace Mengine
{
    class ConstStringHolderMemory
        : public ConstStringHolder
        , public IntrusiveLinked<ConstStringHolderMemory>
    {
    public:
        ConstStringHolderMemory();
        ~ConstStringHolderMemory() override;

    public:
        void setValue( const ConstStringHolder::value_type * _value, ConstStringHolder::size_type _size, hash_type _hash );
    };
}