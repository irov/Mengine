#	pragma once

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

class Camera3dInterface
{
public:
	virtual void setPosition( float x, float y, float z ) = 0;
	virtual void setDirection( float x, float y, float z ) = 0;
	virtual void lookAt( float x, float y, float z ) = 0;
	virtual void setNearClipDistance( float _dist ) = 0;
	virtual void setFarClipDistance( float _dist ) = 0;
	virtual void setAspectRatio( float _aspect ) = 0;
	// more to come...
};

class Entity3dInterface
{
public:
	virtual void setPosition(float x, float y, float z) = 0;
	virtual void setDirection(float w, float x, float y, float z) = 0;
	virtual void setDirection1(float * q) = 0;
};

class MeshInterface
{
	public:
		virtual bool hasSkeleton() const = 0;
		virtual float getBoundingSphereRadius() const = 0;
		virtual const char * getName() const = 0;
		virtual void getBoneOrientation( const char * _name, float & w, float & x, float & y, float & z ) const = 0;
		virtual void getBonePos( const char * _name, float & x, float & y, float & z ) const = 0;
};

class LightInterface
{
public:
	virtual ~LightInterface(){};
	virtual void setPosition( float _x, float _y, float _z ) = 0;
	virtual void setDiffuseColor( float _r, float _g, float _b ) = 0;
	virtual void setSpecularColour( float _r, float _g, float _b ) = 0;
	virtual void setAttenuation(float _range, float _constant, float _linear, float _quadratic) = 0;
	virtual void setDirection(float _x, float _y, float _z) = 0;
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
	virtual Camera3dInterface * createCamera( const char * _name ) = 0;
	virtual Entity3dInterface * create3dEntity(const char * _name, const char * _meshName) = 0;
	virtual LightInterface * createLight(const char * _name) = 0;
	virtual MeshInterface * createMesh(const char * _name) = 0;
};

bool initInterfaceSystem(RenderSystemInterface** _ptrRenderSystem);
void releaseInterfaceSystem(RenderSystemInterface* _ptrRenderSystem);