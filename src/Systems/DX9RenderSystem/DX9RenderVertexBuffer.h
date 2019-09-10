#pragma once

#include "Interface/RenderSystemInterface.h"

#include "DX9RenderResourceHandler.h"

#include "Kernel/Factorable.h"

#include "d3d9.h"

namespace Mengine
{
    class DX9RenderVertexBuffer
        : public RenderVertexBufferInterface
        , public Factorable
        , public DX9RenderResourceHandler
    {
    public:
        DX9RenderVertexBuffer();
        ~DX9RenderVertexBuffer() override;

    public:
        bool initialize( IDirect3DDevice9 * _pD3DDevice, uint32_t _vertexSize, EBufferType _bufferType );
        void finalize();

    protected:
        uint32_t getVertexCount() const override;
        uint32_t getVertexSize() const override;

    protected:
        void enable() override;
        void disable() override;

    protected:
        bool resize( uint32_t _count ) override;

    protected:
        MemoryInterfacePtr lock( uint32_t _offset, uint32_t _size, const Char * _doc ) override;
        bool unlock() override;

    protected:
        bool draw( const void * _buffer, size_t _size, const Char * _doc ) override;

    protected:
        void onRenderReset() override;
        bool onRenderRestore() override;

    protected:
        IDirect3DDevice9 * m_pD3DDevice;

        EBufferType m_bufferType;
        uint32_t m_vertexSize;
        uint32_t m_vertexCount;

        DWORD m_usage;
        D3DFORMAT m_format;
        D3DPOOL m_pool;
        IDirect3DVertexBuffer9 * m_pVB;

        MemoryProxyInterfacePtr m_memory;
    };
    //////////////////////////////////////////////////////////////////////////
    typedef IntrusivePtr<DX9RenderVertexBuffer> DX9RenderVertexBufferPtr;
}