#pragma once

#include "Interface/RenderVertexBufferInterface.h"

#include "Environment/DirectX11/DX11RenderIncluder.h"

#include "DX11RenderResourceHandler.h"
#include "DX11RenderBuffer.h"

#include "Kernel/Factorable.h"

namespace Mengine
{
    //////////////////////////////////////////////////////////////////////////
    class DX11RenderVertexBuffer
        : public RenderVertexBufferInterface
        , public Factorable
        , public DX11RenderBuffer
    {
        DECLARE_FACTORABLE( DX11RenderVertexBuffer );

    public:
        DX11RenderVertexBuffer();
        ~DX11RenderVertexBuffer() override;

    public:
        bool initialize( uint32_t _vertexSize, EBufferType _bufferType ) override;
        void finalize() override;

    protected:
        uint32_t getVertexCount() const override;
        uint32_t getVertexSize() const override;

    public:
        void enable( const ID3D11DeviceContextPtr & _pImmediateContext );
        void disable( const ID3D11DeviceContextPtr & _pImmediateContext );

    protected:
        bool resize( uint32_t _vertexCount ) override;

    protected:
        MemoryInterfacePtr lock( uint32_t _offset, uint32_t _size ) override;
        bool unlock() override;

    protected:
        bool draw( const void * _buffer, uint32_t _offset, uint32_t _count ) override;
    };
    //////////////////////////////////////////////////////////////////////////
    typedef IntrusivePtr<DX11RenderVertexBuffer, RenderVertexBufferInterface> DX11RenderVertexBufferPtr;
    //////////////////////////////////////////////////////////////////////////
}