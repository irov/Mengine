#	pragma once

#	include "Interface/RenderSystemInterface.h"

#	include <d3d9.h>

namespace Menge
{
	class DX9RenderTargetOffscreen
		: public RenderTargetInterface
	{
	public:
		DX9RenderTargetOffscreen();
		~DX9RenderTargetOffscreen();

	public:
		void setServiceProvider( ServiceProviderInterface * _serviceProvider );

	public:
		bool initialize( LPDIRECT3DDEVICE9 _device, uint32_t _width, uint32_t _height );

	public:
		bool begin() override;
		void end() override;

	public:
		bool getData( unsigned char * _buffer, size_t _pitch ) override;

	protected:
		ServiceProviderInterface * m_serviceProvider;

		uint32_t m_width;
		uint32_t m_height;

		LPDIRECT3DDEVICE9 m_device;
		LPDIRECT3DTEXTURE9 m_renderTexture;
		LPDIRECT3DSURFACE9 m_surfacePlain;
		LPDIRECT3DSURFACE9 m_beginSurface;
		LPDIRECT3DSURFACE9 m_oldSurface;
	};
}