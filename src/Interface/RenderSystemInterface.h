#	pragma once

#	include <map>

#	include "Config/Typedef.h"

//#	include "PixelFormat.h"
namespace Menge
{
	class LogSystemInterface;
}

typedef void* WINDOW_HANDLE;

#define MENGE_MAX_TEXTURE_LAYERS 16

struct	TextureDesc
{
	const char * name;
	unsigned int filter;

	void * buffer;
	unsigned int size;
	std::size_t width;
	std::size_t height;
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
	//////////////////////////////////////////////////////////////////////////
	virtual void attachEntity( const char* _bone, EntityInterface* _entity ) = 0;
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

	virtual bool initialize( Menge::LogSystemInterface* _logSystem ) = 0;
	virtual bool createRenderWindow( float _width, float _height, int _bits, bool _fullscreen, WINDOW_HANDLE _winHandle,
		int _FSAAType, int _FSAAQuality ) = 0;
	virtual unsigned int getResolutionList( float ** ) = 0;

	virtual void addResourceLocation( const char* _path ) = 0;
	virtual void initResources() = 0;

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
	virtual void releaseImage( RenderImageInterface * _image ) = 0;
	//
	virtual RenderImageInterface * getImage( const char * _desc ) const = 0;
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

	virtual void	renderLine(const char * _camera, unsigned int _color, const float * _begin, const float * _end) = 0;

	virtual void	beginScene() = 0;
	virtual void	endScene() = 0;
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

	virtual void setTextureFiltering( bool _filter ) = 0;
	virtual SceneNodeInterface * getRootSceneNode() const = 0;
	virtual void setEventListener( RenderSystemListener* _listener ) = 0;
	virtual void onWindowMovedOrResized() = 0;
	virtual void onWindowActive( bool _active ) = 0;
	virtual void onWindowClose() = 0;
};

namespace Menge
{
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
}
/*namespace Menge
{
	class Resource;

	class RenderWindow;
	class RenderTexture;
	class RenderTarget;
	class Texture;
	class TextureUnitState
	{
	public:
		class UVWAddressingMode;
	};
	class Frustum;
	class LayerBlendModeEx;
	class Viewport;
	class VertexDeclaration;
	class VertexBufferBinding;
	class RenderSystemCapabilities;
	class GpuProgram;
	class RenderOperation;
	class Camera;
	class Light;
	typedef std::vector<Light*> TLightList;

	typedef struct _ConfigOption
	{
		String name;
		String currentValue;
		TStringVector possibleValues;
		bool immutable;
	} TConfigOption;

	// Light shading modes.
	enum ShadeOptions
	{
		SO_FLAT,
		SO_GOURAUD,
		SO_PHONG
	};

	enum TrackVertexColourEnum 
	{
		TVC_NONE        = 0x0,
		TVC_AMBIENT     = 0x1,        
		TVC_DIFFUSE     = 0x2,
		TVC_SPECULAR    = 0x4,
		TVC_EMISSIVE    = 0x8
	};

	// Enum identifying the texture type
	enum TextureType
	{
		/// 1D texture, used in combination with 1D texture coordinates
		TEX_TYPE_1D = 1,
		/// 2D texture, used in combination with 2D texture coordinates (default)
		TEX_TYPE_2D = 2,
		/// 3D volume texture, used in combination with 3D texture coordinates
		TEX_TYPE_3D = 3,
		/// 3D cube map, used in combination with 3D texture coordinates
		TEX_TYPE_CUBE_MAP = 4
	};

	/// Enum describing the ways to generate texture coordinates
	enum TexCoordCalcMethod
	{
		/// No calculated texture coordinates
		TEXCALC_NONE,
		/// Environment map based on vertex normals
		TEXCALC_ENVIRONMENT_MAP,
		/// Environment map based on vertex positions
		TEXCALC_ENVIRONMENT_MAP_PLANAR,
		TEXCALC_ENVIRONMENT_MAP_REFLECTION,
		TEXCALC_ENVIRONMENT_MAP_NORMAL,
		/// Projective texture
		TEXCALC_PROJECTIVE_TEXTURE
	};

	enum FilterType
	{
		/// The filter used when shrinking a texture
		FT_MIN,
		/// The filter used when magnifiying a texture
		FT_MAG,
		/// The filter used when determining the mipmap
		FT_MIP
	};

	// Filtering options for textures / mipmaps.
	enum FilterOptions
	{
		/// No filtering, used for FILT_MIP to turn off mipmapping
		FO_NONE,
		/// Use the closest pixel
		FO_POINT,
		/// Average of a 2x2 pixel area, denotes bilinear for MIN and MAG, trilinear for MIP
		FO_LINEAR,
		/// Similar to FO_LINEAR, but compensates for the angle of the texture plane
		FO_ANISOTROPIC
	};

	// Comparison functions used for the depth/stencil buffer operations and others.
	enum CompareFunction
	{
		CMPF_ALWAYS_FAIL,
		CMPF_ALWAYS_PASS,
		CMPF_LESS,
		CMPF_LESS_EQUAL,
		CMPF_EQUAL,
		CMPF_NOT_EQUAL,
		CMPF_GREATER_EQUAL,
		CMPF_GREATER
	};

	//Hardware culling modes based on vertex winding.
	//This setting applies to how the hardware API culls triangles it is sent.
	enum CullingMode
	{
		/// Hardware never culls triangles and renders everything it receives.
		CULL_NONE = 1,
		/// Hardware culls triangles whose vertices are listed clockwise in the view (default).
		CULL_CLOCKWISE = 2,
		/// Hardware culls triangles whose vertices are listed anticlockwise in the view.
		CULL_ANTICLOCKWISE = 3
	};

	// Fog modes.
	enum FogMode
	{
		/// No fog. Duh.
		FOG_NONE,
		/// Fog density increases  exponentially from the camera (fog = 1/e^(distance * density))
		FOG_EXP,
		/// Fog density increases at the square of FOG_EXP, i.e. even quicker (fog = 1/e^(distance * density)^2)
		FOG_EXP2,
		/// Fog density increases linearly between the start and end distances
		FOG_LINEAR
	};

	/// Vertex element type, used to identify the base types of the vertex contents
	enum VertexElementType
	{
		VET_FLOAT1 = 0,
		VET_FLOAT2 = 1,
		VET_FLOAT3 = 2,
		VET_FLOAT4 = 3,
		/// alias to more specific colour type - use the current rendersystem's colour packing
		VET_COLOUR = 4,
		VET_SHORT1 = 5,
		VET_SHORT2 = 6,
		VET_SHORT3 = 7,
		VET_SHORT4 = 8,
		VET_UBYTE4 = 9,
		/// D3D style compact colour
		VET_COLOUR_ARGB = 10,
		/// GL style compact colour
		VET_COLOUR_ABGR = 11
	};

	// The polygon mode to use when rasterising.
	enum PolygonMode
	{
		/// Only points are rendered.
		PM_POINTS = 1,
		/// Wireframe models are rendered.
		PM_WIREFRAME = 2,
		/// Solid polygons are rendered.
		PM_SOLID = 3
	};

	/// Enum describing the various actions which can be taken onthe stencil buffer
	enum StencilOperation
	{
		/// Leave the stencil buffer unchanged
		SOP_KEEP,
		/// Set the stencil value to zero
		SOP_ZERO,
		/// Set the stencil value to the reference value
		SOP_REPLACE,
		/// Increase the stencil value by 1, clamping at the maximum value
		SOP_INCREMENT,
		/// Decrease the stencil value by 1, clamping at 0
		SOP_DECREMENT,
		/// Increase the stencil value by 1, wrapping back to 0 when incrementing the maximum value
		SOP_INCREMENT_WRAP,
		/// Decrease the stencil value by 1, wrapping when decrementing 0
		SOP_DECREMENT_WRAP,
		/// Invert the bits of the stencil buffer
		SOP_INVERT
	};

	// Enumerates the types of programs which can run on the GPU.
	enum GpuProgramType
	{
		GPT_VERTEX_PROGRAM,
		GPT_FRAGMENT_PROGRAM
	};


	typedef std::map< String, TConfigOption > TConfigOptionMap;

	/// Name / value parameter pair (first = name, second = value)
	typedef std::map<String, String> NameValuePairList;

	typedef std::map< String, RenderTarget* > TRenderTargetMap;
	typedef std::multimap< unsigned char, RenderTarget* > TRenderTargetPriorityMap;

	class RenderSystemInterface
	{
	public:

		// 
		
		// Returns the name of the rendering system.
		virtual const String& getName() const = 0;

		//Returns the details of this API's configuration options
		//@remarks
		//Each render system must be able to inform the world
		//of what options must/can be specified for it's
		//operation.
		//@par
		//These are passed as strings for portability, but
		//grouped into a structure (_ConfigOption) which includes
		//both options and current value.
		//@par
		//Note that the settings returned from this call are
		//affected by the options that have been set so far,
		//since some options are interdependent.
		//@par
		//This routine is called automatically by the default
		//configuration dialogue produced by Root::showConfigDialog
		//or may be used by the caller for custom settings dialogs
		//@returns
		//A 'map' of options, i.e. a list of options which is also
		//indexed by option name.
		//virtual TConfigOptionMap& getConfigOptions() = 0;

		//Sets an option for this API
		//@remarks
		//Used to confirm the settings (normally chosen by the user) in
		//order to make the renderer able to initialise with the settings as required.
		//This may be video mode, D3D driver, full screen / windowed etc.
		//Called automatically by the default configuration
		//dialog, and by the restoration of saved settings.
		//These settings are stored and only activated when
		//RenderSystem::initialise or RenderSystem::reinitialise
		//are called.
		//@par
		//If using a custom configuration dialog, it is advised that the
		//caller calls RenderSystem::getConfigOptions
		//again, since some options can alter resulting from a selection.
		//@param
		//name The name of the option to alter.
		//@param
		//value The value to set the option to.
		virtual void setConfigOption( const String& _name, const String& _value ) = 0;

		// Create an object for performing hardware occlusion queries. 
		//virtual HardwareOcclusionQuery* createHardwareOcclusionQuery() = 0;

		// Destroy a hardware occlusion query object. 
		//virtual void destroyHardwareOcclusionQuery( HardwareOcclusionQuery* _hq ) = 0;

		//Validates the options set for the rendering system, returning a message if there are problems.
		//@note
		//If the returned string is empty, there are no problems.
		//virtual String validateConfigOptions() = 0;

		//Start up the renderer using the settings selected (Or the defaults if none have been selected).
		//@remarks
		//Called by Root::setRenderSystem. Shouldn't really be called
		//directly, although  this can be done if the app wants to.
		//@param
		//autoCreateWindow If true, creates a render window
		//automatically, based on settings chosen so far. This saves
		//an extra call to RenderSystem::createRenderWindow
		//for the main render window.
		//@par
		//If an application has more specific window requirements,
		//however (e.g. a level design app), it should specify false
		//for this parameter and do it manually.
		//@returns
		//A pointer to the automatically created window, if requested, otherwise null.
		virtual void initialise( LogSystemInterface* _logInterface ) = 0;

		// Restart the renderer (normally following a change in settings).
		//virtual void reinitialise() = 0;

		// Shutdown the renderer and cleanup resources.
		virtual void shutdown() = 0;

		// Sets the colour & strength of the ambient (global directionless) light in the world.
		//virtual void setAmbientLight( float _r, float _g, float _b ) = 0;

		// Sets the type of light shading required (default = Gouraud).
		//virtual void setShadingType( ShadeOptions _so ) = 0;

		//Sets whether or not dynamic lighting is enabled.
		//@param
		//enabled If true, dynamic lighting is performed on geometry with normals supplied, geometry without
		//normals will not be displayed. If false, no lighting is applied and all geometry will be full brightness.
		//virtual void setLightingEnabled( bool _enabled ) = 0;

		//Sets whether or not W-buffers are enabled if they are avalible for this renderer.
		//@param
		//enabled If true and the renderer supports them W-buffers will be used.  If false 
		//W-buffers will not be used even if avalible.  W-buffers are enabled by default 
		//for 16bit depth buffers and disabled for all other depths.
		//void setWBufferEnabled( bool _enabled );

		// Returns true if the renderer will try to use W-buffers when avalible.
		//bool getWBufferEnabled() const;

		//Creates a new rendering window.
		//@remarks
		//This method creates a new rendering window as specified
		//by the paramteters. The rendering system could be
		//responible for only a single window (e.g. in the case
		//of a game), or could be in charge of multiple ones (in the
		//case of a level editor). The option to create the window
		//as a child of another is therefore given.
		//This method will create an appropriate subclass of
		//RenderWindow depending on the API and platform implementation.
		//@par
		//After creation, this window can be retrieved using getRenderTarget().
		//@param
		//name The name of the window. Used in other methods
		//later like setRenderTarget and getRenderWindow.
		//@param
		//width The width of the new window.
		//@param
		//height The height of the new window.
		//@param
		//fullScreen Specify true to make the window full screen
		//without borders, title bar or menu bar.
		//@param
		//miscParams A NameValuePairList describing the other parameters for the new rendering window. 
		//Options are case sensitive. Unrecognised parameters will be ignored silently.
		//These values might be platform dependent, but these are present for all platorms unless
		//indicated otherwise:

		//Key: "title"
		//Description: The title of the window that will appear in the title bar
		//Values: string
		//Default: RenderTarget name
		//
		//Key: "colourDepth"
		//Description: Colour depth of the resulting rendering window; only applies if fullScreen
		//is set.
		//Values: 16 or 32
		//Default: desktop depth
		//Notes: [W32 specific]
		//
		//Key: "left"
		//Description: screen x coordinate from left
		//Values: positive integers
		//Default: 'center window on screen'
		//Notes: Ignored in case of full screen
		//
		//Key: "top"
		//Description: screen y coordinate from top
		//Values: positive integers
		//Default: 'center window on screen'
		//Notes: Ignored in case of full screen
		//
		//Key: "depthBuffer" [DX9 specific]
		//Description: Use depth buffer
		//Values: false or true
		//Default: true
		//
		//Key: "externalWindowHandle" [API specific]
		//Description: External window handle, for embedding the OGRE context
		//Values: positive integer for W32 (HWND handle)
		//poslong:posint:poslong (display*:screen:windowHandle) or
		//poslong:posint:poslong:poslong (display*:screen:windowHandle:XVisualInfo*) for GLX
		//Default: 0 (None)
		//
		//Key: "externalGLControl" [Win32 OpenGL specific]
		//Description: Let the external window control OpenGL i.e. don't select a pixel format for the window,
		//do not change v-sync and do not swap buffer. When set to true, the calling application
		//is responsible of OpenGL initialization and buffer swapping. It should also create an
		//OpenGL context for its own rendering, Ogre will create one for its use. Then the calling
		//application must also enable Ogre OpenGL context before calling any Ogre function and
		//restore its OpenGL context after these calls. The Ogre OpenGL context can be retrieved 
		//after Ogre initialisation by calling wglGetCurrentDC() and wglGetCurrentContext().
		//It is only used when the externalWindowHandle parameter is used.
		//Values: true, false
		//Default: false
		//
		//Key: "externalGLContext" [Win32 OpenGL specific]
		//Description: Use an externally created GL context
		//Values: <Context as Unsigned Long>
		//Default: 0 (create own context)
		//
		//Key: "parentWindowHandle" [API specific]
		//Description: Parent window handle, for embedding the OGRE context
		//Values: positive integer for W32 (HWND handle)
		//poslong:posint:poslong for GLX (display*:screen:windowHandle)
		//Default: 0 (None)
		//
		//Key: "FSAA"
		//Description: Full screen antialiasing factor
		//Values: 0,2,4,6,...
		//Default: 0 
		//
		//Key: "displayFrequency"
		//Description: Display frequency rate, for fullscreen mode
		//Values: 60...?
		//Default: Desktop vsync rate
		//
		//Key: "vsync"
		//Description: Synchronize buffer swaps to vsync
		//Values: true, false
		//Default: 0
		// 
		//Key: "border" 
		//Description: The type of window border (in windowed mode)
		//Values: none, fixed, resize
		//Default: resize
		//
		//Key: "outerDimensions" 
		//Description: Whether the width/height is expressed as the size of the 
		//outer window, rather than the content area
		//Values: true, false
		//Default: false 
		//
		//Key: "useNVPerfHUD" [DX9 specific]
		//Description: Enable the use of nVidia NVPerfHUD
		//Values: true, false
		//Default: false
		virtual bool createRenderWindow( const String& _name, unsigned int _width, unsigned int _height, 
			bool _fullScreen, const NameValuePairList* _miscParams = 0) = 0;

		//Creates and registers a render texture object.
		//@param name 
		//The name for the new render texture. Note that names must be unique.
		//@param width
		//The requested width for the render texture. See Remarks for more info.
		//@param height
		//The requested width for the render texture. See Remarks for more info.
		//@param texType
		//The type of texture; defaults to TEX_TYPE_2D
		//@param internalFormat
		//The internal format of the texture; defaults to PF_X8R8G8B8
		//@param miscParams This parameter is ignored.
		//@returns
		//On succes, a pointer to a new platform-dependernt, RenderTexture-derived
		//class is returned. On failiure, NULL is returned.
		//@remarks
		//Because a render texture is basically a wrapper around a texture object,
		//the width and height parameters of this method just hint the preferred
		//size for the texture. Depending on the hardware driver or the underlying
		//API, these values might change when the texture is created. The same applies
		//to the internalFormat parameter.
		//@deprecated
		//This method is deprecated, and exists only for backward compatibility. You can create
		//arbitrary rendertextures with the TextureManager::createManual call with usage
		//TU_RENDERTARGET.
		//virtual RenderTexture* createRenderTexture( const String& _name, unsigned int _width, unsigned int _height,
		//	TextureType _texType = TEX_TYPE_2D, PixelFormat _internalFormat = PF_X8R8G8B8, 
		//	const NameValuePairList* _miscParams = 0 ) = 0; 

		//Create a MultiRenderTarget, which is a render target that renders to multiple RenderTextures
		//at once. Surfaces can be bound and unbound at will.
		//This fails if mCapabilities->numMultiRenderTargets() is smaller than 2.
		//virtual MultiRenderTarget * createMultiRenderTarget( const String& _name ) = 0; 

		// Destroys a render window
		//virtual void destroyRenderWindow( const String& _name ) = 0;
		// Destroys a render texture
		//virtual void destroyRenderTexture( const String& _name ) = 0;
		// Destroys a render target of any sort
		//virtual void destroyRenderTarget( const String& _name ) = 0;

		// Attaches the passed render target to the render system.
		//virtual void attachRenderTarget( RenderTarget& _target ) = 0;

		//Returns a pointer to the render target with the passed name, or NULL if that
		//render target cannot be found.
		//virtual RenderTarget * getRenderTarget( const String& _name ) = 0;

		//Detaches the render target with the passed name from the render system and
		//returns a pointer to it.
		//@note
		//If the render target cannot be found, NULL is returned.
		//virtual RenderTarget * detachRenderTarget( const String& _name ) = 0;

		/// Iterator over RenderTargets
		//typedef MapIterator<TRenderTargetMap> TRenderTargetIterator;

		// Returns a specialised MapIterator over all render targets attached to the RenderSystem.
		//virtual RenderTargetIterator getRenderTargetIterator() {
		//	return RenderTargetIterator( mRenderTargets.begin(), mRenderTargets.end() );
		//}

		// Returns a description of an error code.
		//virtual String getErrorDescription( long _errorNumber ) const = 0;

		//Defines whether or now fullscreen render windows wait for the vertical blank before flipping buffers.
		//@remarks
		//By default, all rendering windows wait for a vertical blank (when the CRT beam turns off briefly to move
		//from the bottom right of the screen back to the top left) before flipping the screen buffers. This ensures
		//that the image you see on the screen is steady. However it restricts the frame rate to the refresh rate of
		//the monitor, and can slow the frame rate down. You can speed this up by not waiting for the blank, but
		//this has the downside of introducing 'tearing' artefacts where part of the previous frame is still displayed
		//as the buffers are switched. Speed vs quality, you choose.
		//@note
		//Has NO effect on windowed mode render targets. Only affects fullscreen mode.
		//@param
		//enabled If true, the system waits for vertical blanks - quality over speed. If false it doesn't - speed over quality.
		//virtual void setWaitForVerticalBlank( bool _enabled ) = 0;

		// Returns true if the system is synchronising frames with the monitor vertical blank.
		//virtual bool getWaitForVerticalBlank() const = 0;

		// ------------------------------------------------------------------------
		//                     Internal Rendering Access
		// ------------------------------------------------------------------------

		//Tells the rendersystem to use the attached set of lights (and no others) 
		//up to the number specified (this allows the same list to be used with different
		//count limits)
		//virtual void _useLights( const TLightList& _lights, unsigned short _limit ) = 0;
		// Sets the world transform matrix.
		//virtual void _setWorldMatrix( const float* _m ) = 0;
		// Sets multiple world matrices (vertex blending).
		//virtual void _setWorldMatrices( const Matrix4* _m, unsigned short _count );
		// Sets the view transform matrix 
		//virtual void _setViewMatrix( const float* _m ) = 0;
		// Sets the projection transform matrix
		//virtual void _setProjectionMatrix( const float* _m ) = 0;
		//Utility function for setting all the properties of a texture unit at once.
		//This method is also worth using over the individual texture unit settings because it
		//only sets those settings which are different from the current settings for this
		//unit, thus minimising render state changes.
		//virtual void _setTextureUnitSettings( std::size_t _texUnit, TextureUnitState& _tl ) = 0;
		// Turns off a texture unit.
		//virtual void _disableTextureUnit( std::size_t _texUnit ) = 0;
		// Disables all texture units from the given unit upwards
		//virtual void _disableTextureUnitsFrom( std::size_t _texUnit ) = 0;
		// Sets the surface properties to be used for future rendering.

		//This method sets the the properties of the surfaces of objects
		//to be rendered after it. In this context these surface properties
		//are the amount of each type of light the object reflects (determining
		//it's colour under different types of light), whether it emits light
		//itself, and how shiny it is. Textures are not dealt with here,
		//see the _setTetxure method for details.
		//This method is used by _setMaterial so does not need to be called
		//direct if that method is being used.

		//@param ambient The amount of ambient (sourceless and directionless)
		//light an object reflects. Affected by the colour/amount of ambient light in the scene.
		//@param diffuse The amount of light from directed sources that is
		//reflected (affected by colour/amount of point, directed and spot light sources)
		//@param specular The amount of specular light reflected. This is also
		//affected by directed light sources but represents the colour at the
		//highlights of the object.
		//@param emissive The colour of light emitted from the object. Note that
		//this will make an object seem brighter and not dependent on lights in
		//the scene, but it will not act as a light, so will not illuminate other
		//objects. Use a light attached to the same SceneNode as the object for this purpose.
		//@param shininess A value which only has an effect on specular highlights (so
		//specular must be non-black). The higher this value, the smaller and crisper the
		//specular highlights will be, imitating a more highly polished surface.
		//This value is not constrained to 0.0-1.0, in fact it is likely to
		//be more (10.0 gives a modest sheen to an object).
		//@param tracking A bit field that describes which of the ambient, diffuse, specular
		//and emissive colours follow the vertex colour of the primitive. When a bit in this field is set
		//its ColourValue is ignored. This is a combination of TVC_AMBIENT, TVC_DIFFUSE, TVC_SPECULAR(note that the shininess value is still
		//taken from shininess) and TVC_EMISSIVE. TVC_NONE means that there will be no material property
		//tracking the vertex colours.
		//virtual void _setSurfaceParams( const float* _ambient, const float* _diffuse,
		//	const float* _specular, const float* _emissive, float _shininess,
		//	int _tracking = TVC_NONE ) = 0;

		//Sets whether or not rendering points using OT_POINT_LIST will 
		//render point sprites (textured quads) or plain points.
		//@param enabled True enables point sprites, false returns to normal
		//point rendering.
		//virtual void _setPointSpritesEnabled( bool _enabled ) = 0;

		//Sets the size of points and how they are attenuated with distance.
		//@remarks
		//When performing point rendering or point sprite rendering,
		//point size can be attenuated with distance. The equation for
		//doing this is attenuation = 1 / (constant + linear * dist + quadratic * d^2) .
		//@par
		//For example, to disable distance attenuation (constant screensize) 
		//you would set constant to 1, and linear and quadratic to 0. A
		//standard perspective attenuation would be 0, 1, 0 respectively.
		//virtual void _setPointParameters( float _size, bool _attenuationEnabled, float _constant,
		//	float _linear, float _quadratic, float _minSize, float _maxSize ) = 0;

		//Sets the texture to bind to a given texture unit.

		//User processes would not normally call this direct unless rendering
		//primitives themselves.

		//@param unit The index of the texture unit to modify. Multitexturing 
		//hardware can support multiple units (see 
		//RenderSystemCapabilites::getNumTextureUnits)
		//@param enabled Boolean to turn the unit on/off
		//@param texPtr Pointer to the texture to use.
		//virtual void _setTexture( std::size_t _unit, bool _enabled, const Texture* _texPtr ) = 0;

		//Sets the texture to bind to a given texture unit.

		//User processes would not normally call this direct unless rendering
		//primitives themselves.

		//@param unit The index of the texture unit to modify. Multitexturing 
		//hardware can support multiple units (see 
		//RenderSystemCapabilites::getNumTextureUnits)
		//@param enabled Boolean to turn the unit on/off
		//@param texname The name of the texture to use - this should have
		//already been loaded with TextureManager::load.
		//virtual void _setTexture( std::size_t _unit, bool _enabled, const String& _texname ) = 0;

		//Binds a texture to a vertex sampler.
		//@remarks
		//Not all rendersystems support separate vertex samplers. For those that
		//do, you can set a texture for them, separate to the regular texture
		//samplers, using this method. For those that don't, you should use the
		//regular texture samplers which are shared between the vertex and
		//fragment units; calling this method will throw an exception.
		//@see RenderSystemCapabilites::getVertexTextureUnitsShared
		//virtual void _setVertexTexture( std::size_t _unit, const Texture* _tex ) = 0;

		//Sets the texture coordinate set to use for a texture unit.

		//Meant for use internally - not generally used directly by apps - the Material and TextureUnitState
		//classes let you manage textures far more easily.

		//@param unit Texture unit as above
		//@param index The index of the texture coordinate set to use.
		//virtual void _setTextureCoordSet( std::size_t _unit, std::size_t _index ) = 0;

		//Sets a method for automatically calculating texture coordinates for a stage.
		//Should not be used by apps - for use by Ogre only.
		//@param unit Texture unit as above
		//@param m Calculation method to use
		//@param frustum Optional Frustum param, only used for projective effects
		//virtual void _setTextureCoordCalculation( std::size_t _unit, TexCoordCalcMethod _m, 
		//	const Frustum* _frustum = 0 ) = 0;

		//Sets the texture blend modes from a TextureUnitState record.
		//Meant for use internally only - apps should use the Material
		//and TextureUnitState classes.
		//@param unit Texture unit as above
		//@param bm Details of the blending mode
		//virtual void _setTextureBlendMode( std::size_t _unit, const LayerBlendModeEx& _bm ) = 0;

		//Sets the filtering options for a given texture unit.
		//@param unit The texture unit to set the filtering options for
		//@param minFilter The filter used when a texture is reduced in size
		//@param magFilter The filter used when a texture is magnified
		//@param mipFilter The filter used between mipmap levels, FO_NONE disables mipmapping
		//virtual void _setTextureUnitFiltering( std::size_t _unit, FilterOptions _minFilter,
		//	FilterOptions _magFilter, FilterOptions _mipFilter ) = 0;

		//Sets a single filter for a given texture unit.
		//@param unit The texture unit to set the filtering options for
		//@param ftype The filter type
		//@param filter The filter to be used
		//virtual void _setTextureUnitFiltering( std::size_t _unit, FilterType _ftype, FilterOptions _filter ) = 0;

		// Sets the maximal anisotropy for the specified texture unit.
		//virtual void _setTextureLayerAnisotropy( std::size_t _unit, unsigned int _maxAnisotropy ) = 0;

		// Sets the texture addressing mode for a texture unit.
		//virtual void _setTextureAddressingMode( std::size_t _unit, const TextureUnitState::UVWAddressingMode& _uvw ) = 0;

		// Sets the texture border colour for a texture unit.
		//virtual void _setTextureBorderColour( std::size_t _unit, const float* _colour ) = 0;

		//Sets the mipmap bias value for a given texture unit.
		//@remarks
		//This allows you to adjust the mipmap calculation up or down for a
		//given texture unit. Negative values force a larger mipmap to be used, 
		//positive values force a smaller mipmap to be used. Units are in numbers
		//of levels, so +1 forces the mipmaps to one smaller level.
		//@note Only does something if render system has capability RSC_MIPMAP_LOD_BIAS.
		//virtual void _setTextureMipmapBias( std::size_t _unit, float _bias ) = 0;

		//Sets the texture coordinate transformation matrix for a texture unit.
		//@param unit Texture unit to affect
		//@param xform The 4x4 matrix
		//virtual void _setTextureMatrix( std::size_t unit, const float* _xform ) = 0;

		//Sets the global blending factors for combining subsequent renders with the existing frame contents.
		//The result of the blending operation is:</p>
		//<p align="center">final = (texture * sourceFactor) + (pixel * destFactor)</p>
		//Each of the factors is specified as one of a number of options, as specified in the SceneBlendFactor
		//enumerated type.
		//@param sourceFactor The source factor in the above calculation, i.e. multiplied by the texture colour components.
		//@param destFactor The destination factor in the above calculation, i.e. multiplied by the pixel colour components.
		//virtual void _setSceneBlending( EBlendFactor _sourceFactor, EBlendFactor _destFactor ) = 0;

		//Sets the global alpha rejection approach for future renders.
		//By default images are rendered regardless of texture alpha. This method lets you change that.
		//@param func The comparison function which must pass for a pixel to be written.
		//@param val The value to compare each pixels alpha value to (0-255)
		//virtual void _setAlphaRejectSettings( CompareFunction _func, unsigned char _value ) = 0;

		//Signifies the beginning of a frame, ie the start of rendering on a single viewport. Will occur
		//several times per complete frame if multiple viewports exist.
		//virtual void _beginFrame() = 0;

		// Ends rendering of a frame to the current viewport.
		//virtual void _endFrame() = 0;

		//Sets the provided viewport as the active one for future
		//rendering operations. This viewport is aware of it's own
		//camera and render target. Must be implemented by subclass.

		//@param target Pointer to the appropriate viewport.
		//virtual void _setViewport( Viewport* _vp ) = 0;

		// Get the current active viewport for rendering.
		//virtual Viewport* _getViewport() = 0;

		//Sets the culling mode for the render system based on the 'vertex winding'.
		//A typical way for the rendering engine to cull triangles is based on the
		//'vertex winding' of triangles. Vertex winding refers to the direction in
		//which the vertices are passed or indexed to in the rendering operation as viewed
		//from the camera, and will wither be clockwise or anticlockwise (that's 'counterclockwise' for
		//you Americans out there ;) The default is CULL_CLOCKWISE i.e. that only triangles whose vertices
		//are passed/indexed in anticlockwise order are rendered - this is a common approach and is used in 3D studio models
		//for example. You can alter this culling mode if you wish but it is not advised unless you know what you are doing.
		//You may wish to use the CULL_NONE option for mesh data that you cull yourself where the vertex
		//winding is uncertain.
		//virtual void _setCullingMode( CullingMode _mode ) = 0;

		//virtual CullingMode _getCullingMode() const = 0;

		//Sets the mode of operation for depth buffer tests from this point onwards.
		//Sometimes you may wish to alter the behaviour of the depth buffer to achieve
		//special effects. Because it's unlikely that you'll set these options for an entire frame,
		//but rather use them to tweak settings between rendering objects, this is an internal
		//method (indicated by the '_' prefix) which will be used by a SceneManager implementation
		//rather than directly from the client application.
		//If this method is never called the settings are automatically the same as the default parameters.
		//@param depthTest If true, the depth buffer is tested for each pixel and the frame buffer is only updated
		//if the depth function test succeeds. If false, no test is performed and pixels are always written.
		//@param depthWrite If true, the depth buffer is updated with the depth of the new pixel if the depth test succeeds.
		//If false, the depth buffer is left unchanged even if a new pixel is written.
		//@param depthFunction Sets the function required for the depth test.
		//virtual void _setDepthBufferParams( bool _depthTest = true, bool _depthWrite = true, CompareFunction _depthFunction = CMPF_LESS_EQUAL) = 0;

		//Sets whether or not the depth buffer check is performed before a pixel write.
		//@param enabled If true, the depth buffer is tested for each pixel and the frame buffer is only updated
		//if the depth function test succeeds. If false, no test is performed and pixels are always written.
		//virtual void _setDepthBufferCheckEnabled( bool _enabled = true ) = 0;

		//Sets whether or not the depth buffer is updated after a pixel write.
		//@param enabled If true, the depth buffer is updated with the depth of the new pixel if the depth test succeeds.
		//If false, the depth buffer is left unchanged even if a new pixel is written.
		//virtual void _setDepthBufferWriteEnabled( bool _enabled = true ) = 0;

		//Sets the comparison function for the depth buffer check.
		//Advanced use only - allows you to choose the function applied to compare the depth values of
		//new and existing pixels in the depth buffer. Only an issue if the deoth buffer check is enabled
		//(see _setDepthBufferCheckEnabled)
		//@param  func The comparison between the new depth and the existing depth which must return true
		//for the new pixel to be written.
		//virtual void _setDepthBufferFunction( CompareFunction _func = CMPF_LESS_EQUAL) = 0;

		//Sets whether or not colour buffer writing is enabled, and for which channels. 
		//@remarks
		//For some advanced effects, you may wish to turn off the writing of certain colour
		//channels, or even all of the colour channels so that only the depth buffer is updated
		//in a rendering pass. However, the chances are that you really want to use this option
		//through the Material class.
		//@param red, green, blue, alpha Whether writing is enabled for each of the 4 colour channels.
		//virtual void _setColourBufferWriteEnabled( bool _red, bool _green, bool _blue, bool _alpha ) = 0;
		
		//Sets the depth bias, NB you should use the Material version of this. 
		//@remarks
		//When polygons are coplanar, you can get problems with 'depth fighting' where
		//the pixels from the two polys compete for the same screen pixel. This is particularly
		//a problem for decals (polys attached to another surface to represent details such as
		//bulletholes etc.).
		//@par
		//A way to combat this problem is to use a depth bias to adjust the depth buffer value
		//used for the decal such that it is slightly higher than the true value, ensuring that
		//the decal appears on top.
		//@note
		//The final bias value is a combination of a constant bias and a bias proportional
		//to the maximum depth slope of the polygon being rendered. The final bias
		//is constantBias + slopeScaleBias * maxslope. Slope scale biasing is
		//generally preferable but is not available on older hardware.
		//@param constantBias The constant bias value, expressed as a value in 
		//homogenous depth coordinates.
		//@param slopeScaleBias The bias value which is factored by the maximum slope
		//of the polygon, see the description above. This is not supported by all
		//cards.
		//virtual void _setDepthBias( float _constantBias, float _slopeScaleBias = 0.0f ) = 0;
		
		//Sets the fogging mode for future geometry.
		//@param mode Set up the mode of fog as described in the FogMode enum, or set to FOG_NONE to turn off.
		//@param colour The colour of the fog. Either set this to the same as your viewport background colour,
		//or to blend in with a skydome or skybox.
		//@param expDensity The density of the fog in FOG_EXP or FOG_EXP2 mode, as a value between 0 and 1. The default is 1. i.e. completely opaque, lower values can mean
		//that fog never completely obscures the scene.
		//@param linearStart Distance at which linear fog starts to encroach. The distance must be passed
		//as a parametric value between 0 and 1, with 0 being the near clipping plane, and 1 being the far clipping plane. Only applicable if mode is FOG_LINEAR.
		//@param linearEnd Distance at which linear fog becomes completely opaque.The distance must be passed
		//as a parametric value between 0 and 1, with 0 being the near clipping plane, and 1 being the far clipping plane. Only applicable if mode is FOG_LINEAR.
		//virtual void _setFog( FogMode _mode, const float* _colour, float _expDensity = 1.0f, float _linearStart = 0.0f, float _linearEnd = 1.0f ) = 0;

		// The RenderSystem will keep a count of tris rendered, this resets the count.
		//virtual void _beginGeometryCount();
		// Reports the number of tris rendered since the last _beginGeometryCount call.
		//virtual unsigned int _getFaceCount() const;
		// Reports the number of batches rendered since the last _beginGeometryCount call.
		//virtual unsigned int _getBatchCount() const;
		// Reports the number of vertices passed to the renderer since the last _beginGeometryCount call.
		//virtual unsigned int _getVertexCount() const;

		//Generates a packed data version of the passed in ColourValue suitable for
		//use as with this RenderSystem.
		//@remarks
		//Since different render systems have different colour data formats (eg
		//RGBA for GL, ARGB for D3D) this method allows you to use 1 method for all.
		//@param colour The colour to convert
		//@param pDest Pointer to location to put the result.
		//virtual void convertColourValue( const float* _colour, uint32* _pDest );

		//Get the native VertexElementType for a compact 32-bit colour value
		//for this rendersystem.
		//virtual VertexElementType getColourVertexElementType() const = 0;

		//Converts a uniform projection matrix to suitable for this render system.
		//@remarks
		//Because different APIs have different requirements (some incompatible) for the
		//projection matrix, this method allows each to implement their own correctly and pass
		//back a generic OGRE matrix for storage in the engine.
		//virtual void _convertProjectionMatrix( const float* _matrix, float* _dest, bool _forGpuProgram = false ) = 0;

		//Builds a perspective projection matrix suitable for this render system.
		//@remarks
		//Because different APIs have different requirements (some incompatible) for the
		//projection matrix, this method allows each to implement their own correctly and pass
		//back a generic OGRE matrix for storage in the engine.
		//virtual void _makeProjectionMatrix( const float _fovy, float _aspect, float _nearPlane, float _farPlane, 
		//	float* _dest, bool _forGpuProgram = false ) = 0;

		//Builds a perspective projection matrix for the case when frustum is
		//not centered around camera.
		//@remarks
		//Viewport coordinates are in camera coordinate frame, i.e. camera is 
		//at the origin.
		//virtual void _makeProjectionMatrix( float _left, float _right, float _bottom, float _top, 
		//	float _nearPlane, float _farPlane, float* _dest, bool _forGpuProgram = false) = 0;

		//Builds an orthographic projection matrix suitable for this render system.
		//@remarks
		//Because different APIs have different requirements (some incompatible) for the
		//projection matrix, this method allows each to implement their own correctly and pass
		//back a generic OGRE matrix for storage in the engine.
		//virtual void _makeOrthoMatrix( const float _fovy, float _aspect, float _nearPlane, float _farPlane, 
		//	float* _dest, bool _forGpuProgram = false ) = 0;

		//Update a perspective projection matrix to use 'oblique depth projection'.
		//@remarks
		//This method can be used to change the nature of a perspective 
		//transform in order to make the near plane not perpendicular to the 
		//camera view direction, but to be at some different orientation. 
		//This can be useful for performing arbitrary clipping (e.g. to a 
		//reflection plane) which could otherwise only be done using user
		//clip planes, which are more expensive, and not necessarily supported
		//on all cards.
		//@param matrix The existing projection matrix. Note that this must be a
		//perspective transform (not orthographic), and must not have already
		//been altered by this method. The matrix will be altered in-place.
		//@param plane The plane which is to be used as the clipping plane. This
		//plane must be in CAMERA (view) space.
		//@param forGpuProgram Is this for use with a Gpu program or fixed-function
		//virtual void _applyObliqueDepthProjection( float* _matrix, const float* _plane, bool _forGpuProgram ) = 0;

		// Sets how to rasterise triangles, as points, wireframe or solid polys.
		//virtual void _setPolygonMode( PolygonMode _level ) = 0;

		//Turns stencil buffer checking on or off. 
		//@remarks
		//Stencilling (masking off areas of the rendering target based on the stencil 
		//buffer) canbe turned on or off using this method. By default, stencilling is
		//disabled.
		//virtual void setStencilCheckEnabled( bool _enabled ) = 0;

		//Determines if this system supports hardware accelerated stencil buffer. 
		//@remarks
		//Note that the lack of this function doesn't mean you can't do stencilling, but
		//the stencilling operations will be provided in software, which will NOT be
		//fast.
		//@par
		//Generally hardware stencils are only supported in 32-bit colour modes, because
		//the stencil buffer shares the memory of the z-buffer, and in most cards the 
		//z-buffer has to be the same depth as the colour buffer. This means that in 32-bit
		//mode, 24 bits of the z-buffer are depth and 8 bits are stencil. In 16-bit mode there
		//is no room for a stencil (although some cards support a 15:1 depth:stencil option,
		//this isn't useful for very much) so 8 bits of stencil are provided in software.
		//This can mean that if you use stencilling, your applications may be faster in 
		//32-but colour than in 16-bit, which may seem odd to some people.

		//virtual bool hasHardwareStencil(void) = 0;

		//This method allows you to set all the stencil buffer parameters in one call.
		//@remarks
		//The stencil buffer is used to mask out pixels in the render target, allowing
		//you to do effects like mirrors, cut-outs, stencil shadows and more. Each of
		//your batches of rendering is likely to ignore the stencil buffer, 
		//update it with new values, or apply it to mask the output of the render.
		//The stencil test is:<PRE>
		//(Reference Value & Mask) CompareFunction (Stencil Buffer Value & Mask)</PRE>
		//The result of this will cause one of 3 actions depending on whether the test fails,
		//succeeds but with the depth buffer check still failing, or succeeds with the
		//depth buffer check passing too.
		//@par
		//Unlike other render states, stencilling is left for the application to turn
		//on and off when it requires. This is because you are likely to want to change
		//parameters between batches of arbitrary objects and control the ordering yourself.
		//In order to batch things this way, you'll want to use OGRE's separate render queue
		//groups (see RenderQueue) and register a RenderQueueListener to get notifications
		//between batches.
		//@par
		//There are individual state change methods for each of the parameters set using 
		//this method. 
		//Note that the default values in this method represent the defaults at system 
		//start up too.
		//@param func The comparison function applied.
		//@param refValue The reference value used in the comparison
		//@param mask The bitmask applied to both the stencil value and the reference value 
		//before comparison
		//@param stencilFailOp The action to perform when the stencil check fails
		//@param depthFailOp The action to perform when the stencil check passes, but the
		//depth buffer check still fails
		//@param passOp The action to take when both the stencil and depth check pass.
		//@param twoSidedOperation If set to true, then if you render both back and front faces 
		//(you'll have to turn off culling) then these parameters will apply for front faces, 
		//and the inverse of them will happen for back faces (keep remains the same).
		//virtual void setStencilBufferParams( CompareFunction _func = CMPF_ALWAYS_PASS, 
		//	uint32 _refValue = 0, uint32 _mask = 0xFFFFFFFF, StencilOperation _stencilFailOp = SOP_KEEP, 
		//	StencilOperation _depthFailOp = SOP_KEEP, StencilOperation _passOp = SOP_KEEP, 
		//	bool _twoSidedOperation = false ) = 0;

		// Sets the current vertex declaration, ie the source of vertex data.
		//virtual void setVertexDeclaration( VertexDeclaration* _decl ) = 0;
		// Sets the current vertex buffer binding state.
		//virtual void setVertexBufferBinding( VertexBufferBinding* _binding ) = 0;

		//Sets whether or not normals are to be automatically normalised.
		//@remarks
		//This is useful when, for example, you are scaling SceneNodes such that
		//normals may not be unit-length anymore. Note though that this has an
		//overhead so should not be turn on unless you really need it.
		//@par
		//You should not normally call this direct unless you are rendering
		//world geometry; set it on the Renderable because otherwise it will be
		//overridden by material settings. 
		//virtual void setNormaliseNormals( bool _normalise ) = 0;

		//Render something to the active viewport.

		//Low-level rendering interface to perform rendering
		//operations. Unlikely to be used directly by client
		//applications, since the SceneManager and various support
		//classes will be responsible for calling this method.
		//Can only be called between _beginScene and _endScene

		//@param op A rendering operation instance, which contains
		//details of the operation to be performed.
		//virtual void _render( const RenderOperation& _op );

		// Gets the capabilities of the render system.
		//virtual const RenderSystemCapabilities* getCapabilities() const = 0;

		//Binds a given GpuProgram (but not the parameters). 
		//@remarks Only one GpuProgram of each type can be bound at once, binding another
		//one will simply replace the exsiting one.
		//virtual void bindGpuProgram( GpuProgram* prg );

		//Bind Gpu program parameters.
		//virtual void bindGpuProgramParameters( GpuProgramType _gptype, GpuProgramParametersSharedPtr _params ) = 0;
		// Only binds Gpu program parameters used for passes that have more than one iteration rendering
		//virtual void bindGpuProgramPassIterationParameters(GpuProgramType gptype) = 0;

		//Unbinds GpuPrograms of a given GpuProgramType.
		//@remarks
		//This returns the pipeline to fixed-function processing for this type.
		//virtual void unbindGpuProgram( GpuProgramType _gptype );

		// Returns whether or not a Gpu program of the given type is currently bound.
		//virtual bool isGpuProgramBound( GpuProgramType _gptype );

		// sets the clipping region.
		//virtual void setClipPlanes( const TPlaneVector& _clipPlanes ) = 0;

		// Utility method for initialising all render targets attached to this rendering system.
		//virtual void _initRenderTargets();

		//Utility method to notify all render targets that a camera has been removed, 
		//incase they were referring to it as their viewer. 
		//virtual void _notifyCameraRemoved( const Camera* _cam );

		// Internal method for updating all render targets attached to this rendering system.
		//virtual void _updateAllRenderTargets();

		// Set a clipping plane.
		//virtual void setClipPlane( unsigned short _index, const float* _plane ) = 0;
		// Set a clipping plane.
		//virtual void setClipPlane( unsigned short _index, float _A, float _B, float _C, float _D ) = 0;
		// Enable the clipping plane.
		//virtual void enableClipPlane( unsigned short _index, bool _enable ) = 0;

		//Sets whether or not vertex windings set should be inverted; this can be important
		//for rendering reflections.
		//virtual void setInvertVertexWinding( bool _invert );

		//Sets the 'scissor region' ie the region of the target in which rendering can take place.
		//@remarks
		//This method allows you to 'mask off' rendering in all but a given rectangular area
		//as identified by the parameters to this method.
		//@note
		//Not all systems support this method. Check the RenderSystemCapabilities for the
		//RSC_SCISSOR_TEST capability to see if it is supported.
		//@param enabled True to enable the scissor test, false to disable it.
		//@param left, top, right, bottom The location of the corners of the rectangle, expressed in
		//<i>pixels</i>.
		//virtual void setScissorTest( bool _enabled, std::size_t _left = 0, std::size_t _top = 0, 
		//	std::size_t _right = 800, std::size_t _bottom = 600 ) = 0;

		//Clears one or more frame buffers on the active render target. 
		//@param buffers Combination of one or more elements of FrameBufferType
		//denoting which buffers are to be cleared
		//@param colour The colour to clear the colour buffer with, if enabled
		//@param depth The value to initialise the depth buffer with, if enabled
		//@param stencil The value to initialise the stencil buffer with, if enabled.
		//virtual void clearFrameBuffer( unsigned int _buffers, const float* _colour, 
		//	float _depth = 1.0f, unsigned short _stencil = 0 ) = 0;
		
		//Returns the horizontal texel offset value required for mapping 
		//texel origins to pixel origins in this rendersystem.
		//@remarks
		//Since rendersystems sometimes disagree on the origin of a texel, 
		//mapping from texels to pixels can sometimes be problematic to 
		//implement generically. This method allows you to retrieve the offset
		//required to map the origin of a texel to the origin of a pixel in
		//the horizontal direction.
		//virtual float getHorizontalTexelOffset() = 0;

		//Returns the vertical texel offset value required for mapping 
		//texel origins to pixel origins in this rendersystem.
		//@remarks
		//Since rendersystems sometimes disagree on the origin of a texel, 
		//mapping from texels to pixels can sometimes be problematic to 
		//implement generically. This method allows you to retrieve the offset
		//required to map the origin of a texel to the origin of a pixel in
		//the vertical direction.
		//virtual float getVerticalTexelOffset() = 0;

		//Gets the minimum (closest) depth value to be used when rendering
		//using identity transforms.
		//@remarks
		//When using identity transforms you can manually set the depth
		//of a vertex; however the input values required differ per
		//rendersystem. This method lets you retrieve the correct value.
		//@see Renderable::getUseIdentityView, Renderable::getUseIdentityProjection
		//virtual float getMinimumDepthInputValue() = 0;

		//Gets the maximum (farthest) depth value to be used when rendering
		//using identity transforms.
		//@remarks
		//When using identity transforms you can manually set the depth
		//of a vertex; however the input values required differ per
		//rendersystem. This method lets you retrieve the correct value.
		//@see Renderable::getUseIdentityView, Renderable::getUseIdentityProjection
		//virtual float getMaximumDepthInputValue() = 0;

		//set the current multi pass count value.  This must be set prior to 
		//calling _render() if multiple renderings of the same pass state are 
		//required.
		//@param count Number of times to render the current state.
		//virtual void setCurrentPassIterationCount( const std::size_t _count ) = 0;

		//Defines a listener on the custom events that this render system 
		//can raise.
		//@see RenderSystem::addListener
		//class Listener
		//{
		//public:
		//	Listener() {}
		//	virtual ~Listener() {}

		//	//A rendersystem-specific event occurred.
		//	//@param eventName The name of the event which has occurred
		//	//@param parameters A list of parameters that may belong to this event,
		//	//may be null if there are no parameters
		//	virtual void eventOccurred( const String& _eventName, const NameValuePairList* _parameters = 0) = 0;
		//};

		//Adds a listener to the custom events that this render system can raise.
		//@remarks
		//Some render systems have quite specific, internally generated events 
		//that the application may wish to be notified of. Many applications
		//don't have to worry about these events, and can just trust OGRE to 
		//handle them, but if you want to know, you can add a listener here.
		//@par
		//Events are raised very generically by string name. Perhaps the most 
		//common example of a render system specific event is the loss and 
		//restoration of a device in DirectX; which OGRE deals with, but you 
		//may wish to know when it happens. 
		//@see RenderSystem::getRenderSystemEvents
		//virtual void addListener( Listener* _l ) = 0;

		// Remove a listener to the custom events that this render system can raise.
		//virtual void removeListener( Listener* _l ) = 0;

		//Gets a list of the rendersystem specific events that this rendersystem
		//can raise.
		//@see RenderSystem::addListener
		//virtual const TStringVector& getRenderSystemEvents() const = 0;

		//Tell the rendersystem to perform any prep tasks it needs to directly
		//before other threads which might access the rendering API are registered.
		//@remarks
		//Call this from your main thread before starting your other threads
		//(which themselves should call registerThread()). Note that if you
		//start your own threads, there is a specific startup sequence which 
		//must be respected and requires synchronisation between the threads:
		//<ol>
		//<li>[Main thread]Call preExtraThreadsStarted</li>
		//<li>[Main thread]Start other thread, wait</li>
		//<li>[Other thread]Call registerThread, notify main thread & continue</li>
		//<li>[Main thread]Wake up & call postExtraThreadsStarted</li>
		//</ol>
		//Once this init sequence is completed the threads are independent but
		//this startup sequence must be respected.
		//virtual void preExtraThreadsStarted() = 0;

		//Tell the rendersystem to perform any tasks it needs to directly
		//after other threads which might access the rendering API are registered.
		//@see RenderSystem::preExtraThreadsStarted
		//virtual void postExtraThreadsStarted() = 0;

		//Register the an additional thread which may make calls to rendersystem-related 
		//objects.
		//@remarks
		//This method should only be called by additional threads during their
		//initialisation. If they intend to use hardware rendering system resources 
		//they should call this method before doing anything related to the render system.
		//Some rendering APIs require a per-thread setup and this method will sort that
		//out. It is also necessary to call unregisterThread before the thread shuts down.
		//@note
		//This method takes no parameters - it must be called from the thread being
		//registered and that context is enough.
		//virtual void registerThread() = 0;

		//Unregister an additional thread which may make calls to rendersystem-related objects.
		//@see RenderSystem::registerThread
		//virtual void unregisterThread() = 0;
//	};
//
//	class TextureManagerInterface
//	{
//	public:
//		virtual void reloadAfterLostDevice() = 0;
//
//		virtual PixelFormat getNativeFormat( TextureType ttype, PixelFormat format, int usage ) = 0;
//
//		virtual bool isHardwareFilteringSupported( TextureType ttype, PixelFormat format, int usage,
//			bool preciseFormatOnly = false ) = 0;
//
//		protected:
//		Resource* createImpl( const String& name, ResourceHandle handle, 
//		const String& group, bool isManual, ManualResourceLoader* loader, 
//		const NameValuePairList* createParams ) = 0;
//	};
//
//}*/

bool initInterfaceSystem(::RenderSystemInterface** _ptrRenderSystem);
void releaseInterfaceSystem(::RenderSystemInterface* _ptrRenderSystem);

//bool initInterfaceSystem( Menge::RenderSystemInterface** _ptrRenderSystem);
//void releaseInterfaceSystem( Menge::RenderSystemInterface* _ptrRenderSystem);