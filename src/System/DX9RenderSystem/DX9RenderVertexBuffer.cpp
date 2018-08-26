#include "DX9RenderVertexBuffer.h"

#include "DX9RenderEnum.h"
#include "DX9ErrorHelper.h"

#include "Kernel/Logger.h"

namespace Mengine
{
    //////////////////////////////////////////////////////////////////////////
    DX9RenderVertexBuffer::DX9RenderVertexBuffer()
        : m_pD3DDevice( nullptr )
        , m_vertexSize( 0 )
        , m_vertexCount( 0 )
        , m_usage( 0 )
        , m_format( D3DFMT_UNKNOWN )
        , m_pool( D3DPOOL_MANAGED )
        , m_pVB( nullptr )
    {
    }
    //////////////////////////////////////////////////////////////////////////
    DX9RenderVertexBuffer::~DX9RenderVertexBuffer()
    {
        if( m_pVB != nullptr )
        {
            ULONG ref = m_pVB->Release();
            (void)ref;
            m_pVB = nullptr;
        }
    }
    //////////////////////////////////////////////////////////////////////////
    bool DX9RenderVertexBuffer::initialize( IDirect3DDevice9 * _pD3DDevice, uint32_t _vertexSize, EBufferType _bufferType )
    {
        m_pD3DDevice = _pD3DDevice;
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

        return true;
    }
    //////////////////////////////////////////////////////////////////////////
    uint32_t DX9RenderVertexBuffer::getVertexCount() const
    {
        return m_vertexCount;
    }
    //////////////////////////////////////////////////////////////////////////
    uint32_t DX9RenderVertexBuffer::getVertexSize() const
    {
        return m_vertexSize;
    }
    //////////////////////////////////////////////////////////////////////////
    bool DX9RenderVertexBuffer::resize( uint32_t _count )
    {
        if( m_vertexCount >= _count )
        {
            return true;
        }

        if( m_pVB != nullptr )
        {
            ULONG ref = m_pVB->Release();
            (void)ref;
            m_pVB = nullptr;
        }

        m_vertexCount = _count;

        IDirect3DVertexBuffer9 * pVB = nullptr;
        IF_DXCALL( m_pD3DDevice, CreateVertexBuffer, (m_vertexCount * m_vertexSize, m_usage, 0, m_pool, &pVB, NULL) )
        {
            return false;
        }

        m_pVB = pVB;

        return true;
    }
    //////////////////////////////////////////////////////////////////////////
    MemoryInterfacePtr DX9RenderVertexBuffer::lock( uint32_t _offset, uint32_t _count )
    {
        if( _offset + _count > m_vertexCount )
        {
            LOGGER_ERROR( "DX9RenderVertexBuffer::lock %d offset %d more max size %d"
                , _count
                , _offset
                , m_vertexCount
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
        IF_DXCALL( m_pVB, Lock, (_offset * m_vertexSize, _count * m_vertexSize, &lock_memory, d3d_flag) )
        {
            LOGGER_ERROR( "DX9RenderVertexBuffer::lock %d offset %d invalid lock"
                , _count
                , _offset
            );

            return nullptr;
        }

        MemoryProxyInterfacePtr memory = MEMORY_SERVICE()
            ->createMemoryProxy();

        memory->setBuffer( lock_memory, _count * m_vertexSize, __FILE__, __LINE__ );

        return memory;
    }
    //////////////////////////////////////////////////////////////////////////
    bool DX9RenderVertexBuffer::unlock()
    {
        IF_DXCALL( m_pVB, Unlock, () )
        {
            LOGGER_ERROR( "DX9RenderIndexBuffer::unlock invalid"
            );

            return false;
        }

        return true;
    }
    //////////////////////////////////////////////////////////////////////////
    void DX9RenderVertexBuffer::draw( const void * _buffer, size_t _size )
    {
        (void)_buffer;
        (void)_size;

        //ToDo
    }
    //////////////////////////////////////////////////////////////////////////
    bool DX9RenderVertexBuffer::enable()
    {
        IF_DXCALL( m_pD3DDevice, SetStreamSource, (0, m_pVB, 0, sizeof( RenderVertex2D )) )
        {
            return false;
        }

        return true;
    }
    //////////////////////////////////////////////////////////////////////////
    void DX9RenderVertexBuffer::disable()
    {
        DXCALL( m_pD3DDevice, SetStreamSource, (0, nullptr, 0, 0) );
    }
    //////////////////////////////////////////////////////////////////////////        
    void DX9RenderVertexBuffer::onRenderReset()
    {
        if( m_pVB == nullptr )
        {
            return;
        }

        ULONG refCount = m_pVB->Release();
        (void)refCount;

        m_pVB = nullptr;
    }
    //////////////////////////////////////////////////////////////////////////        
    bool DX9RenderVertexBuffer::onRenderRestore()
    {
        IDirect3DVertexBuffer9 * pVB = nullptr;
        IF_DXCALL( m_pD3DDevice, CreateVertexBuffer, (m_vertexCount * m_vertexSize, m_usage, 0, m_pool, &pVB, NULL) )
        {
            return false;
        }

        m_pVB = pVB;

        return true;
    }
}