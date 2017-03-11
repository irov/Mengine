#	pragma once

#	include "Interface/RenderSystemInterface.h"

#	include "Core/ServantBase.h"


#	include <d3d9.h>

namespace Menge
{
	class DX9RenderIndexBuffer
		: public ServantBase<RenderIndexBufferInterface>
	{
	public:
		DX9RenderIndexBuffer();
		~DX9RenderIndexBuffer();

	public:
		bool initialize( IDirect3DDevice9 * _pD3DDevice, uint32_t _indiciesNum, bool _dynamic );

	public:
        Pointer lock( uint32_t _offset, uint32_t _size, EBufferLockFlag _flags ) override;
		bool unlock() override;

	public:
		bool enable();

	protected:
		IDirect3DDevice9 * m_pD3DDevice;

		uint32_t m_indexNum;
		DWORD m_usage;		
		D3DFORMAT m_format;
		D3DPOOL m_pool;
		IDirect3DIndexBuffer9 * m_pIB;
		bool m_dynamic;
	};
	//////////////////////////////////////////////////////////////////////////
	typedef stdex::intrusive_ptr<DX9RenderIndexBuffer> DX9RenderIndexBufferPtr;
}