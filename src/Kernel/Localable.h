#pragma once

#include "Kernel/Mixin.h"

namespace Mengine
{
    class Localable
        : public Mixin
    {
    public:
        Localable();
        ~Localable() override;

    protected:
        uint32_t incref() override;
        void decref() override;
        uint32_t getrefcount() const override;
    };
}