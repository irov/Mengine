/*
 *	DX8RenderSystem.h
 *
 *	Created by _Berserk_ on 22.1.2009
 *	Copyright 2009 Menge. All rights reserved.
 *
 */

#	pragma once

#	include "Interface/RenderSystemInterface.h"

#	include <d3d8.h>

#	include <map>


namespace Menge
{
	class DX8Texture;
	class DX8RenderTexture;

	class LogServiceInterface;

	struct VBInfo
	{
		size_t length;
		size_t vertexSize;
		DWORD usage;
		DWORD fvf;
		D3DPOOL pool;
		IDirect3DVertexBuffer8* pVB;
	};

	struct IBInfo
	{
		size_t length;
		DWORD usage;
		D3DFORMAT format;
		D3DPOOL pool;
		IDirect3DIndexBuffer8* pIB;
	};

	class DX8RenderSystem
		: public RenderSystemInterface
	{
	public:
		DX8RenderSystem();
		~DX8RenderSystem();

	protected:
		void clear_( DWORD _color );
		bool supportNPOT_() const;
		void onRestoreDevice_();

	public:
		bool initialize( ServiceProviderInterface * _serviceProvider, RenderSystemListener* _listener ) override;
		
	public:
		bool createRenderWindow( size_t _width, size_t _height, int _bits, bool _fullscreen, WindowHandle _winHandle,
			bool _waitForVSync, int _FSAAType, int _FSAAQuality ) override;
		
		void getResolutions( TVectorResolutions & _resolutions ) override;

		void makeProjectionOrthogonal( mt::mat4f & _projectionMatrix
			, float _left, float _right
			, float _top, float _bottom
			, float _near, float _far ) override;

		void makeProjectionPerspective( mt::mat4f & _projectionMatrix, float _fov, float _aspect, float zn, float zf ) override;

		float getTexelOffsetX() const override;
		float getTexelOffsetY() const override;

		void clear( uint32 _color ) override;
		// Render frame into _image
		// int rect[4] - rectangle represents desired frame area in pixels
		void screenshot( RenderImageInterface* _image, const float * _rect ) override;
		// входные данные: матрица 4 на 4
		void setProjectionMatrix( const mt::mat4f & _projection ) override;
		void setModelViewMatrix( const mt::mat4f & _modelview ) override;
		void setTextureMatrix( size_t _stage, const float* _texture ) override;
		void setWorldMatrix( const mt::mat4f & _world ) override;

		VBHandle createVertexBuffer( size_t _verticesNum, size_t _vertexSize ) override;
		void releaseVertexBuffer( VBHandle _vbHandle ) override;
		void* lockVertexBuffer(  VBHandle _vbHandle, size_t _offset, size_t _size, uint32 _flags ) override;
		bool unlockVertexBuffer( VBHandle _vbHandle ) override;
		void setVertexBuffer( VBHandle _vbHandle ) override;

		IBHandle createIndexBuffer( size_t _indiciesNum ) override;
		void releaseIndexBuffer( IBHandle _ibHandle ) override;
		uint16* lockIndexBuffer(  IBHandle _ibHandle ) override;
		bool unlockIndexBuffer( IBHandle _ibHandle ) override;
		void setIndexBuffer( IBHandle _ibHandle, size_t _baseVertexIndex ) override;

		void setVertexDeclaration( size_t _vertexSize, uint32 _declaration ) override;

		void drawIndexedPrimitive( EPrimitiveType _type
			, size_t _baseVertexIndex
			, size_t _minIndex
			, size_t _verticesNum
			, size_t _startIndex
			, size_t _indexCount ) override;

		void setTexture( size_t _stage, RenderImageInterface* _texture ) override;
		void setTextureAddressing( size_t _stage, ETextureAddressMode _modeU, ETextureAddressMode _modeV ) override;
		void setTextureFactor( uint32 _color ) override;

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
		void setAlphaCmpFunc( ECompareFunction _alphaFunc, uint8 _alpha ) override;
		void setLightingEnable( bool _light ) override;

		void setTextureStageColorOp( size_t _stage, ETextureOp _textrueOp,
										ETextureArgument _arg1, ETextureArgument _arg2 ) override;
		void setTextureStageAlphaOp( size_t _stage, ETextureOp _textrueOp,
										ETextureArgument _arg1, ETextureArgument _arg2 ) override;
		void setTextureStageFilter( size_t _stage, ETextureFilterType _filterType, ETextureFilter _filter ) override;
		
		// create empty render image
		RenderImageInterface * createImage( size_t _width, size_t _height, size_t & _realWidth, size_t & _realHeight, PixelFormat& _format ) override;
		
		
		// create render target image
		RenderImageInterface * createRenderTargetImage( size_t& _width, size_t& _height,  size_t & _realWidth, size_t & _realHeight, PixelFormat& _format ) override;
		// отрисовка изображения

		bool beginScene() override;
		void endScene() override;
		void swapBuffers() override;
		void clearFrameBuffer( uint32 _frameBufferTypes, uint32 _color = 0
								, float _depth = 1.0f, uint16 _stencil = 0 ) override;

		void setViewport( const Viewport & _viewport ) override;

		void changeWindowMode( const Resolution & _resolution, bool _fullscreen ) override;
		void setRenderTarget( RenderImageInterface* _renderTarget, bool _clear ) override;

		bool supportTextureFormat( PixelFormat _format ) override;

		void onWindowMovedOrResized() override;
		void onWindowClose() override;

		void setVSync( bool _vSync ) override;
		void setSeparateAlphaBlendMode() override;
	private:
		// Log
		ServiceProviderInterface * m_serviceProvider;
		LogServiceInterface * m_logService;

		RenderSystemListener* m_listener;

		HMODULE m_hd3d8;

		Resolution m_screenResolution;
		int m_screenBits;
		bool m_fullscreen;

		IDirect3D8*			m_pD3D;
		IDirect3DDevice8*	m_pD3DDevice;
		IDirect3DSurface8*	m_screenSurf;
		IDirect3DSurface8*	m_screenDepth;

		D3DPRESENT_PARAMETERS*  d3dpp;
		D3DPRESENT_PARAMETERS   d3dppW;
		D3DPRESENT_PARAMETERS   d3dppFS;

		D3DMATRIX m_matTexture;

		// sync routines
		IDirect3DSurface8*	m_syncTargets[2];
		IDirect3DTexture8*	m_syncTempTex;
		IDirect3DSurface8*	m_syncTemp;
		unsigned int		m_frames;
		void syncCPU_();

		bool m_supportNPOT;
		bool m_supportR8G8B8;

		bool initLost_();
		
		void release_();
		bool restore_();

		void createSyncTargets_();

		HRESULT d3dCreateTexture_( UINT Width, UINT Height, UINT MipLevels,
			DWORD Usage, PixelFormat Format, D3DPOOL Pool, LPDIRECT3DTEXTURE8 * ppTexture );

		HRESULT loadSurfaceFromSurface_( LPDIRECT3DSURFACE8 pDestSurface, CONST RECT * pDestRect,
			LPDIRECT3DSURFACE8 pSrcSurface, CONST RECT * pSrcRect );

		bool m_inRender;

		DX8RenderTexture* m_curRenderTexture;

		VBHandle m_vbHandleCounter;
		IBHandle m_ibHandleCounter;

		typedef std::map<VBHandle, VBInfo> TMapVBInfo;
		TMapVBInfo m_vertexBuffers;

		typedef std::map<VBHandle, IBInfo> TMapIBInfo;
		TMapIBInfo m_indexBuffers;
		
		IBHandle m_currentIB;
		bool m_syncReady;
	};
}	// namespace Menge
