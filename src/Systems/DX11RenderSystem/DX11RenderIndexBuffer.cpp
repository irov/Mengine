#include "DX11RenderIndexBuffer.h"

#include "Interface/MemoryServiceInterface.h"

#include "DX11RenderEnum.h"
#include "DX11ErrorHelper.h"

#include "Kernel/Assertion.h"
#include "Kernel/AssertionMemoryPanic.h"
#include "Kernel/Logger.h"
#include "Kernel/Documentable.h"
#include "Kernel/DocumentHelper.h"
#include "Kernel/DocumentableHelper.h"

#include "stdex/memorycopy.h"

namespace Mengine
{
    //////////////////////////////////////////////////////////////////////////
    DX11RenderIndexBuffer::DX11RenderIndexBuffer()
    {
    }
    //////////////////////////////////////////////////////////////////////////
    DX11RenderIndexBuffer::~DX11RenderIndexBuffer()
    {
    }
    //////////////////////////////////////////////////////////////////////////
    bool DX11RenderIndexBuffer::initialize( uint32_t _indexSize, EBufferType _bufferType )
    {
        m_format = Helper::getD3DIndexFormat();

        return this->initializeBuffer( _indexSize, _bufferType );
    }
    //////////////////////////////////////////////////////////////////////////
    void DX11RenderIndexBuffer::finalize()
    {
        this->finalizeBuffer();
    }
    //////////////////////////////////////////////////////////////////////////
    uint32_t DX11RenderIndexBuffer::getIndexCount() const
    {
        return this->getElementsCount();
    }
    //////////////////////////////////////////////////////////////////////////
    uint32_t DX11RenderIndexBuffer::getIndexSize() const
    {
        return this->getElementSize();
    }
    //////////////////////////////////////////////////////////////////////////
    bool DX11RenderIndexBuffer::resize( uint32_t _indexCount )
    {
        // TODO: in D3D11 we can create static bufferes without lock - if we provide data pointer on creation
        return this->resizeBuffer( D3D11_BIND_INDEX_BUFFER, _indexCount, nullptr );
    }
    //////////////////////////////////////////////////////////////////////////
    MemoryInterfacePtr DX11RenderIndexBuffer::lock( uint32_t _offset, uint32_t _count )
    {
        return this->lockBuffer( _offset, _count );
    }
    //////////////////////////////////////////////////////////////////////////
    bool DX11RenderIndexBuffer::unlock()
    {
        return this->unlockBuffer();
    }
    //////////////////////////////////////////////////////////////////////////
    bool DX11RenderIndexBuffer::draw( const void * _buffer, uint32_t _offset, uint32_t _count )
    {
        return this->drawBuffer( _buffer, _offset, _count );
    }
    //////////////////////////////////////////////////////////////////////////
    void DX11RenderIndexBuffer::enable( const ID3D11DeviceContextPtr & _pImmediateContext )
    {
        UINT sOffset = 0;

        ID3D11Buffer * pD3DBuffer = m_pD3DBuffer.Get();
        _pImmediateContext->IASetIndexBuffer( pD3DBuffer, m_format, sOffset );
    }
    //////////////////////////////////////////////////////////////////////////
    void DX11RenderIndexBuffer::disable( const ID3D11DeviceContextPtr & _pImmediateContext )
    {
        _pImmediateContext->IASetIndexBuffer( nullptr, m_format, 0 );
    }
    //////////////////////////////////////////////////////////////////////////
}