#pragma once

#include "Interface/RenderSystemInterface.h"

#include "DX9RenderResourceHandler.h"

#include "d3d9.h"

namespace Mengine
{
    class DX9RenderIndexBuffer
        : public RenderIndexBufferInterface
        , public Factorable
        , public DX9RenderResourceHandler
    {
    public:
        DX9RenderIndexBuffer();
        ~DX9RenderIndexBuffer() override;

    public:
        bool initialize( IDirect3DDevice9 * _pD3DDevice, uint32_t _indexSize, EBufferType _bufferType );
        void finalize();

    protected:
        uint32_t getIndexCount() const override;
        uint32_t getIndexSize() const override;

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
        uint32_t m_indexSize;
        uint32_t m_indexCount;

        DWORD m_usage;
        D3DFORMAT m_format;
        D3DPOOL m_pool;
        IDirect3DIndexBuffer9 * m_pIB;

        MemoryProxyInterfacePtr m_memory;
    };
    //////////////////////////////////////////////////////////////////////////
    typedef IntrusivePtr<DX9RenderIndexBuffer> DX9RenderIndexBufferPtr;
    //////////////////////////////////////////////////////////////////////////
}