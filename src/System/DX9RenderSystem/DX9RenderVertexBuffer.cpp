#	include "DX9RenderVertexBuffer.h"

#	include "DX9RenderEnum.h"
#	include "DX9ErrorHelper.h"

#	include "Logger/Logger.h"

namespace Menge
{
	//////////////////////////////////////////////////////////////////////////
	DX9RenderVertexBuffer::DX9RenderVertexBuffer()
		: m_pD3DDevice( nullptr )
		, m_vertexDeclaration( 0 )
		, m_vertexNum( 0 )
		, m_usage( 0 )
		, m_format( D3DFMT_UNKNOWN )
		, m_pool( D3DPOOL_MANAGED )
		, m_pVB( nullptr )
		, m_dynamic( false )
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
	bool DX9RenderVertexBuffer::initialize( IDirect3DDevice9 * _pD3DDevice, DWORD _vertexDeclaration, uint32_t _vertexNum, bool _dynamic )
	{
		m_pD3DDevice = _pD3DDevice;
		m_vertexDeclaration = _vertexDeclaration;
		m_vertexNum = _vertexNum;

		m_usage = D3DUSAGE_WRITEONLY;		
		m_dynamic = _dynamic;

        if( m_dynamic == true )
        {
            m_pool = D3DPOOL_DEFAULT;

            m_usage |= D3DUSAGE_DYNAMIC;
        }
        else
        {
            m_pool = D3DPOOL_MANAGED;
        }

		IDirect3DVertexBuffer9 * pVB = nullptr;
		IF_DXCALL( m_pD3DDevice, CreateVertexBuffer, (m_vertexNum * sizeof( RenderVertex2D ), m_usage, m_vertexDeclaration, m_pool, &pVB, NULL) )
		{
			return 0;
		}

		m_pVB = pVB;

		return true;
	}
	//////////////////////////////////////////////////////////////////////////
    Pointer DX9RenderVertexBuffer::lock( uint32_t _offset, uint32_t _count, EBufferLockFlag _flags )
	{
		if( _offset + _count > m_vertexNum )
		{
			LOGGER_ERROR("DX9RenderVertexBuffer::lock %d offset %d more max size %d"
				, _count
				, _offset
				, m_vertexNum
				);

			return nullptr;
		}

		DWORD d3d_flag = s_toD3DBufferLock( _flags );

		void * memory = nullptr;
		IF_DXCALL( m_pVB, Lock, (_offset * sizeof( RenderVertex2D ), _count * sizeof( RenderVertex2D ), &memory, d3d_flag) )
		{
			LOGGER_ERROR("DX9RenderVertexBuffer::lock %d offset %d invalid lock"
				, _count
				, _offset
				);

			return nullptr;
		}

		return memory;
	}
	//////////////////////////////////////////////////////////////////////////
	bool DX9RenderVertexBuffer::unlock()
	{
		IF_DXCALL( m_pVB, Unlock, () )
		{
			LOGGER_ERROR("DX9RenderIndexBuffer::unlock invalid"
				);

			return false;
		}

		return true;
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
}