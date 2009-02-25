#	pragma once

#	include "Config/Typedef.h"

namespace Menge
{
	class LogSystemInterface;
	class ImageDecoderInterface;

	struct	TextureDesc
	{
		String name;

		unsigned int filter;

		void * buffer;
		std::size_t size;

		std::size_t width;
		std::size_t height;

		int	pixelFormat;
	};

	enum EBlendFactor 
	{
		BF_ONE = 0,
		BF_ZERO,
		BF_DEST_COLOUR,
		BF_SOURCE_COLOUR,
		BF_ONE_MINUS_DEST_COLOUR,
		BF_ONE_MINUS_SOURCE_COLOUR,
		BF_DEST_ALPHA,
		BF_SOURCE_ALPHA,
		BF_ONE_MINUS_DEST_ALPHA,
		BF_ONE_MINUS_SOURCE_ALPHA,

		BF_FORCE_DWORD = 0x7fffffff
	};

	enum EPrimitiveType
	{
		PT_POINTLIST = 0,
		PT_LINELIST,
		PT_LINESTRIP,
		PT_TRIANGLELIST,
		PT_TRIANGLESTRIP,
		PT_TRIANGLEFAN,

		PT_FORCE_DWORD = 0x7fffffff
	};

	enum ETextureAddressMode
	{
		TAM_CLAMP = 0,
		TAM_WRAP,
		TAM_MIRROR,

		TAM_FORCE_DWORD = 0x7fffffff
	};

	enum ECullMode
	{
		CM_CULL_NONE = 0,
		CM_CULL_CW,
		CM_CULL_CCW,

		CM_CULL_FORCE_DWORD = 0x7fffffff
	};

	typedef struct _tVertex
	{
		float pos[3];
		float n[3];
		float uv[2];

		_tVertex()
		{
			pos[0] = pos[1] = pos[2] = 0.0f;
			n[0] = n[1] = 0.0f; n[2] = 1.0f;
			uv[0] = uv[1] = 0.0f;
		}

	} TVertex;

	typedef std::size_t VBHandle; // Vertex Buffer Handle
	typedef std::size_t IBHandle; // Index Buffer Handle

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
		virtual std::size_t getWidth() const = 0;
		virtual std::size_t getHeight() const = 0;
		virtual const String & getDescription() const = 0;
		virtual unsigned char* lock( int* _pitch, bool _readOnly = true ) = 0;
		virtual void unlock() = 0;
		virtual PixelFormat getPixelFormat() = 0;
	};

	typedef struct _tMaterial
	{
		RenderImageInterface* texture;
		unsigned int color;
		float* texMatrix;
		_tMaterial()
			: texture( 0 )
			, texMatrix( 0 )
			, color( 0xFFFFFFFF )
		{
		}
	}TMaterial;

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
		virtual bool createRenderWindow( std::size_t _width, std::size_t _height, int _bits, bool _fullscreen, WindowHandle _winHandle,
			int _FSAAType, int _FSAAQuality ) = 0;
		virtual const std::vector<int> & getResolutionList() = 0;

		virtual float getTexelOffsetX() const = 0;
		virtual float getTexelOffsetY() const = 0;
		// Render frame into _image
		// int rect[4] - rectangle represents desired frame area in pixels
		virtual void screenshot( RenderImageInterface* _image, const float * _rect ) = 0;
		// Renders One Frame
		virtual void render() = 0; 
		// установка разрешения контента игры, входные данные: вектор2. 
		virtual void setContentResolution( const std::size_t * _resolution ) = 0;
		// входные данные: матрица 4 на 4
		virtual	void setProjectionMatrix( const float * _projection ) = 0;
		virtual	void setViewMatrix( const float * _view ) = 0;
		virtual	void setWorldMatrix( const float * _world ) = 0;

		virtual VBHandle createVertexBuffer( std::size_t _verticesNum ) = 0;
		virtual void releaseVertexBuffer( VBHandle _vbHandle ) = 0;
		virtual TVertex* lockVertexBuffer(  VBHandle _vbHandle ) = 0;
		virtual void unlockVertexBuffer( VBHandle _vbHandle ) = 0;
		virtual void setVertexBuffer( VBHandle _vbHandle ) = 0;

		virtual IBHandle createIndexBuffer( std::size_t _indiciesNum ) = 0;
		virtual void releaseIndexBuffer( IBHandle _ibHandle ) = 0;
		virtual uint16* lockIndexBuffer(  IBHandle _ibHandle ) = 0;
		virtual void unlockIndexBuffer( IBHandle _ibHandle ) = 0;
		virtual void setIndexBuffer( IBHandle _ibHandle ) = 0;

		virtual void drawIndexedPrimitive( EPrimitiveType _type, std::size_t _baseVertexIndex,
			std::size_t _startIndex, std::size_t _verticesNum, std::size_t _indiciesNum ) = 0;

		virtual void setTexture( std::size_t _stage, RenderImageInterface* _texture ) = 0;
		virtual void setTextureAddressing( std::size_t _stage, ETextureAddressMode _modeU, ETextureAddressMode _modeV ) = 0;
		virtual void setTextureFactor( uint32 _color ) = 0;
		virtual void setBlendFactor( EBlendFactor _src, EBlendFactor _dst ) = 0;
		virtual void setCullMode( ECullMode _mode ) = 0;

		// create empty render image
		virtual RenderImageInterface * createImage( const String & _name, std::size_t _width, std::size_t _height, PixelFormat _format ) = 0;
		// create render target image
		virtual RenderImageInterface * createRenderTargetImage( const String & _name, std::size_t _width, std::size_t _height ) = 0;
		// загрузка изображения
		virtual RenderImageInterface * loadImage( const String& _name, ImageDecoderInterface* _imageDecoder ) = 0;
		// удаления изображения
		virtual void releaseImage( RenderImageInterface * _image ) = 0;
		//
		virtual RenderImageInterface * getImage( const String& _desc ) const = 0;
		//
		// отрисовка изображения

		virtual void	beginScene() = 0;
		virtual void	endScene() = 0;
		virtual void	swapBuffers() = 0;
		virtual	void	beginLayer2D() = 0;
		virtual	void	endLayer2D() = 0;
		virtual	void	beginLayer3D() = 0;
		virtual	void	endLayer3D() = 0;

		virtual void	setRenderArea( const float* _renderArea ) = 0;

		virtual void	setFullscreenMode( std::size_t _width, std::size_t _height, bool _fullscreen ) = 0;
		virtual void	setRenderTarget( const String& _name, bool _clear ) = 0;

		//new
		virtual LightInterface * createLight( const String & _name ) = 0;
		virtual void releaseLight( LightInterface * _light ) = 0;

		virtual void setTextureFiltering( bool _filter ) = 0;
		virtual void onWindowMovedOrResized() = 0;
		virtual void onWindowClose() = 0;
	};
}

bool initInterfaceSystem(Menge::RenderSystemInterface** _ptrRenderSystem);
void releaseInterfaceSystem(Menge::RenderSystemInterface* _ptrRenderSystem);
