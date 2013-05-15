#	pragma once

#	include "Interface/RenderSystemInterface.h"

#   include "OGLTexture.h"

#   include "Utils/Factory/FactoryPool.h"

#	include <map>


#if WIN32
#	include <Windows.h>
#	include <GLee.h>
#	include <GL/GLu.h>

#	include "OGLWindowContext.h"
#elif (TARGET_OS_MAC && !TARGET_OS_IPHONE)
#   include <OpenGL/gl.h>
#elif TARGET_MARMALADE
#   include "IwGL.h"
#   include "s3e.h"
#else
//#	include "TargetConditionals.h"
#	include <OpenGLES/ES1/gl.h>
#	include <OpenGLES/ES1/glext.h>
#endif

namespace Menge
{
    class ServiceProviderInterface;
    class LogServiceInterface;
    
	class OGLWindowContext;
	class OGLTexture;

	class OGLRenderSystem 
		: public RenderSystemInterface
	{
	public:
		OGLRenderSystem();
		~OGLRenderSystem();

    public:
        void setServiceProvider( ServiceProviderInterface * _serviceProvider ) override;
        ServiceProviderInterface * getServiceProvider() const override;

	public:
		bool initialize() override;
        void finalize() override;

    public:
        void setRenderListener( RenderSystemListener * _listener ) override;

	public:
		bool createRenderWindow( std::size_t _width, std::size_t _height, int _bits, bool _fullscreen, WindowHandle _winHandle,
			bool _waitForVSync, int _FSAAType, int _FSAAQuality ) override;


        void makeProjectionOrthogonal( mt::mat4f & _projectionMatrix, const Viewport & _viewport, float _near, float _far ) override;
        void makeProjectionFrustum( mt::mat4f & _projectionMatrix, const Viewport & _viewport, float _near, float _far ) override;
        void makeProjectionPerspective( mt::mat4f & _projectionMatrix, float _fov, float _aspect, float zn, float zf ) override;
        void makeViewMatrixFromViewport( mt::mat4f & _viewMatrix, const Viewport & _viewport ) override;

		float getTexelOffsetX() const override;
		float getTexelOffsetY() const override;

		void screenshot( const RenderImageInterfacePtr & _image, const float * _rect ) override;

		void setProjectionMatrix( const mt::mat4f & _projection ) override;
		void setModelViewMatrix( const mt::mat4f & _view ) override;
		void setTextureMatrix( size_t _stage, const float* _texture ) override;
        void setWorldMatrix( const mt::mat4f & _view ) override;

		VBHandle createVertexBuffer( std::size_t _verticesNum, std::size_t _vertexSize ) override;
		void releaseVertexBuffer( VBHandle _vbHandle ) override;
		void* lockVertexBuffer(  VBHandle _vbHandle, size_t _offset, size_t _size, uint32 _flags ) override;
		bool unlockVertexBuffer( VBHandle _vbHandle ) override;
		void setVertexBuffer( VBHandle _vbHandle ) override;

		IBHandle createIndexBuffer( std::size_t _indiciesNum ) override;
		void releaseIndexBuffer( IBHandle _ibHandle ) override;
		uint16* lockIndexBuffer(  IBHandle _ibHandle ) override;
		bool unlockIndexBuffer( IBHandle _ibHandle ) override;
		void setIndexBuffer( IBHandle _ibHandle, size_t _baseVertexIndex ) override;

		void setVertexDeclaration( size_t _vertexSize, uint32 _declaration ) override;

		void drawIndexedPrimitive( EPrimitiveType _type, std::size_t _baseVertexIndex,
			std::size_t _minIndex, std::size_t _verticesNum, std::size_t _startIndex, std::size_t _indexCount ) override;

		void setTexture( std::size_t _stage, const RenderImageInterfacePtr & _texture ) override;
		void setTextureAddressing( std::size_t _stage, ETextureAddressMode _modeU, ETextureAddressMode _modeV ) override;
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
		void setTextureStageFilter( size_t _stage, ETextureFilterType _filterType, ETextureFilter _filter ) override;
		// create texture
		// [in/out] _width ( desired texture width, returns actual texture width )
		// [in/out] _height ( desired texture height, returns actual texture height )
		// [in/out] _format ( desired texture pixel format, returns actual texture pixel format )
		// returns Texture interface handle or NULL if fails
		RenderImageInterfacePtr createImage( size_t _width, size_t _height, size_t _channels, PixelFormat _format ) override;
		// create render target image
		// [in/out] _width ( desired texture width, returns actual texture width )
		// [in/out] _height ( desired texture height, returns actual texture height )
		// returns Texture interface handle or NULL if fails
		// RenderImageInterface * createRenderTargetImage( std::size_t& _width, std::size_t& _height ) override;
		// �������� �����������
        
        RenderImageInterfacePtr createRenderTargetImage( size_t _width, size_t _height, size_t _channels, PixelFormat _format ) override;

		RenderImageInterfacePtr createDynamicImage( size_t _width, size_t _height, size_t _channels, PixelFormat _format ) override;

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
        ServiceProviderInterface* m_serviceProvider;

        RenderSystemListener * m_listener;
		
		OGLWindowContext* m_windowContext;

		String m_ext;

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
		};

		typedef std::map<VBHandle, MemoryRange> TMapVBufferMemory;
		TMapVBufferMemory m_vBuffersMemory;
		TMapVBufferMemory m_vBuffersLocks;
		
		typedef std::map<IBHandle, MemoryRange> TMapIBufferMemory;
		TMapIBufferMemory m_iBuffersMemory;
		TMapIBufferMemory m_iBuffersLocks;

		size_t m_activeTextureStage;
		GLuint m_activeTexture;

        struct TextureStage
        {
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

        typedef FactoryPool<OGLTexture, 128> TFactoryOGLTexture;
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
