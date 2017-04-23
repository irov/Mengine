#	pragma once

#	include "Interface/RenderSystemInterface.h"

#   include "DX9RenderImage.h"
#   include "DX9RenderVertexShader.h"
#   include "DX9RenderFragmentShader.h"
#   include "DX9RenderProgram.h"
#   include "DX9RenderVertexBuffer.h"
#   include "DX9RenderIndexBuffer.h"

#   include "Core/ServiceBase.h"

#   include "Factory/Factory.h"

#   include "stdex/stl_map.h"

#	include <d3d9.h>

namespace Menge
{
	class DX9RenderSystem
		: public ServiceBase<RenderSystemInterface>
	{
	public:
		DX9RenderSystem();
		~DX9RenderSystem();

	public:
		bool _initialize() override;
        void _finalize() override;

	public:
		const ConstString & getRenderPlatformName() const override;

	public:
		bool createRenderWindow( const Resolution & _resolution, uint32_t _bits, bool _fullscreen
			, bool _waitForVSync, int _FSAAType, int _FSAAQuality, uint32_t _MultiSampleCount ) override;
		
        void makeProjectionOrthogonal( mt::mat4f & _projectionMatrix, const Viewport & _viewport, float _near, float _far ) override;
        void makeProjectionFrustum( mt::mat4f & _projectionMatrix, const Viewport & _viewport, float _near, float _far ) override;
        void makeProjectionPerspective( mt::mat4f & _projectionMatrix, float _fov, float _aspect, float zn, float zf ) override;
        void makeViewMatrixFromViewport( mt::mat4f & _viewMatrix, const Viewport & _viewport ) override;
        void makeViewMatrixLookAt( mt::mat4f & _viewMatrix, const mt::vec3f & _eye, const mt::vec3f & _dir, const mt::vec3f & _up, float _sign ) override;

		void clear( uint32_t _color, bool _force ) override;
		// Render frame into _image
		// int rect[4] - rectangle represents desired frame area in pixels
		bool screenshot( const RenderImageInterfacePtr & _image, const mt::vec4f & _rect ) override;
		// входные данные: матрица 4 на 4
		void setProjectionMatrix( const mt::mat4f & _projection ) override;
		void setModelViewMatrix( const mt::mat4f & _modelView ) override;
		void setTextureMatrix( uint32_t _stage, const mt::mat4f & _texture ) override;
		void setWorldMatrix( const mt::mat4f & _world ) override;

	public:
		RenderVertexBufferInterfacePtr createVertexBuffer( uint32_t _verticesNum, bool _dynamic ) override;
		bool setVertexBuffer( const RenderVertexBufferInterfacePtr & _vertexBuffer ) override;

		RenderIndexBufferInterfacePtr createIndexBuffer( uint32_t _indiciesNum, bool _dynamic ) override;
		bool setIndexBuffer( const RenderIndexBufferInterfacePtr & _indexBuffer ) override;

	public:
		RenderFragmentShaderInterfacePtr createFragmentShader( const ConstString & _name, const void * _buffer, size_t _size, bool _isCompile ) override;
		RenderVertexShaderInterfacePtr createVertexShader( const ConstString & _name, const void * _buffer, size_t _size, bool _isCompile ) override;
		
		RenderProgramInterfacePtr createProgram( const ConstString & _name, const RenderVertexShaderInterfacePtr & _vertex, const RenderFragmentShaderInterfacePtr & _fragment, uint32_t _samplerCount ) override;
		void setProgram( const RenderProgramInterfacePtr & _program ) override;
		void updateProgram( const RenderProgramInterfacePtr & _program ) override;

	public:
		void drawIndexedPrimitive( EPrimitiveType _type
			, uint32_t _baseVertexIndex
			, uint32_t _minIndex
			, uint32_t _verticesNum
			, uint32_t _startIndex
			, uint32_t _indexCount ) override;

		void setTexture( uint32_t _stage, const RenderImageInterfacePtr & _texture ) override;
		void setTextureAddressing( uint32_t _stage, ETextureAddressMode _modeU, ETextureAddressMode _modeV, uint32_t _border ) override;
		void setTextureFactor( uint32_t _color ) override;

		void setBlendFactor( EBlendFactor _src, EBlendFactor _dst, EBlendOp _op ) override;
		
		void setCullMode( ECullMode _mode ) override;

		void setDepthBufferTestEnable( bool _depthTest ) override;
		void setDepthBufferWriteEnable( bool _depthWrite ) override;
		void setDepthBufferCmpFunc( ECompareFunction _depthFunction ) override;

		void setFillMode( EFillMode _mode ) override;
		void setColorBufferWriteEnable( bool _r, bool _g, bool _b, bool _a ) override;
		void setShadeType( EShadeType _sType ) override;
		void setAlphaBlendEnable( bool _alphaBlend ) override;
		void setLightingEnable( bool _light ) override;

		void setTextureStageColorOp( uint32_t _stage, ETextureOp _textrueOp,
										ETextureArgument _arg1, ETextureArgument _arg2 ) override;
		void setTextureStageAlphaOp( uint32_t _stage, ETextureOp _textrueOp,
										ETextureArgument _arg1, ETextureArgument _arg2 ) override;

        void setTextureStageTexCoordIndex( uint32_t _stage, uint32_t _index ) override;
				
		void setTextureStageFilter( uint32_t _stage, ETextureFilter _minification, ETextureFilter _mipmap, ETextureFilter _magnification ) override;
		
		// create empty render image
		RenderImageInterfacePtr createImage( uint32_t _mipmaps, uint32_t _width, uint32_t _height, uint32_t _channels, uint32_t _depth, PixelFormat _format ) override;
		RenderImageInterfacePtr createDynamicImage( uint32_t _width, uint32_t _height, uint32_t _channels, uint32_t _depth, PixelFormat _format ) override;
		
		
		// create render target image
		RenderTargetInterface * createRenderTargetOffscreen( uint32_t _width, uint32_t _height, PixelFormat _format ) override;
		// отрисовка изображения

		bool beginScene() override;
		void endScene() override;
		void swapBuffers() override;
		void clearFrameBuffer( uint32_t _frameBufferTypes, uint32_t _color, float _depth, uint32_t _stencil ) override;

		void setClipplaneCount( uint32_t _count ) override;
		void setClipplane( uint32_t _i, const mt::planef & _plane ) override;

		void setViewport( const Viewport & _viewport ) override;

		void changeWindowMode( const Resolution & _resolution, bool _fullscreen ) override;
		
		bool supportTextureFormat( PixelFormat _format ) const override;
		bool supportTextureNonPow2() const override;

		uint32_t getMaxCombinedTextureImageUnits() const override;

		void onWindowChangeFullscreenPrepare( bool _fullscreen ) override;
		void onWindowChangeFullscreen( bool _fullscreen ) override;
		void onWindowMovedOrResized() override;
		void onWindowClose() override;

		void setVSync( bool _vSync ) override;
		void setSeparateAlphaBlendMode() override;

	public:
		size_t getTextureMemoryUse() const override;
		uint32_t getTextureCount() const override;
		
	public:
		bool lockRenderTarget( const RenderImageInterfacePtr & _renderTarget ) override;
		bool unlockRenderTarget() override;

	protected:
		LPDIRECT3DSURFACE9 m_oldRenderTarget;

    protected:
        void updateVSyncDPP_();
        bool resetDevice_();
		void clear_( DWORD _color );

	protected:
		void updateViewport_( const Viewport & _viewport );

    protected:        
        void fixNPOTSupport_( uint32_t & _width, uint32_t & _height ) const;

	private:	
		ConstString m_renderPlatform;

		HMODULE m_hd3d9;

		Resolution m_windowResolution;
		int m_screenBits;
		bool m_fullscreen;

        IDirect3D9 * m_pD3D;
		IDirect3DDevice9 * m_pD3DDevice;

		D3DPRESENT_PARAMETERS m_d3dppW;
		D3DPRESENT_PARAMETERS m_d3dppFS;

		D3DDISPLAYMODE m_displayMode;

		D3DPRESENT_PARAMETERS * m_d3dpp;

		// sync routines
		unsigned int m_frames;

    protected:	
        bool releaseResources_();
		void release_();
		bool restore_();

		//void createSyncTargets_();

		bool d3dCreateTexture_( uint32_t Width, uint32_t Height, uint32_t MipLevels,
			DWORD Usage, PixelFormat Format, D3DPOOL Pool, LPDIRECT3DTEXTURE9 * ppTexture );

		DX9RenderImagePtr createDX9RenderImage_( IDirect3DTexture9 * _d3dInterface, ERenderImageMode _mode, uint32_t _mipmaps, uint32_t _hwWidth, uint32_t _hwHeight, uint32_t _hwChannels, PixelFormat _hwPixelFormat );

	protected:
		void onDestroyDX9RenderImage_( DX9RenderImage * _image );

	protected:
		UINT m_adapterToUse;
        D3DDEVTYPE m_deviceType;

        Viewport m_viewport;

		DWORD m_fvf;
		IDirect3DVertexDeclaration9 * m_vertexDeclaration;

		FactoryPtr m_factoryRenderVertexShader;
        FactoryPtr m_factoryRenderFragmentShader;
        FactoryPtr m_factoryRenderProgram;
        FactoryPtr m_factoryVertexBuffer;
        FactoryPtr m_factoryIndexBuffer;
        FactoryPtr m_factoryDX9Texture;

        typedef stdex::vector<DX9RenderVertexShaderPtr> TVectorRenderVertexShaders;
        TVectorRenderVertexShaders m_deferredCompileVertexShaders;

        typedef stdex::vector<DX9RenderFragmentShaderPtr> TVectorRenderFragmentShaders;
        TVectorRenderFragmentShaders m_deferredCompileFragmentShaders;

        typedef stdex::vector<DX9RenderProgramPtr> TVectorRenderPrograms;
        TVectorRenderPrograms m_deferredCompilePrograms;


		mt::mat4f m_projectionMatrix;
		mt::mat4f m_modelViewMatrix;
		mt::mat4f m_worldMatrix;

		uint32_t m_dxMaxCombinedTextureImageUnits;

		size_t m_textureMemoryUse;
		uint32_t m_textureCount;

		bool m_vertexBufferEnable;
		bool m_indexBufferEnable;

        bool m_textureEnable[MENGE_MAX_TEXTURE_STAGES];
		
        bool m_waitForVSync;
	};
}	// namespace Menge
