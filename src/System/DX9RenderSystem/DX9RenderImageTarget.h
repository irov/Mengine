#pragma once

#include "Interface/RenderImageInterface.h"

#include "DX9RenderResourceHandler.h"

#include "Kernel/ServantBase.h"

#include <d3d9.h>

namespace Mengine
{
    typedef IntrusivePtr<class DX9RenderTargetTexture> DX9RenderTargetTexturePtr;

	class DX9RenderImageTarget
		: public ServantBase<RenderImageInterface>
	{
	public:
        DX9RenderImageTarget();
        ~DX9RenderImageTarget() override;

    public:
        void initialize( const DX9RenderTargetTexturePtr & _target );
        void finalize();
		
    public:
        void bind( uint32_t _stage ) override;

	protected:
		void setRenderImageProvider( const RenderImageProviderInterfacePtr & _renderImageProvider ) override;
		const RenderImageProviderInterfacePtr & getRenderImageProvider() const override;

	public:
        LPDIRECT3DDEVICE9 getDirect3dDevice9() const;
        LPDIRECT3DTEXTURE9 getDirect3dTexture9() const;

	public:
		ERenderImageMode getMode() const override;

	public:
		uint32_t getHWWidth() const override;
		uint32_t getHWHeight() const override;
        uint32_t getHWChannels() const override;
		uint32_t getHWDepth() const override;
		uint32_t getHWMipmaps() const override;
		
        PixelFormat getHWPixelFormat() const override;

    protected:
        float getHWWidthInv() const override;
        float getHWHeightInv() const override;

	public:
        Pointer lock( size_t * _pitch, uint32_t _level, const Rect & _rect, bool _readOnly ) override;
		bool unlock( uint32_t _level, bool _successful ) override;
        
	protected:
        DX9RenderTargetTexturePtr m_target;
	};
	//////////////////////////////////////////////////////////////////////////
    typedef IntrusivePtr<DX9RenderImageTarget> DX9RenderImageTargetPtr;
}	
