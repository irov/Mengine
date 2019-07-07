#include "DX9RenderIndexBuffer.h"

#include "Interface/MemoryServiceInterface.h"

#include "DX9RenderEnum.h"
#include "DX9ErrorHelper.h"

#include "Kernel/AssertionMemoryPanic.h"
#include "Kernel/Logger.h"
#include "Kernel/Document.h"

namespace Mengine
{
    //////////////////////////////////////////////////////////////////////////
    DX9RenderIndexBuffer::DX9RenderIndexBuffer()
        : m_pD3DDevice( nullptr )
        , m_bufferType( BT_STATIC )
        , m_indexSize( 0 )
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
        this->finalize();
    }
    //////////////////////////////////////////////////////////////////////////
    bool DX9RenderIndexBuffer::initialize( IDirect3DDevice9 * _pD3DDevice, uint32_t _indexSize, EBufferType _bufferType )
    {
        m_pD3DDevice = _pD3DDevice;
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

        m_format = s_getD3DIndexFormat();

        MemoryProxyInterfacePtr memory = MEMORY_SERVICE()
            ->createMemoryProxy( MENGINE_DOCUMENT_FUNCTION );

        MENGINE_ASSERTION_MEMORY_PANIC( memory, false, "invalid create memory proxy" );

        m_memory = memory;

        return true;
    }
    //////////////////////////////////////////////////////////////////////////
    void DX9RenderIndexBuffer::finalize()
    {
        m_memory = nullptr;

        if( m_pIB != nullptr )
        {
            ULONG ref = m_pIB->Release();
            MENGINE_UNUSED( ref );

            m_pIB = nullptr;
        }
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
    bool DX9RenderIndexBuffer::resize( uint32_t _count )
    {
        if( m_indexCount >= _count )
        {
            return true;
        }

        if( m_pIB != nullptr )
        {
            ULONG ref = m_pIB->Release();
            MENGINE_UNUSED( ref );

            m_pIB = nullptr;
        }

        m_indexCount = _count;

        IDirect3DIndexBuffer9 * pIB = nullptr;
        IF_DXCALL( m_pD3DDevice, CreateIndexBuffer, (m_indexCount * m_indexSize, m_usage, m_format, m_pool, &pIB, NULL) )
        {
            return false;
        }

        m_pIB = pIB;

        return true;
    }
    //////////////////////////////////////////////////////////////////////////
    MemoryInterfacePtr DX9RenderIndexBuffer::lock( uint32_t _offset, uint32_t _count, const Char * _doc )
    {
        if( _offset + _count > m_indexCount )
        {
            LOGGER_ERROR( "lock count %d offset %d more max size %d (doc '%s')"
                , _count
                , _offset
                , m_indexCount
                , _doc
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

        void * lock_memory = nullptr;
        IF_DXCALL( m_pIB, Lock, (_offset * m_indexSize, _count * m_indexSize, &lock_memory, d3d_flag) )
        {
            LOGGER_ERROR( "invalid lock count %d offset %d (doc '%s')"
                , _count
                , _offset
                , _doc
            );

            return nullptr;
        }

        m_memory->setBuffer( lock_memory, _count * m_indexSize, MENGINE_DOCUMENT_FUNCTION );

        return m_memory;
    }
    //////////////////////////////////////////////////////////////////////////
    bool DX9RenderIndexBuffer::unlock()
    {
        m_memory->setBuffer( nullptr, 0, MENGINE_DOCUMENT_FUNCTION );

        IF_DXCALL( m_pIB, Unlock, () )
        {
            LOGGER_ERROR( "invalid"
            );

            return false;
        }

        return true;
    }
    //////////////////////////////////////////////////////////////////////////
    bool DX9RenderIndexBuffer::draw( const void * _buffer, size_t _size, const Char * _doc )
    {
        UINT sizeToLock = (UINT)(_size * m_indexSize);

        void * lock_memory = nullptr;
        IF_DXCALL( m_pIB, Lock, (0, sizeToLock, &lock_memory, D3DLOCK_DISCARD) )
        {
            LOGGER_ERROR( "invalid lock size %u (doc '%s')"
                , _size
                , _doc
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
        if( m_pIB == nullptr )
        {
            return;
        }

        ULONG ref = m_pIB->Release();
        MENGINE_UNUSED( ref );

        m_pIB = nullptr;
    }
    //////////////////////////////////////////////////////////////////////////        
    bool DX9RenderIndexBuffer::onRenderRestore()
    {
        IDirect3DIndexBuffer9 * pIB = nullptr;
        IF_DXCALL( m_pD3DDevice, CreateIndexBuffer, (m_indexCount * m_indexSize, m_usage, m_format, m_pool, &pIB, NULL) )
        {
            return false;
        }

        m_pIB = pIB;

        return true;
    }
}