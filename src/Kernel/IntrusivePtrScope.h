#pragma once

#include "Kernel/IntrusivePtrBase.h"

namespace Mengine
{
    class IntrusivePtrScope
    {
    public:
        explicit IntrusivePtrScope( IntrusivePtrBase * _ptr ) noexcept
            : m_ptr( _ptr )
        {
            m_ptr->incref();
        }

        ~IntrusivePtrScope() noexcept
        {
            m_ptr->decref();
        }

    protected:
        IntrusivePtrBase * m_ptr;
    };
}