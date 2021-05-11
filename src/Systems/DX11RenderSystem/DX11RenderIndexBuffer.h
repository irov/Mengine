#pragma once

#include "Interface/RenderIndexBufferInterface.h"

#include "Environment/DirectX11/DirectX11RenderIncluder.h"

#include "DX11RenderResourceHandler.h"
#include "DX11RenderBuffer.h"

#include "Kernel/Factorable.h"

namespace Mengine
{
    //////////////////////////////////////////////////////////////////////////
    class DX11RenderIndexBuffer
        : public RenderIndexBufferInterface
        , public Factorable
        , public DX11RenderBuffer
    {
        DECLARE_FACTORABLE( DX11RenderIndexBuffer );

    public:
        DX11RenderIndexBuffer();
        ~DX11RenderIndexBuffer() override;

    public:
        bool initialize( uint32_t _indexSize, EBufferType _bufferType ) override;
        void finalize() override;

    protected:
        uint32_t getIndexCount() const override;
        uint32_t getIndexSize() const override;

    public:
        void enable( ID3D11DeviceContext * _pD3DImmediateContext );
        void disable( ID3D11DeviceContext * _pD3DImmediateContext );

    protected:
        bool resize( uint32_t _indexCount ) override;

    protected:
        MemoryInterfacePtr lock( uint32_t _offset, uint32_t _count ) override;
        bool unlock() override;

    protected:
        bool draw( const void * _buffer, uint32_t _offset, uint32_t _count ) override;
    };
    //////////////////////////////////////////////////////////////////////////
    typedef IntrusivePtr<DX11RenderIndexBuffer, RenderIndexBufferInterface> DX11RenderIndexBufferPtr;
    //////////////////////////////////////////////////////////////////////////
}