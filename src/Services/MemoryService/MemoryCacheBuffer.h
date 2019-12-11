#pragma once

#include "Interface/ServiceInterface.h"
#include "Interface/MemoryInterface.h"

#include "Kernel/Factorable.h"

#include "Kernel/Factorable.h"

#include "Kernel/FilePath.h"
#include "Kernel/ConstString.h"

#ifdef MENGINE_DEBUG
#include "Kernel/DocumentHelper.h"
#endif

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
        void setBuffer( const void * _ptr, size_t _size, const DocumentPtr & _doc ) override;
        Pointer newBuffer( size_t _size, const DocumentPtr & _doc ) override;

    public:
        MENGINE_INLINE Pointer getBuffer() const override;

        MENGINE_INLINE size_t getSize() const override;
        MENGINE_INLINE bool empty() const override;

    protected:
        void uncache_();

    protected:
        MemoryService * m_memoryManager;

        uint32_t m_bufferId;

        void * m_data;
        size_t m_size;

#ifdef MENGINE_DEBUG
        DocumentPtr m_doc;
#endif
    };
    //////////////////////////////////////////////////////////////////////////
    MENGINE_INLINE Pointer MemoryCacheBuffer::getBuffer() const
    {
        return m_data;
    }
    //////////////////////////////////////////////////////////////////////////
    MENGINE_INLINE size_t MemoryCacheBuffer::getSize() const
    {
        return m_size;
    }
    //////////////////////////////////////////////////////////////////////////
    MENGINE_INLINE bool MemoryCacheBuffer::empty() const
    {
        return m_size == 0;
    }
    //////////////////////////////////////////////////////////////////////////
    typedef IntrusivePtr<MemoryCacheBuffer> MemoryCacheBufferPtr;
    //////////////////////////////////////////////////////////////////////////
}


