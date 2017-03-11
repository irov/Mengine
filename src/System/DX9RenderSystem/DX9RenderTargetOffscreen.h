#	pragma once

#	include "Interface/RenderSystemInterface.h"

#	include "Core/ServantBase.h"

#	include <d3d9.h>

namespace Menge
{
	class DX9RenderTargetOffscreen
		: public ServantBase<RenderTargetInterface>
	{
	public:
		DX9RenderTargetOffscreen();
		~DX9RenderTargetOffscreen();

	public:
		bool initialize( LPDIRECT3DDEVICE9 _device, uint32_t _width, uint32_t _height, PixelFormat _format );
		void finalize();

	public:
		bool begin() override;
		void end() override;

	public:
		bool getData( unsigned char * _buffer, size_t _pitch ) override;

	protected:
		uint32_t m_width;
		uint32_t m_height;
		D3DFORMAT m_format;

		D3DSURFACE_DESC m_desc;

		LPDIRECT3DDEVICE9 m_device;
		LPDIRECT3DTEXTURE9 m_renderTexture;
		LPDIRECT3DSURFACE9 m_surfacePlain;
		LPDIRECT3DSURFACE9 m_beginSurface;
		LPDIRECT3DSURFACE9 m_oldSurface;
	};
}