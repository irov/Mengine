#	pragma once

#	include "Interface/RenderSystemInterface.h"

#   include "Config/String.h"

#   include "MarmaladeTexture.h"
#   include "MarmaladeShader.h"
#   include "MarmaladeProgram.h"

#   include "Factory/FactoryStore.h"

#   include "stdex/binary_vector.h"
#   include "stdex/stl_map.h"

#   include <IwGL.h>
#   include <s3e.h>

namespace Menge
{
    class ServiceProviderInterface;
    class LogServiceInterface;
    
	class OGLWindowContext;
	class MarmaladeTexture;

    struct TextureStage
    {
        TextureStage()
            : minFilter(0)
            , magFilter(0)
            , wrapS(0)
            , wrapT(0)
			, mengeMinFilter(TF_NONE)
            , mengeMipFilter(TF_NONE)
            , texture(0)
            , colorOp(0)
            , colorArg1(0)
            , colorArg2(0)
            , alphaOp(0)
            , alphaArg1(0)
            , alphaArg2(0)
        {

        }
		        
        GLenum minFilter;
        GLenum magFilter;
        GLenum wrapS;
        GLenum wrapT;
        ETextureFilter mengeMinFilter;
        ETextureFilter mengeMipFilter;
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
		const ConstString & getRenderPlatformName() const override;

	public:
		bool createRenderWindow( const Resolution & _resolution, uint32_t _bits, bool _fullscreen, WindowHandle _winHandle,
			bool _waitForVSync, int _FSAAType, int _FSAAQuality ) override;
		
        void makeProjectionOrthogonal( mt::mat4f & _projectionMatrix, const Viewport & _viewport, float _near, float _far ) override;
        void makeProjectionFrustum( mt::mat4f & _projectionMatrix, const Viewport & _viewport, float _near, float _far ) override;
        void makeProjectionPerspective( mt::mat4f & _projectionMatrix, float _fov, float _aspect, float zn, float zf ) override;
        void makeViewMatrixFromViewport( mt::mat4f & _viewMatrix, const Viewport & _viewport ) override;
        void makeViewMatrixLookAt( mt::mat4f & _viewMatrix, const mt::vec3f & _eye, const mt::vec3f & _dir, const mt::vec3f & _up, float _sign ) override;

		bool screenshot( const RenderImageInterfacePtr & _image, const mt::vec4f & _rect ) override;

		void setProjectionMatrix( const mt::mat4f & _projection ) override;
		void setModelViewMatrix( const mt::mat4f & _view ) override;
		void setTextureMatrix( uint32_t _stage, const float* _texture ) override;
        void setWorldMatrix( const mt::mat4f & _view ) override;

		VBHandle createVertexBuffer( uint32_t _verticesNum, uint32_t _vertexSize, bool _dynamic ) override;
		void releaseVertexBuffer( VBHandle _vbHandle ) override;
		void* lockVertexBuffer(  VBHandle _vbHandle, uint32_t _offset, uint32_t _size, EBufferLockFlag _flags ) override;
		bool unlockVertexBuffer( VBHandle _vbHandle ) override;
		void setVertexBuffer( VBHandle _vbHandle ) override;

		IBHandle createIndexBuffer( uint32_t _indiciesNum, bool _dynamic ) override;
		void releaseIndexBuffer( IBHandle _ibHandle ) override;
		RenderIndices * lockIndexBuffer( IBHandle _ibHandle, uint32_t _offset, uint32_t _size, EBufferLockFlag _flags ) override;
		bool unlockIndexBuffer( IBHandle _ibHandle ) override;
		void setIndexBuffer( IBHandle _ibHandle, uint32_t _baseVertexIndex ) override;

		void setVertexDeclaration( uint32_t _vertexSize, uint32_t _declaration ) override;

		void drawIndexedPrimitive( EPrimitiveType _type, uint32_t _baseVertexIndex,
			uint32_t _minIndex, uint32_t _verticesNum, uint32_t _startIndex, uint32_t _indexCount ) override;

		void setTexture( uint32_t _stage, const RenderImageInterfacePtr & _texture ) override;
		void setTextureAddressing( uint32_t _stage, ETextureAddressMode _modeU, ETextureAddressMode _modeV ) override;
		void setTextureFactor( uint32_t _color ) override;
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
		void setAlphaCmpFunc( ECompareFunction _alphaFunc, uint8_t _alpha ) override;
		void setLightingEnable( bool _light ) override;
		void setTextureStageColorOp( uint32_t _stage, ETextureOp _textrueOp,
			ETextureArgument _arg1, ETextureArgument _arg2 ) override;
		void setTextureStageAlphaOp( uint32_t _stage, ETextureOp _textrueOp,
			ETextureArgument _arg1, ETextureArgument _arg2 ) override;
        void setTextureStageTexCoordIndex( uint32_t _stage, uint32_t _index ) override;
		void setTextureStageFilter( uint32_t _stage, ETextureFilterType _filterType, ETextureFilter _filter ) override;

	public:
		RenderShaderInterfacePtr createFragmentShader( const ConstString & _name, const void * _buffer, size_t _size, bool _isCompile ) override;
		RenderShaderInterfacePtr createVertexShader( const ConstString & _name, const void * _buffer, size_t _size, bool _isCompile ) override;

		RenderProgramInterfacePtr createProgram( const ConstString & _name, const RenderShaderInterfacePtr & _fragment, const RenderShaderInterfacePtr & _vertex, uint32_t _samplerCount ) override;
		void setProgram( const RenderProgramInterfacePtr & _program ) override;

	public:
		RenderImageInterfacePtr createImage( uint32_t _mipmaps, uint32_t _width, uint32_t _height, uint32_t _channels, uint32_t _depth, PixelFormat _format ) override;
        RenderImageInterfacePtr createRenderTargetImage( uint32_t _width, uint32_t _height, uint32_t _channels, uint32_t _depth, PixelFormat _format ) override;
		RenderImageInterfacePtr createDynamicImage( uint32_t _width, uint32_t _height, uint32_t _channels, uint32_t _depth, PixelFormat _format ) override;

		bool lockRenderTarget( const RenderImageInterfacePtr & _renderTarget ) override;
		bool unlockRenderTarget() override;

		bool beginScene() override;
		void endScene() override;
		void swapBuffers() override;
		void clearFrameBuffer( uint32_t _frameBufferTypes, uint32_t _color, float _depth, uint32_t _stencil ) override;

		void setViewport( const Viewport & _viewport ) override;

		void changeWindowMode( const Resolution & _resolution, bool _fullscreen ) override;

		bool supportTextureFormat( PixelFormat _format ) const override;

		void onWindowMovedOrResized() override;
		void onWindowClose() override;

		void setVSync( bool _vSync ) override;

        void clear( uint32_t _color ) override;
        
        void setSeparateAlphaBlendMode() override;

    protected:
        void findFormatFromChannels_( PixelFormat _format, uint32_t _channels, PixelFormat & _hwFormat, uint32_t & _hwChannels ) const;

	private:
        ServiceProviderInterface * m_serviceProvider;

		ConstString m_renderPlatform;

        RenderSystemListener * m_listener;
		
		OGLWindowContext * m_windowContext;

		MarmaladeProgramPtr m_currentProgram;

		mt::mat4f m_worldMatrix;
		mt::mat4f m_viewMatrix;
		mt::mat4f m_projectionMatrix;
		
        Resolution m_resolution;

		bool m_supportNPOT;

		GLuint m_currentVertexBuffer;
		GLuint m_currentIndexBuffer;
		//GLuint m_baseVertexIndex;

		GLenum m_srcBlendFactor;
		GLenum m_dstBlendFactor;

		struct MemoryRange
		{
			MemoryRange()
				: pMem(nullptr)
				, size(0)
				, offset(0)
				, flags(BLF_LOCK_NONE)
				, bufId(0)
			{
			}

			unsigned char * pMem;
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

		typedef stdex::map<ConstString, RenderShaderInterface *> TMapRenderShaders;
		TMapRenderShaders m_shaders;

		uint32_t m_activeTextureStage;
		GLuint m_activeTexture;

        typedef FactoryPoolStore<MarmaladeTexture, 128> TFactoryTexture;
        TFactoryTexture m_factoryTexture;

		typedef FactoryPoolStore<MarmaladeShader, 16> TFactoryShader;
		TFactoryShader m_factoryShader;

		typedef FactoryPoolStore<MarmaladeProgram, 16> TFactoryProgram;
		TFactoryProgram m_factoryProgram;

		TextureStage m_textureStage[MENGE_MAX_TEXTURE_STAGES];

		bool m_depthMask;
	};

}	// namespace Menge
