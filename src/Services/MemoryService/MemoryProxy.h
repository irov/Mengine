#pragma once

#include "Interface/MemoryInterface.h"

#include "Kernel/Factorable.h"

#include "stdex/thread_guard.h"

#ifdef MENGINE_DEBUG
#include "Config/String.h"
#endif

namespace Mengine
{
    class MemoryProxy
        : public MemoryProxyInterface
        , public Factorable
    {
    public:
        MemoryProxy();
        ~MemoryProxy() override;

    public:
        void setBuffer( void * _ptr, size_t _size, const Char * _doc ) override;

    public:
        Pointer getBuffer() const override;
        size_t getSize() const override;
        bool empty() const override;

    protected:
        void * m_ptr;
        size_t m_size;

#ifdef MENGINE_DEBUG
        String m_doc;
#endif

        STDEX_THREAD_GUARD_INIT;
    };
    //////////////////////////////////////////////////////////////////////////
    typedef IntrusivePtr<MemoryProxy> MemoryProxyPtr;
    //////////////////////////////////////////////////////////////////////////
}


