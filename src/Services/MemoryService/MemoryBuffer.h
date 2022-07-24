#pragma once

#include "Interface/ServiceInterface.h"
#include "Interface/MemoryInterface.h"

#include "Kernel/Factorable.h"

#ifdef MENGINE_DEBUG
#include "Kernel/DocumentHelper.h"
#endif

namespace Mengine
{
    //////////////////////////////////////////////////////////////////////////
    class MemoryBuffer
        : public MemoryBufferInterface
        , public Factorable
    {
        DECLARE_FACTORABLE( MemoryBuffer );

    public:
        MemoryBuffer();
        ~MemoryBuffer() override;

    public:
        void setBuffer( const void * _ptr, size_t _size ) override;
        Pointer newBuffer( size_t _size ) override;
        void clearBuffer() override;

    public:
        MENGINE_INLINE Pointer getBuffer() const override;
        MENGINE_INLINE size_t getSize() const override;
        MENGINE_INLINE bool empty() const override;

    protected:
        void * m_memory;
        size_t m_size;
    };
    //////////////////////////////////////////////////////////////////////////
    MENGINE_INLINE Pointer MemoryBuffer::getBuffer() const
    {
        return m_memory;
    }
    //////////////////////////////////////////////////////////////////////////
    MENGINE_INLINE size_t MemoryBuffer::getSize() const
    {
        return m_size;
    }
    //////////////////////////////////////////////////////////////////////////
    MENGINE_INLINE bool MemoryBuffer::empty() const
    {
        return m_size == 0;
    }
    //////////////////////////////////////////////////////////////////////////
    typedef IntrusivePtr<MemoryBuffer> MemoryPtr;
    //////////////////////////////////////////////////////////////////////////
}