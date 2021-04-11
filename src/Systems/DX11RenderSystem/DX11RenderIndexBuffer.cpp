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
        : m_bufferType( BT_STATIC )
        , m_indexSize( 0 )
        , m_indexCapacity( 0 )
        , m_indexCount( 0 )
        , m_usage( 0 )
        , m_format( D3DFMT_UNKNOWN )
        , m_pool( D3DPOOL_MANAGED )
        , m_pD3DIndexBuffer( nullptr )
    {
    }
    //////////////////////////////////////////////////////////////////////////
    DX11RenderIndexBuffer::~DX11RenderIndexBuffer()
    {
        MENGINE_ASSERTION_FATAL( m_pD3DIndexBuffer == nullptr );
    }
    //////////////////////////////////////////////////////////////////////////
    bool DX11RenderIndexBuffer::initialize( uint32_t _indexSize, EBufferType _bufferType )
    {
        m_indexSize = _indexSize;
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

        m_format = Helper::getD3DIndexFormat();

        MemoryProxyInterfacePtr memory = MEMORY_SERVICE()
            ->createMemoryProxy( MENGINE_DOCUMENT_FACTORABLE );

        MENGINE_ASSERTION_MEMORY_PANIC( memory, "invalid create memory proxy" );

        m_memory = memory;

        return true;
    }
    //////////////////////////////////////////////////////////////////////////
    void DX11RenderIndexBuffer::finalize()
    {
        m_memory = nullptr;

        DXRELEASE( m_pD3DIndexBuffer );
    }
    //////////////////////////////////////////////////////////////////////////
    uint32_t DX11RenderIndexBuffer::getIndexCount() const
    {
        return m_indexCount;
    }
    //////////////////////////////////////////////////////////////////////////
    uint32_t DX11RenderIndexBuffer::getIndexSize() const
    {
        return m_indexSize;
    }
    //////////////////////////////////////////////////////////////////////////
    bool DX11RenderIndexBuffer::resize( uint32_t _indexCount )
    {
        m_indexCount = _indexCount;

        if( m_indexCapacity >= m_indexCount )
        {
            return true;
        }

        DXRELEASE( m_pD3DIndexBuffer );

        m_indexCapacity = m_indexCount;

        uint32_t bufferSize = m_indexCapacity * m_indexSize;

        IDirect3DIndexBuffer9 * pD3DIndexBuffer = nullptr;
        IF_DXCALL( m_pD3DDevice, CreateIndexBuffer, (bufferSize, m_usage, m_format, m_pool, &pD3DIndexBuffer, NULL) )
        {
            return false;
        }

        m_pD3DIndexBuffer = pD3DIndexBuffer;

        return true;
    }
    //////////////////////////////////////////////////////////////////////////
    MemoryInterfacePtr DX11RenderIndexBuffer::lock( uint32_t _offset, uint32_t _count )
    {
        if( _offset + _count > m_indexCount )
        {
            LOGGER_ERROR( "lock count %u offset %u more max size %u (doc '%s')"
                , _count
                , _offset
                , m_indexCount
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

        uint32_t offsetSize = _offset * m_indexSize;
        uint32_t lockSize = _count * m_indexSize;

        void * lock_memory = nullptr;
        IF_DXCALL( m_pD3DIndexBuffer, Lock, (offsetSize, lockSize, &lock_memory, d3d_flag) )
        {
            LOGGER_ERROR( "invalid lock count %u offset %u (doc '%s')"
                , _count
                , _offset
                , MENGINE_DOCUMENTABLE_STR( this, "DX11RenderIndexBuffer" )
            );

            return nullptr;
        }

        m_memory->setBuffer( lock_memory, lockSize );

        return m_memory;
    }
    //////////////////////////////////////////////////////////////////////////
    bool DX11RenderIndexBuffer::unlock()
    {
        m_memory->setBuffer( nullptr, 0 );

        IF_DXCALL( m_pD3DIndexBuffer, Unlock, () )
        {
            LOGGER_ERROR( "invalid (doc: %s)"
                , MENGINE_DOCUMENTABLE_STR( this, "DX11RenderVertexBuffer" )
            );

            return false;
        }

        return true;
    }
    //////////////////////////////////////////////////////////////////////////
    bool DX11RenderIndexBuffer::draw( const void * _buffer, uint32_t _offset, uint32_t _count )
    {
        if( _offset + _count > m_indexCapacity )
        {
            LOGGER_ERROR( "draw count %u offset %u more capacity %u (doc '%s')"
                , _count
                , _offset
                , m_indexCapacity
                , MENGINE_DOCUMENTABLE_STR( this, "DX11RenderIndexBuffer" )
            );

            return false;
        }

        UINT offsetToLock = (UINT)(_offset * m_indexSize);
        UINT sizeToLock = (UINT)(_count * m_indexSize);

        void * lock_memory = nullptr;
        IF_DXCALL( m_pD3DIndexBuffer, Lock, (offsetToLock, sizeToLock, &lock_memory, D3DLOCK_DISCARD) )
        {
            LOGGER_ERROR( "invalid lock offset %u size %u (doc '%s')"
                , offsetToLock
                , sizeToLock
                , MENGINE_DOCUMENTABLE_STR( this, "DX11RenderIndexBuffer" )
            );

            return false;
        }

        stdex::memorycopy( lock_memory, 0, _buffer, _count * m_indexSize );

        IF_DXCALL( m_pD3DIndexBuffer, Unlock, () )
        {
            LOGGER_ERROR( "invalid unlock (doc: %s)"
                , MENGINE_DOCUMENTABLE_STR( this, "DX11RenderVertexBuffer" )
            );

            return false;
        }

        return true;
    }
    //////////////////////////////////////////////////////////////////////////
    void DX11RenderIndexBuffer::enable()
    {
        DXCALL( m_pD3DDevice, SetIndices, (m_pD3DIndexBuffer) );
    }
    //////////////////////////////////////////////////////////////////////////
    void DX11RenderIndexBuffer::disable()
    {
        DXCALL( m_pD3DDevice, SetIndices, (nullptr) );
    }
    //////////////////////////////////////////////////////////////////////////
    void DX11RenderIndexBuffer::onRenderReset()
    {
        if( m_pool == D3DPOOL_MANAGED )
        {
            return;
        }

        m_memory->setBuffer( nullptr, 0 );

        DXRELEASE( m_pD3DIndexBuffer );
    }
    //////////////////////////////////////////////////////////////////////////
    bool DX11RenderIndexBuffer::onRenderRestore()
    {
        if( m_pool == D3DPOOL_MANAGED )
        {
            return true;
        }

        MENGINE_ASSERTION_FATAL( m_pD3DIndexBuffer == nullptr );

        uint32_t bufferSize = m_indexCapacity * m_indexSize;

        IDirect3DIndexBuffer9 * pD3DIndexBuffer = nullptr;
        IF_DXCALL( m_pD3DDevice, CreateIndexBuffer, (bufferSize, m_usage, m_format, m_pool, &pD3DIndexBuffer, nullptr) )
        {
            return false;
        }

        m_pD3DIndexBuffer = pD3DIndexBuffer;

        return true;
    }
    //////////////////////////////////////////////////////////////////////////
}