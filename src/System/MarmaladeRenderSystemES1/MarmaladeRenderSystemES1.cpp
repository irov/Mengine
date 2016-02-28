#	include "MarmaladeRenderSystemES1.h"

#	include "MarmaladeRenderEnumES1.h"
#	include "MarmaladeRenderErrorES1.h"

#	include "Interface/StringizeInterface.h"

#   include "Logger/Logger.h"

#	include <cmath>

//////////////////////////////////////////////////////////////////////////
#	define GET_A_FLOAT_FROM_ARGB32( argb ) ( ((float)(argb >> 24)) / 255.0f )
#	define GET_R_FLOAT_FROM_ARGB32( argb ) ( ((float)((argb >> 16) & 0xFF)) / 255.0f )
#	define GET_G_FLOAT_FROM_ARGB32( argb ) ( ((float)((argb >> 8) & 0xFF)) / 255.0f )
#	define GET_B_FLOAT_FROM_ARGB32( argb ) ( (float)(argb & 0xFF) / 255.0f )
//////////////////////////////////////////////////////////////////////////
SERVICE_FACTORY( RenderSystemES1, Menge::MarmaladeRenderSystemES1 );
//////////////////////////////////////////////////////////////////////////
namespace Menge
{
	//////////////////////////////////////////////////////////////////////////
	MarmaladeRenderSystemES1::MarmaladeRenderSystemES1()
		: m_glMaxClipPlanes( 0 )
		, m_glMaxCombinedTextureImageUnits(0)
		, m_depthMask( false )
	{
	}
	//////////////////////////////////////////////////////////////////////////
	MarmaladeRenderSystemES1::~MarmaladeRenderSystemES1()
	{
	}
	//////////////////////////////////////////////////////////////////////////
	bool MarmaladeRenderSystemES1::_initialize()
	{
		LOGGER_WARNING( m_serviceProvider )("Initializing OpenGL RenderSystem...");

		if( IwGLIsInitialised() == false )
		{
			if( IwGLInit() == false )
			{
				return false;
			}

			s3eDeviceYield( 0 );
		}

		MARMALADE_RENDER_CHECK_ERROR( m_serviceProvider );

		int32 iwgl_version = IwGLGetInt( IW_GL_VERSION );

		LOGGER_WARNING( m_serviceProvider )("Marmalade IWGL Version: %d (full %d)", iwgl_version >> 8, iwgl_version);

		LOGGER_WARNING( m_serviceProvider )("Vendor      : %s", (const char*)glGetString( GL_VENDOR ));
		LOGGER_WARNING( m_serviceProvider )("Renderer    : %s", (const char*)glGetString( GL_RENDERER ));
		LOGGER_WARNING( m_serviceProvider )("Version     : %s", (const char*)glGetString( GL_VERSION ));
		LOGGER_WARNING( m_serviceProvider )("Extensions  : %s", (const char*)glGetString( GL_EXTENSIONS ));

		MARMALADE_RENDER_CHECK_ERROR( m_serviceProvider );

		for( uint32_t i = 0; i < m_glMaxCombinedTextureImageUnits; ++i )
		{
			GLCALL( m_serviceProvider, glActiveTexture, (GL_TEXTURE0 + i) );
			GLCALL( m_serviceProvider, glTexEnvi, (GL_TEXTURE_ENV, GL_TEXTURE_ENV_MODE, GL_COMBINE) );
			GLCALL( m_serviceProvider, glTexEnvi, (GL_TEXTURE_ENV, GL_OPERAND0_RGB, GL_SRC_COLOR) );
			GLCALL( m_serviceProvider, glTexEnvi, (GL_TEXTURE_ENV, GL_OPERAND1_RGB, GL_SRC_COLOR) );
			GLCALL( m_serviceProvider, glTexEnvi, (GL_TEXTURE_ENV, GL_OPERAND0_ALPHA, GL_SRC_ALPHA) );
			GLCALL( m_serviceProvider, glTexEnvi, (GL_TEXTURE_ENV, GL_OPERAND1_ALPHA, GL_SRC_ALPHA) );
			GLCALL( m_serviceProvider, glDisable, (GL_TEXTURE_2D) );

			m_textureStage[i] = TextureStage();
		}

		GLCALL( m_serviceProvider, glFrontFace, (GL_CW) );
		GLCALL( m_serviceProvider, glDisable, (GL_DEPTH_TEST) );
		GLCALL( m_serviceProvider, glDisable, (GL_STENCIL_TEST) );
		GLCALL( m_serviceProvider, glDisable, (GL_CULL_FACE) );
		GLCALL( m_serviceProvider, glDisable, (GL_LIGHTING) );
		GLCALL( m_serviceProvider, glDisable, (GL_BLEND) );
		GLCALL( m_serviceProvider, glDisable, (GL_ALPHA_TEST) );
		GLCALL( m_serviceProvider, glDisable, (GL_DITHER) );

		GLCALL( m_serviceProvider, glDepthMask, (GL_FALSE) );
		//glPixelStorei( GL_UNPACK_ALIGNMENT, 1 );

		GLCALL( m_serviceProvider, glMatrixMode, (GL_MODELVIEW) );
		GLCALL( m_serviceProvider, glLoadIdentity, () );

		GLCALL( m_serviceProvider, glMatrixMode, (GL_PROJECTION) );
		GLCALL( m_serviceProvider, glLoadIdentity, () );

		GLCALL( m_serviceProvider, glMatrixMode, (GL_TEXTURE) );
		GLCALL( m_serviceProvider, glLoadIdentity, () );

		GLint maxClipPlanes;
		glGetIntegerv( GL_MAX_CLIP_PLANES, &maxClipPlanes );

		m_glMaxClipPlanes = maxClipPlanes;

		GLint maxCombinedTextureImageUnits;
		glGetIntegerv( GL_MAX_COMBINED_TEXTURE_IMAGE_UNITS, &maxCombinedTextureImageUnits );

		m_glMaxCombinedTextureImageUnits = maxCombinedTextureImageUnits;

		if( m_glMaxCombinedTextureImageUnits > MENGE_MAX_TEXTURE_STAGES )
		{
			m_glMaxCombinedTextureImageUnits = MENGE_MAX_TEXTURE_STAGES;
		}

		m_renderPlatform = STRINGIZE_STRING_LOCAL( m_serviceProvider, "Marmalade OpenGL ES1" );

		return true;
	}
	//////////////////////////////////////////////////////////////////////////
	void MarmaladeRenderSystemES1::_finalize()
	{
		m_currentIndexBuffer = nullptr;
		m_currentVertexBuffer = nullptr;

		GLCALL( m_serviceProvider, IwGLTerminate, () );
	}
	//////////////////////////////////////////////////////////////////////////
	const ConstString & MarmaladeRenderSystemES1::getRenderPlatformName() const
	{
		return m_renderPlatform;
	}
	//////////////////////////////////////////////////////////////////////////
	bool MarmaladeRenderSystemES1::createRenderWindow( const Resolution & _resolution, uint32_t _bits, bool _fullscreen, bool _waitForVSync, int _FSAAType, int _FSAAQuality, uint32_t _MultiSampleCount )
	{
		(void)_bits;
		(void)_fullscreen;
		(void)_waitForVSync;
		(void)_FSAAType;
		(void)_FSAAQuality;
		(void)_MultiSampleCount;

		m_resolution = _resolution;
		
		return true;
	}
	//////////////////////////////////////////////////////////////////////////
	bool MarmaladeRenderSystemES1::screenshot( const RenderImageInterfacePtr & _image, const mt::vec4f & _rect )
	{
		(void)_image;
		(void)_rect;
		//ToDo!

		return false;
	}
	//////////////////////////////////////////////////////////////////////////
	void MarmaladeRenderSystemES1::setClipplaneCount( uint32_t _count )
	{
		if( _count > m_glMaxClipPlanes )
		{ 
			return;
		}

		for( uint32_t i = 0; i != _count; ++i )
		{
			GLenum plane = GL_CLIP_PLANE0 + i;

			glEnable( plane );
		}
		
		for( uint32_t i = _count; i != m_glMaxClipPlanes; ++i )
		{
			GLenum plane = GL_CLIP_PLANE0 + i;

			glDisable( plane );
		}
	}
	//////////////////////////////////////////////////////////////////////////
	void MarmaladeRenderSystemES1::setClipplane( uint32_t _i, const mt::planef & _plane )
	{
		if( _i > m_glMaxClipPlanes )
		{
			return;
		}

		GLenum plane = GL_CLIP_PLANE0 + _i;

		glClipPlanef( plane, _plane.buff() );
	}
	//////////////////////////////////////////////////////////////////////////
	void MarmaladeRenderSystemES1::setViewport( const Viewport & _viewport )
	{
		float w = _viewport.getWidth();
		float h = _viewport.getHeight();
		float xb = _viewport.begin.x;
		float yb = _viewport.end.y;

		size_t height = m_resolution.getHeight();

		GLCALL( m_serviceProvider, glViewport, ((GLsizei)xb, height - (GLsizei)(yb), (GLsizei)w, (GLsizei)h) );
	}
	//////////////////////////////////////////////////////////////////////////
	void MarmaladeRenderSystemES1::setProjectionMatrix( const mt::mat4f & _projection )
	{
		GLCALL( m_serviceProvider, glMatrixMode, (GL_PROJECTION) );

		const float * matrix = _projection.buff();
		GLCALL( m_serviceProvider, glLoadMatrixf, (matrix) );
	}
	//////////////////////////////////////////////////////////////////////////
	void MarmaladeRenderSystemES1::setModelViewMatrix( const mt::mat4f & _view )
	{
		GLCALL( m_serviceProvider, glMatrixMode, (GL_MODELVIEW) );

		const float * matrix = _view.buff();
		GLCALL( m_serviceProvider, glLoadMatrixf, (matrix) );
	}
	//////////////////////////////////////////////////////////////////////////
	void MarmaladeRenderSystemES1::setTextureMatrix( uint32_t _stage, const float* _texture )
	{
		// To Do
	}
	//////////////////////////////////////////////////////////////////////////
	void MarmaladeRenderSystemES1::setWorldMatrix( const mt::mat4f & _world )
	{
		// To Do
	}
	//////////////////////////////////////////////////////////////////////////
	RenderVertexBufferInterfacePtr MarmaladeRenderSystemES1::createVertexBuffer( uint32_t _verticesNum, bool _dynamic )
	{
		MarmaladeRenderVertexBufferPtr buffer = m_factoryVertexBuffer.createObject();

		if( buffer->initialize( m_serviceProvider, _verticesNum, _dynamic ) == false )
		{
			return nullptr;
		}

		return buffer;
	}
	//////////////////////////////////////////////////////////////////////////
	bool MarmaladeRenderSystemES1::setVertexBuffer( const RenderVertexBufferInterfacePtr & _vertexBuffer )
	{
		m_currentVertexBuffer = _vertexBuffer;

		return true;
	}
	//////////////////////////////////////////////////////////////////////////
	RenderIndexBufferInterfacePtr MarmaladeRenderSystemES1::createIndexBuffer( uint32_t _indiciesNum, bool _dynamic )
	{
		MarmaladeRenderIndexBufferPtr buffer = m_factoryIndexBuffer.createObject();

		if( buffer->initialize( m_serviceProvider, _indiciesNum, _dynamic ) == false )
		{
			return nullptr;
		}

		return buffer;
	}
	//////////////////////////////////////////////////////////////////////////
	bool MarmaladeRenderSystemES1::setIndexBuffer( const RenderIndexBufferInterfacePtr & _indexBuffer )
	{
		m_currentIndexBuffer = _indexBuffer;

		return true;
	}
	//////////////////////////////////////////////////////////////////////////
	RenderShaderInterfacePtr MarmaladeRenderSystemES1::createFragmentShader( const ConstString & _name, const void * _buffer, size_t _size, bool _isCompile )
	{
		(void)_name;
		(void)_buffer;
		(void)_size;
		(void)_isCompile;

		return nullptr;
	}
	//////////////////////////////////////////////////////////////////////////
	RenderShaderInterfacePtr MarmaladeRenderSystemES1::createVertexShader( const ConstString & _name, const void * _buffer, size_t _size, bool _isCompile )
	{
		(void)_name;
		(void)_buffer;
		(void)_size;
		(void)_isCompile;

		return nullptr;
	}
	//////////////////////////////////////////////////////////////////////////
	RenderProgramInterfacePtr MarmaladeRenderSystemES1::createProgram( const ConstString & _name, const RenderShaderInterfacePtr & _fragment, const RenderShaderInterfacePtr & _vertex, uint32_t _samplerCount )
	{
		(void)_name;
		(void)_fragment;
		(void)_vertex;
		(void)_samplerCount;

		return nullptr;
	}
	//////////////////////////////////////////////////////////////////////////
	void MarmaladeRenderSystemES1::setProgram( const RenderProgramInterfacePtr & _program )
	{
		(void)_program;
	}
	//////////////////////////////////////////////////////////////////////////
	void MarmaladeRenderSystemES1::drawIndexedPrimitive( EPrimitiveType _type,
		uint32_t _baseVertexIndex, uint32_t _minIndex,
		uint32_t _verticesNum, uint32_t _startIndex, uint32_t _indexCount )
	{
		if( m_currentIndexBuffer == nullptr || m_currentVertexBuffer == nullptr )
		{
			return;
		}

		for( uint32_t i = 0; i != m_glMaxCombinedTextureImageUnits; ++i )
		{
			const TextureStage & textureStage = m_textureStage[i];

			GLCALL( m_serviceProvider, glActiveTexture, (GL_TEXTURE0 + i) );

			if( textureStage.texture == nullptr )
			{
				GLCALL( m_serviceProvider, glDisable, (GL_TEXTURE_2D) );

				continue;
			}

			GLuint texture_uid = textureStage.texture->getUId();

			GLCALL( m_serviceProvider, glEnable, (GL_TEXTURE_2D) );
			
			GLCALL( m_serviceProvider, glBindTexture, (GL_TEXTURE_2D, texture_uid) );
			
			GLCALL( m_serviceProvider, glTexEnvi, (GL_TEXTURE_ENV, GL_TEXTURE_ENV_MODE, GL_COMBINE) );

			GLCALL( m_serviceProvider, glTexEnvi, (GL_TEXTURE_ENV, GL_COMBINE_RGB, textureStage.colorOp) );
			GLCALL( m_serviceProvider, glTexEnvi, (GL_TEXTURE_ENV, GL_SRC0_RGB, textureStage.colorArg1) );
			GLCALL( m_serviceProvider, glTexEnvi, (GL_TEXTURE_ENV, GL_SRC1_RGB, textureStage.colorArg2) );
			GLCALL( m_serviceProvider, glTexEnvi, (GL_TEXTURE_ENV, GL_OPERAND0_RGB, GL_SRC_COLOR) );
			GLCALL( m_serviceProvider, glTexEnvi, (GL_TEXTURE_ENV, GL_OPERAND1_RGB, GL_SRC_COLOR) );

			GLCALL( m_serviceProvider, glTexEnvi, (GL_TEXTURE_ENV, GL_COMBINE_ALPHA, textureStage.alphaOp) );
			GLCALL( m_serviceProvider, glTexEnvi, (GL_TEXTURE_ENV, GL_SRC0_ALPHA, textureStage.alphaArg1) );
			GLCALL( m_serviceProvider, glTexEnvi, (GL_TEXTURE_ENV, GL_SRC1_ALPHA, textureStage.alphaArg2) );
			GLCALL( m_serviceProvider, glTexEnvi, (GL_TEXTURE_ENV, GL_OPERAND0_ALPHA, GL_SRC_ALPHA) );
			GLCALL( m_serviceProvider, glTexEnvi, (GL_TEXTURE_ENV, GL_OPERAND1_ALPHA, GL_SRC_ALPHA) );

			GLCALL( m_serviceProvider, glTexParameteri, (GL_TEXTURE_2D, GL_TEXTURE_WRAP_S, textureStage.wrapS) );
			GLCALL( m_serviceProvider, glTexParameteri, (GL_TEXTURE_2D, GL_TEXTURE_WRAP_T, textureStage.wrapT) );
			GLCALL( m_serviceProvider, glTexParameteri, (GL_TEXTURE_2D, GL_TEXTURE_MIN_FILTER, textureStage.minFilter) );
			GLCALL( m_serviceProvider, glTexParameteri, (GL_TEXTURE_2D, GL_TEXTURE_MAG_FILTER, textureStage.magFilter) );
			//GLCALL( m_serviceProvider, glTexParameteri, (GL_TEXTURE_2D, GL_TEXTURE_MIN_FILTER, GL_LINEAR) );
			//GLCALL( m_serviceProvider, glTexParameteri, (GL_TEXTURE_2D, GL_TEXTURE_MAG_FILTER, GL_LINEAR) );
		}

		MarmaladeRenderIndexBufferES1 * ib = stdex::intrusive_get<MarmaladeRenderIndexBufferES1 *>( m_currentIndexBuffer );
		ib->enable();

		MarmaladeRenderVertexBufferES1 * vb = stdex::intrusive_get<MarmaladeRenderVertexBufferES1 *>( m_currentVertexBuffer );
		vb->enable();

		GLCALL( m_serviceProvider, glEnableClientState, (GL_VERTEX_ARRAY) );
		GLCALL( m_serviceProvider, glVertexPointer, (3, GL_FLOAT, sizeof( RenderVertex2D ), reinterpret_cast<const GLvoid *>(offsetof( RenderVertex2D, pos ))) );

		GLCALL( m_serviceProvider, glEnableClientState, (GL_COLOR_ARRAY) );
		GLCALL( m_serviceProvider, glColorPointer, (4, GL_UNSIGNED_BYTE, sizeof( RenderVertex2D ), reinterpret_cast<const GLvoid *>(offsetof( RenderVertex2D, color ))) );

		for( uint32_t i = 0; i != m_glMaxCombinedTextureImageUnits; ++i )
		{
			GLCALL( m_serviceProvider, glClientActiveTexture, (GL_TEXTURE0 + i) );
			GLCALL( m_serviceProvider, glEnableClientState, (GL_TEXTURE_COORD_ARRAY) );

			size_t uv_offset = offsetof( RenderVertex2D, uv ) + sizeof( mt::vec2f ) * i;

			GLCALL( m_serviceProvider, glTexCoordPointer, (2, GL_FLOAT, sizeof( RenderVertex2D ), reinterpret_cast<const GLvoid *>(uv_offset)) );
		}

		GLenum mode = s_getGLPrimitiveMode( _type );
		const RenderIndices * baseIndex = nullptr;
		const RenderIndices * offsetIndex = baseIndex + _startIndex;
				
		GLenum indexType = s_getGLIndexType();

		GLCALL( m_serviceProvider, glDrawElements, (mode, _indexCount, indexType, reinterpret_cast<const GLvoid *>(offsetIndex)) );

		GLCALL( m_serviceProvider, glDisableClientState, (GL_VERTEX_ARRAY) );
		GLCALL( m_serviceProvider, glDisableClientState, (GL_COLOR_ARRAY) );
		GLCALL( m_serviceProvider, glDisableClientState, (GL_TEXTURE_COORD_ARRAY) );

		GLCALL( m_serviceProvider, glBindBuffer, (GL_ARRAY_BUFFER, 0) );
		GLCALL( m_serviceProvider, glBindBuffer, (GL_ELEMENT_ARRAY_BUFFER, 0) );
	}
	//////////////////////////////////////////////////////////////////////////
	void MarmaladeRenderSystemES1::setTexture( uint32_t _stage, const RenderImageInterfacePtr & _texture )
	{
		TextureStage & tStage = m_textureStage[_stage];

		if( tStage.texture != nullptr )
		{
			stdex::intrusive_ptr_release( tStage.texture );
			tStage.texture = nullptr;
		}

		if( _texture != nullptr )
		{
			MarmaladeRenderTextureES1 * texture = stdex::intrusive_get<MarmaladeRenderTextureES1 *>( _texture );

			stdex::intrusive_ptr_setup( tStage.texture, texture );
		}
	}
	//////////////////////////////////////////////////////////////////////////
	void MarmaladeRenderSystemES1::setTextureAddressing( uint32_t _stage, ETextureAddressMode _modeU, ETextureAddressMode _modeV )
	{
		GLenum modeUGL = s_getGLAddressMode( _modeU );
		GLenum modeVGL = s_getGLAddressMode( _modeV );

		m_textureStage[_stage].wrapS = modeUGL;
		m_textureStage[_stage].wrapT = modeVGL;
	}
	//////////////////////////////////////////////////////////////////////////
	void MarmaladeRenderSystemES1::setTextureFactor( uint32_t _color )
	{

	}
	//////////////////////////////////////////////////////////////////////////
	void MarmaladeRenderSystemES1::setBlendFactor( EBlendFactor _src, EBlendFactor _dst, EBlendOp _op )
	{
		GLenum srcBlendFactor = s_toGLBlendFactor( _src );
		GLenum dstBlendFactor = s_toGLBlendFactor( _dst );
		GLenum blendOp = s_toGLBlendFactor( _op );

		GLCALL( m_serviceProvider, glBlendFunc, (srcBlendFactor, dstBlendFactor) );
		GLCALL( m_serviceProvider, glBlendEquation, (blendOp) );
	}
	//////////////////////////////////////////////////////////////////////////
	void MarmaladeRenderSystemES1::setCullMode( ECullMode _mode )
	{
		bool enable = true;
		GLenum face = GL_BACK;

		if( _mode == CM_CULL_CW )
		{
			face = GL_FRONT;
		}
		else if( _mode == CM_CULL_NONE )
		{
			enable = false;
		}

		GLCALL( m_serviceProvider, glCullFace, (face) );

		if( enable == true )
		{
			GLCALL( m_serviceProvider, glEnable, (GL_CULL_FACE) );
		}
		else
		{
			GLCALL( m_serviceProvider, glDisable, (GL_CULL_FACE) );
		}
	}
	/////////////////////////////////////////////////////////////////////////
	void MarmaladeRenderSystemES1::setSeparateAlphaBlendMode()
	{

	}
	//////////////////////////////////////////////////////////////////////////
	void MarmaladeRenderSystemES1::setDepthBufferTestEnable( bool _depthTest )
	{
		if( _depthTest == true )
		{
			GLCALL( m_serviceProvider, glEnable, (GL_DEPTH_TEST) );
		}
		else
		{
			GLCALL( m_serviceProvider, glDisable, (GL_DEPTH_TEST) );
		}
	}
	//////////////////////////////////////////////////////////////////////////
	void MarmaladeRenderSystemES1::setDepthBufferWriteEnable( bool _depthWrite )
	{
		m_depthMask = _depthWrite;

		GLCALL( m_serviceProvider, glDepthMask, (m_depthMask ? GL_TRUE : GL_FALSE) );
	}
	//////////////////////////////////////////////////////////////////////////
	void MarmaladeRenderSystemES1::setDepthBufferCmpFunc( ECompareFunction _depthFunction )
	{
		GLenum cmpFunc = s_toGLCmpFunc( _depthFunction );

		GLCALL( m_serviceProvider, glDepthFunc, (cmpFunc) );
	}
	//////////////////////////////////////////////////////////////////////////
	void MarmaladeRenderSystemES1::setFillMode( EFillMode _mode )
	{
		/*GLenum mode = s_toGLFillMode[ _mode ];
		glPolygonMode( GL_FRONT_AND_BACK, mode );*/
	}
	//////////////////////////////////////////////////////////////////////////
	void MarmaladeRenderSystemES1::setColorBufferWriteEnable( bool _r, bool _g, bool _b, bool _a )
	{
		GLCALL( m_serviceProvider, glColorMask, (_r ? GL_TRUE : GL_FALSE, _g ? GL_TRUE : GL_FALSE, _b ? GL_TRUE : GL_FALSE, _a ? GL_TRUE : GL_FALSE) );
	}
	//////////////////////////////////////////////////////////////////////////
	void MarmaladeRenderSystemES1::setShadeType( EShadeType _sType )
	{
		GLenum model = s_toGLShadeMode( _sType );

		GLCALL( m_serviceProvider, glShadeModel, (model) );
	}
	//////////////////////////////////////////////////////////////////////////
	void MarmaladeRenderSystemES1::setAlphaBlendEnable( bool _alphaBlend )
	{
		if( _alphaBlend == true )
		{
			GLCALL( m_serviceProvider, glEnable, (GL_BLEND) );
		}
		else
		{
			GLCALL( m_serviceProvider, glDisable, (GL_BLEND) );
		}
	}
	//////////////////////////////////////////////////////////////////////////
	void MarmaladeRenderSystemES1::setLightingEnable( bool _light )
	{
		if( _light == true )
		{
			GLCALL( m_serviceProvider, glEnable, (GL_LIGHTING) );
		}
		else
		{
			GLCALL( m_serviceProvider, glDisable, (GL_LIGHTING) );
		}
	}
	//////////////////////////////////////////////////////////////////////////
	void MarmaladeRenderSystemES1::setTextureStageColorOp( uint32_t _stage, ETextureOp _textrueOp, ETextureArgument _arg1, ETextureArgument _arg2 )
	{
		if( _textrueOp == Menge::TOP_SELECTARG2 )
		{
			_arg1 = _arg2;
			_textrueOp = Menge::TOP_SELECTARG1;
		}

		m_textureStage[_stage].colorOp = s_getGLTextureOp( _textrueOp );
		m_textureStage[_stage].colorArg1 = s_getGLTextureArg( _arg1 );
		m_textureStage[_stage].colorArg2 = s_getGLTextureArg( _arg2 );
	}
	//////////////////////////////////////////////////////////////////////////
	void MarmaladeRenderSystemES1::setTextureStageAlphaOp( uint32_t _stage, ETextureOp _textrueOp, ETextureArgument _arg1, ETextureArgument _arg2 )
	{
		if( _textrueOp == Menge::TOP_SELECTARG2 )
		{
			_arg1 = _arg2;
			_textrueOp = Menge::TOP_SELECTARG1;
		}

		m_textureStage[_stage].alphaOp = s_getGLTextureOp( _textrueOp );
		m_textureStage[_stage].alphaArg1 = s_getGLTextureArg( _arg1 );
		m_textureStage[_stage].alphaArg2 = s_getGLTextureArg( _arg2 );
	}
	//////////////////////////////////////////////////////////////////////////
	void MarmaladeRenderSystemES1::setTextureStageTexCoordIndex( uint32_t _stage, uint32_t _index )
	{
		//m_textureStage[_stage].texCoordIndex = _index;
	}
	//////////////////////////////////////////////////////////////////////////
	void MarmaladeRenderSystemES1::setTextureStageFilter( uint32_t _stage, ETextureFilter _minification, ETextureFilter _mipmap, ETextureFilter _magnification )
	{
		TextureStage & tStage = m_textureStage[_stage];

		tStage.minFilter = s_toGLMinFilter( _minification, _mipmap );
		tStage.magFilter = s_toMagFilter( _magnification );
	}
	//////////////////////////////////////////////////////////////////////////
	void MarmaladeRenderSystemES1::findFormatFromChannels_( PixelFormat _format, uint32_t _channels, PixelFormat & _hwFormat, uint32_t & _hwChannels ) const
	{
		switch( _format )
		{
		case PF_UNKNOWN:
			{
				if( _channels == 1 )
				{
					_hwFormat = PF_A8;
					_hwChannels = 1;
				}
				else if( _channels == 3 )
				{
					_hwFormat = PF_X8R8G8B8;
					_hwChannels = 4;
				}
				else if( _channels == 4 )
				{
					_hwFormat = PF_A8R8G8B8;
					_hwChannels = 4;
				}
			}break;
		default:
			{
				_hwFormat = _format;
				_hwChannels = _channels;
			}break;
		}
	}
	//////////////////////////////////////////////////////////////////////////
	RenderImageInterfacePtr MarmaladeRenderSystemES1::createImage( uint32_t _mipmaps, uint32_t _width, uint32_t _height, uint32_t _channels, uint32_t _depth, PixelFormat _format )
	{
		uint32_t hwChannels = 0;
		PixelFormat hwFormat = PF_UNKNOWN;
		this->findFormatFromChannels_( _format, _channels, hwFormat, hwChannels );

		GLint textureInternalFormat = s_toGLInternalFormat( hwFormat );

		if( textureInternalFormat == 0 )
		{
			LOGGER_ERROR( m_serviceProvider )("MarmaladeRenderSystemES1::createImage invalid get GL Texture Internal format for PF %d"
				, hwFormat
				);

			return nullptr;
		}

		GLint textureColorFormat = s_toGLColorFormat( hwFormat );

		if( textureColorFormat == 0 )
		{
			LOGGER_ERROR( m_serviceProvider )("MarmaladeRenderSystemES1::createImage invalid get GL Texture Color format for PF %d"
				, hwFormat
				);

			return nullptr;
		}

		GLint textureColorDataType = s_getGLColorDataType( hwFormat );

		if( textureColorDataType == 0 )
		{
			LOGGER_ERROR( m_serviceProvider )("MarmaladeRenderSystemES1::createImage invalid get GL Color Data Type for PF %d"
				, hwFormat
				);

			return nullptr;
		}

		MarmaladeRenderTextureES1Ptr texture = m_factoryTextureES1.createObject();

		if( texture->initialize(
			m_serviceProvider
			, ERIM_NORMAL
			, _mipmaps
			, _width
			, _height
			, hwChannels
			, hwFormat
			, textureInternalFormat
			, textureColorFormat
			, textureColorDataType ) == false )
		{
			LOGGER_ERROR( m_serviceProvider )("MarmaladeRenderSystemES1::createImage invalid initialize"
				);

			return nullptr;
		}

		return texture;
	}
	//////////////////////////////////////////////////////////////////////////
	bool MarmaladeRenderSystemES1::lockRenderTarget( const RenderImageInterfacePtr & _renderTarget )
	{
		//TODO
		return false;
	}
	//////////////////////////////////////////////////////////////////////////
	bool MarmaladeRenderSystemES1::unlockRenderTarget()
	{
		//TODO
		return false;
	}
	//////////////////////////////////////////////////////////////////////////
	bool MarmaladeRenderSystemES1::beginScene()
	{
		this->clearFrameBuffer( FBT_COLOR, 0, 1.f, 0 );

		m_currentIndexBuffer = nullptr;
		m_currentVertexBuffer = nullptr;

		for( uint32_t i = 0; i != m_glMaxCombinedTextureImageUnits; ++i )
		{
			TextureStage & stage = m_textureStage[i];

			if( stage.texture != nullptr )
			{
				stdex::intrusive_ptr_release( stage.texture );
				stage.texture = nullptr;
			}
		}

		return true;
	}
	//////////////////////////////////////////////////////////////////////////
	void MarmaladeRenderSystemES1::endScene()
	{
	}
	//////////////////////////////////////////////////////////////////////////
	void MarmaladeRenderSystemES1::swapBuffers()
	{
		GLCALL( m_serviceProvider, IwGLSwapBuffers, () );
	}
	//////////////////////////////////////////////////////////////////////////
	void MarmaladeRenderSystemES1::clearFrameBuffer( uint32_t _frameBufferTypes, uint32_t _color, float _depth, uint32_t _stencil )
	{
		GLbitfield frameBufferFlags = 0;

		if( (_frameBufferTypes & FBT_COLOR) != 0 )
		{
			frameBufferFlags |= GL_COLOR_BUFFER_BIT;

			GLCALL( m_serviceProvider, glClearColor, (
				GET_R_FLOAT_FROM_ARGB32( _color ),
				GET_G_FLOAT_FROM_ARGB32( _color ),
				GET_B_FLOAT_FROM_ARGB32( _color ),
				GET_A_FLOAT_FROM_ARGB32( _color ))
				);
		}

		if( (_frameBufferTypes & FBT_DEPTH) != 0 )
		{
			frameBufferFlags |= GL_DEPTH_BUFFER_BIT;

			if( m_depthMask == false )
			{
				GLCALL( m_serviceProvider, glDepthMask, (GL_TRUE) );
			}

			GLCALL( m_serviceProvider, glClearDepthf, (_depth) );
		}

		if( (_frameBufferTypes & FBT_STENCIL) != 0 )
		{
			frameBufferFlags |= GL_STENCIL_BUFFER_BIT;

			GLCALL( m_serviceProvider, glClearStencil, (_stencil) );
		}

		GLCALL( m_serviceProvider, glClear, (frameBufferFlags) );

		if( m_depthMask == false )
		{
			GLCALL( m_serviceProvider, glDepthMask, (GL_FALSE) );
		}
	}
	//////////////////////////////////////////////////////////////////////////
	void MarmaladeRenderSystemES1::changeWindowMode( const Resolution & _resolution, bool _fullscreen )
	{
		m_resolution = _resolution;

		//m_windowContext->setFullscreenMode( _resolution.getWidth(), _resolution.getHeight(), _fullscreen );
		//glViewport( 0, 0, _resolution.getWidth(), _resolution.getHeight() );
		//glViewport( 0, 0, _resolution.getHeight(), _resolution.getWidth() );
		//glScissor( 0, 0, _resolution.getWidth(), _resolution.getHeight() );
		//m_winWidth = _resolution.getWidth();
		//m_winHeight = _resolution.getHeight();
		//m_winContextWidth = _resolution.getWidth();
		//m_winContextHeight = _resolution.getHeight();
	}
	//////////////////////////////////////////////////////////////////////////
	bool MarmaladeRenderSystemES1::supportTextureFormat( PixelFormat _format ) const
	{
		return s_toGLInternalFormat( _format ) != 0;
	}
	//////////////////////////////////////////////////////////////////////////
	bool MarmaladeRenderSystemES1::supportTextureNonPow2() const
	{
		return false;
	}
	//////////////////////////////////////////////////////////////////////////
	uint32_t MarmaladeRenderSystemES1::getMaxCombinedTextureImageUnits() const
	{
		return m_glMaxCombinedTextureImageUnits;
	}
	//////////////////////////////////////////////////////////////////////////
	void MarmaladeRenderSystemES1::onWindowMovedOrResized()
	{

	}
	//////////////////////////////////////////////////////////////////////////
	void MarmaladeRenderSystemES1::onWindowClose()
	{

	}
	//////////////////////////////////////////////////////////////////////////
	void MarmaladeRenderSystemES1::setVSync( bool _vSync )
	{
		//m_windowContext->setVSync( _vSync );
	}
	//////////////////////////////////////////////////////////////////////////
	void MarmaladeRenderSystemES1::clear( uint32_t _color, bool _force )
	{

	}
	//////////////////////////////////////////////////////////////////////////
	RenderImageInterfacePtr MarmaladeRenderSystemES1::createDynamicImage( uint32_t _width, uint32_t _height, uint32_t _channels, uint32_t _depth, PixelFormat _format )
	{
		uint32_t hwChannels = 0;
		PixelFormat hwFormat = PF_UNKNOWN;
		this->findFormatFromChannels_( _format, _channels, hwFormat, hwChannels );

		GLint textureInternalFormat = s_toGLInternalFormat( hwFormat );

		if( textureInternalFormat == 0 )
		{
			LOGGER_ERROR( m_serviceProvider )("MarmaladeRenderSystemES1::createDynamicImage invalid get GL Texture Internal format for PF %d"
				, hwFormat
				);

			return nullptr;
		}

		GLint textureColorFormat = s_toGLColorFormat( hwFormat );

		if( textureColorFormat == 0 )
		{
			LOGGER_ERROR( m_serviceProvider )("MarmaladeRenderSystemES1::createDynamicImage invalid get GL Texture Color format for PF %d"
				, hwFormat
				);

			return nullptr;
		}

		GLint textureColorDataType = s_getGLColorDataType( hwFormat );

		if( textureColorDataType == 0 )
		{
			LOGGER_ERROR( m_serviceProvider )("MarmaladeRenderSystemES1::createDynamicImage invalid get GL Color Data Type for PF %d"
				, hwFormat
				);

			return nullptr;
		}

		MarmaladeRenderTextureES1Ptr texture = m_factoryTextureES1.createObject();

		if( texture->initialize(
			m_serviceProvider
			, ERIM_DYNAMIC
			, 1
			, _width
			, _height
			, hwChannels
			, hwFormat
			, textureInternalFormat
			, textureColorFormat
			, textureColorDataType ) == false )
		{
			LOGGER_ERROR( m_serviceProvider )("MarmaladeRenderSystemES1::createDynamicImage invalid initialize"
				);

			return nullptr;
		}

		return texture;
	}
	//////////////////////////////////////////////////////////////////////////
	RenderTargetInterface * MarmaladeRenderSystemES1::createRenderTargetOffscreen( uint32_t _width, uint32_t _height, PixelFormat _format )
	{
		return nullptr;
	}
	//////////////////////////////////////////////////////////////////////////
	void MarmaladeRenderSystemES1::makeProjectionOrthogonal( mt::mat4f & _projectionMatrix, const Viewport & _viewport, float _near, float _far )
	{
		mt::mat4f scale;
		mt::make_scale_m4( scale, 1.f, 1.f, 1.f );

		mt::mat4f translation;
		mt::make_translation_m4( translation, 0.f, 0.f, 0.f );

		mt::mat4f transform;
		mt::mul_m4_m4( transform, scale, translation );

		mt::mat4f ortho;
		mt::make_projection_ortho_lh_m4( ortho, _viewport.begin.x, _viewport.end.x, _viewport.begin.y, _viewport.end.y, _near, _far );

		mt::mul_m4_m4( _projectionMatrix, transform, ortho );
	}
	//////////////////////////////////////////////////////////////////////////
	void MarmaladeRenderSystemES1::makeProjectionPerspective( mt::mat4f & _projectionMatrix, float _fov, float _aspect, float _zn, float _zf )
	{
		mt::mat4f scale;
		mt::make_scale_m4( scale, 1.0f, 1.0f, 1.0f );

		mt::mat4f translation;
		mt::make_translation_m4( translation, -0.5f, +0.5f, 0.0f );

		mt::mat4f transform;
		mt::mul_m4_m4( transform, scale, translation );

		mt::mat4f projection_fov;
		mt::make_projection_fov_m4( projection_fov, _fov, _aspect, _zn, _zf );

		mt::mul_m4_m4( _projectionMatrix, transform, projection_fov );
	}
	//////////////////////////////////////////////////////////////////////////
	void MarmaladeRenderSystemES1::makeProjectionFrustum( mt::mat4f & _projectionMatrix, const Viewport & _viewport, float _near, float _far )
	{
		mt::mat4f scale;
		mt::make_scale_m4( scale, 1.0f, 1.0f, 1.0f );

		mt::mat4f translation;
		mt::make_translation_m4( translation, -0.5f, -0.5f, 0.0f );

		mt::mat4f transform;
		mt::mul_m4_m4( transform, scale, translation );

		mt::mat4f frustum;
		mt::make_projection_frustum_m4( frustum, _viewport.begin.x, _viewport.end.x, _viewport.begin.y, _viewport.end.y, _near, _far );

		mt::mul_m4_m4( _projectionMatrix, transform, frustum );
	}
	//////////////////////////////////////////////////////////////////////////
	void MarmaladeRenderSystemES1::makeViewMatrixFromViewport( mt::mat4f & _viewMatrix, const Viewport & _viewport )
	{
		mt::mat4f wm;
		mt::ident_m4( wm );

		mt::inv_m4( _viewMatrix, wm );
	}
	//////////////////////////////////////////////////////////////////////////
	void MarmaladeRenderSystemES1::makeViewMatrixLookAt( mt::mat4f & _viewMatrix, const mt::vec3f & _eye, const mt::vec3f & _dir, const mt::vec3f & _up, float _sign )
	{
		mt::make_lookat_m4( _viewMatrix, _eye, _dir, _up, _sign );
	}
}	// namespace Menge
