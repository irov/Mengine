#	pragma once

#	include "Interface/ServiceInterface.h"
#	include "Interface/ImageCodecInterface.h"

#	include "Config/Typedef.h"

#   include "Factory/FactorablePtr.h"

#	include "Core/Viewport.h"
#	include "Core/Resolution.h"
#	include "Core/Rect.h"
#	include "Core/ConstString.h"
#	include "Core/FilePath.h"
#	include "Core/PixelFormat.h"

#   include "stdex/intrusive_ptr.h"

#	include "Math/mat4.h"
#	include "Math/uv4.h"
#	include "Math/plane.h"

#   ifndef MENGE_MAX_TEXTURE_STAGES
#	define MENGE_MAX_TEXTURE_STAGES 4
#   endif

#	ifndef MENGINE_RENDER_VERTEX_UV_COUNT
#	define MENGINE_RENDER_VERTEX_UV_COUNT 4
#	endif

namespace Menge
{
	//////////////////////////////////////////////////////////////////////////
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

		BF_MAX_VALUE
	};
	//////////////////////////////////////////////////////////////////////////
	enum EPrimitiveType
	{
		PT_POINTLIST = 0,
		PT_LINELIST,
		PT_LINESTRIP,
		PT_TRIANGLELIST,
		PT_TRIANGLESTRIP,
		PT_TRIANGLEFAN,

		PT_MAX_VALUE
	};
	//////////////////////////////////////////////////////////////////////////
	enum ETextureAddressMode
	{
		TAM_WRAP = 0,		
		TAM_MIRROR,
		TAM_CLAMP,
		TAM_BORDER,
		TAM_MIRRORONCE,

		TAM_MAX_VALUE
	};
	//////////////////////////////////////////////////////////////////////////
	enum ECullMode
	{
		CM_CULL_NONE = 0,
		CM_CULL_CW,
		CM_CULL_CCW,

		CM_MAX_VALUE
	};
	//////////////////////////////////////////////////////////////////////////
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

		CMPF_MAX_VALUE
	};
	//////////////////////////////////////////////////////////////////////////
	enum EFillMode
	{
		FM_POINT = 0,
		FM_WIREFRAME,
		FM_SOLID,

		FM_MAX_VALUE
	};
	//////////////////////////////////////////////////////////////////////////
	enum EFrameBufferType 
	{
		FBT_COLOR	= 0x1,
		FBT_DEPTH   = 0x2,
		FBT_STENCIL = 0x4,
	};
	//////////////////////////////////////////////////////////////////////////
	enum EShadeType
	{
		SHT_FLAT = 0,
		SHT_GOURAUD,
		SHT_PHONG,

		SHT_MAX_VALUE
	};
	//////////////////////////////////////////////////////////////////////////
	enum ETextureOp
	{
		TOP_DISABLE = 0,
		TOP_SELECTARG1,
		TOP_SELECTARG2,
		TOP_MODULATE,
		TOP_MODULATE2X,
		TOP_MODULATE4X,
		TOP_ADD,
		TOP_SUBTRACT,

		TOP_MAX_VALUE
	};
	//////////////////////////////////////////////////////////////////////////
	enum ETextureArgument
	{
		TARG_CURRENT = 0,
		TARG_DIFFUSE,
		TARG_SPECULAR,
		TARG_TEXTURE,
		TARG_TFACTOR,

		TARG_MAX_VALUE
	};
	//////////////////////////////////////////////////////////////////////////
	enum ETextureFilterType
	{
		TFT_MAGNIFICATION = 0,
		TFT_MINIFICATION,
		TFT_MIPMAP,

		TFT_MAX_VALUE
	};
	//////////////////////////////////////////////////////////////////////////
	enum ETextureFilter
	{
		TF_NONE = 0,
		TF_POINT,
		TF_LINEAR,
		TF_ANISOTROPIC,
		TF_FLATCUBIC,
		TF_GAUSSIANCUBIC,

		TF_MAX_VALUE
	};
	//////////////////////////////////////////////////////////////////////////
	enum EBufferLockFlag
	{
		BLF_LOCK_NONE = 0,
		BLF_LOCK_READONLY,
		BLF_LOCK_DISCARD,
		BLF_LOCK_NOOVERWRITE,
		BLF_LOCK_NOSYSLOCK,

		BLF_MAX_VALUE
	};
	//////////////////////////////////////////////////////////////////////////
	enum ERenderImageMode
	{
		ERIM_NORMAL = 0,
		ERIM_DYNAMIC,
		ERIM_RENDER_TARGET,

		ERIM_MAX_VALUE
	};
	//////////////////////////////////////////////////////////////////////////
	typedef uint32_t VBHandle; // Vertex Buffer Handle
	typedef uint32_t IBHandle; // Index Buffer Handle
	//////////////////////////////////////////////////////////////////////////
	class RenderImageInterface
		: public FactorablePtr
	{
	public:
		virtual ERenderImageMode getMode() const = 0;
		
	public:
		virtual uint32_t getHWWidth() const = 0;
		virtual uint32_t getHWHeight() const = 0;
		virtual uint32_t getHWChannels() const = 0;
		virtual uint32_t getHWDepth() const = 0;

		virtual PixelFormat getHWPixelFormat() const = 0;

	public:
		virtual void * lock( size_t * _pitch, uint32_t _level, const Rect& _rect, bool _readOnly = true ) = 0;
		virtual void unlock( uint32_t _level ) = 0;
	};
	//////////////////////////////////////////////////////////////////////////
	typedef stdex::intrusive_ptr<RenderImageInterface> RenderImageInterfacePtr;
	//////////////////////////////////////////////////////////////////////////
	class RenderTextureInterface
		: public FactorablePtr
	{
	public:
		virtual void release() = 0;

	public:
		virtual const RenderImageInterfacePtr & getImage() const = 0;

	public:
		virtual uint32_t getId() const = 0;

		virtual const Rect & getRect() const = 0;
		virtual const Rect & getHWRect() const = 0;

		virtual const mt::uv4f & getUV() const = 0;

		virtual void setCategory( const ConstString & _category ) = 0;
		virtual const ConstString & getCategory() const = 0;

		virtual void setFileName( const FilePath & _fileName ) = 0;
		virtual const FilePath & getFileName() const = 0;

		virtual uint32_t getMipmaps() const = 0;

		virtual uint32_t getWidth() const = 0;
		virtual uint32_t getHeight() const = 0;
		virtual uint32_t getChannels() const = 0;

		virtual void * lock( size_t * _pitch, uint32_t _miplevel, const Rect & _rect, bool _readOnly = true ) const = 0;
		virtual void unlock( uint32_t _miplevel ) const = 0;

		virtual size_t getMemoryUse() const = 0;
	};
	//////////////////////////////////////////////////////////////////////////
	typedef stdex::intrusive_ptr<RenderTextureInterface> RenderTextureInterfacePtr;
    //////////////////////////////////////////////////////////////////////////
    struct RenderTextureStage
    {
        RenderTextureStage()
			: mipmap( TF_NONE )
			, magnification( TF_LINEAR )
			, minification( TF_LINEAR )
			, addressU(TAM_CLAMP)
			, addressV(TAM_CLAMP)
            , colorOp(TOP_DISABLE)
            , colorArg1(TARG_TEXTURE)
            , colorArg2(TARG_DIFFUSE)
            , alphaOp(TOP_DISABLE)
            , alphaArg1(TARG_TEXTURE)
            , alphaArg2(TARG_DIFFUSE)
            , texCoordIndex(0)
        {
        }

		ETextureFilter mipmap;
		ETextureFilter magnification;
		ETextureFilter minification;

		ETextureAddressMode addressU;
		ETextureAddressMode addressV;

        ETextureOp colorOp;
        ETextureArgument colorArg1;
        ETextureArgument colorArg2;
        ETextureOp alphaOp;
        ETextureArgument alphaArg1;
        ETextureArgument alphaArg2;

        uint32_t texCoordIndex;
    };
	//////////////////////////////////////////////////////////////////////////
	class RenderShaderInterface
		: public FactorablePtr
	{
	public:
		virtual void setServiceProvider( ServiceProviderInterface * _serviceProvider ) = 0;
		virtual ServiceProviderInterface * getServiceProvider() = 0;

	public:
		virtual const ConstString & getName() const = 0;
	};
	//////////////////////////////////////////////////////////////////////////
	typedef stdex::intrusive_ptr<RenderShaderInterface> RenderShaderInterfacePtr;
	//////////////////////////////////////////////////////////////////////////
	class RenderProgramInterface
		: public FactorablePtr
	{
	public:
		virtual void setServiceProvider( ServiceProviderInterface * _serviceProvider ) = 0;
		virtual ServiceProviderInterface * getServiceProvider() = 0;
	
	public:
		virtual const ConstString & getName() const = 0;
	};
	//////////////////////////////////////////////////////////////////////////
	typedef stdex::intrusive_ptr<RenderProgramInterface> RenderProgramInterfacePtr;
	//////////////////////////////////////////////////////////////////////////
	struct RenderStage
	{	
		RenderStage()
			: blendSrc(BF_SOURCE_ALPHA)
			, blendDst(BF_ONE_MINUS_SOURCE_ALPHA)
			, alphaBlendEnable(false)
		{
		}

		RenderTextureStage textureStage[MENGE_MAX_TEXTURE_STAGES];
		
		EBlendFactor blendSrc;
		EBlendFactor blendDst;

		bool alphaBlendEnable;

		RenderProgramInterfacePtr program;
	};
	//////////////////////////////////////////////////////////////////////////
	enum EMaterial
	{
		EM_DEBUG,

		EM_TEXTURE_SOLID,
		EM_TEXTURE_BLEND,
		EM_TEXTURE_BLEND_WC,
		EM_TEXTURE_BLEND_WW,
		EM_TEXTURE_BLEND_CW,
		EM_TEXTURE_INTENSIVE,
		EM_TEXTURE_MULTIPLY,
		EM_TEXTURE_SCREEN,

		EM_TEXTURE_BLEND_ONLYCOLOR,
		EM_TEXTURE_INTENSIVE_ONLYCOLOR,
		EM_TEXTURE_MULTIPLY_ONLYCOLOR,
		EM_TEXTURE_SCREEN_ONLYCOLOR,

		EM_TEXTURE_BLEND_EXTERNAL_ALPHA,
		EM_TEXTURE_BLEND_EXTERNAL_ALPHA_WC,
		EM_TEXTURE_BLEND_EXTERNAL_ALPHA_WW,
		EM_TEXTURE_BLEND_EXTERNAL_ALPHA_CW,
		EM_TEXTURE_INTENSIVE_EXTERNAL_ALPHA,
		EM_TEXTURE_MULTIPLY_EXTERNAL_ALPHA,
		EM_TEXTURE_SCREEN_EXTERNAL_ALPHA,

		EM_TEXTURE_BLEND_EXTERNAL_ALPHA_ONLYCOLOR,
		EM_TEXTURE_INTENSIVE_EXTERNAL_ALPHA_ONLYCOLOR,
		EM_TEXTURE_MULTIPLY_EXTERNAL_ALPHA_ONLYCOLOR,
		EM_TEXTURE_SCREEN_EXTERNAL_ALPHA_ONLYCOLOR,

		EM_COLOR_SOLID,
		EM_COLOR_BLEND,
		EM_COLOR_INTENSIVE,
		EM_COLOR_MULTIPLY,
		EM_COLOR_SCREEN,
		EM_MATERIAL_COUNT
	};
    //////////////////////////////////////////////////////////////////////////
    class RenderMaterialInterface
		: public FactorablePtr
    {
	public:
		virtual const ConstString & getName() const = 0;
		virtual uint32_t getId() const = 0;
    };
	//////////////////////////////////////////////////////////////////////////
	typedef stdex::intrusive_ptr<RenderMaterialInterface> RenderMaterialInterfacePtr;
	//////////////////////////////////////////////////////////////////////////
	const uint32_t VDECL_XYZ = 0x00000002;
	const uint32_t VDECL_XYZRHW = 0x00000004;
	const uint32_t VDECL_NORMAL = 0x00000010;
	const uint32_t VDECL_DIFFUSE = 0x00000040;
	const uint32_t VDECL_SPECULAR = 0x00000080;
	const uint32_t VDECL_TEX1 = 0x00000100;
	const uint32_t VDECL_TEX2 = 0x00000200;
	//////////////////////////////////////////////////////////////////////////
	static const uint32_t Vertex2D_declaration = VDECL_XYZ | VDECL_DIFFUSE | VDECL_TEX2;
	//////////////////////////////////////////////////////////////////////////
	typedef uint16_t RenderIndices;
	//////////////////////////////////////////////////////////////////////////
	typedef stdex::vector<RenderIndices> TVectorRenderIndices;
	//////////////////////////////////////////////////////////////////////////
	struct RenderVertex2D
	{
		mt::vec3f pos;
		uint32_t color;
		mt::vec2f uv[MENGINE_RENDER_VERTEX_UV_COUNT];
	};
	//////////////////////////////////////////////////////////////////////////
	typedef stdex::vector<RenderVertex2D> TVectorRenderVertex2D;
	//////////////////////////////////////////////////////////////////////////
	class RenderVertexBufferInterface
		: public FactorablePtr
	{
	public:
		virtual RenderVertex2D * lock( uint32_t _offset, uint32_t _size, EBufferLockFlag _flags ) = 0;
		virtual bool unlock() = 0;
	};
	//////////////////////////////////////////////////////////////////////////
	typedef stdex::intrusive_ptr<RenderVertexBufferInterface> RenderVertexBufferInterfacePtr;
	//////////////////////////////////////////////////////////////////////////
	class RenderIndexBufferInterface
		: public FactorablePtr
	{
	public:
		virtual RenderIndices * lock( uint32_t _offset, uint32_t _size, EBufferLockFlag _flags ) = 0;
		virtual bool unlock() = 0;
	};
	//////////////////////////////////////////////////////////////////////////
	typedef stdex::intrusive_ptr<RenderIndexBufferInterface> RenderIndexBufferInterfacePtr;
    //////////////////////////////////////////////////////////////////////////
    class RenderMaterialServiceInterface
        : public ServiceInterface
    {
        SERVICE_DECLARE("RenderMaterialManager")

    public:
        virtual bool loadMaterials( const ConstString& _pakName, const FilePath& _fileName ) = 0;
		virtual bool unloadMaterials( const ConstString& _pakName, const FilePath& _fileName ) = 0;

	public:
		virtual void setDefaultTextureFilter( ETextureFilter _mipmap, ETextureFilter _magnification, ETextureFilter _minification ) = 0;

	public:
		virtual const ConstString & getMaterialName( EMaterial _materialId ) const = 0;

	public:
		virtual const RenderStage * cacheStage( const RenderStage & _other ) = 0;

	public:
		virtual RenderMaterialInterfacePtr getMaterial( const ConstString & _materialName
			, EPrimitiveType _primitiveType
			, uint32_t _textureCount
			, const RenderTextureInterfacePtr * _textures ) = 0;

		virtual RenderMaterialInterfacePtr getMaterial2( const RenderStage * _stage
			, EPrimitiveType _primitiveType
			, uint32_t _textureCount
			, const RenderTextureInterfacePtr * _textures ) = 0;

	public:
		virtual void setDebugMaterial( const RenderMaterialInterfacePtr & _debugMaterial ) = 0;
		virtual const RenderMaterialInterfacePtr & getDebugMaterial() const = 0;

	public:
		virtual ETextureFilter getDefaultTextureFilterMipmap() const = 0;
		virtual ETextureFilter getDefaultTextureFilterMagnification() const = 0;
		virtual ETextureFilter getDefaultTextureFilterMinification() const = 0;
    };
    //////////////////////////////////////////////////////////////////////////
#   define RENDERMATERIAL_SERVICE( serviceProvider )\
    ((Menge::RenderMaterialServiceInterface*)SERVICE_GET(serviceProvider, Menge::RenderMaterialServiceInterface))
    //////////////////////////////////////////////////////////////////////////
    struct RenderTextureDebugInfo
    {
        size_t textureMemory;
        uint32_t textureCount;
    };
    //////////////////////////////////////////////////////////////////////////
    class VisitorRenderTextureInterface
    {
    public:
        virtual void visitRenderTexture( const RenderTextureInterface * _texture ) = 0;
    };
    //////////////////////////////////////////////////////////////////////////
    class RenderTextureServiceInterface
        : public ServiceInterface
    {
        SERVICE_DECLARE("RenderTextureManager")

    public:
        virtual RenderTextureInterfacePtr loadTexture( const ConstString& _pakName, const FilePath& _fileName, const ConstString& _codec ) = 0;
		virtual RenderTextureInterfacePtr createRenderTexture( const RenderImageInterfacePtr & _image, uint32_t _mipmaps, uint32_t _width, uint32_t _height, uint32_t _channels ) = 0;

    public:
        virtual RenderTextureInterfacePtr createTexture( uint32_t _mipmaps, uint32_t _width, uint32_t _height, uint32_t _channels, uint32_t _depth, PixelFormat _format ) = 0;
        virtual RenderTextureInterfacePtr createDynamicTexture( uint32_t _width, uint32_t _height, uint32_t _channels, uint32_t _depth, PixelFormat _format ) = 0;
        
	public:
        virtual RenderTextureInterfacePtr getTexture( const ConstString& _pakName, const FilePath & _fileName ) const = 0;
        
        virtual bool hasTexture( const ConstString& _pakName, const FilePath & _fileName, RenderTextureInterfacePtr * _texture ) const = 0;

	public:
		virtual size_t getImageMemoryUse( uint32_t _width, uint32_t _height, uint32_t _channels, uint32_t _depth, PixelFormat _format ) const = 0;

    public:
        //virtual void sweezleAlpha( RenderTextureInterface * _texture, unsigned char * _textureBuffer, size_t _texturePitch ) = 0;
        virtual void imageQuality( const RenderTextureInterfacePtr & _texture, void * _textureBuffer, size_t _texturePitch ) = 0;

        virtual void cacheFileTexture( const ConstString& _pakName, const FilePath& _fileName, const RenderTextureInterfacePtr & _texture ) = 0;

    public:
        virtual bool saveImage( const RenderTextureInterfacePtr & _texture, const ConstString& _fileGroupName, const ConstString & _codecName, const FilePath & _fileName ) = 0;

    public:
        virtual void visitTexture( VisitorRenderTextureInterface * _visitor ) const = 0;

    public:
        virtual const RenderTextureDebugInfo & getDebugInfo() = 0;
    };
    //////////////////////////////////////////////////////////////////////////
#   define RENDERTEXTURE_SERVICE( serviceProvider )\
    ((Menge::RenderTextureServiceInterface*)SERVICE_GET(serviceProvider, Menge::RenderTextureServiceInterface))
	//////////////////////////////////////////////////////////////////////////
	class RenderViewportInterface
	{
	public:
		virtual const Viewport & getViewport() const = 0;
	};
	//////////////////////////////////////////////////////////////////////////
	class RenderClipplaneInterface
	{
	public:
		virtual uint32_t getCount() const = 0;
		virtual const mt::planef & getPlane( uint32_t _index ) const = 0;
	};
	//////////////////////////////////////////////////////////////////////////
	class RenderCameraInterface
	{
	public:
		virtual const mt::mat4f & getCameraViewMatrix() const = 0;
		virtual const mt::mat4f & getCameraViewMatrixInv() const = 0;

		virtual const mt::mat4f & getCameraProjectionMatrix() const = 0;
		virtual const mt::mat4f & getCameraProjectionMatrixInv() const = 0;

	public:
		virtual const mt::box2f & getCameraBBoxWM() const = 0; 

	public:
		virtual const Viewport & getCameraRenderport() const = 0;

	public:
		virtual bool isOrthogonalProjection() const = 0;
	};
	//////////////////////////////////////////////////////////////////////////
	class RenderTargetInterface
	{
	public:
		virtual bool begin() = 0;
		virtual void end() = 0;

	public:
		virtual bool getData( unsigned char * _buffer, size_t _pitch ) = 0;
	};
	//////////////////////////////////////////////////////////////////////////
	class RenderSystemInterface
        : public ServiceInterface
	{
        SERVICE_DECLARE("RenderSystem")

	public:
		virtual const ConstString & getRenderPlatformName() const = 0;

		virtual bool createRenderWindow( const Resolution & _resolution, uint32_t _bits, bool _fullscreen,
			bool _waitForVSync, int _FSAAType, int _FSAAQuality, uint32_t _MultiSampleCount ) = 0;

    public:
        virtual void makeProjectionOrthogonal( mt::mat4f & _projectionMatrix, const Viewport & _viewport, float _near, float _far ) = 0;
        virtual void makeProjectionFrustum( mt::mat4f & _projectionMatrix, const Viewport & _viewport, float _near, float _far ) = 0;
        virtual void makeProjectionPerspective( mt::mat4f & _projectionMatrix, float _fov, float _aspect, float zn, float zf ) = 0;
        virtual void makeViewMatrixFromViewport( mt::mat4f & _viewMatrix, const Viewport & _viewport ) = 0;
        virtual void makeViewMatrixLookAt( mt::mat4f & _viewMatrix, const mt::vec3f & _eye, const mt::vec3f & _dir, const mt::vec3f & _up, float _sign ) = 0;

    public:
		// Render frame into _image
		// int rect[4] - rectangle represents desired frame area in pixels
		virtual bool screenshot( const RenderImageInterfacePtr & _image, const mt::vec4f & _rect ) = 0;

	public:
		// входные данные: матрица 4 на 4
		virtual	void setProjectionMatrix( const mt::mat4f & _projection ) = 0;
		virtual	void setModelViewMatrix( const mt::mat4f & _view ) = 0;
		virtual	void setWorldMatrix( const mt::mat4f & _view ) = 0;

		virtual void setTextureMatrix( uint32_t _stage, const float * _texture ) = 0;

	public:
		virtual RenderVertexBufferInterfacePtr createVertexBuffer( uint32_t _verticesNum, bool _dynamic ) = 0;
		virtual bool setVertexBuffer( const RenderVertexBufferInterfacePtr & _vertexBuffer ) = 0;

		virtual RenderIndexBufferInterfacePtr createIndexBuffer( uint32_t _indiciesNum, bool _dynamic ) = 0;
		virtual bool setIndexBuffer( const RenderIndexBufferInterfacePtr & _indexBuffer ) = 0;
		
	public:
		virtual RenderShaderInterfacePtr createFragmentShader( const ConstString & _name, const void * _buffer, size_t _size, bool _isCompile ) = 0;
		virtual RenderShaderInterfacePtr createVertexShader( const ConstString & _name, const void * _buffer, size_t _size, bool _isCompile ) = 0;
						
		virtual RenderProgramInterfacePtr createProgram( const ConstString & _name, const RenderShaderInterfacePtr & _fragment, const RenderShaderInterfacePtr & _vertex, uint32_t _samplerCount ) = 0;
		virtual void setProgram( const RenderProgramInterfacePtr & _program ) = 0;
        
	public:
		virtual void drawIndexedPrimitive( EPrimitiveType _type, uint32_t _baseVertexIndex,
			uint32_t _minIndex, uint32_t _verticesNum, uint32_t _startIndex, uint32_t _indexCount ) = 0;

		virtual void setTexture( uint32_t _stage, const RenderImageInterfacePtr & _texture ) = 0;
		virtual void setTextureAddressing( uint32_t _stage, ETextureAddressMode _modeU, ETextureAddressMode _modeV ) = 0;
		virtual void setTextureFactor( uint32_t _color ) = 0;
		virtual void setBlendFactor( EBlendFactor _src, EBlendFactor _dst ) = 0;
		virtual void setCullMode( ECullMode _mode ) = 0;
		virtual void setDepthBufferTestEnable( bool _depthTest ) = 0;
		virtual void setDepthBufferWriteEnable( bool _depthWrite ) = 0;
		virtual void setDepthBufferCmpFunc( ECompareFunction _depthFunction ) = 0;
		virtual void setFillMode( EFillMode _mode ) = 0;
		virtual void setColorBufferWriteEnable( bool _r, bool _g, bool _b, bool _a ) = 0;
		virtual void setShadeType( EShadeType _sType ) = 0;
		virtual void setAlphaBlendEnable( bool _alphaBlend ) = 0;
		virtual void setAlphaCmpFunc( ECompareFunction _alphaFunc, uint8_t _alpha ) = 0;
		virtual void setLightingEnable( bool _light ) = 0;
		virtual void setTextureStageColorOp( uint32_t _stage, ETextureOp _textrueOp,
												ETextureArgument _arg1, ETextureArgument _arg2 ) = 0;
		virtual void setTextureStageAlphaOp( uint32_t _stage, ETextureOp _textrueOp,
												ETextureArgument _arg1, ETextureArgument _arg2 ) = 0;

        virtual void setTextureStageTexCoordIndex( uint32_t _stage, uint32_t _index ) = 0;
		virtual void setTextureStageFilter( uint32_t _stage, ETextureFilter _minification, ETextureFilter _mipmap, ETextureFilter _magnification ) = 0;

		virtual RenderImageInterfacePtr createImage( uint32_t _mipmaps, uint32_t _width, uint32_t _height, uint32_t _channels, uint32_t _depth, PixelFormat _format ) = 0;
		virtual RenderImageInterfacePtr createDynamicImage( uint32_t _width, uint32_t _height, uint32_t _channels, uint32_t _depth, PixelFormat _format ) = 0;

		virtual RenderTargetInterface * createRenderTargetOffscreen( uint32_t _width, uint32_t _height, PixelFormat _format ) = 0;

		//
		// отрисовка изображения

		virtual bool beginScene() = 0;
		virtual void endScene() = 0;
		virtual void swapBuffers() = 0;
		virtual void clearFrameBuffer( uint32_t _frameBufferTypes, uint32_t _color, float _depth, uint32_t _stencil ) = 0;

		virtual void setClipplaneCount( uint32_t _count ) = 0;
		virtual void setClipplane( uint32_t _i, const mt::planef & _plane ) = 0;

		virtual void setViewport( const Viewport & _viewport ) = 0;

		virtual void changeWindowMode( const Resolution & _resolution, bool _fullscreen ) = 0;
		
		virtual bool lockRenderTarget( const RenderImageInterfacePtr & _renderTarget ) = 0;
		virtual bool unlockRenderTarget() = 0;

		//new
		//virtual LightInterface * createLight( const String & _name ) = 0;
		//virtual void releaseLight( LightInterface * _light ) = 0;

		virtual void onWindowMovedOrResized() = 0;
		virtual void onWindowClose() = 0;

		virtual void setVSync( bool _vSync ) = 0;
		virtual void clear( uint32_t _color, bool _force ) = 0;

        virtual bool supportTextureFormat( PixelFormat _format ) const = 0;
		virtual bool supportTextureNonPow2() const = 0;

		virtual void setSeparateAlphaBlendMode() = 0;
	};
	//////////////////////////////////////////////////////////////////////////
#   define RENDER_SYSTEM( serviceProvider )\
    ((Menge::RenderSystemInterface*)SERVICE_GET(serviceProvider, Menge::RenderSystemInterface))
	//////////////////////////////////////////////////////////////////////////
    struct RenderDebugInfo
    {
        uint32_t frameCount;
        uint32_t dips;

        double fillrate;
        uint32_t object;
        uint32_t triangle;
		uint32_t batch;
    };
	//////////////////////////////////////////////////////////////////////////
	enum ERenderBatchMode
	{
		ERBM_NONE = 0,
		ERBM_NORMAL = 1,
		ERBM_SMART = 2
	};
	//////////////////////////////////////////////////////////////////////////
	struct RenderObjectState
	{
		const RenderViewportInterface * viewport;
		const RenderCameraInterface * camera;
		const RenderClipplaneInterface * clipplane;
		const RenderTargetInterface * target;
	};
	//////////////////////////////////////////////////////////////////////////
	class RenderServiceInterface
		: public ServiceInterface
	{
        SERVICE_DECLARE("RenderService")

    public:
		virtual void addRenderObject( const RenderObjectState * _state, const RenderMaterialInterfacePtr & _material
            , const RenderVertex2D * _vertices, uint32_t _verticesNum
            , const RenderIndices * _indices, uint32_t _indicesNum
			, const mt::box2f * _bb, bool _debug ) = 0;

		virtual void addRenderQuad( const RenderObjectState * _state, const RenderMaterialInterfacePtr & _material
            , const RenderVertex2D * _vertices, uint32_t _verticesNum
			, const mt::box2f * _bb, bool _debug ) = 0;

		virtual void addRenderLine( const RenderObjectState * _state, const RenderMaterialInterfacePtr & _material
            , const RenderVertex2D * _vertices, uint32_t _verticesNum
			, const mt::box2f * _bb, bool _debug ) = 0;

	public:
		virtual RenderVertex2D * getDebugRenderVertex2D( size_t _count ) = 0;

	public:
		virtual void setBatchMode( ERenderBatchMode _mode ) = 0;
		virtual ERenderBatchMode getBatchMode() const = 0;

	public:
		virtual bool createRenderWindow( const Resolution & _resolution, const Resolution & _contentResolution, const Viewport & _renderViewport, uint32_t _bits, bool _fullscreen, 
			int _FSAAType, int _FSAAQuality, uint32_t _MultiSampleCount ) = 0;

		virtual void changeWindowMode( const Resolution & _resolution, const Resolution & _contentResolution, const Viewport & _renderViewport, bool _fullscreen ) = 0;

	public:
		virtual void clear( uint32_t _color ) = 0;
		virtual bool beginScene() = 0;
		virtual void endScene() = 0;
		virtual void swapBuffers() = 0;
		virtual void screenshot( const RenderTextureInterfacePtr & _renderTargetImage, const mt::vec4f & _rect ) = 0;		
		virtual void setVSync( bool _vSync ) = 0;
		virtual bool getVSync() const = 0;
		virtual void setSeparateAlphaBlendMode() = 0;

    public:
        virtual void makeProjectionOrthogonal( mt::mat4f & _projectionMatrix, const Viewport & _viewport, float _near, float _far ) = 0;
        virtual void makeProjectionFrustum( mt::mat4f & _projectionMatrix, const Viewport & _viewport, float _near, float _far ) = 0;
        virtual void makeProjectionPerspective( mt::mat4f & _projectionMatrix, float _fov, float _aspect, float zn, float zf ) = 0;
        virtual void makeViewMatrixFromViewport( mt::mat4f & _viewMatrix, const Viewport & _viewport ) = 0;
        virtual void makeViewMatrixLookAt( mt::mat4f & _viewMatrix, const mt::vec3f & _eye, const mt::vec3f & _dir, const mt::vec3f & _up, float _sign ) = 0;

	//public:
	//	virtual bool loadTextureRectImageData( const RenderTextureInterfacePtr & _texture, const Rect & _rect, const ImageDecoderInterfacePtr & _imageDecoder ) = 0;

   public:
		virtual void enableDebugStepRenderMode( bool _enable ) = 0;
		virtual bool isDebugStepRenderMode() const = 0;

		virtual void enableRedAlertMode( bool _enable ) = 0;
		virtual bool isRedAlertMode() const = 0;

		virtual void endLimitRenderObjects() = 0;
		virtual void increfLimitRenderObjects() = 0;
		virtual bool decrefLimitRenderObjects() = 0;

    public:
        virtual void onWindowClose() = 0;
        virtual bool isWindowCreated() const = 0;

    public:
        virtual const RenderDebugInfo & getDebugInfo() const = 0;
        virtual void resetFrameCount() = 0;
	};

#   define RENDER_SERVICE( serviceProvider )\
    ((Menge::RenderServiceInterface*)SERVICE_GET(serviceProvider, Menge::RenderServiceInterface))
}
