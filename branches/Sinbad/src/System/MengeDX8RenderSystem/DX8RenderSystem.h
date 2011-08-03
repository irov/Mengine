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

	struct VBInfo
	{
		std::size_t length;
		std::size_t vertexSize;
		DWORD usage;
		DWORD fvf;
		D3DPOOL pool;
		IDirect3DVertexBuffer8* pVB;
	};

	struct IBInfo
	{
		std::size_t length;
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

		void clear( DWORD _color );
		bool supportNPOT() const;
		void onRestoreDevice();

	public:
		bool initialize( LogSystemInterface* _logSystem, RenderSystemListener* _listener ) override;
		
		bool createRenderWindow( std::size_t _width, std::size_t _height, int _bits, bool _fullscreen, WindowHandle _winHandle,
			bool _waitForVSync, int _FSAAType, int _FSAAQuality ) override;
		
		void getResolutions( TVectorResolutions & _resolutions ) override;
		void makeProjection2D( float _left, float _right,
			float _top, float _bottom, 
			float _near, float _far,
			float* _outMatrix ) override;

		float getTexelOffsetX() const override;
		float getTexelOffsetY() const override;
		// Render frame into _image
		// int rect[4] - rectangle represents desired frame area in pixels
		void screenshot( RenderImageInterface* _image, const float * _rect ) override;
		// ������� ������: ������� 4 �� 4
		void setProjectionMatrix( const float * _projection ) override;
		void setModelViewMatrix( const float * _modelview ) override;
		void setTextureMatrix( size_t _stage, const float* _texture ) override;

		VBHandle createVertexBuffer( std::size_t _verticesNum, std::size_t _vertexSize ) override;
		void releaseVertexBuffer( VBHandle _vbHandle ) override;
		void* lockVertexBuffer(  VBHandle _vbHandle, size_t _offset, size_t _size, uint32 _flags ) override;
		bool unlockVertexBuffer( VBHandle _vbHandle ) override;
		void setVertexBuffer( VBHandle _vbHandle ) override;

		IBHandle createIndexBuffer( std::size_t _indiciesNum ) override;
		void releaseIndexBuffer( IBHandle _ibHandle ) override;
		uint16* lockIndexBuffer(  IBHandle _ibHandle ) override;
		bool unlockIndexBuffer( IBHandle _ibHandle ) override;
		void setIndexBuffer( IBHandle _ibHandle ) override;

		void setVertexDeclaration( uint32 _declaration ) override;

		void drawIndexedPrimitive( EPrimitiveType _type, std::size_t _baseVertexIndex,
			std::size_t _minIndex, std::size_t _verticesNum, std::size_t _startIndex, std::size_t _indexCount ) override;

		void setTexture( std::size_t _stage, RenderImageInterface* _texture ) override;
		void setTextureAddressing( std::size_t _stage, ETextureAddressMode _modeU, ETextureAddressMode _modeV ) override;
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
		RenderImageInterface * createImage( std::size_t& _width, std::size_t& _height, PixelFormat& _format ) override;
		// create render target image
		RenderImageInterface * createRenderTargetImage( std::size_t& _width, std::size_t& _height ) override;
		// �������� �����������
		void releaseImage( RenderImageInterface * _image ) override;
		//
		// ��������� �����������

		bool beginScene() override;
		void endScene() override;
		void swapBuffers() override;
		void clearFrameBuffer( uint32 _frameBufferTypes, uint32 _color = 0
								, float _depth = 1.0f, uint16 _stencil = 0 ) override;
		void	beginLayer2D() override;
		void	endLayer2D() override;
		void	beginLayer3D() override;
		void	endLayer3D() override;

		void	setRenderViewport( const Viewport & _viewport ) override;

		void	changeWindowMode( const Resolution & _resolution, bool _fullscreen ) override;
		void	setRenderTarget( RenderImageInterface* _renderTarget, bool _clear ) override;

		LightInterface * createLight( const String & _name ) override;
		void releaseLight( LightInterface * _light ) override;

		void onWindowMovedOrResized() override;
		void onWindowClose() override;

		void setVSync( bool _vSync ) override;

	private:
		D3DMULTISAMPLE_TYPE getMultisampleLevel_( D3DFORMAT _surfaceFormat, bool _windowed );

	private:
		// Log
		LogSystemInterface* m_logSystem;
		RenderSystemListener* m_listener;
		HMODULE m_hd3d8;

		void log( const char* _message, ... );
		void log_error( const char* _message, ... );

		Resolution m_screenResolution;
		int m_screenBits;
		bool m_fullscreen;

		IDirect3D8*				m_pD3D;
		IDirect3DDevice8*		m_pD3DDevice;
		IDirect3DSurface8*	pScreenSurf;
		IDirect3DSurface8*	pScreenDepth;

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

		bool init_lost_();
		void set_clipping_( int _x, int _y, int _w, int _h );
		
		void release_();
		bool restore_();

		void createSyncTargets_();

		HRESULT d3dCreateTexture_( UINT Width, UINT Height, UINT MipLevels,
			DWORD Usage, D3DFORMAT Format, D3DPOOL Pool, LPDIRECT3DTEXTURE8 * ppTexture );
		HRESULT loadSurfaceFromSurface_( LPDIRECT3DSURFACE8 pDestSurface, CONST RECT * pDestRect,
			LPDIRECT3DSURFACE8 pSrcSurface, CONST RECT * pSrcRect );

		bool m_inRender;

		typedef std::list<DX8RenderTexture*> TRenderTextureList;
		TRenderTextureList m_renderTextureList;
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
