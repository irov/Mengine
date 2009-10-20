#	pragma once

#	include "Config/Platform.h" 

#	include "Interface/RenderSystemInterface.h"

#	include <map>

#	include <GLee.h>

namespace Menge
{

	class OGLWindowContext;

	class OGLRenderSystem 
		: public RenderSystemInterface
	{
	public:
		OGLRenderSystem();
		~OGLRenderSystem();

	public:
		bool initialize( LogSystemInterface* _logSystem, RenderSystemListener* _listener ) override;
		bool createRenderWindow( std::size_t _width, std::size_t _height, int _bits, bool _fullscreen, WindowHandle _winHandle,
			bool _waitForVSync, int _FSAAType, int _FSAAQuality ) override;

		void getResolutions( TVectorResolutions & _resolutions ) override;

		float getTexelOffsetX() const override;
		float getTexelOffsetY() const override;
		void screenshot( RenderImageInterface* _image, const float * _rect ) override;
		void render() override; 
		void setContentResolution( const std::size_t * _resolution ) override;

		void setProjectionMatrix( const float * _projection ) override;
		void setModelViewMatrix( const float * _view ) override;
		void setTextureMatrix( size_t _stage, const float* _texture ) override;

		VBHandle createVertexBuffer( std::size_t _verticesNum, std::size_t _vertexSize ) override;
		void releaseVertexBuffer( VBHandle _vbHandle ) override;
		void* lockVertexBuffer(  VBHandle _vbHandle, size_t _offset, size_t _size, uint32 _flags ) override;
		bool unlockVertexBuffer( VBHandle _vbHandle ) override;
		void setVertexBuffer( VBHandle _vbHandle ) override;

		IBHandle createIndexBuffer( std::size_t _indiciesNum ) override;
		void releaseIndexBuffer( IBHandle _ibHandle ) override;
		uint16* lockIndexBuffer(  IBHandle _ibHandle ) override;
		bool unlockIndexBuffer( IBHandle _ibHandle ) override;
		void setIndexBuffer( IBHandle _ibHandle ) override;

		void setVertexDeclaration( uint32 _declaration ) override;

		void drawIndexedPrimitive( EPrimitiveType _type, std::size_t _baseVertexIndex,
			std::size_t _minIndex, std::size_t _verticesNum, std::size_t _startIndex, std::size_t _indexCount ) override;

		void setTexture( std::size_t _stage, RenderImageInterface* _texture ) override;
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
		RenderImageInterface * createImage( std::size_t& _width, std::size_t& _height, PixelFormat& _format ) override;
		// create render target image
		// [in/out] _width ( desired texture width, returns actual texture width )
		// [in/out] _height ( desired texture height, returns actual texture height )
		// returns Texture interface handle or NULL if fails
		RenderImageInterface * createRenderTargetImage( std::size_t& _width, std::size_t& _height ) override;
		// �������� �����������
		void releaseImage( RenderImageInterface * _image ) override;

		bool beginScene() override;
		void endScene() override;
		void swapBuffers() override;
		void clearFrameBuffer( uint32 _frameBufferTypes
			, uint32 _color = 0
			, float _depth = 1.0f
			, uint16 _stencil = 0 ) override;
		void beginLayer2D() override;
		void endLayer2D() override;
		void beginLayer3D() override;
		void endLayer3D() override;

		void setRenderArea( const float* _renderArea ) override;

		void setFullscreenMode( std::size_t _width, std::size_t _height, bool _fullscreen ) override;
		void setRenderTarget( RenderImageInterface* _renderTarget, bool _clear ) override;

		LightInterface * createLight( const String & _name ) override;
		void releaseLight( LightInterface * _light ) override;

		void onWindowMovedOrResized() override;
		void onWindowClose() override;

		void setVSync( bool _vSync ) override;

		void unlockTexture( GLuint _uid, GLint _internalFormat,
			GLsizei _width, GLsizei _height, GLenum _format, GLenum _type, const GLvoid* _data );

	private:
		void log_( const char* _message, ... );
		void log_error_( const char* _message, ... );

	private:
		LogSystemInterface* m_logSystem;
		OGLWindowContext* m_windowContext;
		String m_ext;

		bool m_supportNPOT;

		GLuint m_currentVertexBuffer;
		GLuint m_currentIndexBuffer;

		GLenum m_srcBlendFactor;
		GLenum m_dstBlendFactor;

		struct MemoryRange
		{
			unsigned char* pMem;
			size_t size;
			size_t offset;
		};
		typedef std::map< VBHandle, MemoryRange > TMapVBufferMemory;
		TMapVBufferMemory m_vBuffersMemory;
		TMapVBufferMemory m_vBuffersLocks;

		size_t m_activeTextureStage;
		GLuint m_activeTexture;

		struct TextureStage
		{
			bool enabled;
			GLenum minFilter;
			GLenum magFilter;
			GLenum wrapS;
			GLenum wrapT;
			ETextureFilter mengeMinFilter;
			ETextureFilter mengeMipFilter;
		};

		TextureStage m_textureStage[MENGE_MAX_TEXTURE_STAGES];
	};

}	// namespace Menge
