#pragma once

#include "Kernel/IntrusivePtr.h"
#include "Kernel/IntrusivePtrBase.h"
#include "Kernel/IntrusiveSlugLinkedPtr.h"

namespace Mengine
{
    template<class T>
    class IntrusiveSlug
        : public IntrusiveSlugLinkedPtr<typename T::value_type, typename T::derived_type, IntrusivePtr, IntrusivePtrBase>
    {
    public:
        IntrusiveSlug()
            : IntrusiveSlugLinkedPtr<typename T::value_type, typename T::derived_type, IntrusivePtr, IntrusivePtrBase>( stdex::EILT_SLUG )
        {
        }

        ~IntrusiveSlug()
        {
        }

    protected:
        uint32_t incref() override
        {
            return 1;
        }

        void decref() override
        {
        }

        uint32_t getrefcount() const override
        {
            return 1;
        };
    };
}
