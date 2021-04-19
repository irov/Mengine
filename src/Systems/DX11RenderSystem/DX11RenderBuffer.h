#pragma once

#include "Interface/RenderVertexBufferInterface.h"

#include "Environment/DirectX11/DirectX11RenderIncluder.h"

#include "DX11RenderResourceHandler.h"

#include "Kernel/Factorable.h"

namespace Mengine
{
    //////////////////////////////////////////////////////////////////////////
    class DX11RenderBuffer
        : public DX11RenderResourceHandler
    {
    public:
		DX11RenderBuffer();
        ~DX11RenderBuffer() override;

    public:
		MENGINE_INLINE bool initializeBuffer( uint32_t _elementSize, EBufferType _bufferType );
		MENGINE_INLINE void finalizeBuffer();

    protected:
        uint32_t getElementsCount() const;
        uint32_t getElementSize() const;

    protected:
		MENGINE_INLINE bool resizeBuffer( uint32_t _elementsCount, void* _initData );

    protected:
		MENGINE_INLINE MemoryInterfacePtr lockBuffer( uint32_t _offset, uint32_t _size );
		MENGINE_INLINE bool unlockBuffer();

    protected:
		MENGINE_INLINE bool drawBuffer( const void * _buffer, uint32_t _offset, uint32_t _count );

    protected:
        void onRenderReset() override;
        bool onRenderRestore() override;

    protected:
        EBufferType m_bufferType;
        uint32_t m_elementSize;
        uint32_t m_elementsCapacity;
        uint32_t m_elementsCount;

		D3D11_BUFFER_DESC m_desc;
		DXGI_FORMAT m_format;
		ID3D11Buffer * m_pD3DBuffer;

        MemoryProxyInterfacePtr m_memory;
    };
    //////////////////////////////////////////////////////////////////////////
    //typedef IntrusivePtr<DX11RenderVertexBuffer, RenderVertexBufferInterface> DX11RenderVertexBufferPtr;
    //////////////////////////////////////////////////////////////////////////
}