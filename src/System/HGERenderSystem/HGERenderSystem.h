#	pragma once
#	include "Interface/RenderSystemInterface.h"
#	include "libs/Math/vec2.h"
#	include "libs/Math/box2.h"

#	include "HGE.h"

#	include <map>

class HGERenderSystem :
	public RenderSystemInterface
{
public:
	HGERenderSystem();
	~HGERenderSystem();

	bool initialize( Menge::LogSystemInterface* _logSystem ) override;
	bool createRenderWindow( int _width, int _height, int _bits, bool _fullscreen, WINDOW_HANDLE _winHandle,
		int _FSAAType, int _FSAAQuality ) override;
	unsigned int getResolutionList( int ** ) override;
	void addResourceLocation( const char* _path ) override;
	void initResources() override;
	void screenshot( RenderImageInterface* _image, const int* rect = 0 ) override;
	void render() override; 
	void setContentResolution( const float * _resolution ) override;
	void setProjectionMatrix( const float * _projection ) override;
	void setViewMatrix( const float * _view ) override;
	void setWorldMatrix( const float * _world ) override;
	RenderImageInterface * createImage( const char* _name, unsigned int _width, unsigned int _height ) override;
	RenderImageInterface * createRenderTargetImage( const char* _name, unsigned int _width, unsigned int _height ) override;
	RenderImageInterface * loadImage( const TextureDesc& _desc ) override;
	void releaseImage( RenderImageInterface * _image ) override;
	RenderImageInterface* getImage( const Menge::String& _desc ) const override;
	RenderVideoStreamInterface* loadImageVideoStream( const char* _filename ) override;
	void releaseImageVideoStream( RenderVideoStreamInterface* _image ) override;

	void renderImage(		
		const float * _transform, 
		const float * _a,
		const float * _b,
		const float * _c,
		const float * _d,
		const float * _uv,
		unsigned int _color, 
		const RenderImageInterface * _image,
		EBlendFactor _src,
		EBlendFactor _dst) override;

	void renderMesh( const TVertex* _vertices, std::size_t _verticesNum,
					const Menge::uint16*	_indices, std::size_t _indicesNum,
					TMaterial* _material ) override;

	void	renderLine( unsigned int _color, const float * _begin, const float * _end ) override;

	void	beginScene() override;
	void	endScene()	override;
	void	beginLayer2D() override;
	void	endLayer2D() override;
	void	beginLayer3D() override;
	void	endLayer3D() override;

	void	setRenderArea( const float* _renderArea ) override;

	void	setFullscreenMode( float _width, float _height, bool _fullscreen ) override;
	void	setRenderTarget( const Menge::String& _name ) override;

	CameraInterface * createCamera( const char * _name ) override;
	EntityInterface * createEntity( const char * _name, const char * _meshName ) override;
	LightInterface * createLight( const char * _name ) override;
	SceneNodeInterface * createSceneNode( const std::string & _name ) override;

	void releaseCamera( CameraInterface * _camera ) override;
	void releaseEntity( EntityInterface * _entity ) override;
	void releaseLight( LightInterface * _light ) override;
	void releaseSceneNode( SceneNodeInterface * _interface ) override;

	void setTextureFiltering( bool _filter ) override;
	SceneNodeInterface * getRootSceneNode() const override;
	void setEventListener( RenderSystemListener* _listener ) override;
	void onWindowMovedOrResized() override;
	void onWindowActive( bool _active ) override;
	void onWindowClose() override;

private:
	Menge::LogSystemInterface* m_logSystem;
	mt::vec2f m_contentResolution;
	HGE* m_hge;
	float m_layer;
	mt::box2f m_viewport;
	bool m_inRender;
	Menge::String m_currentRenderTarget;

	typedef std::map< Menge::String, HTARGET > TTargetMap;
	TTargetMap m_targetMap;

	typedef std::map< Menge::String, RenderImageInterface* > TTextureMap;
	TTextureMap m_textureMap;

	bool m_layer3D;
	unsigned int m_clearColor;

	float m_renderX;
	float m_renderY;
};