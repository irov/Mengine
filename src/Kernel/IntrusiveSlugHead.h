#pragma once

#include "Kernel/IntrusivePtr.h"
#include "Kernel/IntrusivePtrBase.h"
#include "Kernel/IntrusiveSlugLinkedPtr.h"

namespace Mengine
{
    template<class T, class D = void>
    class IntrusiveSlugHead
        : public IntrusiveSlugLinkedPtr<T, D, IntrusivePtr, IntrusivePtrBase>
    {
    public:
        IntrusiveSlugHead()
            : IntrusiveSlugLinkedPtr<T, D, IntrusivePtr, IntrusivePtrBase>( stdex::EILT_END )
        {
        }

    protected:
        uint32_t incref() noexcept override
        {
            return 1;
        }

        void decref() noexcept override
        {
        }

        uint32_t getrefcount() const noexcept override
        {
            return 1;
        };
    };
}
