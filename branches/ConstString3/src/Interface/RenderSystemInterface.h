#	pragma once

#	include "ServiceInterface.h"
#	include "Config/Typedef.h"
#	include "Core/Viewport.h"
#	include "Core/Resolution.h"
#	include "Core/Rect.h"
#	include "Math/mat4.h"
#	include "Core/ConstString.h"
#	define MENGE_MAX_TEXTURE_STAGES 8

namespace Menge
{
	class ServiceProviderInterface;

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

	enum ECompareFunction
	{
		CMPF_ALWAYS_FAIL = 0,
		CMPF_ALWAYS_PASS,
		CMPF_LESS,
		CMPF_LESS_EQUAL,
		CMPF_EQUAL,
		CMPF_NOT_EQUAL,
		CMPF_GREATER_EQUAL,
		CMPF_GREATER,

		CMPF_FORCE_DWORD = 0x7fffffff
	};

	enum EFillMode
	{
		FM_POINT = 0,
		FM_WIREFRAME,
		FM_SOLID,

		FM_FORCE_DWORD = 0x7fffffff
	};

	enum EFrameBufferType 
	{
		FBT_COLOR	= 0x1,
		FBT_DEPTH   = 0x2,
		FBT_STENCIL = 0x4,

		FBT_FORCE_DWORD = 0x7fffffff
	};

	enum EShadeType
	{
		SHT_FLAT = 0,
		SHT_GOURAUD,
		SHT_PHONG,

		SHT_FORCE_DWORD = 0x7fffffff
	};

	enum ETextureOp
	{
		TOP_DISABLE = 0,
		TOP_SELECTARG1,
		TOP_SELECTARG2,
		TOP_MODULATE,
		TOP_ADD,
		TOP_SUBSTRACT,

		TOP_FORCE_DWORD = 0x7fffffff
	};

	enum ETextureArgument
	{
		TARG_CURRENT = 0,
		TARG_DIFFUSE,
		TARG_SPECULAR,
		TARG_TEXTURE,
		TARG_TFACTOR,

		TARG_FORCE_DWORD = 0x7fffffff
	};

	enum ETextureFilterType
	{
		TFT_MAGNIFICATION = 0,
		TFT_MINIFICATION,
		TFT_MIPMAP,

		TFT_FORCE_DWORD = 0x7fffffff
	};

	enum ETextureFilter
	{
		TF_NONE = 0,
		TF_POINT,
		TF_LINEAR,
		TF_ANISOTROPIC,
		TF_FLATCUBIC,
		TF_GAUSSIANCUBIC,

		TF_FORCE_DWORD = 0x7fffffff
	};

#define LOCK_READONLY           0x00000010L
#define LOCK_DISCARD             0x00002000L
#define LOCK_NOOVERWRITE        0x00001000L
#define LOCK_NOSYSLOCK          0x00000800L

	typedef size_t VBHandle; // Vertex Buffer Handle
	typedef size_t IBHandle; // Index Buffer Handle

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
		virtual unsigned char* lock( int* _pitch, bool _readOnly = true ) = 0;
        virtual unsigned char* lockRect( int* _pitch, const Rect& _rect, bool _readOnly = true ) = 0;
		virtual void unlock() = 0;
	};

	class ImageDecoderInterface;

	class RenderTextureInterface
	{
	public:
		virtual RenderImageInterface* getInterface() const = 0;
		
		virtual size_t getId() const = 0;

		virtual size_t addRef() const = 0;
		virtual size_t decRef() const = 0;

		virtual const mt::vec4f & getUV() const = 0;

		virtual void setFileName( const WString & _filename ) = 0;
		virtual const WString & getFileName() const = 0;

		virtual size_t getWidth() const = 0;
		virtual size_t getHeight() const = 0;
		virtual PixelFormat getPixelFormat() const = 0;

		virtual unsigned char* lock( int* _pitch, bool _readOnly = true ) const = 0;
		virtual unsigned char* lockRect( int* _pitch, const Rect& _rect, bool _readOnly = true ) const = 0;
		virtual void unlock() const = 0;

		virtual size_t getHWWidth() const = 0;
		virtual size_t getHWHeight() const = 0;
		virtual PixelFormat getHWPixelFormat() const = 0;

		virtual bool loadImageData( ImageDecoderInterface* _imageDecoder ) = 0;
		virtual bool loadImageData( unsigned char* _textureBuffer, size_t _texturePitch, ImageDecoderInterface* _imageDecoder ) = 0;				
	};

	//////////////////////////////////////////////////////////////////////////
	class RenderSystemListener
	{
	public:
		virtual void onDeviceRestored() = 0;
	};
	//////////////////////////////////////////////////////////////////////////
	typedef std::vector<int> TVectorResolutions;
	//////////////////////////////////////////////////////////////////////////
	class RenderSystemInterface
	{
	public:
		virtual bool initialize( ServiceProviderInterface* _serviceProvider, RenderSystemListener* _listener ) = 0;

		virtual bool createRenderWindow( size_t _width, size_t _height, int _bits, bool _fullscreen, WindowHandle _winHandle,
			bool _waitForVSync, int _FSAAType, int _FSAAQuality ) = 0;

		virtual void getResolutions( TVectorResolutions & _resolutions ) = 0;
		virtual void makeProjection2D( float _left, float _right,
										float _top, float _bottom, 
										float _near, float _far,
										float* _outMatrix ) = 0;

		virtual float getTexelOffsetX() const = 0;
		virtual float getTexelOffsetY() const = 0;
		// Render frame into _image
		// int rect[4] - rectangle represents desired frame area in pixels
		virtual void screenshot( RenderImageInterface* _image, const float * _rect ) = 0;
		// входные данные: матрица 4 на 4
		virtual	void setProjectionMatrix( const mt::mat4f & _projection ) = 0;
		virtual	void setModelViewMatrix( const mt::mat4f & _view ) = 0;
		virtual void setTextureMatrix( size_t _stage, const float* _texture ) = 0;

		virtual VBHandle createVertexBuffer( size_t _verticesNum, size_t _vertexSize ) = 0;
		virtual void releaseVertexBuffer( VBHandle _vbHandle ) = 0;
		virtual void* lockVertexBuffer(  VBHandle _vbHandle, size_t _offset, size_t _size, uint32 _flags ) = 0;
		virtual bool unlockVertexBuffer( VBHandle _vbHandle ) = 0;
		virtual void setVertexBuffer( VBHandle _vbHandle ) = 0;

		virtual IBHandle createIndexBuffer( size_t _indiciesNum ) = 0;
		virtual void releaseIndexBuffer( IBHandle _ibHandle ) = 0;
		virtual uint16* lockIndexBuffer(  IBHandle _ibHandle ) = 0;
		virtual bool unlockIndexBuffer( IBHandle _ibHandle ) = 0;
		virtual void setIndexBuffer( IBHandle _ibHandle, size_t _baseVertexIndex ) = 0;

		virtual void setVertexDeclaration( size_t _vertexSize, uint32 _declaration ) = 0;

		virtual void drawIndexedPrimitive( EPrimitiveType _type, size_t _baseVertexIndex,
			size_t _minIndex, size_t _verticesNum, size_t _startIndex, size_t _indexCount ) = 0;

		virtual void setTexture( size_t _stage, RenderImageInterface* _texture ) = 0;
		virtual void setTextureAddressing( size_t _stage, ETextureAddressMode _modeU, ETextureAddressMode _modeV ) = 0;
		virtual void setTextureFactor( uint32 _color ) = 0;
		virtual void setSrcBlendFactor( EBlendFactor _src ) = 0;
		virtual void setDstBlendFactor( EBlendFactor _dst ) = 0;
		virtual void setCullMode( ECullMode _mode ) = 0;
		virtual void setDepthBufferTestEnable( bool _depthTest ) = 0;
		virtual void setDepthBufferWriteEnable( bool _depthWrite ) = 0;
		virtual void setDepthBufferCmpFunc( ECompareFunction _depthFunction ) = 0;
		virtual void setFillMode( EFillMode _mode ) = 0;
		virtual void setColorBufferWriteEnable( bool _r, bool _g, bool _b, bool _a ) = 0;
		virtual void setShadeType( EShadeType _sType ) = 0;
		virtual void setAlphaTestEnable( bool _alphaTest ) = 0;
		virtual void setAlphaBlendEnable( bool _alphaBlend ) = 0;
		virtual void setAlphaCmpFunc( ECompareFunction _alphaFunc, uint8 _alpha ) = 0;
		virtual void setLightingEnable( bool _light ) = 0;
		virtual void setTextureStageColorOp( size_t _stage, ETextureOp _textrueOp,
												ETextureArgument _arg1, ETextureArgument _arg2 ) = 0;
		virtual void setTextureStageAlphaOp( size_t _stage, ETextureOp _textrueOp,
												ETextureArgument _arg1, ETextureArgument _arg2 ) = 0;
		virtual void setTextureStageFilter( size_t _stage, ETextureFilterType _filterType, ETextureFilter _filter ) = 0;
		// create texture
		// [in/out] _width ( desired texture width, returns actual texture width )
		// [in/out] _height ( desired texture height, returns actual texture height )
		// [in/out] _format ( desired texture pixel format, returns actual texture pixel format )
		// returns Texture interface handle or NULL if fails
		virtual RenderImageInterface * createImage( size_t _width, size_t _height, size_t & _realWidth, size_t & _realHeight, PixelFormat& _format ) = 0;
		// create render target image
		// [in/out] _width ( desired texture width, returns actual texture width )
		// [in/out] _height ( desired texture height, returns actual texture height )
		// returns Texture interface handle or NULL if fails
		virtual RenderImageInterface * createRenderTargetImage( size_t& _width, size_t& _height, size_t & _realWidth, size_t & _realHeight, PixelFormat& _format ) = 0;

		// удаления изображения
		virtual void releaseImage( RenderImageInterface * _image ) = 0;
		//
		// отрисовка изображения

		
		virtual bool beginScene() = 0;
		virtual void endScene() = 0;
		virtual void swapBuffers() = 0;
		virtual void clearFrameBuffer( uint32 _frameBufferTypes
											, uint32 _color = 0
											, float _depth = 1.0f
											, uint16 _stencil = 0 ) = 0;
		virtual	void	beginLayer2D() = 0;
		virtual	void	endLayer2D() = 0;
		virtual	void	beginLayer3D() = 0;
		virtual	void	endLayer3D() = 0;

		virtual void	setRenderViewport( const Viewport & _viewport ) = 0;

		virtual void	changeWindowMode( const Resolution & _resolution, bool _fullscreen ) = 0;
		virtual void	setRenderTarget( RenderImageInterface* _renderTarget, bool _clear ) = 0;

		virtual bool supportTextureFormat( PixelFormat _format ) = 0;

		//new
		//virtual LightInterface * createLight( const String & _name ) = 0;
		//virtual void releaseLight( LightInterface * _light ) = 0;

		virtual void onWindowMovedOrResized() = 0;
		virtual void onWindowClose() = 0;

		virtual void setVSync( bool _vSync ) = 0;
		virtual void clear( uint32 _color ) = 0;

		virtual void setSeparateAlphaBlendMode() = 0;

	};

	class RenderServiceInterface
		: public ServiceInterface
	{
	public:
		virtual bool createRenderWindow( const Resolution & _resolution, const Resolution & _contentResolution, const Viewport & _viewport, int _bits, bool _fullscreen, 
			WindowHandle _winHandle, int _FSAAType , int _FSAAQuality ) = 0;

	public:
		virtual void clear( uint32 _color ) = 0;
		virtual bool beginScene() = 0;
		virtual void endScene() = 0;
		virtual void swapBuffers() = 0;
		virtual void screenshot( RenderTextureInterface * _renderTargetImage, const mt::vec4f & _rect ) = 0;
		virtual void releaseTexture( const RenderTextureInterface* _texture ) = 0;
		virtual void setVSync( bool _vSync ) = 0;
		virtual bool getVSync() const = 0;
		virtual void setSeparateAlphaBlendMode() = 0;
	public:
		virtual RenderTextureInterface* createTexture( size_t _width, size_t _height, PixelFormat _format ) = 0;
		virtual RenderTextureInterface * createRenderTargetTexture( size_t _width, size_t _height, PixelFormat _format ) = 0;

		virtual void setRenderTargetTexture( RenderTextureInterface * _image, bool _clear ) = 0;
	};
}

bool initInterfaceSystem(Menge::RenderSystemInterface** _ptrRenderSystem);
void releaseInterfaceSystem(Menge::RenderSystemInterface* _ptrRenderSystem);
