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
        void initialize( IDirect3DTexture9 * _d3dInterface, size_t _hwWidth, size_t _hwHeight, size_t _hwChannels, PixelFormat _hwPixelFormat );

	public:
		IDirect3DTexture9 * getDXTextureInterface() const;		

	public:
		size_t getHWWidth() const override;
		size_t getHWHeight() const override;
        size_t getHWChannels() const override;

        PixelFormat getHWPixelFormat() const override;
        

	public:
        unsigned char * lock( int* _pitch, const Rect& _rect, bool _readOnly ) override;
		void unlock() override;
                		
	protected:
		IDirect3DTexture9 * m_d3dInterface;

		size_t m_hwWidth;
		size_t m_hwHeight;
        size_t m_hwChannels;

        PixelFormat m_hwPixelFormat;        
	};

    typedef IntrusivePtr<DX9Texture> DX9TexturePtr;
}	// namespace Menge
