#include "DX9RenderIndexBuffer.h"

#include "Interface/MemoryServiceInterface.h"

#include "DX9RenderEnum.h"
#include "DX9ErrorHelper.h"

#include "Kernel/AssertionMemoryPanic.h"
#include "Kernel/Logger.h"
#include "Kernel/Documentable.h"
#include "Kernel/DocumentHelper.h"

#include "stdex/memorycopy.h"

namespace Mengine
{
    //////////////////////////////////////////////////////////////////////////
    DX9RenderIndexBuffer::DX9RenderIndexBuffer()
        : m_pD3DDevice( nullptr )
        , m_bufferType( BT_STATIC )
        , m_indexSize( 0 )
        , m_indexCapacity( 0 )
        , m_indexCount( 0 )
        , m_usage( 0 )
        , m_format( D3DFMT_UNKNOWN )
        , m_pool( D3DPOOL_MANAGED )
        , m_pIB( nullptr )
    {
    }
    //////////////////////////////////////////////////////////////////////////
    DX9RenderIndexBuffer::~DX9RenderIndexBuffer()
    {
    }
    //////////////////////////////////////////////////////////////////////////
    void DX9RenderIndexBuffer::setDirect3DDevice9( IDirect3DDevice9 * _pD3DDevice )
    {
        m_pD3DDevice = _pD3DDevice;
    }
    //////////////////////////////////////////////////////////////////////////
    IDirect3DDevice9 * DX9RenderIndexBuffer::getDirect3DDevice9() const
    {
        return m_pD3DDevice;
    }
    //////////////////////////////////////////////////////////////////////////
    bool DX9RenderIndexBuffer::initialize( uint32_t _indexSize, EBufferType _bufferType )
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
    void DX9RenderIndexBuffer::finalize()
    {
        m_memory = nullptr;

        DXRELEASE( m_pIB );
    }
    //////////////////////////////////////////////////////////////////////////
    uint32_t DX9RenderIndexBuffer::getIndexCount() const
    {
        return m_indexCount;
    }
    //////////////////////////////////////////////////////////////////////////
    uint32_t DX9RenderIndexBuffer::getIndexSize() const
    {
        return m_indexSize;
    }
    //////////////////////////////////////////////////////////////////////////
    bool DX9RenderIndexBuffer::resize( uint32_t _indexCount )
    {
        m_indexCount = _indexCount;

        if( m_indexCapacity >= m_indexCount )
        {
            return true;
        }

        DXRELEASE( m_pIB );

        m_indexCapacity = m_indexCount;

        uint32_t bufferSize = m_indexCapacity * m_indexSize;

        IDirect3DIndexBuffer9 * pIB = nullptr;
        IF_DXCALL( m_pD3DDevice, CreateIndexBuffer, (bufferSize, m_usage, m_format, m_pool, &pIB, NULL) )
        {
            return false;
        }

        m_pIB = pIB;

        return true;
    }
    //////////////////////////////////////////////////////////////////////////
    MemoryInterfacePtr DX9RenderIndexBuffer::lock( uint32_t _offset, uint32_t _count )
    {
        if( _offset + _count > m_indexCount )
        {
            LOGGER_ERROR( "lock count %d offset %d more max size %d (doc '%s')"
                , _count
                , _offset
                , m_indexCount
                , MENGINE_DOCUMENTABLE_STR( this, "DX9RenderIndexBuffer" )
            );

            return nullptr;
        }

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
        IF_DXCALL( m_pIB, Lock, (offsetSize, lockSize, &lock_memory, d3d_flag) )
        {
            LOGGER_ERROR( "invalid lock count %d offset %d (doc '%s')"
                , _count
                , _offset
                , MENGINE_DOCUMENTABLE_STR( this, "DX9RenderIndexBuffer" )
            );

            return nullptr;
        }

        m_memory->setBuffer( lock_memory, lockSize );

        return m_memory;
    }
    //////////////////////////////////////////////////////////////////////////
    bool DX9RenderIndexBuffer::unlock()
    {
        m_memory->setBuffer( nullptr, 0 );

        IF_DXCALL( m_pIB, Unlock, () )
        {
            LOGGER_ERROR( "invalid"
            );

            return false;
        }

        return true;
    }
    //////////////////////////////////////////////////////////////////////////
    bool DX9RenderIndexBuffer::draw( const void * _buffer, size_t _size )
    {
        UINT sizeToLock = (UINT)(_size * m_indexSize);

        void * lock_memory = nullptr;
        IF_DXCALL( m_pIB, Lock, (0, sizeToLock, &lock_memory, D3DLOCK_DISCARD) )
        {
            LOGGER_ERROR( "invalid lock size %zu (doc '%s')"
                , _size
                , MENGINE_DOCUMENTABLE_STR( this, "DX9RenderIndexBuffer" )
            );

            return false;
        }

        stdex::memorycopy( lock_memory, 0, _buffer, _size * m_indexSize );

        IF_DXCALL( m_pIB, Unlock, () )
        {
            LOGGER_ERROR( "invalid unlock"
            );

            return false;
        }

        return true;
    }
    //////////////////////////////////////////////////////////////////////////
    void DX9RenderIndexBuffer::enable()
    {
        DXCALL( m_pD3DDevice, SetIndices, (m_pIB) );
    }
    //////////////////////////////////////////////////////////////////////////
    void DX9RenderIndexBuffer::disable()
    {
        DXCALL( m_pD3DDevice, SetIndices, (nullptr) );
    }
    //////////////////////////////////////////////////////////////////////////
    void DX9RenderIndexBuffer::onRenderReset()
    {
        if( m_pool == D3DPOOL_MANAGED )
        {
            return;
        }

        m_memory->setBuffer( nullptr, 0 );

        DXRELEASE( m_pIB );
    }
    //////////////////////////////////////////////////////////////////////////
    bool DX9RenderIndexBuffer::onRenderRestore()
    {
        if( m_pool == D3DPOOL_MANAGED )
        {
            return true;
        }

        uint32_t bufferSize = m_indexCapacity * m_indexSize;

        IDirect3DIndexBuffer9 * pIB = nullptr;
        IF_DXCALL( m_pD3DDevice, CreateIndexBuffer, (bufferSize, m_usage, m_format, m_pool, &pIB, nullptr) )
        {
            return false;
        }

        m_pIB = pIB;

        return true;
    }
}