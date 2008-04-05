#	pragma once

#	include <string>

typedef void* WINDOW_HANDLE;

struct	TextureDesc
{
	const char * name;
	unsigned int filter;

	void * buffer;
	unsigned int size;
};

enum EBlendFactor 
{
	BF_ONE,
	BF_ZERO,
	BF_DEST_COLOUR,
	BF_SOURCE_COLOUR,
	BF_ONE_MINUS_DEST_COLOUR,
	BF_ONE_MINUS_SOURCE_COLOUR,
	BF_DEST_ALPHA,
	BF_SOURCE_ALPHA,
	BF_ONE_MINUS_DEST_ALPHA,
	BF_ONE_MINUS_SOURCE_ALPHA
};

class RenderImageInterface
{
public:
	virtual float getWidth() const = 0;
	virtual float getHeight() const = 0;
	virtual void writeToFile( const char* _filename ) = 0;
	virtual const char * getDescription() const = 0;
};

class EntityInterface;

class SkeletonInterface
{
public:
	virtual ~SkeletonInterface(){};
};

class EntityInterface
{
public:
	virtual SkeletonInterface * getSkeleton() const = 0;
	virtual void setCastsShadow( bool _castsShadow ) = 0;
	virtual void setVisible( bool _visible ) = 0;
	virtual void setMaterial( const std::string & _material ) = 0;
	virtual void setSubEntityMaterial( const std::string & _subEntity, const std::string & _material ) = 0;
	virtual void createRenderToTexture( const char* _cameraName, int _width, int _height  ) = 0;
	virtual void getAABB( float * _min, float * _max ) const = 0;
	virtual float getBoundingRadius() const = 0;

	//////////////////////////////////////////////////////////////////////////
	virtual void setAnimationEnabled( const char* _animName, bool _enabled ) = 0;
	virtual bool getAnimationEnabled( const char* _animName ) = 0;
	virtual void setAnimationTimePosition( const char* _animName, float _timePos ) = 0;
	virtual float getAnimationTimePosition( const char* _animName ) = 0;
	virtual void setAnimationLength( const char* _animName, float _length ) = 0;
	virtual float getAnimationLength( const char* _animName ) = 0;
	virtual void setAnimationWeight( const char* _animName, float _weight ) = 0;
	virtual float getAnimationWeigth( const char* _animName ) = 0;
	virtual void animationAddTime( const char* _animName, float _time ) = 0;
	virtual bool animationHasEnded( const char* _animName ) = 0;
	virtual void animationSetLooped( const char* _animName, bool _looped ) = 0;
	virtual bool animationGetLooped( const char* _animName ) = 0;
};

enum LightType
{
	LT_POINT,
	LT_DIRECTIONAL,
	LT_SPOT
};

class LightInterface
{
public:
	virtual ~LightInterface(){};

	virtual void setType( LightType _type ) = 0;
	virtual LightType getType() const = 0;

	virtual void setAttenuation( float _range, float _constant, float _linear, float _quadratic ) = 0;
	virtual float getAttenuationRange() const = 0;
	virtual float getAttenuationConstFactor() const = 0;
	virtual float getAttenuationLinearFactor() const = 0;
	virtual float getAttenuationQuadraticFactor() const = 0;

	virtual void setSpotlightRange( float _innerAngle, float _outerAngle, float _falloff ) = 0;
	virtual float getSpotlightInnerAngle() const = 0;
	virtual float getSpotlightOuterAngle() const = 0;
	virtual float getSpotlightFalloff() const = 0;

	virtual void setDiffuseColour( float _r, float _g, float _b ) = 0;
	virtual void setSpecularColour( float _r, float _g, float _b ) = 0;

	virtual bool isVisible() const = 0;
	virtual void setVisible( bool _enabled ) = 0;
	virtual void setCastsShadows( bool _enabled ) = 0;

	virtual void setDirection( float _x, float _y, float _z ) = 0;
	virtual void setPosition( float _x, float _y, float _z ) = 0;
};

enum EFrustumPlanes
{
	PLANE_NEAR   = 0,
	PLANE_FAR    = 1,
	PLANE_LEFT   = 2,
	PLANE_RIGHT  = 3,
	PLANE_TOP    = 4,
	PLANE_BOTTOM = 5
};

class CameraInterface
{
public:
	virtual void setOrient(float * _q) = 0;
	virtual void rotate(float * _dir, float _angle) = 0;
	virtual const float * getPosition() const = 0;
	virtual const float * getDirection() const = 0;
	virtual const float * getOrient() const = 0;
	virtual void setPosition( float _x, float _y, float _z ) = 0;
	virtual void setDirection( float _x, float _y, float _z ) = 0;
	virtual void lookAt( float _x, float _y, float _z ) = 0;
	virtual void setNearClipDistance( float _dist ) = 0;
	virtual void setFarClipDistance( float _dist ) = 0;
	virtual void setAspectRatio( float _aspect ) = 0;
	virtual void yaw( float _angle ) = 0;
	virtual void pitch( float _angle ) = 0;
	virtual void roll( float _angle ) = 0;
	virtual void getAABB( float * _min, float * _max ) const = 0;
	virtual bool getSphereFrustumContact(int _numPlane, float x, float y, float z, float R, float & depth, float & px, float & py, float & pz) = 0;
	virtual void translate( float * _v ) = 0;
};

class	SceneNodeInterface
{
public:
	virtual ~SceneNodeInterface(){};
	virtual const float * getWorldOrient() = 0;
	virtual const float * getWorldPosition() = 0;
	virtual float * getLocalPosition() = 0;
	virtual float * getLocalOrient() = 0;
	virtual void setLocalPosition( const float * _position ) = 0;
	virtual void setLocalOrient( const float * _orient ) = 0;
	virtual void translate( const float * _pos ) = 0;
	virtual void setScale( const float * _scale ) = 0;
	virtual void setScale( float _scale ) = 0;
	virtual void yaw( float _angle ) = 0;
	virtual void pitch( float _angle ) = 0;
	virtual void roll( float _angle ) = 0;
	virtual void attachEntity( EntityInterface * _entity ) = 0;
	virtual void attachLight( LightInterface * _light ) = 0;
	virtual void attachCamera( CameraInterface * _camera ) = 0;
	virtual void addChild( SceneNodeInterface * _node ) = 0;
	virtual SceneNodeInterface * createChildSceneNode( const char * _name ) = 0;
};

class RenderVideoStreamInterface : public RenderImageInterface
{
public:
	virtual void play() = 0;
	virtual void pause() = 0;
};
//////////////////////////////////////////////////////////////////////////
class RenderSystemListener
{
public:
	virtual void onDeviceRestored() = 0;
};
//////////////////////////////////////////////////////////////////////////
class	RenderSystemInterface
{
public:

	virtual bool initialize( const char* _driver ) = 0;
	virtual bool createRenderWindow( float _width, float _height, int _bits, bool _fullscreen, WINDOW_HANDLE _winHandle ) = 0;
	virtual unsigned int getResolutionList( float ** ) = 0;
	// Render frame into _image
	// int rect[4] - rectangle represents desired frame area in pixels
	virtual void render( RenderImageInterface* _image, const int* rect = 0 ) = 0;
	// Renders One Frame
	virtual void render() = 0; 
	// установка разрешения контента игры, входные данные: вектор2. 
	virtual void setContentResolution( const float * _resolution ) = 0;
	// входные данные: матрица 4 на 4
	virtual	void setProjectionMatrix( const float * _projection ) = 0;
	virtual	void setViewMatrix( const float * _view ) = 0;
	virtual	void setWorldMatrix( const float * _world ) = 0;
	// create empty render image
	virtual RenderImageInterface * createImage( const char* _name, unsigned int _width, unsigned int _height ) = 0;
	// create render target image
	virtual RenderImageInterface * createRenderTargetImage( const char* _name, unsigned int _width, unsigned int _height, const char* _camera ) = 0;
	// загрузка изображения
	virtual RenderImageInterface * loadImage( const TextureDesc& _desc ) = 0;
	// удаления изображения
	virtual void releaseImage( const char * _description ) = 0;
	//
	virtual RenderImageInterface* getImage( const char * _name ) const = 0;
	//
	virtual RenderVideoStreamInterface* loadImageVideoStream( const char* _filename ) = 0;
	//
	virtual void releaseImageVideoStream( RenderVideoStreamInterface* _image ) = 0;
	// отрисовка изображения
	virtual void renderImage(		
		const char * _camera,
		const float * _transform, // матрица 3 на 3
		const float * _offset,	  // смещение, вектор2
		const float * _uv,		  // текстурные координаты, вектор4, u0, v0, u1, v1
		const float * _size,	  // размер изображения, вектор2
		unsigned int _color,	  // цвет, порядок RGBA
		const RenderImageInterface * _image,
		EBlendFactor _src,
		EBlendFactor _dst) = 0;

	virtual void renderImage(		
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
		EBlendFactor _dst) = 0;

	virtual	void	beginLayer() = 0;
	virtual	void	endLayer() = 0;

	virtual void	setFullscreenMode( float _width, float _height, bool _fullscreen ) = 0;
	virtual void	setViewportDimensions( float _width, float _height, float _renderFactor ) = 0;

	//new
	virtual CameraInterface * createCamera( const char * _name ) = 0;
	virtual EntityInterface * createEntity( const char * _name, const char * _meshName ) = 0;
	virtual LightInterface * createLight( const char * _name ) = 0;
	virtual SceneNodeInterface * createSceneNode( const std::string & _name ) = 0;

	virtual void releaseCamera( CameraInterface * _camera ) = 0;
	virtual void releaseEntity( EntityInterface * _entity ) = 0;
	virtual void releaseLight( LightInterface * _light ) = 0;
	virtual void releaseSceneNode( SceneNodeInterface * _interface ) = 0;

	virtual SceneNodeInterface * getRootSceneNode() const = 0;
	virtual void setEventListener( RenderSystemListener* _listener ) = 0;
};

bool initInterfaceSystem(RenderSystemInterface** _ptrRenderSystem);
void releaseInterfaceSystem(RenderSystemInterface* _ptrRenderSystem);