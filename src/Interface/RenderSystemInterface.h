#pragma once

#include "Interface/ServiceInterface.h"
#include "Interface/ServantInterface.h"
#include "Interface/MemoryInterface.h"
#include "Interface/RenderEnumInterface.h"

#include "Factory/Factorable.h"

#include "Core/Viewport.h"
#include "Core/Resolution.h"
#include "Core/Rect.h"
#include "Core/ConstString.h"
#include "Core/FilePath.h"
#include "Core/PixelFormat.h"
#include "Core/Pointer.h"
#include "Core/RenderVertex2D.h"
#include "Core/RenderIndex.h"

#include "Config/Typedef.h"

#include "stdex/intrusive_ptr.h"

#include "math/mat4.h"
#include "math/uv4.h"
#include "math/plane.h"

namespace Mengine
{
	//////////////////////////////////////////////////////////////////////////
	typedef uint32_t VBHandle; // Vertex Buffer Handle
	typedef uint32_t IBHandle; // Index Buffer Handle
    //////////////////////////////////////////////////////////////////////////
    struct RenderImageDesc
    {
        uint32_t mipmaps;
        uint32_t width;
        uint32_t height;
        uint32_t channels;
        uint32_t depth;
        PixelFormat format;
    };
	//////////////////////////////////////////////////////////////////////////
	typedef IntrusivePtr<class RenderImageInterface> RenderImageInterfacePtr;
	//////////////////////////////////////////////////////////////////////////
	class RenderImageLoaderInterface
		: public ServantInterface
	{
	public:
		virtual RenderImageDesc getImageDesc() const = 0;

	public:
		virtual bool load( const RenderImageInterfacePtr & _image ) const = 0;
        //virtual bool load( void * _buffer, uint32_t _pitch ) const = 0;
	};
	//////////////////////////////////////////////////////////////////////////
	typedef IntrusivePtr<RenderImageLoaderInterface> RenderImageLoaderInterfacePtr;
    //////////////////////////////////////////////////////////////////////////
    class RenderImageProviderInterface
        : public ServantInterface
    {
    public:
        virtual RenderImageLoaderInterfacePtr getLoader() const = 0;
    };
    //////////////////////////////////////////////////////////////////////////
    typedef IntrusivePtr<RenderImageProviderInterface> RenderImageProviderInterfacePtr;
	//////////////////////////////////////////////////////////////////////////
	class RenderImageInterface
		: public ServantInterface
	{
    public:
        virtual void setRenderImageProvider( const RenderImageProviderInterfacePtr & _renderImageProvider ) = 0;
        virtual const RenderImageProviderInterfacePtr & getRenderImageProvider() const = 0;

	public:
		virtual ERenderImageMode getMode() const = 0;
		
	public:
        virtual uint32_t getHWMipmaps() const = 0;
		virtual uint32_t getHWWidth() const = 0;
		virtual uint32_t getHWHeight() const = 0;
		virtual uint32_t getHWChannels() const = 0;
		virtual uint32_t getHWDepth() const = 0;

		virtual PixelFormat getHWPixelFormat() const = 0;

    public:
        virtual float getHWWidthInv() const = 0;
        virtual float getHWHeightInv() const = 0;
		
	public:
		virtual Pointer lock( size_t * _pitch, uint32_t _level, const Rect& _rect, bool _readOnly = true ) = 0;
		virtual bool unlock( uint32_t _level, bool _successful ) = 0;
	};
	//////////////////////////////////////////////////////////////////////////
	typedef IntrusivePtr<RenderImageInterface> RenderImageInterfacePtr;
	//////////////////////////////////////////////////////////////////////////
	class RenderTextureInterface
		: public ServantInterface
	{
	public:
		virtual void release() = 0;

	public:
		virtual const RenderImageInterfacePtr & getImage() const = 0;

	public:
		virtual uint32_t getId() const = 0;
        
    public:
		virtual const Rect & getRect() const = 0;

		virtual const mt::uv4f & getUV() const = 0;

		virtual void setCategory( const ConstString & _category ) = 0;
		virtual const ConstString & getCategory() const = 0;

		virtual void setFileName( const FilePath & _fileName ) = 0;
		virtual const FilePath & getFileName() const = 0;

    public:
		virtual uint32_t getWidth() const = 0;
		virtual uint32_t getHeight() const = 0;

    public:
        virtual float getWidthInv() const = 0;
        virtual float getHeightInv() const = 0;

	public:
		virtual bool isPow2() const = 0;
	};
	//////////////////////////////////////////////////////////////////////////
	typedef IntrusivePtr<RenderTextureInterface> RenderTextureInterfacePtr;
    //////////////////////////////////////////////////////////////////////////
    struct RenderTextureStage
    {
        RenderTextureStage()
			: mipmap( TF_NONE )
			, magnification( TF_LINEAR )
			, minification( TF_LINEAR )
			, addressU(TAM_CLAMP)
			, addressV(TAM_CLAMP)
			, addressBorder(0x00000000)
        {
        }

		ETextureFilter mipmap;
		ETextureFilter magnification;
		ETextureFilter minification;

		ETextureAddressMode addressU;
		ETextureAddressMode addressV;
		uint32_t addressBorder;
    };
    //////////////////////////////////////////////////////////////////////////
    class RenderVertexAttributeInterface
        : public ServantInterface
    {
    public:
        virtual const ConstString & getName() const = 0;
        virtual uint32_t getElementSize() const = 0;

    public:
        virtual void addAttribute( const ConstString & _uniform, uint32_t _size, EVertexAttributeType _type, bool _normalized, uint32_t _stride, uint32_t _offset ) = 0;

    public:
        virtual bool enable() = 0;
        virtual void disable() = 0;
    };
    //////////////////////////////////////////////////////////////////////////
    typedef IntrusivePtr<RenderVertexAttributeInterface> RenderVertexAttributeInterfacePtr;
	//////////////////////////////////////////////////////////////////////////
	class RenderFragmentShaderInterface
		: public ServantInterface
	{
	public:
		virtual const ConstString & getName() const = 0;
	};
	//////////////////////////////////////////////////////////////////////////
	typedef IntrusivePtr<RenderFragmentShaderInterface> RenderFragmentShaderInterfacePtr;
	//////////////////////////////////////////////////////////////////////////
	class RenderVertexShaderInterface
		: public ServantInterface
	{
	public:
		virtual const ConstString & getName() const = 0;
	};
	//////////////////////////////////////////////////////////////////////////
	typedef IntrusivePtr<RenderVertexShaderInterface> RenderVertexShaderInterfacePtr;
	//////////////////////////////////////////////////////////////////////////
	class RenderProgramInterface
		: public ServantInterface
	{
	public:
		virtual const ConstString & getName() const = 0;

    public:
        virtual RenderVertexAttributeInterfacePtr getVertexAttribute() const = 0;
		virtual RenderVertexShaderInterfacePtr getVertexShader() const = 0;
		virtual RenderFragmentShaderInterfacePtr getFragmentShader() const = 0;		
	};
	//////////////////////////////////////////////////////////////////////////
	typedef IntrusivePtr<RenderProgramInterface> RenderProgramInterfacePtr;
	//////////////////////////////////////////////////////////////////////////
	struct RenderMaterialStage
	{
		RenderMaterialStage()
			: id(0)
			, blendSrc( BF_SOURCE_ALPHA )
			, blendDst( BF_ONE_MINUS_SOURCE_ALPHA )
			, blendOp( BOP_ADD )
			, alphaBlendEnable( false )
		{
		}

		uint32_t id;

		RenderTextureStage textureStage[MENGINE_MAX_TEXTURE_STAGES];

		RenderProgramInterfacePtr program;

		EBlendFactor blendSrc;
		EBlendFactor blendDst;
		EBlendOp blendOp;

		bool alphaBlendEnable;
	};
    //////////////////////////////////////////////////////////////////////////
    class RenderMaterialInterface
		: public Factorable
    {
	public:
		virtual const ConstString & getName() const = 0;
		virtual uint32_t getId() const = 0;

	public:
		virtual void update( float _time, float _timing ) = 0;

	public:
		virtual EPrimitiveType getPrimitiveType() const = 0;
		virtual uint32_t getTextureCount() const = 0;
		virtual const RenderTextureInterfacePtr & getTexture( uint32_t _index ) const = 0;
		virtual const RenderMaterialStage * getStage() const = 0;
    };
	//////////////////////////////////////////////////////////////////////////
	typedef IntrusivePtr<RenderMaterialInterface> RenderMaterialInterfacePtr;
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
	class RenderVertexBufferInterface
		: public ServantInterface
	{
    public:
        virtual uint32_t getVertexCount() const = 0;
        virtual uint32_t getVertexSize() const = 0;

    public:
        virtual bool enable() = 0;
        virtual void disable() = 0;

    public:
        virtual bool resize( uint32_t _count ) = 0;

	public:
		virtual MemoryInterfacePtr lock( uint32_t _offset, uint32_t _size ) = 0;
		virtual bool unlock() = 0;

    public:
        virtual void draw( const void * _buffer, size_t _size ) = 0;
	};
	//////////////////////////////////////////////////////////////////////////
	typedef IntrusivePtr<RenderVertexBufferInterface> RenderVertexBufferInterfacePtr;
	//////////////////////////////////////////////////////////////////////////
	class RenderIndexBufferInterface
		: public ServantInterface
	{
    public:
        virtual uint32_t getIndexCount() const = 0;
        virtual uint32_t getIndexSize() const = 0;

    public:
        virtual bool enable() = 0;
        virtual void disable() = 0;

    public:
        virtual bool resize( uint32_t _count ) = 0;

	public:
		virtual MemoryInterfacePtr lock( uint32_t _offset, uint32_t _size ) = 0;
		virtual bool unlock() = 0;

    public:
        virtual void draw( const void * _buffer, size_t _size ) = 0;
	};
	//////////////////////////////////////////////////////////////////////////
	typedef IntrusivePtr<RenderIndexBufferInterface> RenderIndexBufferInterfacePtr;
    //////////////////////////////////////////////////////////////////////////
    class RenderMaterialServiceInterface
        : public ServiceInterface
    {
        SERVICE_DECLARE("RenderMaterialManager")

    public:
        virtual bool loadMaterials( const ConstString& _pakName, const FilePath& _fileName ) = 0;
		virtual bool unloadMaterials( const ConstString& _pakName, const FilePath& _fileName ) = 0;

    public:
        virtual const RenderVertexShaderInterfacePtr & getVertexShader( const ConstString & _name ) const = 0;
        virtual const RenderFragmentShaderInterfacePtr & getFragmentShader( const ConstString & _name ) const = 0;
        virtual const RenderVertexAttributeInterfacePtr & getVertexAttribute( const ConstString & _name ) const = 0;

        virtual const RenderProgramInterfacePtr & getProgram( const ConstString & _name ) const = 0;

	public:
		virtual void setDefaultTextureFilter( ETextureFilter _mipmap, ETextureFilter _magnification, ETextureFilter _minification ) = 0;

	public:
		virtual const RenderMaterialStage * cacheStage( const RenderMaterialStage & _other ) = 0;

	public:
		virtual RenderMaterialInterfacePtr getMaterial( const ConstString & _materialName
			, EPrimitiveType _primitiveType
			, uint32_t _textureCount
			, const RenderTextureInterfacePtr * _textures ) = 0;

		virtual RenderMaterialInterfacePtr getMaterial2( const ConstString & _materialName
			, const RenderMaterialStage * _stage
			, EPrimitiveType _primitiveType
			, uint32_t _textureCount
			, const RenderTextureInterfacePtr * _textures ) = 0;

        virtual RenderMaterialInterfacePtr getMaterial3( EMaterial _materialId
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
#   define RENDERMATERIAL_SERVICE()\
    ((Mengine::RenderMaterialServiceInterface*)SERVICE_GET(Mengine::RenderMaterialServiceInterface))
    //////////////////////////////////////////////////////////////////////////
    class VisitorRenderTextureInterface
    {
    public:
        virtual void visitRenderTexture( const RenderTextureInterfacePtr & _texture ) = 0;
    };
    //////////////////////////////////////////////////////////////////////////
    class RenderTextureServiceInterface
        : public ServiceInterface
    {
        SERVICE_DECLARE("RenderTextureManager")

    public:
        virtual RenderTextureInterfacePtr loadTexture( const ConstString& _pakName, const FilePath& _fileName, const ConstString& _codec ) = 0;
		virtual RenderTextureInterfacePtr createRenderTexture( const RenderImageInterfacePtr & _image, uint32_t _width, uint32_t _height ) = 0;

    public:
        virtual RenderTextureInterfacePtr createTexture( uint32_t _mipmaps, uint32_t _width, uint32_t _height, uint32_t _channels, uint32_t _depth, PixelFormat _format ) = 0;
        virtual RenderTextureInterfacePtr createDynamicTexture( uint32_t _width, uint32_t _height, uint32_t _channels, uint32_t _depth, PixelFormat _format ) = 0;
        
	public:
        virtual RenderTextureInterfacePtr getTexture( const ConstString& _pakName, const FilePath & _fileName ) const = 0;
        
        virtual bool hasTexture( const ConstString& _pakName, const FilePath & _fileName, RenderTextureInterfacePtr * _texture ) const = 0;

	public:
		virtual size_t getImageMemoryUse( uint32_t _width, uint32_t _height, uint32_t _channels, uint32_t _depth, PixelFormat _format ) const = 0;

    public:
        virtual void cacheFileTexture( const ConstString& _pakName, const FilePath& _fileName, const RenderTextureInterfacePtr & _texture ) = 0;

    public:
        virtual bool saveImage( const RenderTextureInterfacePtr & _texture, const ConstString& _fileGroupName, const ConstString & _codecName, const FilePath & _fileName ) = 0;

    public:
        virtual void visitTexture( VisitorRenderTextureInterface * _visitor ) const = 0;
    };
    //////////////////////////////////////////////////////////////////////////
#   define RENDERTEXTURE_SERVICE()\
    ((Mengine::RenderTextureServiceInterface*)SERVICE_GET(Mengine::RenderTextureServiceInterface))
	//////////////////////////////////////////////////////////////////////////
	class RenderViewportInterface
        : public Mixin
	{
	public:
		virtual const Viewport & getViewport() const = 0;
	};
    //////////////////////////////////////////////////////////////////////////
    typedef IntrusivePtr<RenderViewportInterface> RenderViewportInterfacePtr;
	//////////////////////////////////////////////////////////////////////////
	class RenderScissorInterface
        : public Mixin
	{
	public:
		virtual const Viewport & getScissorViewport() const = 0;
	};
    //////////////////////////////////////////////////////////////////////////
    typedef IntrusivePtr<RenderScissorInterface> RenderScissorInterfacePtr;
	//////////////////////////////////////////////////////////////////////////
	class RenderCameraInterface
        : public Mixin
	{
	public:
		virtual const mt::mat4f & getCameraViewMatrix() const = 0;
		virtual const mt::mat4f & getCameraViewMatrixInv() const = 0;

		virtual const mt::mat4f & getCameraProjectionMatrix() const = 0;
		virtual const mt::mat4f & getCameraProjectionMatrixInv() const = 0;

	public:
		virtual const mt::mat4f & getCameraViewProjectionMatrix() const = 0;
        virtual const mt::mat4f & getCameraViewProjectionMatrixInv() const = 0;
	};
    //////////////////////////////////////////////////////////////////////////
    typedef IntrusivePtr<RenderCameraInterface> RenderCameraInterfacePtr;
	//////////////////////////////////////////////////////////////////////////
	class RenderTargetInterface
		: public Mixin
	{
    public:
        virtual uint32_t getWidth() const = 0;
        virtual uint32_t getHeight() const = 0;
        virtual PixelFormat getFormat() const = 0;

    public:
        virtual uint32_t getHWWidth() const = 0;
        virtual uint32_t getHWHeight() const = 0;

	public:
		virtual bool begin() = 0;
		virtual void end() = 0;

	public:
		virtual bool getData( unsigned char * _buffer, size_t _pitch ) = 0;
	};
    //////////////////////////////////////////////////////////////////////////
    typedef IntrusivePtr<RenderTargetInterface> RenderTargetInterfacePtr;
	//////////////////////////////////////////////////////////////////////////
	class RenderSystemInterface
        : public ServiceInterface
	{
        SERVICE_DECLARE("RenderSystem")

	public:
        virtual ERenderPlatform getRenderPlatformType() const = 0;
		virtual const ConstString & getRenderPlatformName() const = 0;

    public:
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
		virtual	void setViewMatrix( const mt::mat4f & _view ) = 0;
		virtual	void setWorldMatrix( const mt::mat4f & _view ) = 0;

		virtual void setTextureMatrix( uint32_t _stage, const mt::mat4f & _texture ) = 0;

	public:
		virtual RenderVertexBufferInterfacePtr createVertexBuffer( uint32_t _vertexSize, EBufferType _bufferType ) = 0;
		virtual bool setVertexBuffer( const RenderVertexBufferInterfacePtr & _vertexBuffer ) = 0;

    public:
		virtual RenderIndexBufferInterfacePtr createIndexBuffer( uint32_t _indexSize, EBufferType _bufferType ) = 0;
		virtual bool setIndexBuffer( const RenderIndexBufferInterfacePtr & _indexBuffer ) = 0;
		
	public:
        virtual RenderVertexAttributeInterfacePtr createVertexAttribute( const ConstString & _name, uint32_t _elementSize ) = 0;
		virtual RenderFragmentShaderInterfacePtr createFragmentShader( const ConstString & _name, const MemoryInterfacePtr & _memory, bool _compile ) = 0;
		virtual RenderVertexShaderInterfacePtr createVertexShader( const ConstString & _name, const MemoryInterfacePtr & _memory, bool _compile ) = 0;
						
		virtual RenderProgramInterfacePtr createProgram( const ConstString & _name, const RenderVertexShaderInterfacePtr & _vertex, const RenderFragmentShaderInterfacePtr & _fragment, const RenderVertexAttributeInterfacePtr & _vertexAttribute, uint32_t _samplerCount ) = 0;
		virtual void setProgram( const RenderProgramInterfacePtr & _program ) = 0;
		virtual void updateProgram( const RenderProgramInterfacePtr & _program ) = 0;
        
	public:
		virtual void drawIndexedPrimitive( EPrimitiveType _type, uint32_t _baseVertexIndex,
			uint32_t _minIndex, uint32_t _vertexCount, uint32_t _startIndex, uint32_t _indexCount ) = 0;

		virtual void setTexture( uint32_t _stage, const RenderImageInterfacePtr & _texture ) = 0;
		virtual void setTextureAddressing( uint32_t _stage, ETextureAddressMode _modeU, ETextureAddressMode _modeV, uint32_t _border ) = 0;
		virtual void setTextureFactor( uint32_t _color ) = 0;
		virtual void setBlendFactor( EBlendFactor _src, EBlendFactor _dst, EBlendOp _op ) = 0;
		virtual void setCullMode( ECullMode _mode ) = 0;
		virtual void setDepthBufferTestEnable( bool _depthTest ) = 0;
		virtual void setDepthBufferWriteEnable( bool _depthWrite ) = 0;
		virtual void setDepthBufferCmpFunc( ECompareFunction _depthFunction ) = 0;
		virtual void setFillMode( EFillMode _mode ) = 0;
		virtual void setColorBufferWriteEnable( bool _r, bool _g, bool _b, bool _a ) = 0;
		virtual void setAlphaBlendEnable( bool _alphaBlend ) = 0;

    public:
		virtual void setTextureStageFilter( uint32_t _stage, ETextureFilter _minification, ETextureFilter _mipmap, ETextureFilter _magnification ) = 0;

    public:
		virtual RenderImageInterfacePtr createImage( uint32_t _mipmaps, uint32_t _width, uint32_t _height, uint32_t _channels, uint32_t _depth, PixelFormat _format ) = 0;
		virtual RenderImageInterfacePtr createDynamicImage( uint32_t _width, uint32_t _height, uint32_t _channels, uint32_t _depth, PixelFormat _format ) = 0;

        virtual RenderTargetInterfacePtr createRenderTargetTexture( uint32_t _width, uint32_t _height, PixelFormat _format ) = 0;
		virtual RenderTargetInterfacePtr createRenderTargetOffscreen( uint32_t _width, uint32_t _height, PixelFormat _format ) = 0;

        virtual RenderImageInterfacePtr createRenderTargetImage( const RenderTargetInterfacePtr & _renderTarget ) = 0;

    public:
		virtual bool beginScene() = 0;
		virtual void endScene() = 0;
		virtual void swapBuffers() = 0;
		virtual void clearFrameBuffer( uint32_t _frameBufferTypes, uint32_t _color, float _depth, uint32_t _stencil ) = 0;

		virtual void setScissor( const Viewport & _viewport ) = 0;
        virtual void removeScissor() = 0;

		virtual void setViewport( const Viewport & _viewport ) = 0;

		virtual void changeWindowMode( const Resolution & _resolution, bool _fullscreen ) = 0;
		
		virtual bool lockRenderTarget( const RenderImageInterfacePtr & _renderTarget ) = 0;
		virtual bool unlockRenderTarget() = 0;

		//new
		//virtual LightInterface * createLight( const String & _name ) = 0;
		//virtual void releaseLight( LightInterface * _light ) = 0;

		virtual void onWindowMovedOrResized() = 0;
		virtual void onWindowClose() = 0;

		virtual void onWindowChangeFullscreenPrepare( bool _fullscreen ) = 0;
		virtual void onWindowChangeFullscreen( bool _fullscreen ) = 0;

		virtual void setVSync( bool _vSync ) = 0;
		virtual void clear( uint32_t _color, bool _force ) = 0;

        virtual bool supportTextureFormat( PixelFormat _format ) const = 0;
		virtual bool supportTextureNonPow2() const = 0;
		virtual uint32_t getMaxCombinedTextureImageUnits() const = 0;

		virtual void setSeparateAlphaBlendMode() = 0;

		virtual size_t getTextureMemoryUse() const = 0;
		virtual uint32_t getTextureCount() const = 0;
	};
	//////////////////////////////////////////////////////////////////////////
#   define RENDER_SYSTEM()\
    ((Mengine::RenderSystemInterface*)SERVICE_GET(Mengine::RenderSystemInterface))
	//////////////////////////////////////////////////////////////////////////
    struct RenderServiceDebugInfo
    {
        uint32_t frameCount;
        uint32_t dips;

        float fillrate;
        uint32_t object;
        uint32_t triangle;
		uint32_t batch;
    };
	//////////////////////////////////////////////////////////////////////////
	struct RenderState
	{
		RenderViewportInterfacePtr viewport;
		RenderCameraInterfacePtr camera;
		RenderScissorInterfacePtr scissor;

		RenderTargetInterfacePtr target;
	};
	//////////////////////////////////////////////////////////////////////////
	class RenderServiceInterface
		: public ServiceInterface
	{
        SERVICE_DECLARE("RenderService")

    public:
        virtual void addRenderMesh( const RenderState * _state, const RenderMaterialInterfacePtr & _material
            , const RenderVertexBufferInterfacePtr & _vertexBuffer
            , const RenderIndexBufferInterfacePtr & _indexBuffer
            , uint32_t _indexCount ) = 0;

		virtual void addRenderObject( const RenderState * _state, const RenderMaterialInterfacePtr & _material
            , const RenderVertex2D * _vertices, uint32_t _vertexCount
            , const RenderIndex * _indices, uint32_t _indicesNum
			, const mt::box2f * _bb, bool _debug ) = 0;

		virtual void addRenderQuad( const RenderState * _state, const RenderMaterialInterfacePtr & _material
            , const RenderVertex2D * _vertices, uint32_t _vertexCount
			, const mt::box2f * _bb, bool _debug ) = 0;

		virtual void addRenderLine( const RenderState * _state, const RenderMaterialInterfacePtr & _material
            , const RenderVertex2D * _vertices, uint32_t _vertexCount
			, const mt::box2f * _bb, bool _debug ) = 0;

	public:
		virtual RenderVertex2D * getDebugRenderVertex2D( size_t _count ) = 0;

	public:
		virtual void setBatchMode( ERenderBatchMode _mode ) = 0;
		virtual ERenderBatchMode getBatchMode() const = 0;

	public:
		virtual bool createRenderWindow( const Resolution & _resolution, const Resolution & _contentResolution, const Viewport & _renderViewport, uint32_t _bits, bool _fullscreen, 
			int _FSAAType, int _FSAAQuality ) = 0;

        virtual void destroyRenderWindow() = 0;

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

   public:
       virtual void enableDebugFillrateCalcMode( bool _enable ) = 0;
       virtual bool isDebugFillrateCalcMode() const = 0;

		virtual void enableDebugStepRenderMode( bool _enable ) = 0;
		virtual bool isDebugStepRenderMode() const = 0;

		virtual void enableRedAlertMode( bool _enable ) = 0;
		virtual bool isRedAlertMode() const = 0;

		virtual void endLimitRenderObjects() = 0;
		virtual void increfLimitRenderObjects() = 0;
		virtual bool decrefLimitRenderObjects() = 0;

    public:
        virtual void onWindowClose() = 0;
		virtual void onDeviceLostPrepare() = 0;
		virtual void onDeviceLostRestore() = 0;
        virtual bool isWindowCreated() const = 0;

    public:
        virtual const RenderServiceDebugInfo & getDebugInfo() const = 0;
        virtual void resetFrameCount() = 0;
	};

#   define RENDER_SERVICE()\
    ((Mengine::RenderServiceInterface*)SERVICE_GET(Mengine::RenderServiceInterface))
}
