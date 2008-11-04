#	pragma once

#	include "Config/Platform.h"

#	include "Interface/RenderSystemInterface.h"
#	include "libs/Math/vec2.h"
#	include "libs/Math/box2.h"

#	include <map>

#if MENGE_PLATFORM_MACOSX
#	include <AGL/AGL.h>
#else if MENGE_PLATFORM_WIN32
#	define GLEW_STATIC
#	include "OpenGL/glew.h"
#	define WIN32_LEAN_AND_MEAN
#	include <Windows.h>
#	include "OpenGL/GL.h"
#	define GL_GLEXT_PROTOTYPES
#	include "OpenGL/glext.h"
#endif

class OGLTexture;

class FrameBufferObject;

class OGLRenderSystem 
	: public Menge::RenderSystemInterface
{
public:
	OGLRenderSystem();
	~OGLRenderSystem();

	bool initialize( Menge::LogSystemInterface* _logSystem ) override;
	bool createRenderWindow( std::size_t _width, std::size_t _height, int _bits, bool _fullscreen, Menge::WindowHandle _winHandle,
		int _FSAAType, int _FSAAQuality ) override;
	const std::vector<int> & getResolutionList() override;
	void screenshot( Menge::RenderImageInterface* _image, const float * rect = 0 ) override;
	void render() override; 
	void setContentResolution( const std::size_t * _resolution ) override;
	void setProjectionMatrix( const float * _projection ) override;
	void setViewMatrix( const float * _view ) override;
	void setWorldMatrix( const float * _world ) override;
	Menge::RenderImageInterface * createImage( const Menge::String & _name, float _width, float _height ) override;
	Menge::RenderImageInterface * createRenderTargetImage( const Menge::String & _name, float _width, float _height ) override;

	Menge::RenderImageInterface * loadImage( const Menge::String& _name, std::size_t _width, std::size_t _height, const Menge::TextureDesc& _desc ) override;

	bool supportNPOT() override;

	void releaseImage( Menge::RenderImageInterface * _image ) override;
	Menge::RenderImageInterface* getImage( const Menge::String& _desc ) const override;

	void renderImage(		
		const float * _renderVertex,
		const float * _uv,
		unsigned int _color, 
		const Menge::RenderImageInterface * _image,
		Menge::EBlendFactor _src,
		Menge::EBlendFactor _dst) override;

	void renderTriple(  
		const float * _a, 
		const float * _b, 
		const float * _c, 
		const float * _uv0, 
		const float * _uv1,
		const float * _uv2, 
		unsigned int _color,  
		const Menge::RenderImageInterface * _image, 
		Menge::EBlendFactor _src, 
		Menge::EBlendFactor _dst ) override;

	void renderMesh( const Menge::TVertex* _vertices, std::size_t _verticesNum,
		const Menge::uint16 * _indices, std::size_t _indicesNum,
		Menge::TMaterial* _material ) override;

	void	renderLine( unsigned int _color, const float * _begin, const float * _end ) override;

	void	beginScene() override;
	void	endScene()	override;
	void	beginLayer2D() override;
	void	endLayer2D() override;
	void	beginLayer3D() override;
	void	endLayer3D() override;

	void	swapBuffers() override;

	void	setRenderArea( const float* _renderArea ) override;

	void	setFullscreenMode( std::size_t _width, std::size_t _height, bool _fullscreen ) override;
	void	setRenderTarget( const Menge::String& _name, bool _clear ) override;

	Menge::CameraInterface * createCamera( const Menge::String & _name ) override;
	Menge::EntityInterface * createEntity( const Menge::String & _name, const Menge::String & _meshName ) override;
	Menge::LightInterface * createLight( const Menge::String & _name ) override;

	void releaseCamera( Menge::CameraInterface * _camera ) override;
	void releaseEntity( Menge::EntityInterface * _entity ) override;
	void releaseLight( Menge::LightInterface * _light ) override;

	void setTextureFiltering( bool _filter ) override;
	void onWindowMovedOrResized() override;
	void onWindowClose() override;

	int  getNumDIP() const override;

	void renderText(const Menge::String & _text, const float * _pos, unsigned long _color) override;

private:
	Menge::LogSystemInterface* m_logSystem;

#if MENGE_PLATFORM_WIN32
	HDC m_hdc;
	HGLRC m_glrc;
#elif MENGE_PLATFORM_MACOSX
	AGLContext m_aglContext;
	WindowRef m_windowRef;
#endif

	GLint m_textureType;
	GLint _getTextureType();

	float m_layer;
	Menge::String m_currentRenderTarget;

	struct RenderTargetInfo
	{
		FrameBufferObject * handle;
		OGLTexture* texture;
		bool dirty;
	};

	typedef std::map<Menge::String, RenderTargetInfo> TTargetMap;
	TTargetMap m_targetMap;


	typedef std::map< Menge::String, Menge::RenderImageInterface* > TTextureMap;
	TTextureMap m_textureMap;
	//SDL_Surface * m_screen;
	//SDL_Rect** m_videoModes;
	
	GLfloat m_projMatrix[16];
	GLfloat m_worldMatrix[16];
	GLfloat m_viewMatrix[16];

	static const int VertexBufferSize = 10000;
	static const int IndexBufferSize = VertexBufferSize * 6 / 4;

	Menge::TVertex * VertArray;
	Menge::uint16 * IndexArray;

	int n_prim;
	GLint m_curTexture;
	int m_curPrimType;
	int m_currSrcBlend;
	int m_currDstBlend;

	void _glEnable2D();
	void _glEnable3D();

	void initBatching();
	void deleteBatching();
	void Gfx_RenderQuad(const Menge::TVertex *quad, GLint tex, GLint srcBlend, GLint dstBlend);
	void Gfx_RenderTriple(const Menge::TVertex *quad,GLint tex, GLint srcBlend, GLint dstBlend);
	void renderBatch();

	bool m_layer3D;
	std::size_t m_windowWidth;
	std::size_t m_windowHeight;

	std::size_t m_contentResolution[2];

	GLint m_viewport[4];
	bool m_supportNPOT;
	Menge::String m_ext;
};