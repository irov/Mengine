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
		uint32_t length;
		uint32_t vertexSize;
		DWORD usage;
		DWORD fvf;
		D3DPOOL pool;
		IDirect3DVertexBuffer9* pVB;
		bool dynamic;
	};

	struct IBInfo
	{
		uint32_t length;
		DWORD usage;
		D3DFORMAT format;
		D3DPOOL pool;
		IDirect3DIndexBuffer9* pIB;
		bool dynamic;
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
		bool createRenderWindow( const Resolution & _resolution, uint32_t _bits, bool _fullscreen, WindowHandle _winHandle,
			bool _waitForVSync, int _FSAAType, int _FSAAQuality ) override;
		
        void makeProjectionOrthogonal( mt::mat4f & _projectionMatrix, const Viewport & _viewport, float _near, float _far ) override;
        void makeProjectionFrustum( mt::mat4f & _projectionMatrix, const Viewport & _viewport, float _near, float _far ) override;
        void makeProjectionPerspective( mt::mat4f & _projectionMatrix, float _fov, float _aspect, float zn, float zf ) override;
        void makeViewMatrixFromViewport( mt::mat4f & _viewMatrix, const Viewport & _viewport ) override;
        void makeViewMatrixLookAt( mt::mat4f & _viewMatrix, const mt::vec3f & _eye, const mt::vec3f & _dir, const mt::vec3f & _up, float _sign ) override;

		void clear( uint32_t _color ) override;
		// Render frame into _image
		// int rect[4] - rectangle represents desired frame area in pixels
		bool screenshot( const RenderImageInterfacePtr & _image, const mt::vec4f & _rect ) override;
		// входные данные: матрица 4 на 4
		void setProjectionMatrix( const mt::mat4f & _projection ) override;
		void setModelViewMatrix( const mt::mat4f & _modelview ) override;
		void setTextureMatrix( uint32_t _stage, const float* _texture ) override;
		void setWorldMatrix( const mt::mat4f & _world ) override;

		VBHandle createVertexBuffer( uint32_t _verticesNum, uint32_t _vertexSize, bool _dynamic ) override;
		void releaseVertexBuffer( VBHandle _vbHandle ) override;
		void * lockVertexBuffer(  VBHandle _vbHandle, uint32_t _offset, uint32_t _size, EBufferLockFlag _flags ) override;
		bool unlockVertexBuffer( VBHandle _vbHandle ) override;
		void setVertexBuffer( VBHandle _vbHandle ) override;

		IBHandle createIndexBuffer( uint32_t _indiciesNum, bool _dynamic ) override;
		void releaseIndexBuffer( IBHandle _ibHandle ) override;
		void * lockIndexBuffer( IBHandle _ibHandle, uint32_t _offset, uint32_t _size, EBufferLockFlag _flags ) override;
		bool unlockIndexBuffer( IBHandle _ibHandle ) override;
		void setIndexBuffer( IBHandle _ibHandle, uint32_t _baseVertexIndex ) override;

		void setVertexDeclaration( uint32_t _vertexSize, uint32_t _declaration ) override;

        RenderShaderInterface * createShader( const void * _code, size_t _len ) override;
        void setShader( RenderShaderInterface * _shader ) override;

		void drawIndexedPrimitive( EPrimitiveType _type
			, uint32_t _baseVertexIndex
			, uint32_t _minIndex
			, uint32_t _verticesNum
			, uint32_t _startIndex
			, uint32_t _indexCount ) override;

		void setTexture( uint32_t _stage, const RenderImageInterfacePtr & _texture ) override;
		void setTextureAddressing( uint32_t _stage, ETextureAddressMode _modeU, ETextureAddressMode _modeV ) override;
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

		void setTextureStageColorOp( uint32_t _stage, ETextureOp _textrueOp,
										ETextureArgument _arg1, ETextureArgument _arg2 ) override;
		void setTextureStageAlphaOp( uint32_t _stage, ETextureOp _textrueOp,
										ETextureArgument _arg1, ETextureArgument _arg2 ) override;

        void setTextureStageTexCoordIndex( uint32_t _stage, uint32_t _index ) override;

		void setTextureStageFilter( uint32_t _stage, ETextureFilterType _filterType, ETextureFilter _filter ) override;
		
		// create empty render image
		RenderImageInterfacePtr createImage( uint32_t _mipmaps, uint32_t _width, uint32_t _height, uint32_t _channels, uint32_t _depth, PixelFormat _format ) override;
		RenderImageInterfacePtr createDynamicImage( uint32_t _width, uint32_t _height, uint32_t _channels, uint32_t _depth, PixelFormat _format ) override;
		
		
		// create render target image
		RenderImageInterfacePtr createRenderTargetImage( uint32_t _width, uint32_t _height, uint32_t _channels, uint32_t _depth, PixelFormat _format ) override;
		// отрисовка изображения

		bool beginScene() override;
		void endScene() override;
		void swapBuffers() override;
		void clearFrameBuffer( uint32_t _frameBufferTypes, uint32_t _color, float _depth, uint32_t _stencil ) override;

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
		void updateViewport_( const Viewport & _viewport );

    protected:        
        void fixNPOTSupport_( uint32_t & _width, uint32_t & _height ) const;

	private:
		ServiceProviderInterface * m_serviceProvider;

        RenderSystemListener * m_listener;

		HMODULE m_hd3d9;

		Resolution m_windowResolution;
		int m_screenBits;
		bool m_fullscreen;

        IDirect3D9 * m_pD3D;
		IDirect3DDevice9 * m_pD3DDevice;

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

		bool d3dCreateTexture_( uint32_t Width, uint32_t Height, uint32_t MipLevels,
			DWORD Usage, PixelFormat Format, D3DPOOL Pool, LPDIRECT3DTEXTURE9 * ppTexture );

		void refreshRenderStates_();

	protected:
		DWORD m_renderStates[256];
		DWORD m_textureStageStates[MENGE_MAX_TEXTURE_STAGES][64];
		DWORD m_samplerStates[MENGE_MAX_TEXTURE_STAGES][16];

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

        IDirect3DTexture9 * m_currentTexture[MENGE_MAX_TEXTURE_STAGES];

        typedef FactoryPoolStore<DX9Texture, 128> TFactoryDX9Texture;
        TFactoryDX9Texture m_factoryDX9Texture;

        typedef FactoryPoolStore<DX9RenderTexture, 4> TFactoryDX9RenderTexture;
        TFactoryDX9RenderTexture m_factoryDX9RenderTexture;
		
		bool m_syncReady;
        bool m_waitForVSync;
	};
}	// namespace Menge
