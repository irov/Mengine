#pragma once

#include "Kernel/ConstStringHolder.h"

namespace Mengine
{
    class ConstStringHolderLocalString
        : public ConstStringHolder
    {
    public:
        ConstStringHolderLocalString();
        ~ConstStringHolderLocalString() override;
    };
}