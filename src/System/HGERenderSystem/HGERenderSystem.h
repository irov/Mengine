#	pragma once
#	include "Interface/RenderSystemInterface.h"

class HGE;

class HGERenderSystem :
	public RenderSystemInterface
{
public:
	HGERenderSystem();
	~HGERenderSystem();

	bool initialize( Menge::LogSystemInterface* _logSystem ) override;
	bool createRenderWindow( float _width, float _height, int _bits, bool _fullscreen, WINDOW_HANDLE _winHandle,
		int _FSAAType, int _FSAAQuality ) override;
	unsigned int getResolutionList( float ** ) override;
	void addResourceLocation( const char* _path ) override;
	void initResources() override;
	void render( RenderImageInterface* _image, const int* rect = 0 ) override;
	void render() override; 
	void setContentResolution( const float * _resolution ) override;
	void setProjectionMatrix( const float * _projection ) override;
	void setViewMatrix( const float * _view ) override;
	void setWorldMatrix( const float * _world ) override;
	RenderImageInterface * createImage( const char* _name, unsigned int _width, unsigned int _height ) override;
	RenderImageInterface * createRenderTargetImage( const char* _name, unsigned int _width, unsigned int _height, const char* _camera ) override;
	RenderImageInterface * loadImage( const TextureDesc& _desc ) override;
	void releaseImage( RenderImageInterface * _image ) override;
	RenderImageInterface * getImage( const char * _desc ) const override;
	RenderVideoStreamInterface* loadImageVideoStream( const char* _filename ) override;
	void releaseImageVideoStream( RenderVideoStreamInterface* _image ) override;
	void renderImage(		
		const char * _camera,
		const float * _transform, // матрица 3 на 3
		const float * _offset,	  // смещение, вектор2
		const float * _uv,		  // текстурные координаты, вектор4, u0, v0, u1, v1
		const float * _size,	  // размер изображения, вектор2
		unsigned int _color,	  // цвет, порядок RGBA
		const RenderImageInterface * _image,
		EBlendFactor _src,
		EBlendFactor _dst) override;

	void renderImage(		
		const char * _camera,
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

	void	renderLine(const char * _camera, unsigned int _color, const float * _begin, const float * _end) override;

	void	beginScene() override;
	void	endScene()	override;
	void	beginLayer() override;
	void	endLayer() override;

	void	setFullscreenMode( float _width, float _height, bool _fullscreen ) override;
	void	setViewportDimensions( float _width, float _height, float _renderFactor ) override;
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
	HGE* m_hge;
	float m_layer;
};