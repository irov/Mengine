#pragma once

#include "Interface/ServiceInterface.h"
#include "Interface/MemoryInterface.h"

#include "Kernel/Factorable.h"

namespace Mengine
{
    class MemoryBuffer
        : public MemoryBufferInterface
        , public Factorable
    {
    public:
        MemoryBuffer();
        ~MemoryBuffer() override;

    public:
        void setBuffer( const void * _ptr, size_t _size, const Char * _doc, const Char * _file, uint32_t _line ) override;
        Pointer newBuffer( size_t _size, const Char * _doc, const Char * _file, uint32_t _line ) override;

    public:
        inline Pointer getBuffer() const override;
        inline size_t getSize() const override;
        inline bool empty() const override;

    protected:
        void * m_memory;
        const Char * m_doc;
        size_t m_size;

#ifdef MENGINE_DEBUG
        const Char * m_file;
        uint32_t m_line;
#endif
    };
    //////////////////////////////////////////////////////////////////////////
    inline Pointer MemoryBuffer::getBuffer() const
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