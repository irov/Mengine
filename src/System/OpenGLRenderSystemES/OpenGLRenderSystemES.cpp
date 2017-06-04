#	include "OpenGLRenderSystemES.h"
#	include "OpenGLRenderEnumES.h"
#	include "OpenGLRenderErrorES.h"

#	include "Interface/StringizeInterface.h"

#   include "Logger/Logger.h"

#   include "Factory/FactoryPool.h"

#	include <cmath>

#	include <stdex/allocator.h>

//////////////////////////////////////////////////////////////////////////
#	define GET_A_FLOAT_FROM_ARGB32( argb ) ( ((float)(argb >> 24)) / 255.0f )
#	define GET_R_FLOAT_FROM_ARGB32( argb ) ( ((float)((argb >> 16) & 0xFF)) / 255.0f )
#	define GET_G_FLOAT_FROM_ARGB32( argb ) ( ((float)((argb >> 8) & 0xFF)) / 255.0f )
#	define GET_B_FLOAT_FROM_ARGB32( argb ) ( (float)(argb & 0xFF) / 255.0f )
//////////////////////////////////////////////////////////////////////////
SERVICE_FACTORY(RenderSystem, Menge::OpenGLRenderSystemES);
//////////////////////////////////////////////////////////////////////////
namespace Menge
{
	//////////////////////////////////////////////////////////////////////////
	OpenGLRenderSystemES::OpenGLRenderSystemES()
		: m_depthMask( false )
        , m_renderWindowCreate(false)
		, m_glMaxClipPlanes(0)
		, m_glMaxCombinedTextureImageUnits(0)
	{
		mt::ident_m4( m_worldMatrix );
		mt::ident_m4( m_viewMatrix );
		mt::ident_m4( m_projectionMatrix );
	}
	//////////////////////////////////////////////////////////////////////////
	OpenGLRenderSystemES::~OpenGLRenderSystemES()
	{
	}
	//////////////////////////////////////////////////////////////////////////
	bool OpenGLRenderSystemES::_initialize()
	{
		LOGGER_WARNING(m_serviceProvider)("Initializing OpenGL RenderSystem...");

		m_renderPlatform = STRINGIZE_STRING_LOCAL( m_serviceProvider, "Marmalade" );

		m_factoryVertexBuffer = new FactoryPool<OpenGLRenderVertexBufferES, 8>( m_serviceProvider );
		m_factoryIndexBuffer = new FactoryPool<OpenGLRenderIndexBufferES, 8>( m_serviceProvider );

		m_factoryRenderImage = Helper::makeFactoryPool<OpenGLRenderImageES, 128>( m_serviceProvider, this, &OpenGLRenderSystemES::onRenderImageDestroy_ );

		m_factoryRenderFragmentShader = new FactoryPool<OpenGLRenderFragmentShaderES, 16>( m_serviceProvider );
		m_factoryRenderVertexShader = new FactoryPool<OpenGLRenderVertexShaderES, 16>( m_serviceProvider );
		m_factoryRenderProgram = new FactoryPool<OpenGLRenderProgramES, 16>( m_serviceProvider );

		return true;
	}
	//////////////////////////////////////////////////////////////////////////
    void OpenGLRenderSystemES::_finalize()
    {
		m_currentIndexBuffer = nullptr;
		m_currentVertexBuffer = nullptr;

		m_currentProgram = nullptr;
        
        m_factoryVertexBuffer = nullptr;
        m_factoryIndexBuffer = nullptr;
        
        m_factoryRenderImage = nullptr;
        m_factoryRenderFragmentShader = nullptr;
        m_factoryRenderVertexShader = nullptr;
        m_factoryRenderProgram = nullptr;

    }
	//////////////////////////////////////////////////////////////////////////
	const ConstString & OpenGLRenderSystemES::getRenderPlatformName() const
	{
		return m_renderPlatform;
	}
	//////////////////////////////////////////////////////////////////////////
	bool OpenGLRenderSystemES::createRenderWindow( const Resolution & _resolution, uint32_t _bits, bool _fullscreen, bool _waitForVSync, int _FSAAType, int _FSAAQuality, uint32_t _MultiSampleCount )
	{
		(void)_bits;
		(void)_fullscreen;
		(void)_waitForVSync;
		(void)_FSAAType;
		(void)_FSAAQuality;
		(void)_MultiSampleCount;

        m_resolution = _resolution;
        
        OPENGL_RENDER_CHECK_ERROR( m_serviceProvider );
        
        LOGGER_WARNING(m_serviceProvider)("Vendor      : %s", (const char*)glGetString( GL_VENDOR ) );
        LOGGER_WARNING(m_serviceProvider)("Renderer    : %s", (const char*)glGetString( GL_RENDERER ) );
        LOGGER_WARNING(m_serviceProvider)("Version     : %s", (const char*)glGetString( GL_VERSION ) );
        LOGGER_WARNING(m_serviceProvider)("Extensions  : %s", (const char*)glGetString( GL_EXTENSIONS ) );
        
        OPENGL_RENDER_CHECK_ERROR( m_serviceProvider );
        
        GLCALL( m_serviceProvider, glFrontFace, ( GL_CW ) );
        GLCALL( m_serviceProvider, glDisable, ( GL_DEPTH_TEST ) );
        GLCALL( m_serviceProvider, glDisable, ( GL_STENCIL_TEST ) );
        GLCALL( m_serviceProvider, glDisable, ( GL_CULL_FACE ) );

        GLCALL( m_serviceProvider, glDisable, ( GL_BLEND ) );
        
        GLCALL( m_serviceProvider, glDisable, ( GL_DITHER ) );
        
        GLCALL( m_serviceProvider, glDepthMask, ( GL_FALSE ) );
        
        GLint maxClipPlanes;
        glGetIntegerv( GL_MAX_CLIP_PLANES, &maxClipPlanes );
        
        m_glMaxClipPlanes = maxClipPlanes;
        
        GLint maxCombinedTextureImageUnits;
        glGetIntegerv( GL_MAX_COMBINED_TEXTURE_IMAGE_UNITS, &maxCombinedTextureImageUnits );
        
        m_glMaxCombinedTextureImageUnits = maxCombinedTextureImageUnits;
        
        for( TVectorRenderVertexShaders::iterator
            it = m_deferredCompileVertexShaders.begin(),
            it_end = m_deferredCompileVertexShaders.end();
            it != it_end;
            ++it )
        {
            const OpenGLRenderVertexShaderESPtr & shader = *it;
            
            if( shader->compile() == false )
            {
                return false;
            }
        }
        
        m_deferredCompileVertexShaders.clear();
        
        for( TVectorRenderFragmentShaders::iterator
            it = m_deferredCompileFragmentShaders.begin(),
            it_end = m_deferredCompileFragmentShaders.end();
            it != it_end;
            ++it )
        {
            const OpenGLRenderFragmentShaderESPtr & shader = *it;
            
            if( shader->compile() == false )
            {
                return false;
            }
        }
        
        m_deferredCompileFragmentShaders.clear();
        
        for( TVectorRenderPrograms::iterator
            it = m_deferredCompilePrograms.begin(),
            it_end = m_deferredCompilePrograms.end();
            it != it_end;
            ++it )
        {
            const OpenGLRenderProgramESPtr & program = *it;
            
            if( program->compile() == false )
            {
                return false;
            }
        }
        
        m_deferredCompilePrograms.clear();
        
        m_renderWindowCreate = true;
		        		
        return true;
	}
	//////////////////////////////////////////////////////////////////////////
	bool OpenGLRenderSystemES::screenshot( const RenderImageInterfacePtr & _image, const mt::vec4f & _rect )
	{
		(void)_image;
		(void)_rect;
        //ToDo!
		
		return false;
	}
	//////////////////////////////////////////////////////////////////////////
	void OpenGLRenderSystemES::setClipplaneCount( uint32_t _count )
	{
		//ToDo
	}
	//////////////////////////////////////////////////////////////////////////
	void OpenGLRenderSystemES::setClipplane( uint32_t _i, const mt::planef & _plane )
	{ 
		//ToDo
	}
    //////////////////////////////////////////////////////////////////////////
    void OpenGLRenderSystemES::setViewport( const Viewport & _viewport )
    {
        float w = _viewport.getWidth();
        float h = _viewport.getHeight();
        float xb = _viewport.begin.x;
        float yb = _viewport.end.y;

        size_t height = m_resolution.getHeight();

        GLCALL( m_serviceProvider, glViewport, ( (GLint)xb, (GLint)height - (GLint)(yb), (GLsizei)w, (GLsizei)h ) );
    }
	//////////////////////////////////////////////////////////////////////////
	void OpenGLRenderSystemES::setProjectionMatrix( const mt::mat4f & _projection )
	{
		m_projectionMatrix = _projection;
	}
	//////////////////////////////////////////////////////////////////////////
	void OpenGLRenderSystemES::setModelViewMatrix( const mt::mat4f & _view )
	{
		m_viewMatrix = _view;
	}
	//////////////////////////////////////////////////////////////////////////
	void OpenGLRenderSystemES::setTextureMatrix( uint32_t _stage, const mt::mat4f & _texture )
	{
        // To Do
	}
    //////////////////////////////////////////////////////////////////////////
    void OpenGLRenderSystemES::setWorldMatrix( const mt::mat4f & _world )
    {
		m_worldMatrix = _world;
    }
	//////////////////////////////////////////////////////////////////////////
	RenderVertexBufferInterfacePtr OpenGLRenderSystemES::createVertexBuffer( uint32_t _verticesNum, bool _dynamic )
	{
		OpenGLRenderVertexBufferESPtr buffer = m_factoryVertexBuffer->createObject();

		if( buffer->initialize( m_serviceProvider, _verticesNum, _dynamic ) == false )
		{
			return nullptr;
		}

		return buffer;
	}
	//////////////////////////////////////////////////////////////////////////
	bool OpenGLRenderSystemES::setVertexBuffer( const RenderVertexBufferInterfacePtr & _vertexBuffer )
	{
		m_currentVertexBuffer = _vertexBuffer;

		return true;
	}
	//////////////////////////////////////////////////////////////////////////
	RenderIndexBufferInterfacePtr OpenGLRenderSystemES::createIndexBuffer( uint32_t _indiciesNum, bool _dynamic )
	{
		OpenGLRenderIndexBufferESPtr buffer = m_factoryIndexBuffer->createObject();

		if( buffer->initialize( m_serviceProvider, _indiciesNum, _dynamic ) == false )
		{
			return nullptr;
		}

		return buffer;
	}
	//////////////////////////////////////////////////////////////////////////
	bool OpenGLRenderSystemES::setIndexBuffer( const RenderIndexBufferInterfacePtr & _indexBuffer )
	{
		m_currentIndexBuffer = _indexBuffer;

		return true;
	}
	//////////////////////////////////////////////////////////////////////////
	RenderFragmentShaderInterfacePtr OpenGLRenderSystemES::createFragmentShader( const ConstString & _name, const void * _buffer, size_t _size, bool _isCompile )
	{
		OpenGLRenderFragmentShaderESPtr shader = m_factoryRenderFragmentShader->createObject();

        if( shader == nullptr )
        {
            LOGGER_ERROR( m_serviceProvider )("OpenGLRenderSystemES::createFragmentShader invalid create shader %s"
                                              , _name.c_str()
                                              );
            
            return nullptr;
        }
        
		shader->setServiceProvider( m_serviceProvider );
        
        MemoryInterfacePtr memory = MEMORY_SERVICE(m_serviceProvider)
            ->createMemory();
        
        if( memory == nullptr )
        {
            LOGGER_ERROR( m_serviceProvider )("OpenGLRenderSystemES::createFragmentShader invalid create memory for shader %s"
                                              , _name.c_str()
                                              );
            
            return nullptr;
        }
        
        memory->setMemory( _buffer, _size, __FILE__, __LINE__ );
        
        if( shader->initialize( _name, memory, _isCompile ) == false )
        {
            LOGGER_ERROR( m_serviceProvider )("OpenGLRenderSystemES::createFragmentShader invalid initialize shader %s"
                                              , _name.c_str()
                                              );
            
            return nullptr;
        }
        
        if( m_renderWindowCreate == true )
        {
            if( shader->compile() == false )
            {
                LOGGER_ERROR( m_serviceProvider )("OpenGLRenderSystemES::createFragmentShader invalid compile shader %s"
                                                  , _name.c_str()
                                                  );
                
                return nullptr;
            }
        }
        else
        {
            m_deferredCompileFragmentShaders.push_back( shader );
        }
        
        return shader;
	}
	//////////////////////////////////////////////////////////////////////////
	RenderVertexShaderInterfacePtr OpenGLRenderSystemES::createVertexShader( const ConstString & _name, const void * _buffer, size_t _size, bool _isCompile )
	{
		OpenGLRenderVertexShaderESPtr shader = m_factoryRenderVertexShader->createObject();

        if( shader == nullptr )
        {
            LOGGER_ERROR( m_serviceProvider )("OpenGLRenderSystemES::createVertexShader invalid create shader %s"
                                              , _name.c_str()
                                              );
            
            return nullptr;
        }

        
		shader->setServiceProvider( m_serviceProvider );

        MemoryInterfacePtr memory = MEMORY_SERVICE( m_serviceProvider )
        ->createMemory();
        
        if( memory == nullptr )
        {
            LOGGER_ERROR( m_serviceProvider )("OpenGLRenderSystemES::createVertexShader invalid create memory for shader %s"
                                              , _name.c_str()
                                              );
            
            return nullptr;
        }
        
        memory->setMemory( _buffer, _size, __FILE__, __LINE__ );
        
        if( shader->initialize( _name, memory, _isCompile ) == false )
        {
            LOGGER_ERROR( m_serviceProvider )("OpenGLRenderSystemES::createVertexShader invalid initialize shader %s"
                                              , _name.c_str()
                                              );
            
            return nullptr;
        }
        
        if( m_renderWindowCreate == true )
        {
            if( shader->compile() == false )
            {
                LOGGER_ERROR( m_serviceProvider )("OpenGLRenderSystemES::createVertexShader invalid compile shader %s"
                                                  , _name.c_str()
                                                  );
                
                return nullptr;
            }
        }
        else
        {
            m_deferredCompileVertexShaders.push_back( shader );
        }
        
        return shader;
	}
	//////////////////////////////////////////////////////////////////////////
	RenderProgramInterfacePtr OpenGLRenderSystemES::createProgram( const ConstString & _name, const RenderVertexShaderInterfacePtr & _vertex, const RenderFragmentShaderInterfacePtr & _fragment, uint32_t _samplerCount )
	{
		OpenGLRenderProgramESPtr program = m_factoryRenderProgram->createObject();
        
        if( program == nullptr )
        {
            LOGGER_ERROR( m_serviceProvider )("OpenGLRenderSystemES::createProgram invalid create program %s"
                                              , _name.c_str()
                                              );
            
            return nullptr;
        }

		program->setServiceProvider( m_serviceProvider );
				
        if( program->initialize( _name, _vertex, _fragment, _samplerCount ) == false )
        {
            LOGGER_ERROR( m_serviceProvider )("OpenGLRenderSystemES::createProgram invalid initialize program %s"
                                              , _name.c_str()
                                              );
            
            return nullptr;
        }
        
        if( m_renderWindowCreate == true )
        {
            if( program->compile() == false )
            {
                LOGGER_ERROR( m_serviceProvider )("OpenGLRenderSystemES::createProgram invalid compile program %s"
                                                  , _name.c_str()
                                                  );
                
                return nullptr;
            }
        }
        else
        {
            m_deferredCompilePrograms.push_back( program );
        }

		return program;
	}
	//////////////////////////////////////////////////////////////////////////
	void OpenGLRenderSystemES::setProgram( const RenderProgramInterfacePtr & _program )
	{
		m_currentProgram = stdex::intrusive_static_cast<OpenGLRenderProgramESPtr>(_program);
	}
	//////////////////////////////////////////////////////////////////////////
	void OpenGLRenderSystemES::updateProgram( const RenderProgramInterfacePtr & _program )
	{
		(void)_program;
	}
	//////////////////////////////////////////////////////////////////////////
	void OpenGLRenderSystemES::bindBuffer_( GLuint _vertexId, GLuint _indexId )
	{
		GLCALL( m_serviceProvider, glBindBuffer, (GL_ARRAY_BUFFER, _vertexId) );
		GLCALL( m_serviceProvider, glBindBuffer, (GL_ELEMENT_ARRAY_BUFFER, _indexId) );

		GLCALL( m_serviceProvider, glEnableClientState, (GL_VERTEX_ARRAY) );
		GLCALL( m_serviceProvider, glVertexPointer, (3, GL_FLOAT, sizeof( RenderVertex2D ), reinterpret_cast<const GLvoid *>(offsetof( RenderVertex2D, position ))) );

		GLCALL( m_serviceProvider, glEnableClientState, (GL_COLOR_ARRAY) );
		GLCALL( m_serviceProvider, glColorPointer, (4, GL_UNSIGNED_BYTE, sizeof( RenderVertex2D ), reinterpret_cast<const GLvoid *>(offsetof( RenderVertex2D, color ))) );

		GLCALL( m_serviceProvider, glEnableClientState, (GL_TEXTURE_COORD_ARRAY) );

		for( uint32_t i = 0; i != MENGINE_RENDER_VERTEX_UV_COUNT; ++i )
		{
			GLCALL( m_serviceProvider, glClientActiveTexture, (GL_TEXTURE0 + i) );
			GLCALL( m_serviceProvider, glTexCoordPointer, (2, GL_FLOAT, sizeof( RenderVertex2D ), reinterpret_cast<const GLvoid *>(offsetof( RenderVertex2D, uv ) + sizeof( mt::vec2f ) * i)) );
		}
	}
	//////////////////////////////////////////////////////////////////////////
	void OpenGLRenderSystemES::drawIndexedPrimitive( EPrimitiveType _type, 
		uint32_t _baseVertexIndex,  uint32_t _minIndex, 
		uint32_t _verticesNum, uint32_t _startIndex, uint32_t _indexCount )
	{
		if( m_currentIndexBuffer == nullptr || m_currentVertexBuffer == nullptr )
		{
			return;
		}

		if( m_currentProgram != nullptr )
		{			
			m_currentProgram->enable();
			
			m_currentProgram->bindMatrix( m_worldMatrix, m_viewMatrix, m_projectionMatrix );
		}

		for( uint32_t i = 0; i != MENGE_MAX_TEXTURE_STAGES; ++i )
		{
			const TextureStage & textureStage = m_textureStage[i];

			GLCALL( m_serviceProvider, glActiveTexture, (GL_TEXTURE0 + i) );

			if( textureStage.texture == nullptr )
			{	
				GLCALL( m_serviceProvider, glBindTexture, (GL_TEXTURE_2D, 0) );

				continue;
			}

			GLuint texture_uid = textureStage.texture->getUId();

			GLCALL( m_serviceProvider, glBindTexture, (GL_TEXTURE_2D, texture_uid) );

			if( m_currentProgram != nullptr )
			{
				m_currentProgram->bindTexture( i );
			}

			GLCALL( m_serviceProvider, glTexParameteri, ( GL_TEXTURE_2D, GL_TEXTURE_WRAP_S, textureStage.wrapS ) );
			GLCALL( m_serviceProvider, glTexParameteri, ( GL_TEXTURE_2D, GL_TEXTURE_WRAP_T, textureStage.wrapT ) );
			GLCALL( m_serviceProvider, glTexParameteri, ( GL_TEXTURE_2D, GL_TEXTURE_MIN_FILTER, textureStage.minFilter ) );
			GLCALL( m_serviceProvider, glTexParameteri, ( GL_TEXTURE_2D, GL_TEXTURE_MAG_FILTER, textureStage.magFilter ) );
		}

		OpenGLRenderIndexBufferES * ib = stdex::intrusive_get<OpenGLRenderIndexBufferES *>(m_currentIndexBuffer);
		ib->enable();

		OpenGLRenderVertexBufferES * vb = stdex::intrusive_get<OpenGLRenderVertexBufferES *>(m_currentVertexBuffer);
		vb->enable();
		
		GLCALL( m_serviceProvider, glEnableVertexAttribArray, (VERTEX_POSITION_ARRAY) );
		GLCALL( m_serviceProvider, glEnableVertexAttribArray, (VERTEX_COLOR_ARRAY) );

		for( uint32_t i = 0; i != MENGINE_RENDER_VERTEX_UV_COUNT; ++i )
		{
			GLCALL( m_serviceProvider, glEnableVertexAttribArray, (VERTEX_UV0_ARRAY + i) );
		}
		
		GLCALL( m_serviceProvider, glVertexAttribPointer, (VERTEX_POSITION_ARRAY, 3, GL_FLOAT, GL_FALSE, sizeof( RenderVertex2D ), reinterpret_cast<const GLvoid *>(offsetof( RenderVertex2D, position ))) );
		GLCALL( m_serviceProvider, glVertexAttribPointer, (VERTEX_COLOR_ARRAY, 4, GL_UNSIGNED_BYTE, GL_TRUE, sizeof( RenderVertex2D ), reinterpret_cast<const GLvoid *>(offsetof( RenderVertex2D, color ))) );

		for( uint32_t i = 0; i != MENGINE_RENDER_VERTEX_UV_COUNT; ++i )
		{
			size_t uv_offset = offsetof( RenderVertex2D, uv ) + sizeof( mt::vec2f ) * i;

			GLCALL( m_serviceProvider, glVertexAttribPointer, (VERTEX_UV0_ARRAY + i, 2, GL_FLOAT, GL_FALSE, sizeof( RenderVertex2D ), reinterpret_cast<const GLvoid *>(uv_offset)) );
		}

        GLenum mode = s_getGLPrimitiveMode( _type );
		const RenderIndices * baseIndex = nullptr;
		const RenderIndices * offsetIndex = baseIndex + _startIndex;

		GLenum indexType = s_getGLIndexType();

		GLCALL( m_serviceProvider, glDrawElements, (mode, _indexCount, indexType, reinterpret_cast<const GLvoid *>(offsetIndex)) );

		GLCALL( m_serviceProvider, glDisableVertexAttribArray, (VERTEX_POSITION_ARRAY) );
		GLCALL( m_serviceProvider, glDisableVertexAttribArray, (VERTEX_COLOR_ARRAY) );
		
		for( uint32_t i = 0; i != MENGINE_RENDER_VERTEX_UV_COUNT; ++i )
		{
			GLCALL( m_serviceProvider, glDisableVertexAttribArray, (VERTEX_UV0_ARRAY + i) );
		}
		
		for( uint32_t i = 0; i != MENGE_MAX_TEXTURE_STAGES; ++i )
		{
			TextureStage & textureStage = m_textureStage[i];

			if( textureStage.texture == nullptr )
			{
				break;
			}

			GLCALL( m_serviceProvider, glActiveTexture, (GL_TEXTURE0 + i) );

			GLCALL( m_serviceProvider, glBindTexture, (GL_TEXTURE_2D, 0) );

			stdex::intrusive_ptr_release( textureStage.texture );
		}

		if( m_currentProgram != nullptr )
		{
			m_currentProgram->disable();
			m_currentProgram = nullptr;
		}

		GLCALL( m_serviceProvider, glBindBuffer, ( GL_ARRAY_BUFFER, 0 ) );
		GLCALL( m_serviceProvider, glBindBuffer, ( GL_ELEMENT_ARRAY_BUFFER, 0 ) );
	}
	//////////////////////////////////////////////////////////////////////////
	void OpenGLRenderSystemES::setTexture( uint32_t _stage, const RenderImageInterfacePtr & _texture )
	{        
        TextureStage & tStage = m_textureStage[_stage];

        if( _texture != nullptr )
        {
            OpenGLRenderImageES * texture = stdex::intrusive_get<OpenGLRenderImageES *>(_texture);
            
			stdex::intrusive_ptr_setup( tStage.texture, texture );
        }
        else
        {			
			stdex::intrusive_ptr_release( tStage.texture );
        }
	}
	//////////////////////////////////////////////////////////////////////////
	void OpenGLRenderSystemES::setTextureAddressing( uint32_t _stage, ETextureAddressMode _modeU, ETextureAddressMode _modeV, uint32_t _border )
	{
        GLenum modeUGL = s_getGLAddressMode( _modeU );
        GLenum modeVGL = s_getGLAddressMode( _modeV );

        m_textureStage[_stage].wrapS = modeUGL;
        m_textureStage[_stage].wrapT = modeVGL;

		m_textureStage[_stage].border = _border;
	}
	//////////////////////////////////////////////////////////////////////////
	void OpenGLRenderSystemES::setTextureFactor( uint32_t _color )
	{

	}
	//////////////////////////////////////////////////////////////////////////
	void OpenGLRenderSystemES::setBlendFactor( EBlendFactor _src, EBlendFactor _dst, EBlendOp _op )
	{
		GLenum srcBlendFactor = s_toGLBlendFactor( _src );
		GLenum dstBlendFactor = s_toGLBlendFactor( _dst );
		GLenum blendOp = s_toGLBlendFactor( _op );

		GLCALL( m_serviceProvider, glBlendFunc, ( srcBlendFactor, dstBlendFactor ) );
		GLCALL( m_serviceProvider, glBlendEquation, (blendOp) );
	}
	//////////////////////////////////////////////////////////////////////////
	void OpenGLRenderSystemES::setCullMode( ECullMode _mode )
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
		
        GLCALL( m_serviceProvider, glCullFace, ( face ) );
		
        if( enable == true )
        {
            GLCALL( m_serviceProvider, glEnable, ( GL_CULL_FACE ) );
        }
        else
        {
            GLCALL( m_serviceProvider, glDisable, ( GL_CULL_FACE ) );
        }
	}
    /////////////////////////////////////////////////////////////////////////
    void OpenGLRenderSystemES::setSeparateAlphaBlendMode()
    {
    
    }
	//////////////////////////////////////////////////////////////////////////
	void OpenGLRenderSystemES::setDepthBufferTestEnable( bool _depthTest )
	{
        if( _depthTest == true )
        {
		    GLCALL( m_serviceProvider, glEnable, ( GL_DEPTH_TEST ) );
        }
        else
        {
            GLCALL( m_serviceProvider, glDisable, ( GL_DEPTH_TEST ) );
        }
	}
	//////////////////////////////////////////////////////////////////////////
	void OpenGLRenderSystemES::setDepthBufferWriteEnable( bool _depthWrite )
	{
		m_depthMask = _depthWrite;

		GLCALL( m_serviceProvider, glDepthMask, ( m_depthMask ? GL_TRUE : GL_FALSE ) );
	}
	//////////////////////////////////////////////////////////////////////////
	void OpenGLRenderSystemES::setDepthBufferCmpFunc( ECompareFunction _depthFunction )
	{
		GLenum cmpFunc = s_toGLCmpFunc( _depthFunction );

		GLCALL( m_serviceProvider, glDepthFunc, ( cmpFunc ) );
	}
	//////////////////////////////////////////////////////////////////////////
	void OpenGLRenderSystemES::setFillMode( EFillMode _mode )
	{
		/*GLenum mode = s_toGLFillMode[ _mode ];
		glPolygonMode( GL_FRONT_AND_BACK, mode );*/
	}
	//////////////////////////////////////////////////////////////////////////
	void OpenGLRenderSystemES::setColorBufferWriteEnable( bool _r, bool _g, bool _b, bool _a )
	{
		GLCALL( m_serviceProvider, glColorMask, ( _r ? GL_TRUE : GL_FALSE, _g ? GL_TRUE : GL_FALSE, _b ? GL_TRUE : GL_FALSE, _a ? GL_TRUE : GL_FALSE ) );
	}
	//////////////////////////////////////////////////////////////////////////
	void OpenGLRenderSystemES::setShadeType( EShadeType _sType )
	{
		GLenum model = s_toGLShadeMode( _sType );
		GLCALL( m_serviceProvider, glShadeModel, ( model ) );
	}
	//////////////////////////////////////////////////////////////////////////
	void OpenGLRenderSystemES::setAlphaBlendEnable( bool _alphaBlend )
	{
		if( _alphaBlend == true )
        {
            GLCALL( m_serviceProvider, glEnable, ( GL_BLEND ) );
        }
        else
        {
            GLCALL( m_serviceProvider, glDisable, ( GL_BLEND ) );
        }
	}
	//////////////////////////////////////////////////////////////////////////
	void OpenGLRenderSystemES::setLightingEnable( bool _light )
	{
        (void)_light;
	}
	//////////////////////////////////////////////////////////////////////////
	void OpenGLRenderSystemES::setTextureStageColorOp( uint32_t _stage, ETextureOp _textrueOp,  ETextureArgument _arg1, ETextureArgument _arg2 )
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
	void OpenGLRenderSystemES::setTextureStageAlphaOp( uint32_t _stage, ETextureOp _textrueOp,  ETextureArgument _arg1, ETextureArgument _arg2 )
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
    void OpenGLRenderSystemES::setTextureStageTexCoordIndex( uint32_t _stage, uint32_t _index )
    {
        //m_textureStage[_stage].texCoordIndex = _index;
    }
	//////////////////////////////////////////////////////////////////////////
	void OpenGLRenderSystemES::setTextureStageFilter( uint32_t _stage, ETextureFilter _minification, ETextureFilter _mipmap, ETextureFilter _magnification )
	{
        TextureStage & tStage = m_textureStage[_stage];

		tStage.minFilter = s_toGLMinFilter( _minification, _mipmap );
		tStage.magFilter = s_toMagFilter( _magnification );
	}
    //////////////////////////////////////////////////////////////////////////
    void OpenGLRenderSystemES::findFormatFromChannels_( PixelFormat _format, uint32_t _channels, PixelFormat & _hwFormat, uint32_t & _hwChannels ) const
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
	RenderImageInterfacePtr OpenGLRenderSystemES::createImage( uint32_t _mipmaps, uint32_t _width, uint32_t _height, uint32_t _channels, uint32_t _depth, PixelFormat _format )
	{
        uint32_t hwChannels = 0;
        PixelFormat hwFormat = PF_UNKNOWN;
		this->findFormatFromChannels_( _format, _channels, hwFormat, hwChannels );

		GLint textureInternalFormat = s_toGLInternalFormat( hwFormat );

		if( textureInternalFormat == 0 )
		{
			LOGGER_ERROR(m_serviceProvider)("OpenGLRenderSystemES::createImage invalid get GL Texture Internal format for PF %d"
				, hwFormat
				);

			return nullptr;
		}

		GLint textureColorFormat = s_toGLColorFormat( hwFormat );

		if( textureColorFormat == 0 )
		{
			LOGGER_ERROR(m_serviceProvider)("OpenGLRenderSystemES::createImage invalid get GL Texture Color format for PF %d"
				, hwFormat
				);

			return nullptr;
		}

		GLint textureColorDataType = s_getGLColorDataType( hwFormat );

		if( textureColorDataType == 0 )
		{
			LOGGER_ERROR( m_serviceProvider )("OpenGLRenderSystemES::createImage invalid get GL Color Data Type for PF %d"
				, hwFormat
				);

			return nullptr;
		}

		OpenGLRenderImageESPtr image = m_factoryRenderImage->createObject();

		if( image->initialize( ERIM_NORMAL
			, _mipmaps
			, _width
			, _height
			, hwChannels
			, hwFormat
			, textureInternalFormat
			, textureColorFormat
			, textureColorDataType ) == false )
		{
			LOGGER_ERROR( m_serviceProvider )("OpenGLRenderSystemES::createImage invalid initialize"
				);

			return nullptr;
		}

		OpenGLRenderImageES * image_ptr = image.get();
		m_images.push_back( image_ptr );

		return image;
	}
	//////////////////////////////////////////////////////////////////////////
	bool OpenGLRenderSystemES::lockRenderTarget( const RenderImageInterfacePtr & _renderTarget )
	{
		//TODO
		return false;
	}
	//////////////////////////////////////////////////////////////////////////
	bool OpenGLRenderSystemES::unlockRenderTarget()
	{
		//TODO
		return false;
	}
    //////////////////////////////////////////////////////////////////////////
    bool OpenGLRenderSystemES::beginScene()
    {
		this->clearFrameBuffer( FBT_COLOR, 0, 1.f, 0 );

		m_currentIndexBuffer = nullptr;
		m_currentVertexBuffer = nullptr;

		m_currentProgram = nullptr;

        return true;
    }
	//////////////////////////////////////////////////////////////////////////
	void OpenGLRenderSystemES::endScene()
	{		
	}
	//////////////////////////////////////////////////////////////////////////
	void OpenGLRenderSystemES::swapBuffers()
	{
	}
	//////////////////////////////////////////////////////////////////////////
	void OpenGLRenderSystemES::clearFrameBuffer( uint32_t _frameBufferTypes, uint32_t _color, float _depth, uint32_t _stencil )
	{
		GLbitfield frameBufferFlags = 0;
		
        if( ( _frameBufferTypes & FBT_COLOR ) != 0 )
		{
			frameBufferFlags |= GL_COLOR_BUFFER_BIT;

			GLCALL( m_serviceProvider, glClearColor, ( 
				GET_R_FLOAT_FROM_ARGB32( _color ), 
				GET_G_FLOAT_FROM_ARGB32( _color ), 
				GET_B_FLOAT_FROM_ARGB32( _color ), 
				GET_A_FLOAT_FROM_ARGB32( _color ) )
				);
		}

		if( ( _frameBufferTypes & FBT_DEPTH ) != 0 )
		{
			frameBufferFlags |= GL_DEPTH_BUFFER_BIT;

			if( m_depthMask == false )
			{
				GLCALL( m_serviceProvider, glDepthMask, ( GL_TRUE ) );
			}

			GLCALL( m_serviceProvider, glClearDepthf, ( _depth ) );
		}

		if( (_frameBufferTypes & FBT_STENCIL) != 0 )
		{
			frameBufferFlags |= GL_STENCIL_BUFFER_BIT;

			GLCALL( m_serviceProvider, glClearStencil, ( _stencil ) );
		}

		GLCALL( m_serviceProvider, glClear, ( frameBufferFlags ) );

		if( m_depthMask == false )
		{
			GLCALL( m_serviceProvider, glDepthMask, ( GL_FALSE ) );
		}
	}
	//////////////////////////////////////////////////////////////////////////
	void OpenGLRenderSystemES::changeWindowMode( const Resolution & _resolution, bool _fullscreen )
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
	bool OpenGLRenderSystemES::supportTextureFormat( PixelFormat _format ) const
	{
		return s_toGLInternalFormat( _format ) != 0;
	}
	//////////////////////////////////////////////////////////////////////////
	bool OpenGLRenderSystemES::supportTextureNonPow2() const
	{
		return false;
	}
	//////////////////////////////////////////////////////////////////////////
	uint32_t OpenGLRenderSystemES::getMaxCombinedTextureImageUnits() const
	{
		return m_glMaxCombinedTextureImageUnits;
	}
	//////////////////////////////////////////////////////////////////////////
	void OpenGLRenderSystemES::onWindowMovedOrResized()
	{

	}
	//////////////////////////////////////////////////////////////////////////
	void OpenGLRenderSystemES::onWindowClose()
	{

	}
	//////////////////////////////////////////////////////////////////////////
	void OpenGLRenderSystemES::onWindowChangeFullscreenPrepare( bool _fullscreen )
	{
		(void)_fullscreen;

		for( TVectorImages::iterator
			it = m_images.begin(),
			it_end = m_images.end();
			it != it_end;
			++it )
		{
			OpenGLRenderImageES * image = *it;

			image->release();
		}
	}
	//////////////////////////////////////////////////////////////////////////
	void OpenGLRenderSystemES::onWindowChangeFullscreen( bool _fullscreen )
	{
		(void)_fullscreen;

		for( TVectorImages::iterator
			it = m_images.begin(),
			it_end = m_images.end();
			it != it_end;
			++it )
		{
			OpenGLRenderImageES * image = *it;

			image->reload();
		}
	}
	//////////////////////////////////////////////////////////////////////////
	void OpenGLRenderSystemES::setVSync( bool _vSync )
	{
		//m_windowContext->setVSync( _vSync );
	}
    //////////////////////////////////////////////////////////////////////////
	void OpenGLRenderSystemES::clear( uint32_t _color, bool _force )
    {
		(void)_color;
		(void)_force;
    }
	//////////////////////////////////////////////////////////////////////////
	RenderImageInterfacePtr OpenGLRenderSystemES::createDynamicImage( uint32_t _width, uint32_t _height, uint32_t _channels, uint32_t _depth, PixelFormat _format )
	{
		uint32_t hwChannels = 0;
		PixelFormat hwFormat = PF_UNKNOWN;
		this->findFormatFromChannels_( _format, _channels, hwFormat, hwChannels );

		GLint textureInternalFormat = s_toGLInternalFormat( hwFormat );

		if( textureInternalFormat == 0 )
		{
			LOGGER_ERROR(m_serviceProvider)("OpenGLRenderSystemES::createDynamicImage invalid get GL Texture Internal format for PF %d"
				, hwFormat
				);

			return nullptr;
		}

		GLint textureColorFormat = s_toGLColorFormat( hwFormat );

		if( textureColorFormat == 0 )
		{
			LOGGER_ERROR(m_serviceProvider)("OpenGLRenderSystemES::createDynamicImage invalid get GL Texture Color format for PF %d"
				, hwFormat
				);

			return nullptr;
		}

		GLint textureColorDataType = s_getGLColorDataType( hwFormat );

		if( textureColorDataType == 0 )
		{
			LOGGER_ERROR( m_serviceProvider )("OpenGLRenderSystemES::createDynamicImage invalid get GL Color Data Type for PF %d"
				, hwFormat
				);

			return nullptr;
		}

		OpenGLRenderImageESPtr image = m_factoryRenderImage->createObject();

		if( image->initialize( ERIM_DYNAMIC
			, 1
			, _width
			, _height
			, hwChannels
			, hwFormat
			, textureInternalFormat
			, textureColorFormat
			, textureColorDataType ) == false )
		{
			LOGGER_ERROR( m_serviceProvider )("OpenGLRenderSystemES::createDynamicImage invalid initialize"
				);

			return nullptr;
		}

		return image;
	}
	//////////////////////////////////////////////////////////////////////////
	RenderTargetInterface * OpenGLRenderSystemES::createRenderTargetOffscreen( uint32_t _width, uint32_t _height, PixelFormat _format )
	{
		return nullptr;
	}
	//////////////////////////////////////////////////////////////////////////
    void OpenGLRenderSystemES::makeProjectionOrthogonal( mt::mat4f & _projectionMatrix, const Viewport & _viewport, float _near, float _far )
	{
        mt::mat4f scale;
        mt::make_scale_m4( scale, 1.f, 1.f, 1.f );

        mt::mat4f translation;
        mt::make_translation_m4( translation, 0.f, 0.f, 0.f );

        mt::mat4f transform;
        mt::mul_m4_m4( transform, scale, translation );

        mt::mat4f ortho;
        mt::make_projection_ortho_lh_m4(ortho, _viewport.begin.x, _viewport.end.x, _viewport.begin.y, _viewport.end.y, _near, _far );

        mt::mul_m4_m4( _projectionMatrix, transform, ortho );
	}
	//////////////////////////////////////////////////////////////////////////
	void OpenGLRenderSystemES::makeProjectionPerspective( mt::mat4f & _projectionMatrix, float _fov, float _aspect, float _zn, float _zf )
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
	void OpenGLRenderSystemES::makeProjectionFrustum( mt::mat4f & _projectionMatrix, const Viewport & _viewport, float _near, float _far )
	{
		mt::mat4f scale;
		mt::make_scale_m4( scale, 1.0f, 1.0f, 1.0f );

		mt::mat4f translation;
		mt::make_translation_m4( translation, -0.5f, -0.5f, 0.0f );

		mt::mat4f transform;
		mt::mul_m4_m4( transform, scale, translation );

		mt::mat4f frustum;
		mt::make_projection_frustum_m4(frustum, _viewport.begin.x, _viewport.end.x, _viewport.begin.y, _viewport.end.y, _near, _far );

		mt::mul_m4_m4( _projectionMatrix, transform, frustum );
	}
    //////////////////////////////////////////////////////////////////////////
    void OpenGLRenderSystemES::makeViewMatrixFromViewport( mt::mat4f & _viewMatrix, const Viewport & _viewport )
    {
        mt::mat4f wm;
        mt::ident_m4( wm );

        mt::inv_m4_m4( _viewMatrix, wm );
    }
    //////////////////////////////////////////////////////////////////////////
    void OpenGLRenderSystemES::makeViewMatrixLookAt( mt::mat4f & _viewMatrix, const mt::vec3f & _eye, const mt::vec3f & _dir, const mt::vec3f & _up, float _sign )
    {
        mt::make_lookat_m4( _viewMatrix, _eye, _dir, _up, _sign );
    }
	//////////////////////////////////////////////////////////////////////////
	size_t OpenGLRenderSystemES::getTextureMemoryUse() const
	{
		return 0U;
	}
	//////////////////////////////////////////////////////////////////////////
	uint32_t OpenGLRenderSystemES::getTextureCount() const
	{
		return 0U;
	}
	//////////////////////////////////////////////////////////////////////////
	void OpenGLRenderSystemES::onRenderImageDestroy_( OpenGLRenderImageES * _image )
	{
		TVectorImages::iterator it_found = std::find( m_images.begin(), m_images.end(), _image );

		if( it_found == m_images.end() )
		{
			return;
		}

		*it_found = m_images.back();
		m_images.pop_back();
	}
}	// namespace Menge
