#include "DX11RenderVertexBuffer.h"

#include "Interface/MemoryServiceInterface.h"

#include "DX11RenderEnum.h"
#include "DX11ErrorHelper.h"

#include "Kernel/Logger.h"
#include "Kernel/Documentable.h"
#include "Kernel/DocumentHelper.h"
#include "Kernel/DocumentableHelper.h"
#include "Kernel/Assertion.h"
#include "Kernel/AssertionMemoryPanic.h"

#include "stdex/memorycopy.h"

namespace Mengine
{
    //////////////////////////////////////////////////////////////////////////
    DX11RenderVertexBuffer::DX11RenderVertexBuffer()
    {
        m_desc.BindFlags = D3D11_BIND_VERTEX_BUFFER;
    }
    //////////////////////////////////////////////////////////////////////////
    DX11RenderVertexBuffer::~DX11RenderVertexBuffer()
    {
    }
    //////////////////////////////////////////////////////////////////////////
    bool DX11RenderVertexBuffer::initialize( uint32_t _vertexSize, EBufferType _bufferType )
    {
        return initializeBuffer( _vertexSize, _bufferType );
    }
    //////////////////////////////////////////////////////////////////////////
    void DX11RenderVertexBuffer::finalize()
    {
        finalizeBuffer();
    }
    //////////////////////////////////////////////////////////////////////////
    uint32_t DX11RenderVertexBuffer::getVertexCount() const
    {
        return getElementsCount();
    }
    //////////////////////////////////////////////////////////////////////////
    uint32_t DX11RenderVertexBuffer::getVertexSize() const
    {
        return getElementSize();
    }
    //////////////////////////////////////////////////////////////////////////
    bool DX11RenderVertexBuffer::resize( uint32_t _vertexCount )
    {
        // TODO: in D3D11 we can create static bufferes without lock - if we provide data pointer on creation
        return resizeBuffer( _vertexCount, nullptr );
    }
    //////////////////////////////////////////////////////////////////////////
    MemoryInterfacePtr DX11RenderVertexBuffer::lock( uint32_t _offset, uint32_t _count )
    {
        return lockBuffer( _offset, _count );
    }
    //////////////////////////////////////////////////////////////////////////
    bool DX11RenderVertexBuffer::unlock()
    {
        return unlockBuffer();
    }
    //////////////////////////////////////////////////////////////////////////
    bool DX11RenderVertexBuffer::draw( const void * _buffer, uint32_t _offset, uint32_t _count )
    {
        return drawBuffer( _buffer, _offset, _count );
    }
    //////////////////////////////////////////////////////////////////////////
    void DX11RenderVertexBuffer::enable( const ID3D11DeviceContextPtr & _pImmediateContext )
    {
        UINT sStride = m_elementSize;
        UINT sOffset = 0;

        ID3D11Buffer * d3dBuffer = m_pD3DBuffer.Get();
        _pImmediateContext->IASetVertexBuffers( 0, 1, &d3dBuffer, &sStride, &sOffset );
    }
    //////////////////////////////////////////////////////////////////////////
    void DX11RenderVertexBuffer::disable( const ID3D11DeviceContextPtr & _pImmediateContext )
    {
        UINT sStride = 0;
        UINT sOffset = 0;        
        _pImmediateContext->IASetVertexBuffers( 0, 1, nullptr, &sStride, &sOffset );
    }
    //////////////////////////////////////////////////////////////////////////
}