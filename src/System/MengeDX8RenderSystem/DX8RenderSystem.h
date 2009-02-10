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
	class DX8SystemFont;

	struct TQuad
	{
		TVertex v[4];
		IDirect3DTexture8* tex;

		EBlendFactor srcBlend;
		EBlendFactor dstBlend;
	};

	struct TTriple
	{
		TVertex v[3];
		IDirect3DTexture8* tex;

		EBlendFactor srcBlend;
		EBlendFactor dstBlend;
	};

	class DX8RenderSystem
		: public RenderSystemInterface
	{
	public:
		DX8RenderSystem();
		~DX8RenderSystem();

		void clear( DWORD _color );
		bool supportNPOT() const;
		void setTextureMatrix( const float* _texture );
		void onRestoreDevice();
		void render_quad_( TQuad* _quad );

	public:
		bool initialize( LogSystemInterface* _logSystem ) override;
		bool createRenderWindow( std::size_t _width, std::size_t _height, int _bits, bool _fullscreen, WindowHandle _winHandle,
			int _FSAAType, int _FSAAQuality ) override;
		const std::vector<int> & getResolutionList() override;

		// Render frame into _image
		// int rect[4] - rectangle represents desired frame area in pixels
		void screenshot( RenderImageInterface* _image, const float * _rect ) override;
		// Renders One Frame
		void render() override; 
		// установка разрешения контента игры, входные данные: вектор2. 
		void setContentResolution( const std::size_t * _resolution ) override;
		// входные данные: матрица 4 на 4
		void setProjectionMatrix( const float * _projection ) override;
		void setViewMatrix( const float * _view ) override;
		void setWorldMatrix( const float * _world ) override;
		// create empty render image
		RenderImageInterface * createImage( const String & _name, std::size_t _width, std::size_t _height, PixelFormat _format ) override;
		// create render target image
		RenderImageInterface * createRenderTargetImage( const String & _name, std::size_t _width, std::size_t _height ) override;
		// загрузка изображения
		RenderImageInterface * loadImage( const String& _name, ImageDecoderInterface* _imageDecoder ) override;
		// удаления изображения
		void releaseImage( RenderImageInterface * _image ) override;
		//
		RenderImageInterface * getImage( const String& _desc ) const override;
		//
		// отрисовка изображения
		//void drawIndexedPrimitive( EPrimitiveType _type, const TVertex* _vertices, std::size_t _verticesNum,
		//	const uint16* _indicies, std::size_t _inidiciesNum ) override;

		void renderImage(		
			const float * _renderVertex,
			const float * _uv,
			unsigned int _color, 
			const RenderImageInterface * _image,
			EBlendFactor _src,
			EBlendFactor _dst) override;

		void renderTriple(  
			const float * _a, 
			const float * _b, 
			const float * _c, 
			const float * _uv0, 
			const float * _uv1,
			const float * _uv2,
			unsigned int _color,  
			const RenderImageInterface * _image, 
			EBlendFactor _src, 
			EBlendFactor _dst ) override;

		void	renderMesh( const TVertex* _vertices, std::size_t _verticesNum,
			const uint16* _indices, std::size_t _indicesNum,
			TMaterial* _material ) override;

		void	renderLine( unsigned int _color, const float * _begin, const float * _end) override;

		void	beginScene() override;
		void	endScene() override;
		void	swapBuffers() override;
		void	beginLayer2D() override;
		void	endLayer2D() override;
		void	beginLayer3D() override;
		void	endLayer3D() override;

		void	setRenderArea( const float* _renderArea ) override;

		void	setFullscreenMode( std::size_t _width, std::size_t _height, bool _fullscreen ) override;
		void	setRenderTarget( const String& _name, bool _clear ) override;

		CameraInterface * createCamera( const String & _name ) override;
		EntityInterface * createEntity( const String & _name, const String & _meshName ) override;
		LightInterface * createLight( const String & _name ) override;

		void releaseCamera( CameraInterface * _camera ) override;
		void releaseEntity( EntityInterface * _entity ) override;
		void releaseLight( LightInterface * _light ) override;

		void setTextureFiltering( bool _filter ) override;
		void onWindowMovedOrResized() override;
		void onWindowClose() override;

		int  getNumDIP() const override;
		void renderText(const String & _text, const float * _pos, unsigned long _color) override;

	private:
		// Log
		LogSystemInterface* m_logSystem;
		void log( const char* _message, ... );
		void log_error( const char* _message, ... );

		std::size_t m_contentResolution[2];
		std::size_t m_screenResolution[2];
		int m_screenBits;
		bool m_fullscreen;

		IDirect3D8*				m_pD3D;
		IDirect3DDevice8*		m_pD3DDevice;
		IDirect3DVertexBuffer8*	pVB;
		IDirect3DIndexBuffer8*	pIB;
		IDirect3DVertexBuffer8*	pVB3D;
		IDirect3DIndexBuffer8*	pIB3D;
		IDirect3DSurface8*	pScreenSurf;
		IDirect3DSurface8*	pScreenDepth;

		D3DPRESENT_PARAMETERS*  d3dpp;
		D3DPRESENT_PARAMETERS   d3dppW;
		D3DPRESENT_PARAMETERS   d3dppFS;

		D3DMATRIX m_matView;
		D3DMATRIX m_matProj;
		D3DMATRIX m_matWorld;
		D3DMATRIX m_matTexture;

		// sync routines
		IDirect3DSurface8*	m_syncTargets[2];
		IDirect3DTexture8*	m_syncTempTex;
		IDirect3DSurface8*	m_syncTemp;
		unsigned int		m_frames;
		void syncCPU_();

		bool m_supportNPOT;

		int format_id_( D3DFORMAT _format );
		void matIdent_( D3DMATRIX* _mtx );
		void matMul_( D3DMATRIX* _out, D3DMATRIX* _mtxl, D3DMATRIX* _mtxr );
		void matOrthoOffCenterLH_( D3DMATRIX* _out,
			float l, float r, float b, float t,	float zn, float zf );

		void setProjectionMatrix_( std::size_t _width, std::size_t _height );
		bool init_lost_();
		void render_batch_( bool _endScene );
		void render_triple_( TTriple* _triple );
		bool begin_scene_( DX8RenderTexture* _target = NULL );
		void set_clipping_( int _x = 0, int _y = 0, int _w = 0, int _h = 0 );
		bool gfx_restore_();
		void gfx_done_();
		HRESULT d3dCreateTexture_( UINT Width, UINT Height, UINT MipLevels,
			DWORD Usage, D3DFORMAT Format, D3DPOOL Pool, LPDIRECT3DTEXTURE8 * ppTexture );
		HRESULT loadSurfaceFromSurface_( LPDIRECT3DSURFACE8 pDestSurface, CONST RECT * pDestRect,
			LPDIRECT3DSURFACE8 pSrcSurface, CONST RECT * pSrcRect );
		void setBlendState_( EBlendFactor _srcBlend, EBlendFactor _dstBlend );

		void prepare2D_();
		void prepare3D_();

		bool m_layer3D;
		bool m_inRender;
		float m_layer;
		int	nPrim;
		int	NumDips;
		unsigned int m_clearColor;
		int	CurPrimType;
		String m_currentRenderTarget;
		EBlendFactor m_currSrcBlend;
		EBlendFactor m_currDstBlend;
		bool m_texFilter;

		typedef std::map<String, DX8Texture*> TTextureMap;
		TTextureMap m_textureMap;
		IDirect3DTexture8* m_curTexture;

		typedef std::map<String, DX8RenderTexture*> TRenderTextureMap;
		TRenderTextureMap m_renderTextureMap;
		DX8RenderTexture* m_curRenderTexture;

		TVertex* VertArray;

		DX8SystemFont* m_systemFont;

		EPrimitiveType m_currentPrimitiveType;
	};
}	// namespace Menge
