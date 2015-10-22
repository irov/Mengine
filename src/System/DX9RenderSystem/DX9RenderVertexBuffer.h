#	pragma once

#	include "Interface/RenderSystemInterface.h"

#	include <d3d9.h>

namespace Menge
{
	class DX9RenderVertexBuffer
		: public RenderVertexBufferInterface
	{
	public:
		DX9RenderVertexBuffer();
		~DX9RenderVertexBuffer();

	public:
		bool initialize( ServiceProviderInterface * _serviceProvider, IDirect3DDevice9 * _pD3DDevice, DWORD _vertexDeclaration, uint32_t _vertexNum, bool _dynamic );

	public:
		RenderVertex2D * lock( uint32_t _offset, uint32_t _size, EBufferLockFlag _flags ) override;
		bool unlock() override;

	public:
		bool enable();

	protected:
		ServiceProviderInterface * m_serviceProvider;
		IDirect3DDevice9 * m_pD3DDevice;

		DWORD m_vertexDeclaration;
		uint32_t m_vertexNum;
		DWORD m_usage;
		D3DFORMAT m_format;
		D3DPOOL m_pool;
		IDirect3DVertexBuffer9 * m_pVB;
		bool m_dynamic;
	};
}