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

#   ifndef MENGE_MAX_TEXTURE_STAGES
#	define MENGE_MAX_TEXTURE_STAGES 2
#   endif

namespace Menge
{
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
		TF_GAUSSIANCUBIC
	};

    const unsigned int LOCK_READONLY = 0x00000010L;
    const unsigned int LOCK_DISCARD = 0x00002000L;
    const unsigned int LOCK_NOOVERWRITE = 0x00001000L;
    const unsigned int LOCK_NOSYSLOCK = 0x00000800L;

	typedef size_t VBHandle; // Vertex Buffer Handle
	typedef size_t IBHandle; // Index Buffer Handle
	//////////////////////////////////////////////////////////////////////////
	enum ERenderImageMode
	{
		ERIM_NORMAL,
		ERIM_DYNAMIC,
		ERIM_RENDER_TARGET
	};
	//////////////////////////////////////////////////////////////////////////
	class RenderImageInterface
		: public FactorablePtr
	{
	public:
		virtual ERenderImageMode getMode() const = 0;

	public:
		virtual size_t getHWWidth() const = 0;
		virtual size_t getHWHeight() const = 0;
		virtual size_t getHWChannels() const = 0;

		virtual PixelFormat getHWPixelFormat() const = 0;

	public:	
		virtual unsigned char* lock( int* _pitch, const Rect& _rect, bool _readOnly = true ) = 0;
		virtual void unlock() = 0;
	};
	//////////////////////////////////////////////////////////////////////////
	typedef stdex::intrusive_ptr<RenderImageInterface> RenderImageInterfacePtr;
	//////////////////////////////////////////////////////////////////////////
	class RenderTextureInterface
		: public FactorablePtr
	{
	public:
		virtual const RenderImageInterfacePtr & getImage() const = 0;

		virtual size_t getId() const = 0;

		virtual const Rect & getRect() const = 0;
		virtual const Rect & getHWRect() const = 0;

		virtual const mt::vec4f & getUV() const = 0;

		virtual void setFileName( const FilePath & _filename ) = 0;
		virtual const FilePath & getFileName() const = 0;

		virtual size_t getWidth() const = 0;
		virtual size_t getHeight() const = 0;
		virtual size_t getChannels() const = 0;

		virtual unsigned char* lock( int * _pitch, const Rect & _rect, bool _readOnly = true ) const = 0;
		virtual void unlock() const = 0;

		virtual size_t getMemoryUse() const = 0;
	};
	//////////////////////////////////////////////////////////////////////////
	typedef stdex::intrusive_ptr<RenderTextureInterface> RenderTextureInterfacePtr;
	//////////////////////////////////////////////////////////////////////////
	class RenderTextureInterfaceListener
	{
	public:
		virtual void onRenderTextureRelease( const RenderTextureInterface * _texture ) = 0;
	};
    //////////////////////////////////////////////////////////////////////////
    struct RenderTextureStage
    {
        RenderTextureStage()
			: addressU(TAM_CLAMP)
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

		ETextureAddressMode addressU;
		ETextureAddressMode addressV;

        ETextureOp colorOp;
        ETextureArgument colorArg1;
        ETextureArgument colorArg2;
        ETextureOp alphaOp;
        ETextureArgument alphaArg1;
        ETextureArgument alphaArg2;

        size_t texCoordIndex;
    };
	//////////////////////////////////////////////////////////////////////////
	struct RenderStage
	{		
		RenderTextureStage textureStage[MENGE_MAX_TEXTURE_STAGES];

		EBlendFactor blendSrc;
		EBlendFactor blendDst;

		bool depthBufferWriteEnable;
		bool alphaTestEnable;
		bool alphaBlendEnable;
	};
    //////////////////////////////////////////////////////////////////////////
    class RenderMaterialInterface
		: public FactorablePtr
    {
    };
	//////////////////////////////////////////////////////////////////////////
	typedef stdex::intrusive_ptr<RenderMaterialInterface> RenderMaterialInterfacePtr;
    //////////////////////////////////////////////////////////////////////////
    class RenderShaderInterface
    {
    };
    //////////////////////////////////////////////////////////////////////////
    class RenderMaterialServiceInterface
        : public ServiceInterface
    {
        SERVICE_DECLARE("RenderMaterialManager")

    public:
        virtual bool initialize() = 0;
        virtual void finalize() = 0;

    public:
        virtual bool loadMaterials( const ConstString& _pakName, const FilePath& _filename ) = 0;

	public:
		virtual RenderMaterialInterfacePtr getMaterial( const ConstString & _stageName
			, bool _wrapU
			, bool _wrapV
			, EPrimitiveType _primitiveType
			, size_t _textureCount
			, const RenderTextureInterfacePtr * _textures ) = 0;
    };
    //////////////////////////////////////////////////////////////////////////
#   define RENDERMATERIAL_SERVICE( serviceProvider )\
    (Menge::Helper::getService<Menge::RenderMaterialServiceInterface>(serviceProvider))
    //////////////////////////////////////////////////////////////////////////
    const int VDECL_XYZ = 0x002;
    const int VDECL_XYZRHW = 0x004;
    const int VDECL_NORMAL = 0x010;
    const int VDECL_DIFFUSE = 0x040;
    const int VDECL_SPECULAR = 0x080;
    const int VDECL_TEX1 = 0x100;
    const int VDECL_TEX2 = 0x200;
    //////////////////////////////////////////////////////////////////////////
    struct RenderVertex2D
    {
        mt::vec3f pos;
        uint32_t color;
        mt::vec2f uv;
        mt::vec2f uv2;
    };
    //////////////////////////////////////////////////////////////////////////
    static const uint32_t Vertex2D_declaration = VDECL_XYZ | VDECL_DIFFUSE | VDECL_TEX2;
    //////////////////////////////////////////////////////////////////////////
    typedef std::vector<RenderVertex2D> TVectorRenderVertex2D;
    //////////////////////////////////////////////////////////////////////////
    struct RenderTextureDebugInfo
    {
        size_t textureMemory;
        size_t textureCount;
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
        virtual bool initialize() = 0;
        virtual void finalize() = 0;

    public:
        virtual RenderTextureInterfacePtr loadTexture( const ConstString& _pakName, const FilePath& _filename, const ConstString& _codec ) = 0;

    public:
        virtual RenderTextureInterfacePtr createTexture( size_t _width, size_t _height, size_t _channels, PixelFormat _format ) = 0;
        virtual RenderTextureInterfacePtr createDynamicTexture( size_t _width, size_t _height, size_t _channels, PixelFormat _format ) = 0;
        virtual RenderTextureInterfacePtr createRenderTargetTexture( size_t _width, size_t _height, size_t _channels, PixelFormat _format ) = 0;

	public:
        virtual RenderTextureInterfacePtr getTexture( const FilePath & _filename ) const = 0;
        
        virtual bool hasTexture( const FilePath & _filename, RenderTextureInterfacePtr * _texture ) const = 0;

    public:
        //virtual void sweezleAlpha( RenderTextureInterface * _texture, unsigned char * _textureBuffer, size_t _texturePitch ) = 0;
        virtual void imageQuality( const RenderTextureInterfacePtr & _texture, unsigned char * _textureBuffer, size_t _texturePitch ) = 0;

        virtual void cacheFileTexture( const FilePath& _filename, const RenderTextureInterfacePtr & _texture ) = 0;

    public:
        virtual bool saveImage( const RenderTextureInterfacePtr & _texture, const ConstString& _fileSystemName, const ConstString & _codecName, const FilePath & _filename ) = 0;

    public:
        virtual void visitTexture( VisitorRenderTextureInterface * _visitor ) const = 0;

    public:
        virtual const RenderTextureDebugInfo & getDebugInfo() = 0;
    };
    //////////////////////////////////////////////////////////////////////////
#   define RENDERTEXTURE_SERVICE( serviceProvider )\
    (Menge::Helper::getService<Menge::RenderTextureServiceInterface>(serviceProvider))
	//////////////////////////////////////////////////////////////////////////
	class RenderViewportInterface
	{
	public:
		virtual const Viewport & getViewport() const = 0;
	};
	//////////////////////////////////////////////////////////////////////////
	class RenderCameraInterface
	{
	public:
		virtual const mt::mat4f & getCameraWorldMatrix() const = 0;
		virtual const mt::mat4f & getCameraProjectionMatrix() const = 0;
		virtual const mt::mat4f & getCameraViewMatrix() const = 0;

	public:
		virtual const Viewport & getCameraRenderport() const = 0;

	public:
		virtual const ConstString & getRenderTarget() const = 0;

	public:
		virtual bool isOrthogonalProjection() const = 0;
	};
	//////////////////////////////////////////////////////////////////////////
	class RenderSystemListener        
	{
	public:
        virtual void onRenderSystemDeviceLost() = 0;
		virtual bool onRenderSystemDeviceRestored() = 0;
	};
	//////////////////////////////////////////////////////////////////////////
	class RenderSystemInterface
        : public ServiceInterface
	{
        SERVICE_DECLARE("RenderSystem")

	public:
		virtual bool initialize() = 0;
        virtual void finalize() = 0;

    public:
        virtual void setRenderListener( RenderSystemListener * _listener ) = 0;

		virtual bool createRenderWindow( const Resolution & _resolution, int _bits, bool _fullscreen, WindowHandle _winHandle,
			bool _waitForVSync, int _FSAAType, int _FSAAQuality ) = 0;

    public:
        virtual void makeProjectionOrthogonal( mt::mat4f & _projectionMatrix, const Viewport & _viewport, float _near, float _far ) = 0;
        virtual void makeProjectionFrustum( mt::mat4f & _projectionMatrix, const Viewport & _viewport, float _near, float _far ) = 0;
        virtual void makeProjectionPerspective( mt::mat4f & _projectionMatrix, float _fov, float _aspect, float zn, float zf ) = 0;
        virtual void makeViewMatrixFromViewport( mt::mat4f & _viewMatrix, const Viewport & _viewport ) = 0;
        virtual void makeViewMatrixLookAt( mt::mat4f & _viewMatrix, const mt::vec3f & _eye, const mt::vec3f & _dir, const mt::vec3f & _up, float _sign ) = 0;

    public:
		// Render frame into _image
		// int rect[4] - rectangle represents desired frame area in pixels
		virtual bool screenshot( const RenderImageInterfacePtr & _image, const float * _rect ) = 0;
		// входные данные: матрица 4 на 4
		virtual	void setProjectionMatrix( const mt::mat4f & _projection ) = 0;
		virtual	void setModelViewMatrix( const mt::mat4f & _view ) = 0;
		virtual	void setWorldMatrix( const mt::mat4f & _view ) = 0;

		virtual void setTextureMatrix( size_t _stage, const float* _texture ) = 0;

		virtual VBHandle createVertexBuffer( size_t _verticesNum, size_t _vertexSize, bool _dynamic ) = 0;
		virtual void releaseVertexBuffer( VBHandle _vbHandle ) = 0;
		virtual void * lockVertexBuffer(  VBHandle _vbHandle, size_t _offset, size_t _size, uint32_t _flags ) = 0;
		virtual bool unlockVertexBuffer( VBHandle _vbHandle ) = 0;
		virtual void setVertexBuffer( VBHandle _vbHandle ) = 0;

		virtual IBHandle createIndexBuffer( size_t _indiciesNum, bool _dynamic ) = 0;
		virtual void releaseIndexBuffer( IBHandle _ibHandle ) = 0;
		virtual void * lockIndexBuffer( IBHandle _ibHandle, size_t _offset, size_t _size, uint32_t _flags ) = 0;
		virtual bool unlockIndexBuffer( IBHandle _ibHandle ) = 0;
		virtual void setIndexBuffer( IBHandle _ibHandle, size_t _baseVertexIndex ) = 0;

		virtual void setVertexDeclaration( size_t _vertexSize, uint32_t _declaration ) = 0;

        virtual RenderShaderInterface * createShader( const void * _code, size_t _len ) = 0;
        virtual void setShader( RenderShaderInterface * _shader ) = 0;

		virtual void drawIndexedPrimitive( EPrimitiveType _type, size_t _baseVertexIndex,
			size_t _minIndex, size_t _verticesNum, size_t _startIndex, size_t _indexCount ) = 0;

		virtual void setTexture( size_t _stage, const RenderImageInterfacePtr & _texture ) = 0;
		virtual void setTextureAddressing( size_t _stage, ETextureAddressMode _modeU, ETextureAddressMode _modeV ) = 0;
		virtual void setTextureFactor( uint32_t _color ) = 0;
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
		virtual void setAlphaCmpFunc( ECompareFunction _alphaFunc, uint8_t _alpha ) = 0;
		virtual void setLightingEnable( bool _light ) = 0;
		virtual void setTextureStageColorOp( size_t _stage, ETextureOp _textrueOp,
												ETextureArgument _arg1, ETextureArgument _arg2 ) = 0;
		virtual void setTextureStageAlphaOp( size_t _stage, ETextureOp _textrueOp,
												ETextureArgument _arg1, ETextureArgument _arg2 ) = 0;

        virtual void setTextureStageTexCoordIndex( size_t _stage, size_t _index ) = 0;
		virtual void setTextureStageFilter( size_t _stage, ETextureFilterType _filterType, ETextureFilter _filter ) = 0;
		// create texture
		// [in/out] _width ( desired texture width, returns actual texture width )
		// [in/out] _height ( desired texture height, returns actual texture height )
		// [in/out] _format ( desired texture pixel format, returns actual texture pixel format )
		// returns Texture interface handle or NULL if fails
		virtual RenderImageInterfacePtr createImage( size_t _width, size_t _height, size_t _channels, PixelFormat _format ) = 0;
		virtual RenderImageInterfacePtr createDynamicImage( size_t _width, size_t _height, size_t _channels, PixelFormat _format ) = 0;
		// create render target image
		// [in/out] _width ( desired texture width, returns actual texture width )
		// [in/out] _height ( desired texture height, returns actual texture height )
		// returns Texture interface handle or NULL if fails
		virtual RenderImageInterfacePtr createRenderTargetImage( size_t _width, size_t _height, size_t _channels, PixelFormat _format ) = 0;

		//
		// отрисовка изображения

		
		virtual bool beginScene() = 0;
		virtual void endScene() = 0;
		virtual void swapBuffers() = 0;
		virtual void clearFrameBuffer( uint32_t _frameBufferTypes
											, uint32_t _color = 0
											, float _depth = 1.0f
											, uint16_t _stencil = 0 ) = 0;

		virtual void setViewport( const Viewport & _viewport ) = 0;

		virtual void changeWindowMode( const Resolution & _resolution, bool _fullscreen ) = 0;
		virtual bool setRenderTarget( const RenderImageInterfacePtr & _renderTarget, bool _clear ) = 0;

		//new
		//virtual LightInterface * createLight( const String & _name ) = 0;
		//virtual void releaseLight( LightInterface * _light ) = 0;

		virtual void onWindowMovedOrResized() = 0;
		virtual void onWindowClose() = 0;

		virtual void setVSync( bool _vSync ) = 0;
		virtual void clear( uint32_t _color ) = 0;

        virtual bool supportTextureFormat( PixelFormat _format ) const = 0;

		virtual void setSeparateAlphaBlendMode() = 0;
	};

#   define RENDER_SYSTEM( serviceProvider )\
    (Menge::Helper::getService<Menge::RenderSystemInterface>(serviceProvider))

    struct RenderDebugInfo
    {
        size_t frameCount;
        size_t dips;

        double fillrate;
        size_t object;
        size_t triangle;
		size_t batch;
        //size_t megatextures;
    };

	enum ERenderBatchMode
	{
		ERBM_NONE,
		ERBM_NORMAL,
		ERBM_SMART
	};

	class RenderServiceInterface
		: public ServiceInterface
	{
        SERVICE_DECLARE("RenderService")

    public:
        virtual bool initialize( size_t _maxVertexCount, size_t _maxIndexCount ) = 0;
        virtual void finalize() = 0;

    public:
        virtual void addRenderObject( const RenderViewportInterface * _viewport, const RenderCameraInterface * _camera, const RenderMaterialInterfacePtr & _material
            , const RenderVertex2D * _vertices, size_t _verticesNum
            , const uint16_t * _indices, size_t _indicesNum
			, const mt::box2f * _bb ) = 0;

        virtual void addRenderQuad( const RenderViewportInterface * _viewport, const RenderCameraInterface * _camera, const RenderMaterialInterfacePtr & _material
            , const RenderVertex2D * _vertices, size_t _verticesNum
			, const mt::box2f * _bb ) = 0;

        virtual void addRenderLine( const RenderViewportInterface * _viewport, const RenderCameraInterface * _camera, const RenderMaterialInterfacePtr & _material
            , const RenderVertex2D * _vertices, size_t _verticesNum
			, const mt::box2f * _bb ) = 0;

	public:
		virtual void setDebugMaterial( const RenderMaterialInterfacePtr & _debugMaterial ) = 0;
		virtual const RenderMaterialInterfacePtr & getDebugMaterial() const = 0;
		virtual RenderVertex2D * getDebugRenderVertex2D( size_t _count ) = 0;

	public:
		virtual void setBatchMode( ERenderBatchMode _mode ) = 0;
		virtual ERenderBatchMode getBatchMode() const = 0;

	public:
		virtual bool createRenderWindow( const Resolution & _resolution, const Resolution & _contentResolution, const Viewport & _viewport, int _bits, bool _fullscreen, 
			WindowHandle _winHandle, int _FSAAType , int _FSAAQuality ) = 0;

		virtual void changeWindowMode( const Resolution & _resolution, const Resolution & _contentResolution, const Viewport & _viewport, bool _fullscreen ) = 0;

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
		virtual void setRenderTargetTexture( const RenderTextureInterfacePtr & _texture, bool _clear ) = 0;

   public:
        virtual void enableDebugMode( bool _enable ) = 0;

    public:
        virtual void onWindowClose() = 0;
        virtual bool isWindowCreated() const = 0;

    public:
        virtual const RenderDebugInfo & getDebugInfo() const = 0;
        virtual void resetFrameCount() = 0;

        virtual size_t getMemorySize( size_t _width, size_t _height, size_t _depth, PixelFormat _format ) const = 0;
	};

#   define RENDER_SERVICE( serviceProvider )\
    (Menge::Helper::getService<Menge::RenderServiceInterface>(serviceProvider))
}
