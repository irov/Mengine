#	include "DX9RenderIndexBuffer.h"

#	include "DX9RenderEnum.h"
#	include "DX9ErrorHelper.h"

#	include "Logger/Logger.h"

namespace Menge
{
	//////////////////////////////////////////////////////////////////////////
	DX9RenderIndexBuffer::DX9RenderIndexBuffer()
		: m_pD3DDevice(nullptr)
		, m_indexNum(0)
		, m_usage( 0 )
		, m_format( D3DFMT_UNKNOWN )
		, m_pool( D3DPOOL_MANAGED )
		, m_pIB(nullptr)
		, m_dynamic(false)
	{
	}
	//////////////////////////////////////////////////////////////////////////
	DX9RenderIndexBuffer::~DX9RenderIndexBuffer()
	{
		if( m_pIB != nullptr )
		{
			ULONG ref = m_pIB->Release();
			(void)ref;
			m_pIB = nullptr;
		}
	}
	//////////////////////////////////////////////////////////////////////////
	bool DX9RenderIndexBuffer::initialize( IDirect3DDevice9 * _pD3DDevice, uint32_t _indexNum, bool _dynamic )
	{
		m_pD3DDevice = _pD3DDevice;
		m_indexNum = _indexNum;

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
		
		m_format = s_getD3DIndexFormat();

		IDirect3DIndexBuffer9 * ib = nullptr;
		IF_DXCALL( m_pD3DDevice, CreateIndexBuffer, (m_indexNum * sizeof( RenderIndices ), m_usage, m_format, m_pool, &ib, NULL) )
		{
			return false;
		}

		m_pIB = ib;

		return true;
	}
	//////////////////////////////////////////////////////////////////////////
    Pointer DX9RenderIndexBuffer::lock( uint32_t _offset, uint32_t _count, EBufferLockFlag _flags )
	{
		if( _offset + _count > m_indexNum )
		{
			LOGGER_ERROR("DX9RenderIndexBuffer::lock %d offset %d more max size %d"
				, _count
				, _offset
				, m_indexNum
				);

			return nullptr;
		}

		DWORD d3d_flag = s_toD3DBufferLock( _flags );

		void * memory = nullptr;
		IF_DXCALL( m_pIB, Lock, (_offset * sizeof( RenderIndices ), _count * sizeof( RenderIndices ), &memory, d3d_flag) )
		{
			LOGGER_ERROR("DX9RenderIndexBuffer::lock %d offset %d invalid lock"
				, _count
				, _offset
				);

			return nullptr;
		}

		return memory;
	}
	//////////////////////////////////////////////////////////////////////////
	bool DX9RenderIndexBuffer::unlock()
	{
		IF_DXCALL( m_pIB, Unlock, () )
		{
			LOGGER_ERROR("DX9RenderIndexBuffer::unlock invalid"				
				);

			return false;
		}

		return true;
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
}