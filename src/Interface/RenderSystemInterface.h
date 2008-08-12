#	pragma once

#	include "Config/Typedef.h"

namespace Menge
{
	class LogSystemInterface;

	struct	TextureDesc
	{
		String name;

		unsigned int filter;

		void * buffer;
		std::size_t size;

		float width;
		float height;

		int	pixelFormat;
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

	typedef struct _tVertex
	{
		float pos[3];
		float n[3];
		unsigned long col;
		float uv[2];

		_tVertex()
			: col( 0xFFFFFFFF )
		{
			uv[0] = uv[1] = 0.0f;
		}

		_tVertex( const _tVertex& _other )
		{
			//std::copy( (_tVertex*)&_other, &_other + sizeof( _tVertex ), &(*this) );
			operator=( _other );
		}

		const _tVertex& operator=( const _tVertex& _other )
		{
			pos[0] = _other.pos[0]; pos[1] = _other.pos[1]; pos[2] = _other.pos[2];
			n[0] = _other.n[0]; n[1] = _other.n[1]; n[2] = _other.n[2];
			col = _other.col;
			uv[0] = _other.uv[0]; uv[1] = _other.uv[1];
			return (*this);
		}

	} TVertex;

	// The pixel format used for images, textures, and render surfaces

	enum PixelFormat
	{
		/// Unknown pixel format.
		PF_UNKNOWN = 0,
		/// 8-bit pixel format, all bits luminace.
		PF_L8 = 1,
		PF_BYTE_L = PF_L8,
		/// 16-bit pixel format, all bits luminace.
		PF_L16 = 2,
		PF_SHORT_L = PF_L16,
		/// 8-bit pixel format, all bits alpha.
		PF_A8 = 3,
		PF_BYTE_A = PF_A8,
		/// 8-bit pixel format, 4 bits alpha, 4 bits luminace.
		PF_A4L4 = 4,
		/// 2 byte pixel format, 1 byte luminance, 1 byte alpha
		PF_BYTE_LA = 5,
		/// 16-bit pixel format, 5 bits red, 6 bits green, 5 bits blue.
		PF_R5G6B5 = 6,
		/// 16-bit pixel format, 5 bits red, 6 bits green, 5 bits blue.
		PF_B5G6R5 = 7,
		/// 8-bit pixel format, 2 bits blue, 3 bits green, 3 bits red.
		PF_R3G3B2 = 31,
		/// 16-bit pixel format, 4 bits for alpha, red, green and blue.
		PF_A4R4G4B4 = 8,
		/// 16-bit pixel format, 5 bits for blue, green, red and 1 for alpha.
		PF_A1R5G5B5 = 9,
		/// 24-bit pixel format, 8 bits for red, green and blue.
		PF_R8G8B8 = 10,
		/// 24-bit pixel format, 8 bits for blue, green and red.
		PF_B8G8R8 = 11,
		/// 32-bit pixel format, 8 bits for alpha, red, green and blue.
		PF_A8R8G8B8 = 12,
		/// 32-bit pixel format, 8 bits for blue, green, red and alpha.
		PF_A8B8G8R8 = 13,
		/// 32-bit pixel format, 8 bits for blue, green, red and alpha.
		PF_B8G8R8A8 = 14,
		/// 32-bit pixel format, 8 bits for red, green, blue and alpha.
		PF_R8G8B8A8 = 28,
		/// 32-bit pixel format, 8 bits for red, 8 bits for green, 8 bits for blue
		/// like PF_A8R8G8B8, but alpha will get discarded
		PF_X8R8G8B8 = 26,
		/// 32-bit pixel format, 8 bits for blue, 8 bits for green, 8 bits for red
		/// like PF_A8B8G8R8, but alpha will get discarded
		PF_X8B8G8R8 = 27,
#if MENGE_ENDIAN == MENGE_ENDIAN_BIG
		/// 3 byte pixel format, 1 byte for red, 1 byte for green, 1 byte for blue
		PF_BYTE_RGB = PF_R8G8B8,
		/// 3 byte pixel format, 1 byte for blue, 1 byte for green, 1 byte for red
		PF_BYTE_BGR = PF_B8G8R8,
		/// 4 byte pixel format, 1 byte for blue, 1 byte for green, 1 byte for red and one byte for alpha
		PF_BYTE_BGRA = PF_B8G8R8A8,
		/// 4 byte pixel format, 1 byte for red, 1 byte for green, 1 byte for blue, and one byte for alpha
		PF_BYTE_RGBA = PF_R8G8B8A8,
#else
		/// 3 byte pixel format, 1 byte for red, 1 byte for green, 1 byte for blue
		PF_BYTE_RGB = PF_B8G8R8,
		/// 3 byte pixel format, 1 byte for blue, 1 byte for green, 1 byte for red
		PF_BYTE_BGR = PF_R8G8B8,
		/// 4 byte pixel format, 1 byte for blue, 1 byte for green, 1 byte for red and one byte for alpha
		PF_BYTE_BGRA = PF_A8R8G8B8,
		/// 4 byte pixel format, 1 byte for red, 1 byte for green, 1 byte for blue, and one byte for alpha
		PF_BYTE_RGBA = PF_A8B8G8R8,
#endif        
		/// 32-bit pixel format, 2 bits for alpha, 10 bits for red, green and blue.
		PF_A2R10G10B10 = 15,
		/// 32-bit pixel format, 10 bits for blue, green and red, 2 bits for alpha.
		PF_A2B10G10R10 = 16,
		/// DDS (DirectDraw Surface) DXT1 format
		PF_DXT1 = 17,
		/// DDS (DirectDraw Surface) DXT2 format
		PF_DXT2 = 18,
		/// DDS (DirectDraw Surface) DXT3 format
		PF_DXT3 = 19,
		/// DDS (DirectDraw Surface) DXT4 format
		PF_DXT4 = 20,
		/// DDS (DirectDraw Surface) DXT5 format
		PF_DXT5 = 21,
		// 16-bit pixel format, 16 bits (float) for red
		PF_FLOAT16_R = 32,
		// 48-bit pixel format, 16 bits (float) for red, 16 bits (float) for green, 16 bits (float) for blue
		PF_FLOAT16_RGB = 22,
		// 64-bit pixel format, 16 bits (float) for red, 16 bits (float) for green, 16 bits (float) for blue, 16 bits (float) for alpha
		PF_FLOAT16_RGBA = 23,
		// 16-bit pixel format, 16 bits (float) for red
		PF_FLOAT32_R = 33,
		// 96-bit pixel format, 32 bits (float) for red, 32 bits (float) for green, 32 bits (float) for blue
		PF_FLOAT32_RGB = 24,
		// 128-bit pixel format, 32 bits (float) for red, 32 bits (float) for green, 32 bits (float) for blue, 32 bits (float) for alpha
		PF_FLOAT32_RGBA = 25,
		// 32-bit, 2-channel s10e5 floating point pixel format, 16-bit green, 16-bit red
		PF_FLOAT16_GR = 35,
		// 64-bit, 2-channel floating point pixel format, 32-bit green, 32-bit red
		PF_FLOAT32_GR = 36,
		// Depth texture format
		PF_DEPTH = 29,
		// 64-bit pixel format, 16 bits for red, green, blue and alpha
		PF_SHORT_RGBA = 30,
		// 32-bit pixel format, 16-bit green, 16-bit red
		PF_SHORT_GR = 34,
		// 48-bit pixel format, 16 bits for red, green and blue
		PF_SHORT_RGB = 37,
		// Number of pixel formats currently defined
		PF_COUNT = 38
	};

	class RenderImageInterface
	{
	public:
		virtual float getWidth() const = 0;
		virtual float getHeight() const = 0;
		virtual void writeToFile( const String & _filename ) = 0;
		virtual const String & getDescription() const = 0;
		virtual unsigned char* lock() = 0;
		virtual void unlock() = 0;
		virtual PixelFormat getPixelFormat() = 0;
	};

	typedef struct _tMaterial
	{
		RenderImageInterface* texture;
		unsigned int color;
		_tMaterial()
			: texture( 0 )
			, color( 0xFFFFFFFF )
		{
		}
	}TMaterial;

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
		virtual void setMaterial( const String & _material ) = 0;
		virtual void setSubEntityMaterial( const String & _subEntity, const String & _material ) = 0;
		virtual void createRenderToTexture( const String & _cameraName, int _width, int _height  ) = 0;
		virtual void getAABB( float * _min, float * _max ) const = 0;
		virtual float getBoundingRadius() const = 0;

		//////////////////////////////////////////////////////////////////////////
		virtual void setAnimationEnabled( const String & _animName, bool _enabled ) = 0;
		virtual bool getAnimationEnabled( const String & _animName ) = 0;
		virtual void setAnimationTimePosition( const String & _animName, float _timePos ) = 0;
		virtual float getAnimationTimePosition( const String & _animName ) = 0;
		virtual void setAnimationLength( const String & _animName, float _length ) = 0;
		virtual float getAnimationLength( const String & _animName ) = 0;
		virtual void setAnimationWeight( const String & _animName, float _weight ) = 0;
		virtual float getAnimationWeigth( const String & _animName ) = 0;
		virtual void animationAddTime( const String & _animName, float _time ) = 0;
		virtual bool animationHasEnded( const String & _animName ) = 0;
		virtual void animationSetLooped( const String & _animName, bool _looped ) = 0;
		virtual bool animationGetLooped( const String & _animName ) = 0;
		//////////////////////////////////////////////////////////////////////////
		virtual void attachEntity( const String & _bone, EntityInterface* _entity ) = 0;
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
		virtual const float * getLocalOrient() = 0;
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
		virtual void setFixedYawAxis( bool _fixed ) = 0;
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

		virtual bool initialize( LogSystemInterface* _logSystem ) = 0;
		virtual bool createRenderWindow( int _width, int _height, int _bits, bool _fullscreen, WindowHandle _winHandle,
			int _FSAAType, int _FSAAQuality ) = 0;
		virtual const std::vector<int> & getResolutionList() = 0;

		// Render frame into _image
		// int rect[4] - rectangle represents desired frame area in pixels
		virtual void screenshot( RenderImageInterface* _image, const float * _rect ) = 0;
		// Renders One Frame
		virtual void render() = 0; 
		// установка разрешения контента игры, входные данные: вектор2. 
		virtual void setContentResolution( const float * _resolution ) = 0;
		// входные данные: матрица 4 на 4
		virtual	void setProjectionMatrix( const float * _projection ) = 0;
		virtual	void setViewMatrix( const float * _view ) = 0;
		virtual	void setWorldMatrix( const float * _world ) = 0;
		// create empty render image
		virtual RenderImageInterface * createImage( const String & _name, float _width, float _height ) = 0;
		// create render target image
		virtual RenderImageInterface * createRenderTargetImage( const String & _name, float _width, float _height ) = 0;
		// загрузка изображения
		virtual RenderImageInterface * loadImage( const TextureDesc& _desc ) = 0;
		// удаления изображения
		virtual void releaseImage( RenderImageInterface * _image ) = 0;
		//
		virtual RenderImageInterface * getImage( const String& _desc ) const = 0;
		//
		virtual RenderVideoStreamInterface* loadImageVideoStream( const String & _filename ) = 0;
		//
		virtual void releaseImageVideoStream( RenderVideoStreamInterface* _image ) = 0;
		// отрисовка изображения

		virtual void renderImage(		
			const float * _renderVertex,
			const float * _uv,
			unsigned int _color, 
			const RenderImageInterface * _image,
			EBlendFactor _src,
			EBlendFactor _dst) = 0;

		virtual void renderTriple(const float * _transform,  
			const float * _a, 
			const float * _b, 
			const float * _c, 
			const float * _uv0, 
			const float * _uv1,
			const float * _uv2,
			unsigned int _color,  
			const RenderImageInterface * _image, 
			EBlendFactor _src, 
			EBlendFactor _dst ) = 0;

		virtual void	renderMesh( const TVertex* _vertices, std::size_t _verticesNum,
			const uint16* _indices, std::size_t _indicesNum,
			TMaterial* _material ) = 0;

		virtual void	renderLine( unsigned int _color, const float * _begin, const float * _end) = 0;

		virtual void	beginScene() = 0;
		virtual void	endScene() = 0;
		virtual	void	beginLayer2D() = 0;
		virtual	void	endLayer2D() = 0;
		virtual	void	beginLayer3D() = 0;
		virtual	void	endLayer3D() = 0;

		virtual void	setRenderArea( const float* _renderArea ) = 0;

		virtual void	setFullscreenMode( float _width, float _height, bool _fullscreen ) = 0;
		virtual void	setRenderTarget( const String& _name, bool _clear ) = 0;

		//new
		virtual CameraInterface * createCamera( const String & _name ) = 0;
		virtual EntityInterface * createEntity( const String & _name, const String & _meshName ) = 0;
		virtual LightInterface * createLight( const String & _name ) = 0;
		virtual SceneNodeInterface * createSceneNode( const String & _name ) = 0;

		virtual void releaseCamera( CameraInterface * _camera ) = 0;
		virtual void releaseEntity( EntityInterface * _entity ) = 0;
		virtual void releaseLight( LightInterface * _light ) = 0;
		virtual void releaseSceneNode( SceneNodeInterface * _interface ) = 0;

		virtual void setTextureFiltering( bool _filter ) = 0;
		virtual SceneNodeInterface * getRootSceneNode() const = 0;
		virtual void setEventListener( RenderSystemListener* _listener ) = 0;
		virtual void onWindowMovedOrResized() = 0;
		virtual void onWindowActive( bool _active ) = 0;
		virtual void onWindowClose() = 0;
		virtual int  getNumDIP() const = 0;
	};
}

bool initInterfaceSystem(Menge::RenderSystemInterface** _ptrRenderSystem);
void releaseInterfaceSystem(Menge::RenderSystemInterface* _ptrRenderSystem);