#	pragma once

#	include "Interface/RenderSystemInterface.h"

#   include "DX9RenderTexture.h"

#   include "Factory/FactoryStore.h"

#   include "stdex/binary_vector.h"

#	include <d3d9.h>
#	include <d3dx9math.h>
#   include <D3DX9Shader.h>

namespace Menge
{
	struct VBInfo
	{
		size_t length;
		size_t vertexSize;
		DWORD usage;
		DWORD fvf;
		D3DPOOL pool;
		IDirect3DVertexBuffer9* pVB;
	};

	struct IBInfo
	{
		size_t length;
		DWORD usage;
		D3DFORMAT format;
		D3DPOOL pool;
		IDirect3DIndexBuffer9* pIB;
	};

	class DX9RenderSystem
		: public RenderSystemInterface
	{
	public:
		DX9RenderSystem();
		~DX9RenderSystem();
        
    public:
        void setServiceProvider( ServiceProviderInterface * _serviceProvider ) override;
        ServiceProviderInterface * getServiceProvider() const override;

    protected:
		void clear_( DWORD _color );

	public:
		bool initialize() override;
        void finalize() override;

    public:
        void setRenderListener( RenderSystemListener * _listener ) override;
		
	public:
		bool createRenderWindow( const Resolution & _resolution, int _bits, bool _fullscreen, WindowHandle _winHandle,
			bool _waitForVSync, int _FSAAType, int _FSAAQuality ) override;
		
        void makeProjectionOrthogonal( mt::mat4f & _projectionMatrix, const Viewport & _viewport, float _near, float _far ) override;
        void makeProjectionFrustum( mt::mat4f & _projectionMatrix, const Viewport & _viewport, float _near, float _far ) override;
        void makeProjectionPerspective( mt::mat4f & _projectionMatrix, float _fov, float _aspect, float zn, float zf ) override;
        void makeViewMatrixFromViewport( mt::mat4f & _viewMatrix, const Viewport & _viewport ) override;
        void makeViewMatrixLookAt( mt::mat4f & _viewMatrix, const mt::vec3f & _eye, const mt::vec3f & _dir, const mt::vec3f & _up, float _sign ) override;

		void clear( uint32_t _color ) override;
		// Render frame into _image
		// int rect[4] - rectangle represents desired frame area in pixels
		bool screenshot( const RenderImageInterfacePtr & _image, const float * _rect ) override;
		// входные данные: матрица 4 на 4
		void setProjectionMatrix( const mt::mat4f & _projection ) override;
		void setModelViewMatrix( const mt::mat4f & _modelview ) override;
		void setTextureMatrix( size_t _stage, const float* _texture ) override;
		void setWorldMatrix( const mt::mat4f & _world ) override;

		VBHandle createVertexBuffer( size_t _verticesNum, size_t _vertexSize, bool _dynamic ) override;
		void releaseVertexBuffer( VBHandle _vbHandle ) override;
		void * lockVertexBuffer(  VBHandle _vbHandle, size_t _offset, size_t _size, uint32_t _flags ) override;
		bool unlockVertexBuffer( VBHandle _vbHandle ) override;
		void setVertexBuffer( VBHandle _vbHandle ) override;

		IBHandle createIndexBuffer( size_t _indiciesNum, bool _dynamic ) override;
		void releaseIndexBuffer( IBHandle _ibHandle ) override;
		void * lockIndexBuffer( IBHandle _ibHandle, size_t _offset, size_t _size, uint32_t _flags ) override;
		bool unlockIndexBuffer( IBHandle _ibHandle ) override;
		void setIndexBuffer( IBHandle _ibHandle, size_t _baseVertexIndex ) override;

		void setVertexDeclaration( size_t _vertexSize, uint32_t _declaration ) override;

        RenderShaderInterface * createShader( const void * _code, size_t _len ) override;
        void setShader( RenderShaderInterface * _shader ) override;

		void drawIndexedPrimitive( EPrimitiveType _type
			, size_t _baseVertexIndex
			, size_t _minIndex
			, size_t _verticesNum
			, size_t _startIndex
			, size_t _indexCount ) override;

		void setTexture( size_t _stage, const RenderImageInterfacePtr & _texture ) override;
		void setTextureAddressing( size_t _stage, ETextureAddressMode _modeU, ETextureAddressMode _modeV ) override;
		void setTextureFactor( uint32_t _color ) override;

		void setSrcBlendFactor( EBlendFactor _src ) override;
		void setDstBlendFactor( EBlendFactor _dst ) override;

		void setCullMode( ECullMode _mode ) override;

		void setDepthBufferTestEnable( bool _depthTest ) override;
		void setDepthBufferWriteEnable( bool _depthWrite ) override;
		void setDepthBufferCmpFunc( ECompareFunction _depthFunction ) override;

		void setFillMode( EFillMode _mode ) override;
		void setColorBufferWriteEnable( bool _r, bool _g, bool _b, bool _a ) override;
		void setShadeType( EShadeType _sType ) override;
		void setAlphaTestEnable( bool _alphaTest ) override;
		void setAlphaBlendEnable( bool _alphaBlend ) override;
		void setAlphaCmpFunc( ECompareFunction _alphaFunc, uint8_t _alpha ) override;
		void setLightingEnable( bool _light ) override;

		void setTextureStageColorOp( size_t _stage, ETextureOp _textrueOp,
										ETextureArgument _arg1, ETextureArgument _arg2 ) override;
		void setTextureStageAlphaOp( size_t _stage, ETextureOp _textrueOp,
										ETextureArgument _arg1, ETextureArgument _arg2 ) override;

        void setTextureStageTexCoordIndex( size_t _stage, size_t _index ) override;

		void setTextureStageFilter( size_t _stage, ETextureFilterType _filterType, ETextureFilter _filter ) override;
		
		// create empty render image
		RenderImageInterfacePtr createImage( size_t _width, size_t _height, size_t _channels, PixelFormat _format ) override;
		RenderImageInterfacePtr createDynamicImage( size_t _width, size_t _height, size_t _channels, PixelFormat _format ) override;
		
		
		// create render target image
		RenderImageInterfacePtr createRenderTargetImage( size_t _width, size_t _height, size_t _channels, PixelFormat _format ) override;
		// отрисовка изображения

		bool beginScene() override;
		void endScene() override;
		void swapBuffers() override;
		void clearFrameBuffer( uint32_t _frameBufferTypes, uint32_t _color = 0, float _depth = 1.0f, uint16_t _stencil = 0 ) override;

		void setViewport( const Viewport & _viewport ) override;

		void changeWindowMode( const Resolution & _resolution, bool _fullscreen ) override;
		bool setRenderTarget( const RenderImageInterfacePtr & _renderTarget, bool _clear ) override;

		bool supportTextureFormat( PixelFormat _format ) const override;

		void onWindowMovedOrResized() override;
		void onWindowClose() override;

		void setVSync( bool _vSync ) override;
		void setSeparateAlphaBlendMode() override;

    protected:
        void updateVSyncDPP_();
        bool resetDevice_();

    protected:        
        void fixNPOTSupport_( size_t & _width, size_t & _height ) const;

	private:
		ServiceProviderInterface * m_serviceProvider;

        RenderSystemListener* m_listener;

		HMODULE m_hd3d9;

		Resolution m_windowResolution;
		int m_screenBits;
		bool m_fullscreen;

        IDirect3D9 * m_pD3D;
		IDirect3DDevice9 * m_pD3DDevice;
		IDirect3DSurface9 * m_screenSurf;
		IDirect3DSurface9 *	m_screenDepth;

        D3DCAPS9 m_caps;

		D3DPRESENT_PARAMETERS m_d3dppW;
		D3DPRESENT_PARAMETERS m_d3dppFS;

		D3DPRESENT_PARAMETERS * m_d3dpp;

		D3DMATRIX m_matTexture;

		// sync routines
		//IDirect3DSurface9 * m_syncTargets[2];
		//IDirect3DTexture9 *	m_syncTempTex;
		//IDirect3DSurface9 * m_syncTemp;
		unsigned int m_frames;

    protected:
		void syncCPU_();

		bool initLost_();
		
        bool releaseResources_();
		void release_();
		bool restore_();

		//void createSyncTargets_();

		bool d3dCreateTexture_( UINT Width, UINT Height, UINT MipLevels,
			DWORD Usage, PixelFormat Format, D3DPOOL Pool, LPDIRECT3DTEXTURE9 * ppTexture );

		bool loadSurfaceFromSurface_( LPDIRECT3DSURFACE9 pDestSurface, CONST RECT * pDestRect,
			LPDIRECT3DSURFACE9 pSrcSurface, CONST RECT * pSrcRect );

		void refreshRenderStates_();

	protected:
		DWORD m_renderStates[256];
		DWORD m_textureStageStates[MENGE_MAX_TEXTURE_STAGES][64];
		DWORD m_samplerStates[MENGE_MAX_TEXTURE_STAGES][16];

		DX8RenderTexturePtr m_curRenderTexture;

        mt::mat4f m_projection;
        mt::mat4f m_modelview;

        UINT m_adapterToUse;
        D3DDEVTYPE m_deviceType;

        Viewport m_viewport;

		VBHandle m_vbHandleCounter;
		IBHandle m_ibHandleCounter;

		typedef stdex::binary_vector<VBHandle, VBInfo> TMapVBInfo;
		TMapVBInfo m_vertexBuffers;

        VBHandle m_currentVB;

		typedef stdex::binary_vector<VBHandle, IBInfo> TMapIBInfo;
		TMapIBInfo m_indexBuffers;

        IBHandle m_currentIB;

        IDirect3DTexture9 * m_currentTexture[8];

        typedef FactoryPoolStore<DX9Texture, 128> TFactoryDX9Texture;
        TFactoryDX9Texture m_factoryDX9Texture;

        typedef FactoryPoolStore<DX9RenderTexture, 4> TFactoryDX9RenderTexture;
        TFactoryDX9RenderTexture m_factoryDX9RenderTexture;
		
		bool m_syncReady;
        bool m_waitForVSync;
	};
}	// namespace Menge
