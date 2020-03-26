#pragma once

#include "Kernel/ConstStringHolder.h"

namespace Mengine
{
    class ConstStringHolderLocalString
        : public ConstStringHolder
        , public stdex::intrusive_linked<ConstStringHolderLocalString>
    {
    public:
        ConstStringHolderLocalString();
        ~ConstStringHolderLocalString() override;
    };
}