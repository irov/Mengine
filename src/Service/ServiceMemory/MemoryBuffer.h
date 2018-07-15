#pragma once

#include "Interface/ServiceInterface.h"
#include "Interface/MemoryInterface.h"

#include "Kernel/ServantBase.h"

#include "Kernel/Factorable.h"

namespace Mengine
{
    class MemoryBuffer
        : public ServantBase<MemoryBufferInterface>
    {
    public:
        MemoryBuffer();
        ~MemoryBuffer() override;

    public:
        void setMemory( const void * _ptr, size_t _size, const Char * _file, uint32_t _line ) override;
        Pointer newMemory( size_t _size, const char * _file, uint32_t _line ) override;

    public:
        inline Pointer getMemory() const override;
        inline size_t getSize() const override;
        inline bool empty() const override;

    protected:
        void * m_memory;
        size_t m_size;

#ifndef NDEBUG
        const Char * m_file;
        uint32_t m_line;
#endif
    };
    //////////////////////////////////////////////////////////////////////////
    inline Pointer MemoryBuffer::getMemory() const
    {
        return m_memory;
    }
    //////////////////////////////////////////////////////////////////////////
    inline size_t MemoryBuffer::getSize() const
    {
        return m_size;
    }
    //////////////////////////////////////////////////////////////////////////
    inline bool MemoryBuffer::empty() const
    {
        return m_size == 0;
    }
    //////////////////////////////////////////////////////////////////////////
    typedef IntrusivePtr<MemoryBuffer> MemoryPtr;
}