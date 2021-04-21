#include "DX11RenderBuffer.h"

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
    DX11RenderBuffer::DX11RenderBuffer()
        : m_bufferType( BT_STATIC )
        , m_elementSize( 0 )
        , m_elementsCapacity( 0 )
        , m_elementsCount( 0 )
        , m_format(DXGI_FORMAT_UNKNOWN)
        , m_pD3DBuffer( nullptr )
    {
    }
    //////////////////////////////////////////////////////////////////////////
    DX11RenderBuffer::~DX11RenderBuffer()
    {
        MENGINE_ASSERTION_FATAL(m_pD3DBuffer == nullptr );
    }
    //////////////////////////////////////////////////////////////////////////
    bool DX11RenderBuffer::initializeBuffer(uint32_t _elementSize, EBufferType _bufferType)
    {
        m_elementSize = _elementSize;
        m_bufferType = _bufferType;

		m_desc.CPUAccessFlags = 0;
		m_desc.MiscFlags = 0;

        switch( m_bufferType )
        {
        case BT_STATIC:
            {
				m_desc.Usage = D3D11_USAGE_DEFAULT;
            }break;
        case BT_STREAM:
        case BT_DYNAMIC:
            {
				m_desc.Usage = D3D11_USAGE_DYNAMIC;
            }break;
        };

        MemoryProxyInterfacePtr memory = MEMORY_SERVICE()
            ->createMemoryProxy( MENGINE_DOCUMENT_FACTORABLE );

        MENGINE_ASSERTION_MEMORY_PANIC( memory, "invalid create memory proxy" );

        m_memory = memory;

        return true;
    }
    //////////////////////////////////////////////////////////////////////////
    void DX11RenderBuffer::finalizeBuffer()
    {
        m_memory = nullptr;

        DXRELEASE(m_pD3DBuffer);
    }
    //////////////////////////////////////////////////////////////////////////
    uint32_t DX11RenderBuffer::getElementsCount() const
    {
        return m_elementsCount;
    }
    //////////////////////////////////////////////////////////////////////////
    uint32_t DX11RenderBuffer::getElementSize() const
    {
        return m_elementSize;
    }
    //////////////////////////////////////////////////////////////////////////
    bool DX11RenderBuffer::resizeBuffer( uint32_t _elementsCount, void* _initData)
    {
        m_elementsCount = _elementsCount;

        if( m_elementsCapacity >= m_elementsCount)
        {
            return true;
        }

        DXRELEASE(m_pD3DBuffer);

		m_elementsCapacity = m_elementsCount;

        uint32_t bufferSize = m_elementsCapacity * m_elementSize;
		m_desc.ByteWidth = (UINT)(bufferSize);

		// Define the resource data.
		D3D11_SUBRESOURCE_DATA InitData;
		InitData.pSysMem = _initData;
		InitData.SysMemPitch = 0;
		InitData.SysMemSlicePitch = 0;

        IF_DXCALL( m_pD3DDevice, CreateBuffer, (&m_desc, &InitData, &m_pD3DBuffer) )
        {
            return false;
        }

        return true;
    }
    //////////////////////////////////////////////////////////////////////////
    MemoryInterfacePtr DX11RenderBuffer::lockBuffer( uint32_t _offset, uint32_t _count )
    {
        if( _offset + _count > m_elementsCount)
        {
            LOGGER_ERROR( "lock count %u offset %u more max size %u (doc: %s)"
                , _count
                , _offset
                , m_elementsCount
                , MENGINE_DOCUMENTABLE_STR( this, "DX11RenderIndexBuffer" )
            );

            return nullptr;
        }

        MENGINE_ASSERTION_FATAL( m_memory->getBuffer() == nullptr );

        uint32_t offsetSize = _offset * m_elementSize;
        uint32_t lockSize = _count * m_elementSize;

		D3D11_MAPPED_SUBRESOURCE mappedResource;

		// TODO: to remove release from code better get context ones for DX11RenderResourceHandler
		ID3D11DeviceContext *pImmediateContext = nullptr;
		m_pD3DDevice->GetImmediateContext(&pImmediateContext);

		auto hResult = pImmediateContext->Map(m_pD3DBuffer, 0, D3D11_MAP_READ, 0, &mappedResource);
		if (FAILED(hResult))
		{
			// TODO: add error log
			return nullptr;
		}

		pImmediateContext->Release();

        m_memory->setBuffer(mappedResource.pData, mappedResource.RowPitch);

        return m_memory;
    }
    //////////////////////////////////////////////////////////////////////////
    bool DX11RenderBuffer::unlockBuffer()
    {
        m_memory->setBuffer( nullptr, 0 );

		ID3D11DeviceContext *pImmediateContext = nullptr;
		m_pD3DDevice->GetImmediateContext(&pImmediateContext);

		pImmediateContext->Unmap(m_pD3DBuffer, 0);
		pImmediateContext->Release();

        return true;
    }
    //////////////////////////////////////////////////////////////////////////
    bool DX11RenderBuffer::drawBuffer( const void * _buffer, uint32_t _offset, uint32_t _count )
    {
        if( _count > m_elementsCount)
        {
            LOGGER_ERROR( "draw count %u more capacity %u (doc: %s)"
                , _count
                , m_elementsCount
                , MENGINE_DOCUMENTABLE_STR( this, "DX11RenderIndexBuffer" )
            );

            return false;
        }

		// TODO: somehow needs to reinterpret offset to subresource
        // UINT offsetToLock = (UINT)(_offset * m_elementSize);

		D3D11_MAP mapFlags;
		switch (m_bufferType)
		{
		case BT_STATIC:
		{
			mapFlags = D3D11_MAP_WRITE;
		}break;
		case BT_STREAM:
		case BT_DYNAMIC:
		{
			mapFlags = D3D11_MAP_WRITE_DISCARD;
		}break;
		};

		D3D11_MAPPED_SUBRESOURCE mappedResource;

		ID3D11DeviceContext *pImmediateContext = nullptr;
		m_pD3DDevice->GetImmediateContext(&pImmediateContext);

		auto hResult = pImmediateContext->Map(m_pD3DBuffer, 0, D3D11_MAP_WRITE, 0, &mappedResource);
		if (FAILED(hResult))
		{
			// TODO: add error log
			return nullptr;
		}

        stdex::memorycopy(mappedResource.pData, 0, _buffer, _count * m_elementSize );

		pImmediateContext->Unmap(m_pD3DBuffer, 0);

		m_elementsCount = _count;

        return true;
    }
    //////////////////////////////////////////////////////////////////////////
    void DX11RenderBuffer::onRenderReset()
    {

	}
    //////////////////////////////////////////////////////////////////////////
    bool DX11RenderBuffer::onRenderRestore()
    {
		return true;
    }
    //////////////////////////////////////////////////////////////////////////
}