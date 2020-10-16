#pragma once

#include "Kernel/ConstStringHolder.h"
#include "Kernel/IntrusiveLinked.h"

namespace Mengine
{
    class ConstStringHolderLocalString
        : public ConstStringHolder
        , public IntrusiveLinked<ConstStringHolderLocalString>
    {
    public:
        ConstStringHolderLocalString();
        ~ConstStringHolderLocalString() override;
    };
}