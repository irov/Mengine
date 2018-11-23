#include "DX9RenderIndexBuffer.h"

#include "Interface/MemoryServiceInterface.h"

#include "DX9RenderEnum.h"
#include "DX9ErrorHelper.h"

#include "Kernel/AssertionMemoryPanic.h"
#include "Kernel/Logger.h"

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

        return true;
    }
    //////////////////////////////////////////////////////////////////////////
    void DX9RenderIndexBuffer::finalize()
    {
        if( m_pIB != nullptr )
        {
            ULONG ref = m_pIB->Release();
            (void)ref;
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
            (void)ref;
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
    MemoryInterfacePtr DX9RenderIndexBuffer::lock( uint32_t _offset, uint32_t _count )
    {
        if( _offset + _count > m_indexCount )
        {
            LOGGER_ERROR( "lock count %d offset %d more max size %d"
                , _count
                , _offset
                , m_indexCount
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
            LOGGER_ERROR( "invalid lock count %d offset %d"
                , _count
                , _offset
            );

            return nullptr;
        }

        MemoryProxyInterfacePtr memory = MEMORY_SERVICE()
            ->createMemoryProxy();

        MENGINE_ASSERTION_MEMORY_PANIC( memory, nullptr )("invalid create memory proxy");

        memory->setBuffer( lock_memory, _count * m_indexSize, __FILE__, __LINE__ );

        return memory;
    }
    //////////////////////////////////////////////////////////////////////////
    bool DX9RenderIndexBuffer::unlock()
    {
        IF_DXCALL( m_pIB, Unlock, () )
        {
            LOGGER_ERROR( "DX9RenderIndexBuffer::unlock invalid"
            );

            return false;
        }

        return true;
    }
    //////////////////////////////////////////////////////////////////////////
    void DX9RenderIndexBuffer::draw( const void * _buffer, size_t _size )
    {
        (void)_buffer;
        (void)_size;
        //ToDO
    }
    //////////////////////////////////////////////////////////////////////////
    bool DX9RenderIndexBuffer::enable()
    {
        IF_DXCALL( m_pD3DDevice, SetIndices, (m_pIB) )
        {
            return false;
        }

        return true;
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

        ULONG refCount = m_pIB->Release();
        (void)refCount;

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