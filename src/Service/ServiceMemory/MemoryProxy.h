#pragma once

#include "Interface/StreamInterface.h"
#include "Interface/MemoryInterface.h"

#include "Core/ServantBase.h"

#include "stdex/thread_guard.h"

namespace Mengine
{
    class MemoryProxy
        : public ServantBase<MemoryProxyInterface>
    {
    public:
        MemoryProxy();
        ~MemoryProxy() override;

    public:
        void setMemory( void * _ptr, size_t _size, const Char * _file, uint32_t _line ) override;

    public:
        Pointer getMemory() const override;
        size_t getSize() const override;
        bool empty() const override;

    protected:
        void * m_ptr;
        size_t m_size;

        const Char * m_file;
        uint32_t m_line;

        STDEX_THREAD_GUARD_INIT;
    };
    //////////////////////////////////////////////////////////////////////////
    typedef stdex::intrusive_ptr<MemoryProxy> MemoryProxyPtr;
    //////////////////////////////////////////////////////////////////////////
}


