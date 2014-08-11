#	pragma once

#	include "Interface/RenderSystemInterface.h"

#   include "Config/String.h"

#   include "MarmaladeTexture.h"

#   include "Factory/FactoryStore.h"

#   include "stdex/binary_vector.h"

#   include "IwGL.h"
#   include "s3e.h"

namespace Menge
{
    class ServiceProviderInterface;
    class LogServiceInterface;
    
	class OGLWindowContext;
	class MarmaladeTexture;

    struct TextureStage
    {
        TextureStage()
            : enabled(false)
            , minFilter(TF_LINEAR)
            , magFilter(TF_LINEAR)
            , wrapS(GL_CLAMP_TO_EDGE)
            , wrapT(GL_CLAMP_TO_EDGE)
            , mengeMinFilter(TF_LINEAR)
            , mengeMipFilter(TF_LINEAR)
            , texture(0)
            , colorOp(GL_MODULATE)
            , colorArg1(GL_TEXTURE)
            , colorArg2(GL_PRIMARY_COLOR)
            , alphaOp(GL_MODULATE)
            , alphaArg1(GL_TEXTURE)
            , alphaArg2(GL_PRIMARY_COLOR)
        {

        }

        bool enabled;
        GLenum minFilter;
        GLenum magFilter;
        GLenum wrapS;
        GLenum wrapT;
        Menge::ETextureFilter mengeMinFilter;
        Menge::ETextureFilter mengeMipFilter;
        GLuint texture;
        GLenum colorOp;
        GLenum colorArg1;
        GLenum colorArg2;
        GLenum alphaOp;
        GLenum alphaArg1;
        GLenum alphaArg2;
    };
    
	class MarmaladeRenderSystem 
		: public RenderSystemInterface
	{
	public:
		MarmaladeRenderSystem();
		~MarmaladeRenderSystem();

    public:
        void setServiceProvider( ServiceProviderInterface * _serviceProvider ) override;
        ServiceProviderInterface * getServiceProvider() const override;

	public:
		bool initialize() override;
        void finalize() override;

    public:
        void setRenderListener( RenderSystemListener * _listener ) override;

	public:
		bool createRenderWindow( const Resolution & _resolution, int _bits, bool _fullscreen, WindowHandle _winHandle,
			bool _waitForVSync, int _FSAAType, int _FSAAQuality ) override;


        void makeProjectionOrthogonal( mt::mat4f & _projectionMatrix, const Viewport & _viewport, float _near, float _far ) override;
        void makeProjectionFrustum( mt::mat4f & _projectionMatrix, const Viewport & _viewport, float _near, float _far ) override;
        void makeProjectionPerspective( mt::mat4f & _projectionMatrix, float _fov, float _aspect, float zn, float zf ) override;
        void makeViewMatrixFromViewport( mt::mat4f & _viewMatrix, const Viewport & _viewport ) override;
        void makeViewMatrixLookAt( mt::mat4f & _viewMatrix, const mt::vec3f & _eye, const mt::vec3f & _dir, const mt::vec3f & _up, float _sign ) override;

		bool screenshot( const RenderImageInterfacePtr & _image, const float * _rect ) override;

		void setProjectionMatrix( const mt::mat4f & _projection ) override;
		void setModelViewMatrix( const mt::mat4f & _view ) override;
		void setTextureMatrix( size_t _stage, const float* _texture ) override;
        void setWorldMatrix( const mt::mat4f & _view ) override;

		VBHandle createVertexBuffer( size_t _verticesNum, size_t _vertexSize, bool _dynamic ) override;
		void releaseVertexBuffer( VBHandle _vbHandle ) override;
		void* lockVertexBuffer(  VBHandle _vbHandle, size_t _offset, size_t _size, EBufferLockFlag _flags ) override;
		bool unlockVertexBuffer( VBHandle _vbHandle ) override;
		void setVertexBuffer( VBHandle _vbHandle ) override;

		IBHandle createIndexBuffer( size_t _indiciesNum, bool _dynamic ) override;
		void releaseIndexBuffer( IBHandle _ibHandle ) override;
		void * lockIndexBuffer( IBHandle _ibHandle, size_t _offset, size_t _size, EBufferLockFlag _flags ) override;
		bool unlockIndexBuffer( IBHandle _ibHandle ) override;
		void setIndexBuffer( IBHandle _ibHandle, size_t _baseVertexIndex ) override;

		void setVertexDeclaration( size_t _vertexSize, uint32 _declaration ) override;

		void drawIndexedPrimitive( EPrimitiveType _type, size_t _baseVertexIndex,
			size_t _minIndex, size_t _verticesNum, size_t _startIndex, size_t _indexCount ) override;

		void setTexture( size_t _stage, const RenderImageInterfacePtr & _texture ) override;
		void setTextureAddressing( size_t _stage, ETextureAddressMode _modeU, ETextureAddressMode _modeV ) override;
		void setTextureFactor( uint32 _color ) override;
		void setSrcBlendFactor( EBlendFactor _src ) override;
		void setDstBlendFactor( EBlendFactor _dst ) override;
		void setCullMode( ECullMode _mode ) override;
		void setDepthBufferTestEnable( bool _depthTest ) override;
		void setDepthBufferWriteEnable( bool _depthWrite ) override;
		void setDepthBufferCmpFunc( ECompareFunction _depthFunction ) override;
		void setFillMode( EFillMode _mode ) override;
		void setColorBufferWriteEnable( bool _r, bool _g, bool _b, bool _a ) override;
		void setShadeType( EShadeType _sType ) override;
		void setAlphaTestEnable( bool _alphaTest ) override;
		void setAlphaBlendEnable( bool _alphaBlend ) override;
		void setAlphaCmpFunc( ECompareFunction _alphaFunc, uint8 _alpha ) override;
		void setLightingEnable( bool _light ) override;
		void setTextureStageColorOp( size_t _stage, ETextureOp _textrueOp,
			ETextureArgument _arg1, ETextureArgument _arg2 ) override;
		void setTextureStageAlphaOp( size_t _stage, ETextureOp _textrueOp,
			ETextureArgument _arg1, ETextureArgument _arg2 ) override;
        void setTextureStageTexCoordIndex( size_t _stage, size_t _index ) override;
		void setTextureStageFilter( size_t _stage, ETextureFilterType _filterType, ETextureFilter _filter ) override;
		
        RenderShaderInterface * createShader( const void * _code, size_t _len ) override;
        void setShader( RenderShaderInterface * _shader ) override;
        
        
        // create texture
		// [in/out] _width ( desired texture width, returns actual texture width )
		// [in/out] _height ( desired texture height, returns actual texture height )
		// [in/out] _format ( desired texture pixel format, returns actual texture pixel format )
		// returns Texture interface handle or NULL if fails
		RenderImageInterfacePtr createImage( size_t _width, size_t _height, size_t _channels, size_t _depth, PixelFormat _format ) override;
		// create render target image
		// [in/out] _width ( desired texture width, returns actual texture width )
		// [in/out] _height ( desired texture height, returns actual texture height )
		// returns Texture interface handle or NULL if fails
		// RenderImageInterface * createRenderTargetImage( size_t& _width, size_t& _height ) override;
		// удаления изображения
        
        RenderImageInterfacePtr createRenderTargetImage( size_t _width, size_t _height, size_t _channels, size_t _depth, PixelFormat _format ) override;

		RenderImageInterfacePtr createDynamicImage( size_t _width, size_t _height, size_t _channels, size_t _depth, PixelFormat _format ) override;

		bool beginScene() override;
		void endScene() override;
		void swapBuffers() override;
		void clearFrameBuffer( uint32 _frameBufferTypes
			, uint32 _color = 0
			, float _depth = 1.0f
			, uint16 _stencil = 0 ) override;

		void setViewport( const Viewport & _viewport ) override;

		void changeWindowMode( const Resolution & _resolution, bool _fullscreen ) override;
		bool setRenderTarget( const RenderImageInterfacePtr & _renderTarget, bool _clear ) override;

		bool supportTextureFormat( PixelFormat _format ) const override;

		void onWindowMovedOrResized() override;
		void onWindowClose() override;

		void setVSync( bool _vSync ) override;

        void clear( uint32 _color ) override;
        
        void setSeparateAlphaBlendMode() override;

    protected:
        PixelFormat findFormatFromChannels_( size_t & _channels, PixelFormat _format ) const;

	private:
        ServiceProviderInterface * m_serviceProvider;

        RenderSystemListener * m_listener;
		
		OGLWindowContext* m_windowContext;

        Resolution m_resolution;

		bool m_supportNPOT;

		GLuint m_currentVertexBuffer;
		GLuint m_currentIndexBuffer;
		//GLuint m_baseVertexIndex;

		GLenum m_srcBlendFactor;
		GLenum m_dstBlendFactor;

		struct MemoryRange
		{
			unsigned char* pMem;
			size_t size;
			size_t offset;
            EBufferLockFlag flags;

			GLuint bufId;
		};

		VBHandle m_VBHandleGenerator;
		IBHandle m_IBHandleGenerator;

        typedef stdex::binary_vector<VBHandle, MemoryRange> TMapVBufferMemory;
		TMapVBufferMemory m_vBuffersMemory;
		TMapVBufferMemory m_vBuffersLocks;
		
		typedef stdex::binary_vector<IBHandle, MemoryRange> TMapIBufferMemory;
		TMapIBufferMemory m_iBuffersMemory;
		TMapIBufferMemory m_iBuffersLocks;

		size_t m_activeTextureStage;
		GLuint m_activeTexture;

        typedef FactoryPoolStore<MarmaladeTexture, 128> TFactoryOGLTexture;
        TFactoryOGLTexture m_factoryOGLTexture;

		TextureStage m_textureStage[MENGE_MAX_TEXTURE_STAGES];
		size_t m_winWidth;
		size_t m_winHeight;
		size_t m_winContextWidth;
		size_t m_winContextHeight;

		bool m_depthMask;
		OGLTexturePtr m_activeRenderTarget;
	};

}	// namespace Menge
