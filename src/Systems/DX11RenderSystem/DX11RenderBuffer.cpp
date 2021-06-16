#include "DX11RenderBuffer.h"

#include "Interface/MemoryServiceInterface.h"

#include "DX11RenderEnum.h"
#include "DX11RenderErrorHelper.h"

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
        , m_elementsCount( 0 )
        , m_elementsCapacity( 0 )
        , m_format( DXGI_FORMAT_UNKNOWN )
    {
    }
    //////////////////////////////////////////////////////////////////////////
    DX11RenderBuffer::~DX11RenderBuffer()
    {
        MENGINE_ASSERTION_FATAL( m_pD3DBuffer == nullptr );
    }
    //////////////////////////////////////////////////////////////////////////
    bool DX11RenderBuffer::initializeBuffer( uint32_t _elementSize, EBufferType _bufferType )
    {
        m_elementSize = _elementSize;
        m_bufferType = _bufferType;

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

        m_pD3DBuffer = nullptr;

        m_elementsCount = 0;
        m_elementsCapacity = 0;
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
    bool DX11RenderBuffer::resizeBuffer( D3D11_BIND_FLAG _bindFlag, uint32_t _elementsCount, const void * _initData )
    {
        m_elementsCount = _elementsCount;

        if( m_elementsCapacity >= m_elementsCount )
        {
            return true;
        }

        m_pD3DBuffer = nullptr;

        m_elementsCapacity = m_elementsCount;

        uint32_t bufferSize = m_elementsCapacity * m_elementSize;

        D3D11_BUFFER_DESC desc;
        desc.CPUAccessFlags = 0;
        desc.MiscFlags = 0;
        desc.BindFlags = _bindFlag;

        switch( m_bufferType )
        {
        case BT_STATIC:
            {
                desc.Usage = D3D11_USAGE_DEFAULT;
            }break;
        case BT_STREAM:
        case BT_DYNAMIC:
            {
                desc.Usage = D3D11_USAGE_DYNAMIC;
                desc.CPUAccessFlags = D3D11_CPU_ACCESS_WRITE;
            }break;
        };

        desc.StructureByteStride = (UINT)m_elementSize;
        desc.ByteWidth = (UINT)bufferSize;

        // Define the resource data.
        D3D11_SUBRESOURCE_DATA InitData;
        InitData.pSysMem = _initData;
        InitData.SysMemPitch = 0;
        InitData.SysMemSlicePitch = 0;

        const ID3D11DevicePtr & pD3DDevice = this->getDirect3D11Device();

        ID3D11Buffer * pD3DBuffer;
        IF_DXCALL( pD3DDevice, CreateBuffer, (&desc, _initData == nullptr ? nullptr : &InitData, &pD3DBuffer) )
        {
            return false;
        }

        m_pD3DBuffer.Attach( pD3DBuffer );

        return true;
    }
    //////////////////////////////////////////////////////////////////////////
    MemoryInterfacePtr DX11RenderBuffer::lockBuffer( uint32_t _offset, uint32_t _count )
    {
        if( _offset + _count > m_elementsCount )
        {
            LOGGER_ERROR( "lock count %u offset %u more max size %u (doc: %s)"
                , _count
                , _offset
                , m_elementsCount
                , MENGINE_DOCUMENTABLE_STR( this, "DX11RenderBuffer" )
            );

            return nullptr;
        }

        if( m_bufferType != BT_DYNAMIC )
        {
            LOGGER_ERROR( "lock for read for Dynamic buffer is not supported" );

            return nullptr;
        }

        MENGINE_ASSERTION_FATAL( m_memory->getBuffer() == nullptr );

        //uint32_t offsetSize = _offset * m_elementSize;
        //uint32_t lockSize = _count * m_elementSize;

        D3D11_MAPPED_SUBRESOURCE mappedResource;

        ID3D11DeviceContextPtr pImmediateContext = this->getDirect3D11ImmediateContext();

        IF_DXCALL( pImmediateContext, Map, (m_pD3DBuffer.Get(), 0, D3D11_MAP_WRITE_DISCARD, 0, &mappedResource) )
        {
            // TODO: add error log
            return nullptr;
        }

        m_memory->setBuffer( mappedResource.pData, mappedResource.RowPitch );

        return m_memory;
    }
    //////////////////////////////////////////////////////////////////////////
    bool DX11RenderBuffer::unlockBuffer()
    {
        m_memory->setBuffer( nullptr, 0 );

        ID3D11DeviceContextPtr pImmediateContext = this->getDirect3D11ImmediateContext();

        pImmediateContext->Unmap( m_pD3DBuffer.Get(), 0 );

        return true;
    }
    //////////////////////////////////////////////////////////////////////////
    bool DX11RenderBuffer::drawBuffer( const void * _buffer, uint32_t _offset, uint32_t _count )
    {
        MENGINE_UNUSED( _offset );

        if( _count > m_elementsCount )
        {
            LOGGER_ERROR( "draw count %u more capacity %u (doc: %s)"
                , _count
                , m_elementsCount
                , MENGINE_DOCUMENTABLE_STR( this, "DX11RenderBuffer" )
            );

            return false;
        }

        // TODO: somehow needs to reinterpret offset to subresource
        // UINT offsetToLock = (UINT)(_offset * m_elementSize);

        D3D11_MAP mapFlags = D3D11_MAP_WRITE_DISCARD;
        switch( m_bufferType )
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

        ID3D11DeviceContextPtr pImmediateContext = this->getDirect3D11ImmediateContext();

        IF_DXCALL( pImmediateContext, Map, (m_pD3DBuffer.Get(), 0, mapFlags, 0, &mappedResource) )
        {
            // TODO: add error log
            return nullptr;
        }

        stdex::memorycopy( mappedResource.pData, 0, _buffer, _count * m_elementSize );

        pImmediateContext->Unmap( m_pD3DBuffer.Get(), 0 );

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