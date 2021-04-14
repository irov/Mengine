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
        : m_bufferType( BT_STATIC )
        , m_vertexSize( 0 )
        , m_vertexCapacity( 0 )
        , m_vertexCount( 0 )
        , m_usage( 0 )
        , m_format( D3DFMT_UNKNOWN )
        , m_pool( D3DPOOL_MANAGED )
        , m_pD3DVertexBuffer( nullptr )
    {
    }
    //////////////////////////////////////////////////////////////////////////
    DX11RenderVertexBuffer::~DX11RenderVertexBuffer()
    {
        MENGINE_ASSERTION_FATAL( m_pD3DVertexBuffer == nullptr );
    }
    //////////////////////////////////////////////////////////////////////////
    bool DX11RenderVertexBuffer::initialize( uint32_t _vertexSize, EBufferType _bufferType )
    {
        m_vertexSize = _vertexSize;
        m_bufferType = _bufferType;

        switch( m_bufferType )
        {
        case BT_STATIC:
            {
                m_pool = D3DPOOL_MANAGED;

                m_usage = D3DUSAGE_WRITEONLY;
            }break;
        case BT_STREAM:
        case BT_DYNAMIC:
            {
                m_pool = D3DPOOL_DEFAULT;

                m_usage = D3DUSAGE_WRITEONLY | D3DUSAGE_DYNAMIC;
            }break;
        };

        MemoryProxyInterfacePtr memory = MEMORY_SERVICE()
            ->createMemoryProxy( MENGINE_DOCUMENT_FACTORABLE );

        MENGINE_ASSERTION_MEMORY_PANIC( memory, "invalid create memory proxy" );

        m_memory = memory;

        return true;
    }
    //////////////////////////////////////////////////////////////////////////
    void DX11RenderVertexBuffer::finalize()
    {
        m_memory = nullptr;

        DXRELEASE( m_pD3DVertexBuffer );
    }
    //////////////////////////////////////////////////////////////////////////
    uint32_t DX11RenderVertexBuffer::getVertexCount() const
    {
        return m_vertexCount;
    }
    //////////////////////////////////////////////////////////////////////////
    uint32_t DX11RenderVertexBuffer::getVertexSize() const
    {
        return m_vertexSize;
    }
    //////////////////////////////////////////////////////////////////////////
    bool DX11RenderVertexBuffer::resize( uint32_t _vertexCount )
    {
        m_vertexCount = _vertexCount;

        if( m_vertexCapacity >= m_vertexCount )
        {
            return true;
        }

        DXRELEASE( m_pD3DVertexBuffer );

        m_vertexCapacity = m_vertexCount;

        uint32_t bufferSize = m_vertexCapacity * m_vertexSize;

        IDirect3DVertexBuffer9 * pD3DVertexBuffer = nullptr;
        IF_DXCALL( m_pD3DDevice, CreateVertexBuffer, (bufferSize, m_usage, 0, m_pool, &pD3DVertexBuffer, NULL) )
        {
            return false;
        }

        m_pD3DVertexBuffer = pD3DVertexBuffer;

        return true;
    }
    //////////////////////////////////////////////////////////////////////////
    MemoryInterfacePtr DX11RenderVertexBuffer::lock( uint32_t _offset, uint32_t _count )
    {
        if( _offset + _count > m_vertexCount )
        {
            LOGGER_ERROR( "lock count %u offset %u more max size %u (doc: %s)"
                , _count
                , _offset
                , m_vertexCount
                , MENGINE_DOCUMENTABLE_STR( this, "DX11RenderIndexBuffer" )
            );

            return nullptr;
        }

        MENGINE_ASSERTION_FATAL( m_memory->getBuffer() == nullptr );

        DWORD d3d_flag;
        switch( m_bufferType )
        {
        case BT_STATIC:
            d3d_flag = 0;
            break;
        case BT_DYNAMIC:
            d3d_flag = D3DLOCK_DISCARD;
            break;
        default:
            return nullptr;
        };

        uint32_t offsetSize = _offset * m_vertexSize;
        uint32_t lockSize = _count * m_vertexSize;

        void * lock_memory = nullptr;
        IF_DXCALL( m_pD3DVertexBuffer, Lock, (offsetSize, lockSize, &lock_memory, d3d_flag) )
        {
            LOGGER_ERROR( "%d offset %d invalid lock doc '%s'"
                , _count
                , _offset
                , MENGINE_DOCUMENTABLE_STR( this, "DX11RenderVertexBuffer" )
            );

            return nullptr;
        }

        m_memory->setBuffer( lock_memory, lockSize );

        return m_memory;
    }
    //////////////////////////////////////////////////////////////////////////
    bool DX11RenderVertexBuffer::unlock()
    {
        m_memory->setBuffer( nullptr, 0 );

        IF_DXCALL( m_pD3DVertexBuffer, Unlock, () )
        {
            LOGGER_ERROR( "invalid unlock"
            );

            return false;
        }

        return true;
    }
    //////////////////////////////////////////////////////////////////////////
    bool DX11RenderVertexBuffer::draw( const void * _buffer, uint32_t _offset, uint32_t _count )
    {
        if( _count > m_vertexCapacity )
        {
            LOGGER_ERROR( "draw count %u more capacity %u (doc: %s)"
                , _count
                , m_vertexCount
                , MENGINE_DOCUMENTABLE_STR( this, "DX11RenderIndexBuffer" )
            );

            return false;
        }

        UINT offsetToLock = (UINT)(_offset * m_vertexSize);
        UINT sizeToLock = (UINT)(_count * m_vertexSize);

        void * lock_memory = nullptr;
        IF_DXCALL( m_pD3DVertexBuffer, Lock, (offsetToLock, sizeToLock, &lock_memory, D3DLOCK_DISCARD) )
        {
            LOGGER_ERROR( "invalid lock offset %u size %u (doc %s)"
                , offsetToLock
                , sizeToLock
                , MENGINE_DOCUMENTABLE_STR( this, "DX11RenderVertexBuffer" )
            );

            return false;
        }

        stdex::memorycopy( lock_memory, 0, _buffer, _count * m_vertexSize );

        IF_DXCALL( m_pD3DVertexBuffer, Unlock, () )
        {
            LOGGER_ERROR( "invalid unlock (doc: %s)"
                , MENGINE_DOCUMENTABLE_STR( this, "DX11RenderVertexBuffer" )
            );

            return false;
        }

        m_vertexCount = _count;

        return true;
    }
    //////////////////////////////////////////////////////////////////////////
    void DX11RenderVertexBuffer::enable()
    {
        DXCALL( m_pD3DDevice, SetStreamSource, (0, m_pD3DVertexBuffer, 0, m_vertexSize) );
    }
    //////////////////////////////////////////////////////////////////////////
    void DX11RenderVertexBuffer::disable()
    {
        DXCALL( m_pD3DDevice, SetStreamSource, (0, nullptr, 0, 0) );
    }
    //////////////////////////////////////////////////////////////////////////
    void DX11RenderVertexBuffer::onRenderReset()
    {
        if( m_pool == D3DPOOL_MANAGED )
        {
            return;
        }

        m_memory->setBuffer( nullptr, 0 );

        DXRELEASE( m_pD3DVertexBuffer );
    }
    //////////////////////////////////////////////////////////////////////////
    bool DX11RenderVertexBuffer::onRenderRestore()
    {
        if( m_pool == D3DPOOL_MANAGED )
        {
            return true;
        }

        MENGINE_ASSERTION_FATAL( m_pD3DVertexBuffer == nullptr );

        uint32_t bufferSize = m_vertexCapacity * m_vertexSize;

        IDirect3DVertexBuffer9 * pD3DVertexBuffer = nullptr;
        IF_DXCALL( m_pD3DDevice, CreateVertexBuffer, (bufferSize, m_usage, 0, m_pool, &pD3DVertexBuffer, nullptr) )
        {
            return false;
        }

        m_pD3DVertexBuffer = pD3DVertexBuffer;

        return true;
    }
    //////////////////////////////////////////////////////////////////////////
}