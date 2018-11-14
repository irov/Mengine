#pragma once

#include "Kernel/MemoryAllocator.h"

namespace Mengine
{
    template<class Base>
    class FactorableUnique
        : public Base
        , public MemoryAllocator<Base>
    {
    public:
        template<class ... Args>
        FactorableUnique( Args ... args )
            : Base( args ... )
        {
        }

        ~FactorableUnique() override
        {
        }

    public:
        void destroy() override
        {
            this->_destroy();

            delete this;
        }
    };
}
