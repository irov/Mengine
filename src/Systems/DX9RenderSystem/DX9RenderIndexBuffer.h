#pragma once

#include "Interface/RenderIndexBufferInterface.h"

#include "Environment/DirectX9/DX9RenderIncluder.h"

#include "DX9RenderResourceHandler.h"

#include "Kernel/Factorable.h"

namespace Mengine
{
    //////////////////////////////////////////////////////////////////////////
    class DX9RenderIndexBuffer
        : public RenderIndexBufferInterface
        , public Factorable
        , public DX9RenderResourceHandler
    {
        DECLARE_FACTORABLE( DX9RenderIndexBuffer );

    public:
        DX9RenderIndexBuffer();
        ~DX9RenderIndexBuffer() override;

    public:
        bool initialize( uint32_t _indexSize, EBufferType _bufferType ) override;
        void finalize() override;

    protected:
        uint32_t getIndexCount() const override;
        uint32_t getIndexSize() const override;

    public:
        void enable();
        void disable();

    protected:
        bool resize( uint32_t _indexCount ) override;

    protected:
        MemoryInterfacePtr lock( uint32_t _offset, uint32_t _count ) override;
        bool unlock() override;

    protected:
        bool draw( const void * _buffer, uint32_t _offset, uint32_t _count ) override;

    protected:
        void onRenderReset() override;
        bool onRenderRestore() override;

    protected:
        EBufferType m_bufferType;
        uint32_t m_indexSize;
        uint32_t m_indexCapacity;
        uint32_t m_indexCount;

        DWORD m_usage;
        D3DFORMAT m_format;
        D3DPOOL m_pool;
        IDirect3DIndexBuffer9 * m_pD3DIndexBuffer;

        MemoryProxyInterfacePtr m_memory;
    };
    //////////////////////////////////////////////////////////////////////////
    typedef IntrusivePtr<DX9RenderIndexBuffer, RenderIndexBufferInterface> DX9RenderIndexBufferPtr;
    //////////////////////////////////////////////////////////////////////////
}