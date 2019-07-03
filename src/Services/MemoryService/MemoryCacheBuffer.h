#pragma once

#include "Interface/ServiceInterface.h"
#include "Interface/MemoryInterface.h"

#include "Kernel/Factorable.h"

#include "Kernel/Factorable.h"

#include "Kernel/FilePath.h"
#include "Kernel/ConstString.h"

namespace Mengine
{
    class MemoryService;

    class MemoryCacheBuffer
        : public MemoryBufferInterface
        , public Factorable
    {
    public:
        MemoryCacheBuffer();
        ~MemoryCacheBuffer() override;

    public:
        void setMemoryManager( MemoryService * _memoryManager );

    public:
        void setBuffer( const void * _ptr, size_t _size, const Char * _doc ) override;
        Pointer newBuffer( size_t _size, const Char * _doc ) override;

    public:
        inline Pointer getBuffer() const override;

        inline size_t getSize() const override;
        inline bool empty() const override;

    protected:
        void uncache_();

    protected:
        MemoryService * m_memoryManager;

        uint32_t m_bufferId;

        void * m_data;
        size_t m_size;

#ifdef MENGINE_DEBUG
        const Char * m_doc;
#endif
    };
    //////////////////////////////////////////////////////////////////////////	
    inline Pointer MemoryCacheBuffer::getBuffer() const
    {
        return m_data;
    }
    //////////////////////////////////////////////////////////////////////////
    inline size_t MemoryCacheBuffer::getSize() const
    {
        return m_size;
    }
    //////////////////////////////////////////////////////////////////////////
    inline bool MemoryCacheBuffer::empty() const
    {
        return m_size == 0;
    }
    //////////////////////////////////////////////////////////////////////////
    typedef IntrusivePtr<MemoryCacheBuffer> MemoryCacheBufferPtr;
    //////////////////////////////////////////////////////////////////////////
}


