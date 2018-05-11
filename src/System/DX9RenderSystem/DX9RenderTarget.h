#pragma once

#include "Interface/RenderSystemInterface.h"

#include "Core/ServantBase.h"

#include <d3d9.h>

namespace Mengine
{
	class DX9RenderTarget
        : public Factorable
        , public RenderTargetInterface
	{
	public:
        DX9RenderTarget();
		~DX9RenderTarget() override;

	public:
		bool initialize( LPDIRECT3DDEVICE9 _device, uint32_t _width, uint32_t _height, PixelFormat _format );
		void finalize();

    protected:
        virtual bool _initialize();
        virtual void _finalize();

    public:
        uint32_t getWidth() const override;
        uint32_t getHeight() const override;
        PixelFormat getFormat() const override;

    public:
        uint32_t getHWWidth() const override;
        uint32_t getHWHeight() const override;

	public:
		bool begin() override;
		void end() override;

    public:
        bool getData( unsigned char * _buffer, size_t _pitch ) override;
 
    public:
        LPDIRECT3DTEXTURE9 getDX9RenderTexture() const;

	protected:
		uint32_t m_width;
		uint32_t m_height;
        PixelFormat m_format;

        uint32_t m_hwWidth;
        uint32_t m_hwHeight;
        
		LPDIRECT3DDEVICE9 m_device;
		LPDIRECT3DTEXTURE9 m_renderTexture;
		LPDIRECT3DSURFACE9 m_surface;
		LPDIRECT3DSURFACE9 m_oldSurface;
	};
    //////////////////////////////////////////////////////////////////////////
    typedef IntrusivePtr<DX9RenderTarget> DX9RenderTargetPtr;
}