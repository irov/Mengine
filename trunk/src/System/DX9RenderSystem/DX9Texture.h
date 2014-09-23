#	pragma once

#	include "Interface/RenderSystemInterface.h"

#	include <d3d9.h>

namespace Menge
{
	class DX9Texture
		: public RenderImageInterface
	{
	public:
		DX9Texture();
        ~DX9Texture();

    public:
        void initialize( ServiceProviderInterface * _serviceProvider, IDirect3DTexture9 * _d3dInterface, ERenderImageMode _mode, uint32_t _hwWidth, uint32_t _hwHeight, uint32_t _hwChannels, PixelFormat _hwPixelFormat );
		
	public:
		IDirect3DTexture9 * getDXTextureInterface() const;		

	public:
		ERenderImageMode getMode() const override;

	public:
		uint32_t getHWWidth() const override;
		uint32_t getHWHeight() const override;
        uint32_t getHWChannels() const override;
		uint32_t getHWDepth() const override;

        PixelFormat getHWPixelFormat() const override;
        
	public:
        void * lock( size_t * _pitch, const Rect& _rect, bool _readOnly ) override;
		void unlock() override;
		               		
	protected:
		ServiceProviderInterface * m_serviceProvider;

		IDirect3DTexture9 * m_d3dTexture;

		ERenderImageMode m_mode;

		uint32_t m_hwWidth;
		uint32_t m_hwHeight;
        uint32_t m_hwChannels;

        PixelFormat m_hwPixelFormat;
	};

    typedef stdex::intrusive_ptr<DX9Texture> DX9TexturePtr;
}	// namespace Menge
