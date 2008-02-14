#	pragma once

#	include <string>

struct	TextureDesc
{
	const char *	name;
	size_t			filter;

	void*			buffer;
	size_t			size;
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
	virtual size_t getWidth() const = 0;
	virtual size_t getHeight() const = 0;
	virtual void writeToFile( const char* _filename ) = 0;
};

class EntityInterface;

class SkeletonInterface
{
public:
	virtual ~SkeletonInterface(){};

	virtual void enableAnimation( const std::string & _anim, bool _enable ) = 0;
	virtual void advanceAnimation( const std::string & _anim, float _timeDelta ) = 0;
	virtual void advanceAllAnimations( float _timeDelta ) = 0;
	virtual void setAnimationWeight( const std::string & _anim, float _weight ) = 0;
	virtual void attachEntityToBone( const std::string & _bone, EntityInterface * _entity ) = 0;
};

class EntityInterface
{
public:
/*	virtual bool hasSkeleton() const = 0;
	virtual const float * getBoneOrientation( const char * _name ) = 0;
	virtual const float * getBonePosition( const char * _name ) = 0;
	virtual void play( const char * _name ) = 0;
	virtual void play( 
		const char * _name1, float _weight1, 
		const char * _name2, float _weight2 ) = 0;
	virtual void stop() = 0;
	virtual void setLooped( bool _looped ) = 0;
	virtual const char * getName() const = 0;
	virtual void update( float _timing ) = 0;
*/
	virtual SkeletonInterface * getSkeleton() const = 0;
	virtual void setCastsShadow( bool _castsShadow ) = 0;
	virtual void setVisible( bool _visible ) = 0;
	virtual void setMaterial( const std::string & _material ) = 0;
	virtual void setSubEntityMaterial( const std::string & _subEntity, const std::string & _material ) = 0;
};

class LightInterface
{
public:
	enum LightType
	{
		LT_POINT,
		LT_DIRECTIONAL,
		LT_SPOT
	};

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

	virtual bool isEnabled() const = 0;
	virtual void setEnabled( bool _enabled ) = 0;
	virtual void setCastsShadows( bool _enabled ) = 0;

	virtual void setDirection( float _x, float _y, float _z ) = 0;
};

class CameraInterface
{
public:
	virtual void setPosition( float _x, float _y, float _z ) = 0;
	virtual void setDirection( float _x, float _y, float _z ) = 0;
	virtual void lookAt( float _x, float _y, float _z ) = 0;
	virtual void setNearClipDistance( float _dist ) = 0;
	virtual void setFarClipDistance( float _dist ) = 0;
	virtual void setAspectRatio( float _aspect ) = 0;
	virtual void yaw( float _angle ) = 0;
	virtual void pitch( float _angle ) = 0;
	virtual void roll( float _angle ) = 0;
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
	virtual void addChild( SceneNodeInterface * _node ) = 0;
};

class RenderVideoStreamInterface : public RenderImageInterface
{
public:
	virtual void play() = 0;
	virtual void pause() = 0;
};

class	RenderSystemInterface
{
public:

	// Render frame into _image
	// int rect[4] - rectangle represents desired frame area in pixels
	virtual void render( RenderImageInterface* _image, const int* rect = 0 ) = 0;
	// Renders One Frame
	virtual void render() = 0; 
	// ��������� ���������� �������� ����, ������� ������: ������2. 
	virtual void setContentResolution( const float * _resolution ) = 0;
	// ������� ������: ������� 4 �� 4
	virtual	void setProjectionMatrix( const float * _projection ) = 0;
	virtual	void setViewMatrix( const float * _view ) = 0;
	virtual	void setWorldMatrix( const float * _world ) = 0;
	// create empty render image
	virtual RenderImageInterface * createImage( const char* _name, unsigned int _width, unsigned int _height ) = 0;
	// create render target image
	virtual RenderImageInterface * createRenderTargetImage( const char* _name, unsigned int _width, unsigned int _height ) = 0;
	// �������� �����������
	virtual RenderImageInterface * loadImage( const TextureDesc& _desc ) = 0;
	// �������� �����������
	virtual void releaseImage( RenderImageInterface * _image ) = 0;
	//
	virtual RenderVideoStreamInterface* loadImageVideoStream( const char* _filename ) = 0;
	//
	virtual void releaseImageVideoStream( RenderVideoStreamInterface* _image ) = 0;
	// ��������� �����������
	virtual void renderImage(		
		const float * _transform, // ������� 3 �� 3
		const float * _offset,	  // ��������, ������2
		const float * _uv,		  // ���������� ����������, ������4, u0, v0, u1, v1
		const float * _size,	  // ������ �����������, ������2
		unsigned int _color,	  // ����, ������� RGBA
		const RenderImageInterface * _image,
		EBlendFactor _src,
		EBlendFactor _dst) = 0;

	virtual void renderImage(		
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

	virtual void	setFullscreenMode( bool _fullscreen ) = 0;
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
};

bool initInterfaceSystem(RenderSystemInterface** _ptrRenderSystem);
void releaseInterfaceSystem(RenderSystemInterface* _ptrRenderSystem);