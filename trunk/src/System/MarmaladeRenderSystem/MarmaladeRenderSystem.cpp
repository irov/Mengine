
#	include "MarmaladeRenderSystem.h"

#	include "Interface/StringizeInterface.h"

#	include "MarmaladeRenderError.h"

#   include "Logger/Logger.h"

#	include <cmath>

#	include <stdex/allocator.h>

//////////////////////////////////////////////////////////////////////////
#	define GET_A_FLOAT_FROM_ARGB32( argb ) ( ((float)(argb >> 24)) / 255.0f )
#	define GET_R_FLOAT_FROM_ARGB32( argb ) ( ((float)((argb >> 16) & 0xFF)) / 255.0f )
#	define GET_G_FLOAT_FROM_ARGB32( argb ) ( ((float)((argb >> 8) & 0xFF)) / 255.0f )
#	define GET_B_FLOAT_FROM_ARGB32( argb ) ( (float)(argb & 0xFF) / 255.0f )
//////////////////////////////////////////////////////////////////////////
#define VERTEX_ARRAY			0
#define COLOR_ARRAY				1
#define UV0_ARRAY				2
#define UV1_ARRAY				3
//////////////////////////////////////////////////////////////////////////
SERVICE_FACTORY(RenderSystem, Menge::RenderSystemInterface, Menge::MarmaladeRenderSystem);
//////////////////////////////////////////////////////////////////////////
namespace Menge
{
    //////////////////////////////////////////////////////////////////////////
    static const GLenum s_toGLBlendFactor[] = 
    {
		GL_ONE						// BF_ONE
	    , GL_ZERO					// BF_ZERO
	    , GL_DST_COLOR				// BF_DEST_COLOUR
	    , GL_SRC_COLOR				// BF_SOURCE_COLOUR
	    , GL_ONE_MINUS_DST_COLOR	// BF_ONE_MINUS_DEST_COLOUR
	    , GL_ONE_MINUS_SRC_COLOR	// BF_ONE_MINUS_SOURCE_COLOUR
	    , GL_DST_ALPHA				// BF_DEST_ALPHA
	    , GL_SRC_ALPHA				// BF_SOURCE_ALPHA
	    , GL_ONE_MINUS_DST_ALPHA	// BF_ONE_MINUS_DEST_ALPHA
	    , GL_ONE_MINUS_SRC_ALPHA	// BF_ONE_MINUS_SOURCE_ALPHA
    };
    //////////////////////////////////////////////////////////////////////////
    static const GLenum s_toGLCmpFunc[] =
    {
		GL_NEVER		// CMPF_ALWAYS_FAIL
	    , GL_ALWAYS		// CMPF_ALWAYS_PASS
	    , GL_LESS		// CMPF_LESS
	    , GL_LEQUAL		// CMPF_LESS_EQUAL
	    , GL_EQUAL		// CMPF_EQUAL
	    , GL_NOTEQUAL	// CMPF_NOT_EQUAL
	    , GL_GEQUAL		// CMPF_GREATER_EQUAL
	    , GL_GREATER	// CMPF_GREATER
    };
    //////////////////////////////////////////////////////////////////////////
    /*static const GLenum s_toGLFillMode[] =
    {
		    GL_POINT	// FM_POINT
	    , GL_LINE	// FM_WIREFRAME
	    , GL_FILL	// FM_SOLID
    };*/
    //////////////////////////////////////////////////////////////////////////
    static const GLenum s_toGLShadeMode[] = 
    {
		GL_FLAT		// SHT_FLAT
	    , GL_SMOOTH	// SHT_GOURAUD
	    , GL_SMOOTH	// SHT_PHONG
    };
    //////////////////////////////////////////////////////////////////////////
    static const GLenum s_toMagFilter( Menge::ETextureFilter _magFilter )
    {
		switch( _magFilter )
		{
		case Menge::TF_NONE:
		case Menge::TF_POINT:
			return GL_NEAREST;
			break;
		case Menge::TF_LINEAR:
		case Menge::TF_ANISOTROPIC:
		case Menge::TF_FLATCUBIC:
		case Menge::TF_GAUSSIANCUBIC:
			return GL_LINEAR;
			break;
		default:
			{
			}break;
		};

		return GL_NEAREST;
    };
    //////////////////////////////////////////////////////////////////////////
    static GLenum s_toGLMinFilter( Menge::ETextureFilter _minFilter, Menge::ETextureFilter _mipFilter )
    {
	    switch( _minFilter )
	    {
	    case Menge::TF_NONE:
	    case Menge::TF_POINT:
		    switch( _mipFilter )
		    {
		    case Menge::TF_NONE:
			    return GL_NEAREST;
		    case Menge::TF_POINT:
			    return GL_NEAREST_MIPMAP_NEAREST;
		    case Menge::TF_ANISOTROPIC:
		    case Menge::TF_LINEAR:
		    case Menge::TF_FLATCUBIC:
		    case Menge::TF_GAUSSIANCUBIC:
			    return GL_NEAREST_MIPMAP_LINEAR;
		    default:;
		    }
		    break;
	    case Menge::TF_ANISOTROPIC:
	    case Menge::TF_LINEAR:
	    case Menge::TF_FLATCUBIC:
	    case Menge::TF_GAUSSIANCUBIC:
		    switch( _mipFilter )
		    {
		    case Menge::TF_NONE:
			    return GL_LINEAR;
		    case Menge::TF_POINT:
			    return GL_LINEAR_MIPMAP_NEAREST;
		    case Menge::TF_ANISOTROPIC:
		    case Menge::TF_LINEAR:
		    case Menge::TF_FLATCUBIC:
		    case Menge::TF_GAUSSIANCUBIC:
			    return GL_LINEAR_MIPMAP_LINEAR;
		    default:;
		    }
		    break;
	    default:;
	    }

	    return GL_NEAREST_MIPMAP_NEAREST;
    }
    //////////////////////////////////////////////////////////////////////////
    static int s_toGLInternalFormat( Menge::PixelFormat _format )
    {
	    switch( _format )
	    {
	    case Menge::PF_X8B8G8R8:
	    case Menge::PF_X8R8G8B8:
	    case Menge::PF_A8B8G8R8:
	    case Menge::PF_A8R8G8B8:
	    case Menge::PF_B8G8R8A8:
	    case Menge::PF_R8G8B8A8:
		    //return GL_RGBA8;
		    return GL_RGBA;
		case PF_R8G8B8:
			return GL_RGB;
		case Menge::PF_A8:
		    return GL_ALPHA;
		    //return GL_LUMINANCE;
		case Menge::PF_ETC1:
			return GL_ETC1_RGB8_OES;
		case Menge::PF_PVRTC4_RGB:
			return GL_COMPRESSED_RGB_PVRTC_4BPPV1_IMG;
		case Menge::PF_PVRTC4_RGBA:
			return GL_COMPRESSED_RGBA_PVRTC_4BPPV1_IMG;
		case Menge::PF_DXT1:
			return GL_COMPRESSED_RGB_S3TC_DXT1_EXT;			
	    default:;
	    }

	    return 0;
    }
    //////////////////////////////////////////////////////////////////////////
    static GLenum s_toGLColorFormat( Menge::PixelFormat _format )
    {
	    switch( _format )
	    {
		case Menge::PF_R8G8B8:
			return GL_RGB;
	    case Menge::PF_X8R8G8B8:
	    case Menge::PF_A8R8G8B8:
		    return GL_RGBA;
	    case Menge::PF_A8:
		    return GL_ALPHA;

		case Menge::PF_ETC1:
			return GL_ETC1_RGB8_OES;
		case Menge::PF_PVRTC4_RGB:
			return GL_COMPRESSED_RGB_PVRTC_4BPPV1_IMG;
		case Menge::PF_PVRTC4_RGBA:
			return GL_COMPRESSED_RGBA_PVRTC_4BPPV1_IMG;
		case Menge::PF_DXT1:
			return GL_COMPRESSED_RGB_S3TC_DXT1_EXT;
	    default:;
	    }

	    return 0;
    }
    //////////////////////////////////////////////////////////////////////////
    static GLenum s_getGLColorDataType( Menge::PixelFormat _format )
    {
	    switch( _format )
	    {
		case Menge::PF_R8G8B8:
			return GL_UNSIGNED_BYTE;
	    case Menge::PF_X8R8G8B8:
	    case Menge::PF_A8R8G8B8:
		    return GL_UNSIGNED_BYTE;
	    case Menge::PF_A8:
		    return GL_UNSIGNED_BYTE;
		case PF_DXT1:
		case PF_ETC1:
		case PF_PVRTC4_RGB:
		case PF_PVRTC4_RGBA:
			return GL_UNSIGNED_BYTE;
	    default:;
	    }

	    return 0;
    }
	//////////////////////////////////////////////////////////////////////////
	static GLenum s_getGLPrimitiveMode( EPrimitiveType _mode )
	{
		switch( _mode )
		{
		case PT_POINTLIST:
			return GL_POINTS;
		case PT_LINELIST:
			return GL_LINES;
		case PT_LINESTRIP:
			return GL_LINE_STRIP;
		case PT_TRIANGLELIST:
			return GL_TRIANGLES;
		case PT_TRIANGLESTRIP:
			return GL_TRIANGLE_STRIP;
		case PT_TRIANGLEFAN:
			return GL_TRIANGLE_FAN;
		default:;
		}

		return GL_POINTS;
	}
	//////////////////////////////////////////////////////////////////////////
	static GLenum s_getGLAddressMode( ETextureAddressMode _mode )
	{
		switch( _mode )
		{
		case TAM_CLAMP:
			return GL_CLAMP_TO_EDGE;
		case TAM_WRAP:
			return GL_REPEAT;
		default:;
		}

		return GL_CLAMP_TO_EDGE;
	}
	//////////////////////////////////////////////////////////////////////////
	static GLenum s_getGLTextureArg( ETextureArgument _arg )
	{
		switch( _arg )
		{
		case TARG_CURRENT:
			return GL_PREVIOUS;
		case TARG_DIFFUSE:
			return GL_PRIMARY_COLOR;
		case TARG_SPECULAR:
			return GL_PRIMARY_COLOR;
		case TARG_TEXTURE:
			return GL_TEXTURE;
		case TARG_TFACTOR:
			return GL_CONSTANT;
		default:;
		}

		return GL_PREVIOUS;
	}
	//////////////////////////////////////////////////////////////////////////
	static GLenum s_getGLTextureOp( ETextureOp _op )
	{
		switch( _op )
		{
		case TOP_DISABLE:
			return GL_REPLACE;
		case TOP_SELECTARG1:
			return GL_REPLACE;
		case TOP_SELECTARG2:
			return GL_REPLACE;
		case TOP_MODULATE:
			return GL_MODULATE;
		case TOP_ADD:
			return GL_ADD;
		case TOP_SUBSTRACT:
			return GL_SUBTRACT;
		default:;
		}
		
		return GL_REPLACE;
	}
	//////////////////////////////////////////////////////////////////////////
	MarmaladeRenderSystem::MarmaladeRenderSystem()
		: m_serviceProvider(nullptr)
		, m_supportNPOT(false)
		, m_currentVertexBuffer(0)
		, m_currentIndexBuffer(0)
		, m_srcBlendFactor(GL_ONE)
		, m_dstBlendFactor(GL_ZERO)
		, m_activeTextureStage(0)
		, m_activeTexture(0)
		, m_VBHandleGenerator(0)
		, m_IBHandleGenerator(0)
		, m_depthMask(false)
	{
		mt::ident_m4( m_worldMatrix );
		mt::ident_m4( m_viewMatrix );
		mt::ident_m4( m_projectionMatrix );
	}
	//////////////////////////////////////////////////////////////////////////
	MarmaladeRenderSystem::~MarmaladeRenderSystem()
	{
	}
    //////////////////////////////////////////////////////////////////////////
    void MarmaladeRenderSystem::setServiceProvider( ServiceProviderInterface * _serviceProvider )
    {
        m_serviceProvider = _serviceProvider;
    }
    //////////////////////////////////////////////////////////////////////////
    ServiceProviderInterface * MarmaladeRenderSystem::getServiceProvider() const
    {
        return m_serviceProvider;
    }
	//////////////////////////////////////////////////////////////////////////
	bool MarmaladeRenderSystem::initialize()
	{
		LOGGER_WARNING(m_serviceProvider)("Initializing OpenGL RenderSystem...");

		if ( IwGLIsInitialised() == false )
		{
			if( IwGLInit() == false )
			{
				return false;
			}

			s3eDeviceYield( 0 );
		}			

		MARMALADE_RENDER_CHECK_ERROR( m_serviceProvider );

		LOGGER_WARNING(m_serviceProvider)("Vendor      : %s", (const char*)glGetString( GL_VENDOR ) );
		LOGGER_WARNING(m_serviceProvider)("Renderer    : %s", (const char*)glGetString( GL_RENDERER ) );
		LOGGER_WARNING(m_serviceProvider)("Version     : %s", (const char*)glGetString( GL_VERSION ) );
		LOGGER_WARNING(m_serviceProvider)("Extensions  : %s", (const char*)glGetString( GL_EXTENSIONS ) );

		MARMALADE_RENDER_CHECK_ERROR( m_serviceProvider );

		if( sizeof(RenderVertex2D) != 32 )
		{
			return false;
		}

		for( uint32_t i = 0; i < MENGE_MAX_TEXTURE_STAGES; ++i )
		{
			GLCALL( m_serviceProvider, glActiveTexture, (GL_TEXTURE0 + i) );
// 			GLCALL( m_serviceProvider, glTexEnvi, (GL_TEXTURE_ENV, GL_TEXTURE_ENV_MODE, GL_COMBINE) );
// 			GLCALL( m_serviceProvider, glTexEnvi, (GL_TEXTURE_ENV, GL_OPERAND0_RGB, GL_SRC_COLOR) );
// 			GLCALL( m_serviceProvider, glTexEnvi, (GL_TEXTURE_ENV, GL_OPERAND1_RGB, GL_SRC_COLOR) );
// 			GLCALL( m_serviceProvider, glTexEnvi, (GL_TEXTURE_ENV, GL_OPERAND0_ALPHA, GL_SRC_ALPHA) );
// 			GLCALL( m_serviceProvider, glTexEnvi, (GL_TEXTURE_ENV, GL_OPERAND1_ALPHA, GL_SRC_ALPHA) );
			//GLCALL( m_serviceProvider, glDisable, (GL_TEXTURE_2D) );

			m_textureStage[i] = TextureStage();
		}

		GLCALL( m_serviceProvider, glFrontFace, ( GL_CW ) );
		GLCALL( m_serviceProvider, glDisable, ( GL_DEPTH_TEST ) );
		GLCALL( m_serviceProvider, glDisable, ( GL_STENCIL_TEST ) );
		GLCALL( m_serviceProvider, glDisable, ( GL_CULL_FACE ) );
		GLCALL( m_serviceProvider, glDisable, ( GL_LIGHTING ) );
		GLCALL( m_serviceProvider, glDisable, ( GL_BLEND ) );
		GLCALL( m_serviceProvider, glDisable, ( GL_ALPHA_TEST ) );
		GLCALL( m_serviceProvider, glDisable, ( GL_DITHER ) );

		GLCALL( m_serviceProvider, glDepthMask, ( GL_FALSE ) );

		m_renderPlatform = STRINGIZE_STRING_LOCAL( m_serviceProvider, "Marmalade" );
						
		return true;
	}
	//////////////////////////////////////////////////////////////////////////
    void MarmaladeRenderSystem::finalize()
    {
		GLCALL( m_serviceProvider, IwGLTerminate, () );		
    }
	//////////////////////////////////////////////////////////////////////////
	const ConstString & MarmaladeRenderSystem::getRenderPlatformName() const
	{
		return m_renderPlatform;
	}
    //////////////////////////////////////////////////////////////////////////
    void MarmaladeRenderSystem::setRenderListener( RenderSystemListener * _listener )
    {
        m_listener = _listener;
    }
	//////////////////////////////////////////////////////////////////////////
	bool MarmaladeRenderSystem::createRenderWindow( const Resolution & _resolution, uint32_t _bits,
												bool _fullscreen, WindowHandle _winHandle, 
												bool _waitForVSync, int _FSAAType, int _FSAAQuality )
	{
        m_resolution = _resolution;


        		
        return true;
	}
	//////////////////////////////////////////////////////////////////////////
	bool MarmaladeRenderSystem::screenshot( const RenderImageInterfacePtr & _image, const mt::vec4f & _rect )
	{
		(void)_image;
		(void)_rect;
        //ToDo!
		
		return false;
	}
    //////////////////////////////////////////////////////////////////////////
    void MarmaladeRenderSystem::setViewport( const Viewport & _viewport )
    {
        float w = _viewport.getWidth();
        float h = _viewport.getHeight();
        float xb = _viewport.begin.x;
        float yb = _viewport.end.y;

        size_t height = m_resolution.getHeight();

        GLCALL( m_serviceProvider, glViewport, ( (GLsizei)xb, height - (GLsizei)(yb), (GLsizei)w, (GLsizei)h ) );
    }
	//////////////////////////////////////////////////////////////////////////
	void MarmaladeRenderSystem::setProjectionMatrix( const mt::mat4f & _projection )
	{
		m_projectionMatrix = _projection;
	}
	//////////////////////////////////////////////////////////////////////////
	void MarmaladeRenderSystem::setModelViewMatrix( const mt::mat4f & _view )
	{
		m_viewMatrix = _view;
	}
	//////////////////////////////////////////////////////////////////////////
	void MarmaladeRenderSystem::setTextureMatrix( uint32_t _stage, const float* _texture )
	{
        // To Do
	}
    //////////////////////////////////////////////////////////////////////////
    void MarmaladeRenderSystem::setWorldMatrix( const mt::mat4f & _world )
    {
		m_worldMatrix = _world;
    }
	//////////////////////////////////////////////////////////////////////////
	VBHandle MarmaladeRenderSystem::createVertexBuffer( uint32_t _verticesNum, uint32_t _vertexSize, bool _dynamic )
	{
		size_t size = _verticesNum * _vertexSize;
		unsigned char * memory = (unsigned char *)stdex_malloc( size );

		MemoryRange memRange;
		memRange.pMem = memory;
		memRange.size = size;
		memRange.offset = 0;
		memRange.flags = BLF_LOCK_NONE;
        
        GLenum usage = GL_STATIC_DRAW;

        if( _dynamic == true )
        {
            usage = GL_DYNAMIC_DRAW;
        }

		GLuint bufId = 0;
		GLCALL( m_serviceProvider, glGenBuffers, ( 1, &bufId ) );

		GLCALL( m_serviceProvider, glBindBuffer, ( GL_ARRAY_BUFFER, bufId ) );
		GLCALL( m_serviceProvider, glBufferData, ( GL_ARRAY_BUFFER, memRange.size, NULL, usage ) );
		GLCALL( m_serviceProvider, glBindBuffer, ( GL_ARRAY_BUFFER, 0 ) );

		memRange.bufId = bufId;
		
        VBHandle vbHandle = ++m_VBHandleGenerator;
		m_vBuffersMemory.insert( vbHandle, memRange );

        return vbHandle;
	}
	//////////////////////////////////////////////////////////////////////////
	void MarmaladeRenderSystem::releaseVertexBuffer( VBHandle _vbHandle )
	{
		MemoryRange * range;
		if( m_vBuffersMemory.has( _vbHandle, &range) == false )
		{
			return;
		}
		
		stdex_free( range->pMem );

		if( _vbHandle == m_currentVertexBuffer )
		{
			GLCALL( m_serviceProvider, glBindBuffer, ( GL_ARRAY_BUFFER, 0 ) );
			
			m_currentVertexBuffer = 0;
		}

		GLCALL( m_serviceProvider, glDeleteBuffers, ( 1, &range->bufId ) );

		m_vBuffersMemory.erase( _vbHandle );
	}
	//////////////////////////////////////////////////////////////////////////
	void * MarmaladeRenderSystem::lockVertexBuffer( VBHandle _vbHandle, uint32_t _offset, uint32_t _size, EBufferLockFlag _flags )
	{
		MemoryRange * range;
		if( m_vBuffersMemory.has( _vbHandle, &range) == false )
		{
			return nullptr;
		}

        MemoryRange memRange;
		memRange.pMem = range->pMem + _offset;
		memRange.size = _size;
		memRange.offset = _offset;
		memRange.flags = _flags;
		memRange.bufId = range->bufId;
		
        m_vBuffersLocks.insert( _vbHandle, memRange );
		
        void * mem = static_cast<void *>(memRange.pMem);

        return mem;
	}
	//////////////////////////////////////////////////////////////////////////
	bool MarmaladeRenderSystem::unlockVertexBuffer( VBHandle _vbHandle )
	{
		MemoryRange * range;
		if( m_vBuffersLocks.has( _vbHandle, &range) == false )
		{
			return false;
		}
	
		GLCALL( m_serviceProvider, glBindBuffer, ( GL_ARRAY_BUFFER, range->bufId ) );
		GLCALL( m_serviceProvider, glBufferSubData, ( GL_ARRAY_BUFFER, range->offset, range->size, range->pMem ) );
		GLCALL( m_serviceProvider, glBindBuffer, ( GL_ARRAY_BUFFER, 0 ) );

		m_vBuffersLocks.erase( _vbHandle );		

		return true;
	}
	//////////////////////////////////////////////////////////////////////////
	void MarmaladeRenderSystem::setVertexBuffer( VBHandle _vbHandle )
	{
		m_currentVertexBuffer = _vbHandle;
	}
	//////////////////////////////////////////////////////////////////////////
	IBHandle MarmaladeRenderSystem::createIndexBuffer( uint32_t _indiciesNum, bool _dynamic )
	{
		size_t size = _indiciesNum * sizeof( uint16 );
		unsigned char * memory = (unsigned char *)stdex_malloc( size );

        MemoryRange memRange;
		memRange.pMem = memory;
		memRange.size = size;
		memRange.offset = 0;
		memRange.flags = BLF_LOCK_NONE;

        GLenum usage = GL_STATIC_DRAW;

        if( _dynamic == true )
        {
            usage = GL_DYNAMIC_DRAW;
        }

		GLuint bufId = 0;

#	ifndef __MACH__
		GLCALL( m_serviceProvider, glGenBuffers, ( 1, &bufId ) );
				
		GLCALL( m_serviceProvider, glBindBuffer, ( GL_ELEMENT_ARRAY_BUFFER, bufId ) );

		GLCALL( m_serviceProvider, glBufferData, ( GL_ELEMENT_ARRAY_BUFFER, memRange.size, NULL, usage ) );
		GLCALL( m_serviceProvider, glBindBuffer, ( GL_ELEMENT_ARRAY_BUFFER, 0 ) );
#	endif

		memRange.bufId = bufId;
		
		IBHandle ibHandle = ++m_IBHandleGenerator;
		m_iBuffersMemory.insert( ibHandle, memRange );
		
		return ibHandle;
	}
	//////////////////////////////////////////////////////////////////////////
	void MarmaladeRenderSystem::releaseIndexBuffer( IBHandle _ibHandle )
	{
		MemoryRange * range;
		if( m_iBuffersMemory.has( _ibHandle, &range) == false )
		{
			return;
		}

		if( _ibHandle == m_currentIndexBuffer )
		{
#	ifndef __MACH__
			GLCALL( m_serviceProvider, glBindBuffer, ( GL_ELEMENT_ARRAY_BUFFER, 0 ) );
#	endif
			
			m_currentIndexBuffer = 0;
		}

		stdex_free( range->pMem );

#	ifndef __MACH__
		GLCALL( m_serviceProvider, glDeleteBuffers, ( 1, &range->bufId ) );
#	endif

		m_iBuffersMemory.erase( _ibHandle );		
	}
	//////////////////////////////////////////////////////////////////////////
	RenderIndices2D * MarmaladeRenderSystem::lockIndexBuffer( IBHandle _ibHandle, uint32_t _offset, uint32_t _size, EBufferLockFlag _flags )
	{
		MemoryRange * range;
		if( m_iBuffersMemory.has( _ibHandle, &range) == false )
		{
			return nullptr;
		}
		
        MemoryRange memRange;
		memRange.pMem = range->pMem + _offset;
		memRange.size = _size;
		memRange.offset = _offset;
		memRange.flags = _flags;
		memRange.bufId = range->bufId;

		m_iBuffersLocks.insert( _ibHandle, memRange );
		
        RenderIndices2D * mem = reinterpret_cast<RenderIndices2D *>(memRange.pMem);

        return mem;
	}
	//////////////////////////////////////////////////////////////////////////
	bool MarmaladeRenderSystem::unlockIndexBuffer( IBHandle _ibHandle )
	{
		MemoryRange * range;
		if( m_iBuffersLocks.has( _ibHandle, &range) == false )
		{
			return nullptr;
		}
		
#	ifndef __MACH__
		GLCALL( m_serviceProvider, glBindBuffer, ( GL_ELEMENT_ARRAY_BUFFER, range->bufId ) );
		GLCALL( m_serviceProvider, glBufferSubData, ( GL_ELEMENT_ARRAY_BUFFER, range->offset, range->size, range->pMem ) );
		GLCALL( m_serviceProvider, glBindBuffer, ( GL_ELEMENT_ARRAY_BUFFER, 0 ) );
#	endif		
		
		m_iBuffersLocks.erase( _ibHandle );		

		return true;
	}
	//////////////////////////////////////////////////////////////////////////
	void MarmaladeRenderSystem::setIndexBuffer( IBHandle _ibHandle, uint32_t _baseVertexIndex )
	{
		m_currentIndexBuffer = _ibHandle;        
	}
	//////////////////////////////////////////////////////////////////////////
	void MarmaladeRenderSystem::setVertexDeclaration( uint32_t _vertexSize, uint32_t _declaration )
	{
        // To Do
	}
	//////////////////////////////////////////////////////////////////////////
	RenderShaderInterfacePtr MarmaladeRenderSystem::createFragmentShader( const void * _buffer, size_t _size, bool _isCompile )
	{
		MarmaladeShaderPtr shader = m_factoryShader.createObjectT();

		shader->setServiceProvider( m_serviceProvider );

		if( shader->initialize( GL_FRAGMENT_SHADER, _buffer, _size, _isCompile ) == false )
		{
			LOGGER_ERROR( m_serviceProvider )("MarmaladeRenderSystem::createFragmentShader invalid initialize shader"
				);

			return nullptr;
		}

		return shader;
	}
	//////////////////////////////////////////////////////////////////////////
	RenderShaderInterfacePtr MarmaladeRenderSystem::createVertexShader( const void * _buffer, size_t _size, bool _isCompile )
	{
		MarmaladeShaderPtr shader = m_factoryShader.createObjectT();

		shader->setServiceProvider( m_serviceProvider );

		if( shader->initialize( GL_VERTEX_SHADER, _buffer, _size, _isCompile ) == false )
		{
			LOGGER_ERROR( m_serviceProvider )("MarmaladeRenderSystem::createVertexShader invalid initialize shader"
				);

			return nullptr;
		}

		return shader;
	}
	//////////////////////////////////////////////////////////////////////////
	RenderProgramInterfacePtr MarmaladeRenderSystem::createProgram( const RenderShaderInterfacePtr & _fragment, const RenderShaderInterfacePtr & _vertex )
	{
		MarmaladeProgramPtr program = m_factoryProgram.createObjectT();

		program->setServiceProvider( m_serviceProvider );
		
		if( program->initialize( _fragment, _vertex ) == false )
		{
			LOGGER_ERROR( m_serviceProvider )("MarmaladeRenderSystem::createProgram invalid initialize program"
				);

			return nullptr;
		}

		return program;
	}
	//////////////////////////////////////////////////////////////////////////
	void MarmaladeRenderSystem::setProgram( const RenderProgramInterfacePtr & _program )
	{
		m_currentProgram = stdex::intrusive_static_cast<MarmaladeProgramPtr>(_program);
	}
	//////////////////////////////////////////////////////////////////////////
	void MarmaladeRenderSystem::drawIndexedPrimitive( EPrimitiveType _type, 
		uint32_t _baseVertexIndex,  uint32_t _minIndex, 
		uint32_t _verticesNum, uint32_t _startIndex, uint32_t _indexCount )
	{
		if( m_currentProgram != nullptr )
		{			
			m_currentProgram->use();
			
			m_currentProgram->bindMatrix( m_worldMatrix, m_viewMatrix, m_projectionMatrix );
		}

		for (uint32_t i = 0; i != MENGE_MAX_TEXTURE_STAGES; ++i)
		{
			const TextureStage & textureStage = m_textureStage[i];

			GLCALL(m_serviceProvider, glActiveTexture, (GL_TEXTURE0 + i));

			if( textureStage.texture == 0 )
			{				
				break;
			}
						
			GLCALL(m_serviceProvider, glBindTexture, (GL_TEXTURE_2D, textureStage.texture));

			if( m_currentProgram != nullptr )
			{
				m_currentProgram->bindTexture( i, textureStage.texture );
			}

			GLCALL( m_serviceProvider, glTexParameteri, ( GL_TEXTURE_2D, GL_TEXTURE_WRAP_S, textureStage.wrapS ) );
			GLCALL( m_serviceProvider, glTexParameteri, ( GL_TEXTURE_2D, GL_TEXTURE_WRAP_T, textureStage.wrapT ) );
			GLCALL( m_serviceProvider, glTexParameteri, ( GL_TEXTURE_2D, GL_TEXTURE_MIN_FILTER, textureStage.minFilter ) );
			GLCALL( m_serviceProvider, glTexParameteri, ( GL_TEXTURE_2D, GL_TEXTURE_MAG_FILTER, textureStage.magFilter ) );
		}

		MemoryRange * vb_range;
		if( m_vBuffersMemory.has( m_currentVertexBuffer, &vb_range ) == false )
		{
			return;
		}

		MemoryRange * ib_range;
		if( m_iBuffersMemory.has( m_currentIndexBuffer, &ib_range ) == false )
		{
			return;
		}
				

#	ifndef __MACH__
		GLCALL( m_serviceProvider, glBindBuffer, ( GL_ARRAY_BUFFER, vb_range->bufId ) );
		GLCALL( m_serviceProvider, glBindBuffer, ( GL_ELEMENT_ARRAY_BUFFER, ib_range->bufId ) );

		//glVertexAttribPointer(0, 3, GL_FLOAT, false, sizeof(RenderVertex2D), reinterpret_cast<const GLvoid *>(0));
		GLCALL( m_serviceProvider, glEnableVertexAttribArray, ( VERTEX_ARRAY ) );
		GLCALL( m_serviceProvider, glEnableVertexAttribArray, ( COLOR_ARRAY ) );
		GLCALL( m_serviceProvider, glEnableVertexAttribArray, ( UV0_ARRAY ) );
		GLCALL( m_serviceProvider, glEnableVertexAttribArray, ( UV1_ARRAY ) );
		GLCALL( m_serviceProvider, glVertexAttribPointer, ( VERTEX_ARRAY, 3, GL_FLOAT, GL_FALSE, sizeof( RenderVertex2D ), reinterpret_cast<const GLvoid *>(0) ) );
		GLCALL( m_serviceProvider, glVertexAttribPointer, ( COLOR_ARRAY, 4, GL_UNSIGNED_BYTE, GL_TRUE, sizeof( RenderVertex2D ), reinterpret_cast<const GLvoid *>(12) ) );
		GLCALL( m_serviceProvider, glVertexAttribPointer, ( UV0_ARRAY, 2, GL_FLOAT, GL_FALSE, sizeof( RenderVertex2D ), reinterpret_cast<const GLvoid *>(16) ) );
		GLCALL( m_serviceProvider, glVertexAttribPointer, ( UV1_ARRAY, 2, GL_FLOAT, GL_FALSE, sizeof( RenderVertex2D ), reinterpret_cast<const GLvoid *>(24) ) );

		//GLCALL( m_serviceProvider, glEnableClientState, ( GL_VERTEX_ARRAY ) );
		//GLCALL( m_serviceProvider, glVertexPointer, ( 3, GL_FLOAT, 32, reinterpret_cast<const GLvoid *>( 0 ) ) );

		//GLCALL( m_serviceProvider, glEnableClientState, ( GL_COLOR_ARRAY ) );
		//GLCALL( m_serviceProvider, glColorPointer, ( 4, GL_UNSIGNED_BYTE, 32, reinterpret_cast<const GLvoid *>( 12 ) ) );

		//GLCALL( m_serviceProvider, glEnableClientState, ( GL_TEXTURE_COORD_ARRAY ) );

		//GLCALL( m_serviceProvider, glClientActiveTexture, ( GL_TEXTURE0 ) );
		//GLCALL( m_serviceProvider, glTexCoordPointer, ( 2, GL_FLOAT, 32, reinterpret_cast<const GLvoid *>( 16 ) ) );
		//GLCALL( m_serviceProvider, glClientActiveTexture, ( GL_TEXTURE1 ) );
		//GLCALL( m_serviceProvider, glTexCoordPointer, ( 2, GL_FLOAT, 32, reinterpret_cast<const GLvoid *>( 24 ) ) );

        GLenum mode = s_getGLPrimitiveMode( _type );
		const uint16_t * baseIndex = nullptr;
		const uint16_t * offsetIndex = baseIndex + _startIndex;		
		GLCALL( m_serviceProvider, glDrawElements, ( mode, _indexCount, GL_UNSIGNED_SHORT, reinterpret_cast<const GLvoid *>(offsetIndex) ) );
#	else	
		//////////////////////////////////////////////////////////////////////////
		//////////////////////////////////////////////////////////////////////////
		//Mac dont support 'glDrawElements indices offset' - create static buffers
		//////////////////////////////////////////////////////////////////////////
		//////////////////////////////////////////////////////////////////////////
		GLuint bufId = 0;
		GLCALL( m_serviceProvider, glGenBuffers, ( 1, &bufId ) );

		GLCALL( m_serviceProvider, glBindBuffer, ( GL_ELEMENT_ARRAY_BUFFER, bufId ) );

		GLCALL( m_serviceProvider, glBufferData, ( GL_ELEMENT_ARRAY_BUFFER, _indexCount * sizeof(uint16_t), nullptr, GL_STATIC_DRAW ) );
		GLCALL( m_serviceProvider, glBindBuffer, ( GL_ELEMENT_ARRAY_BUFFER, 0 ) );

		const uint16_t * baseIndex = reinterpret_cast<const uint16_t *>(ib_range->pMem);
		const uint16_t * offsetIndex = baseIndex + _startIndex;

		GLCALL( m_serviceProvider, glBindBuffer, ( GL_ELEMENT_ARRAY_BUFFER, bufId ) );
		GLCALL( m_serviceProvider, glBufferSubData, ( GL_ELEMENT_ARRAY_BUFFER, 0, _indexCount * sizeof(uint16_t), offsetIndex ) );
		GLCALL( m_serviceProvider, glBindBuffer, ( GL_ELEMENT_ARRAY_BUFFER, 0 ) );

		GLCALL( m_serviceProvider, glBindBuffer, ( GL_ARRAY_BUFFER, vb_range->bufId ) );
		GLCALL( m_serviceProvider, glBindBuffer, ( GL_ELEMENT_ARRAY_BUFFER, bufId ) );
				
		GLCALL( m_serviceProvider, glEnableClientState, ( GL_VERTEX_ARRAY ) );
		GLCALL( m_serviceProvider, glVertexPointer, ( 3, GL_FLOAT, 32, reinterpret_cast<const GLvoid *>( 0 ) ) );

		GLCALL( m_serviceProvider, glEnableClientState, ( GL_COLOR_ARRAY ) );
		GLCALL( m_serviceProvider, glColorPointer, ( 4, GL_UNSIGNED_BYTE, 32, reinterpret_cast<const GLvoid *>( 12 ) ) );

		GLCALL( m_serviceProvider, glEnableClientState, ( GL_TEXTURE_COORD_ARRAY ) );

		GLCALL( m_serviceProvider, glClientActiveTexture, ( GL_TEXTURE0 ) );
		GLCALL( m_serviceProvider, glTexCoordPointer, ( 2, GL_FLOAT, 32, reinterpret_cast<const GLvoid *>( 16 ) ) );
		GLCALL( m_serviceProvider, glClientActiveTexture, ( GL_TEXTURE1 ) );
		GLCALL( m_serviceProvider, glTexCoordPointer, ( 2, GL_FLOAT, 32, reinterpret_cast<const GLvoid *>( 24 ) ) );

		GLenum mode = s_getGLPrimitiveMode( _type );

		GLCALL( m_serviceProvider, glDrawElements, ( mode, _indexCount, GL_UNSIGNED_SHORT, nullptr ) );
		
		GLCALL( m_serviceProvider, glDeleteBuffers, ( 1, &bufId ) );
#	endif

// 		GLCALL( m_serviceProvider, glDisableClientState, ( GL_VERTEX_ARRAY ) );
// 		GLCALL( m_serviceProvider, glDisableClientState, ( GL_COLOR_ARRAY ) );
// 		GLCALL( m_serviceProvider, glDisableClientState, ( GL_TEXTURE_COORD_ARRAY ) );

		GLCALL( m_serviceProvider, glBindBuffer, ( GL_ARRAY_BUFFER, 0 ) );
		GLCALL( m_serviceProvider, glBindBuffer, ( GL_ELEMENT_ARRAY_BUFFER, 0 ) );
	}
	//////////////////////////////////////////////////////////////////////////
	void MarmaladeRenderSystem::setTexture( uint32_t _stage, const RenderImageInterfacePtr & _texture )
	{        
        TextureStage & tStage = m_textureStage[_stage];

        if( _texture != nullptr )
        {
            MarmaladeTexture * texture = stdex::intrusive_get<MarmaladeTexture>(_texture);
            
            tStage.texture = texture->getUId();
            tStage.wrapS = texture->getWrapS();
            tStage.wrapT = texture->getWrapT();
            tStage.minFilter = texture->getMinFilter();
            tStage.magFilter = texture->getMagFilter();
        }
        else
        {
            tStage.texture = 0;
        }
	}
	//////////////////////////////////////////////////////////////////////////
	void MarmaladeRenderSystem::setTextureAddressing( uint32_t _stage, ETextureAddressMode _modeU, ETextureAddressMode _modeV )
	{
        GLenum modeUGL = s_getGLAddressMode( _modeU );
        GLenum modeVGL = s_getGLAddressMode( _modeV );

        m_textureStage[_stage].wrapS = modeUGL;
        m_textureStage[_stage].wrapT = modeVGL;
	}
	//////////////////////////////////////////////////////////////////////////
	void MarmaladeRenderSystem::setTextureFactor( uint32_t _color )
	{

	}
	//////////////////////////////////////////////////////////////////////////
	void MarmaladeRenderSystem::setSrcBlendFactor( EBlendFactor _src )
	{
		m_srcBlendFactor = s_toGLBlendFactor[ _src ];

		GLCALL( m_serviceProvider, glBlendFunc, ( m_srcBlendFactor, m_dstBlendFactor ) );
	}
	//////////////////////////////////////////////////////////////////////////
	void MarmaladeRenderSystem::setDstBlendFactor( EBlendFactor _dst )
	{
		m_dstBlendFactor = s_toGLBlendFactor[ _dst ];

		GLCALL( m_serviceProvider, glBlendFunc, ( m_srcBlendFactor, m_dstBlendFactor ) );
	}
	//////////////////////////////////////////////////////////////////////////
	void MarmaladeRenderSystem::setCullMode( ECullMode _mode )
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
    void MarmaladeRenderSystem::setSeparateAlphaBlendMode()
    {
    
    }
	//////////////////////////////////////////////////////////////////////////
	void MarmaladeRenderSystem::setDepthBufferTestEnable( bool _depthTest )
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
	void MarmaladeRenderSystem::setDepthBufferWriteEnable( bool _depthWrite )
	{
		m_depthMask = _depthWrite;

		GLCALL( m_serviceProvider, glDepthMask, ( m_depthMask ? GL_TRUE : GL_FALSE ) );
	}
	//////////////////////////////////////////////////////////////////////////
	void MarmaladeRenderSystem::setDepthBufferCmpFunc( ECompareFunction _depthFunction )
	{
		GLenum cmpFunc = s_toGLCmpFunc[ _depthFunction ];

		GLCALL( m_serviceProvider, glDepthFunc, ( cmpFunc ) );
	}
	//////////////////////////////////////////////////////////////////////////
	void MarmaladeRenderSystem::setFillMode( EFillMode _mode )
	{
		/*GLenum mode = s_toGLFillMode[ _mode ];
		glPolygonMode( GL_FRONT_AND_BACK, mode );*/
	}
	//////////////////////////////////////////////////////////////////////////
	void MarmaladeRenderSystem::setColorBufferWriteEnable( bool _r, bool _g, bool _b, bool _a )
	{
		GLCALL( m_serviceProvider, glColorMask, ( _r ? GL_TRUE : GL_FALSE, _g ? GL_TRUE : GL_FALSE, _b ? GL_TRUE : GL_FALSE, _a ? GL_TRUE : GL_FALSE ) );
	}
	//////////////////////////////////////////////////////////////////////////
	void MarmaladeRenderSystem::setShadeType( EShadeType _sType )
	{
		GLenum model = s_toGLShadeMode[ _sType ];
		GLCALL( m_serviceProvider, glShadeModel, ( model ) );
	}
	//////////////////////////////////////////////////////////////////////////
	void MarmaladeRenderSystem::setAlphaTestEnable( bool _alphaTest )
	{
        if( _alphaTest == true )
        {
            GLCALL( m_serviceProvider, glEnable, ( GL_ALPHA_TEST ) );
        }
        else
        {
            GLCALL( m_serviceProvider, glDisable, ( GL_ALPHA_TEST ) );
        }
	}
	//////////////////////////////////////////////////////////////////////////
	void MarmaladeRenderSystem::setAlphaBlendEnable( bool _alphaBlend )
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
	void MarmaladeRenderSystem::setAlphaCmpFunc( ECompareFunction _alphaFunc, uint8_t _alpha )
	{
		//GLenum cmpFunc = s_toGLCmpFunc[_alphaFunc];
		//GLclampf ref = (GLclampf)(static_cast<float>( _alpha ) / 255.0f);

		//GLCALL( m_serviceProvider, glAlphaFunc, ( cmpFunc, ref ) );
	}
	//////////////////////////////////////////////////////////////////////////
	void MarmaladeRenderSystem::setLightingEnable( bool _light )
	{
		if( _light == true )
        {
            GLCALL( m_serviceProvider, glEnable, ( GL_LIGHTING ) );
        }
        else
        {
            GLCALL( m_serviceProvider, glDisable, ( GL_LIGHTING ) );
        }
	}
	//////////////////////////////////////////////////////////////////////////
	void MarmaladeRenderSystem::setTextureStageColorOp( uint32_t _stage, ETextureOp _textrueOp,  ETextureArgument _arg1, ETextureArgument _arg2 )
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
	void MarmaladeRenderSystem::setTextureStageAlphaOp( uint32_t _stage, ETextureOp _textrueOp,  ETextureArgument _arg1, ETextureArgument _arg2 )
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
    void MarmaladeRenderSystem::setTextureStageTexCoordIndex( uint32_t _stage, uint32_t _index )
    {
        //m_textureStage[_stage].texCoordIndex = _index;
    }
	//////////////////////////////////////////////////////////////////////////
	void MarmaladeRenderSystem::setTextureStageFilter( uint32_t _stage, ETextureFilterType _filterType, ETextureFilter _filter )
	{
        TextureStage & tStage = m_textureStage[_stage];

        if( _filterType == Menge::TFT_MINIFICATION )
        {
            tStage.mengeMinFilter = _filter;
            tStage.minFilter = s_toGLMinFilter( tStage.mengeMinFilter, tStage.mengeMipFilter );
        }
        else if( _filterType == Menge::TFT_MIPMAP )
        {
            tStage.mengeMipFilter = _filter;
            tStage.minFilter = s_toGLMinFilter( tStage.mengeMinFilter, tStage.mengeMipFilter );
        }
        else if( _filterType == Menge::TFT_MAGNIFICATION )
        {
            tStage.magFilter = s_toMagFilter( _filter );
        }
	}
    //////////////////////////////////////////////////////////////////////////
    void MarmaladeRenderSystem::findFormatFromChannels_( PixelFormat _format, uint32_t _channels, PixelFormat & _hwFormat, uint32_t & _hwChannels ) const
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
	RenderImageInterfacePtr MarmaladeRenderSystem::createImage( uint32_t _mipmaps, uint32_t _width, uint32_t _height, uint32_t _channels, uint32_t _depth, PixelFormat _format )
	{
        uint32_t hwChannels = 0;
        PixelFormat hwFormat = PF_UNKNOWN;
		this->findFormatFromChannels_( _format, _channels, hwFormat, hwChannels );

		GLint textureInternalFormat = s_toGLInternalFormat( hwFormat );

		if( textureInternalFormat == 0 )
		{
			LOGGER_ERROR(m_serviceProvider)("MarmaladeRenderSystem::createImage invalid get GL Texture Internal format for PF %d"
				, hwFormat
				);

			return nullptr;
		}

		GLint textureColorFormat = s_toGLColorFormat( hwFormat );

		if( textureColorFormat == 0 )
		{
			LOGGER_ERROR(m_serviceProvider)("MarmaladeRenderSystem::createImage invalid get GL Texture Color format for PF %d"
				, hwFormat
				);

			return nullptr;
		}

		GLint textureColorDataType = s_getGLColorDataType( hwFormat );

		if( textureColorDataType == 0 )
		{
			LOGGER_ERROR( m_serviceProvider )("MarmaladeRenderSystem::createImage invalid get GL Color Data Type for PF %d"
				, hwFormat
				);

			return nullptr;
		}

		GLuint tuid = 0;
		GLCALL( m_serviceProvider, glGenTextures, (1, &tuid) );

		if( tuid == 0 )
		{

		}

		MarmaladeTexturePtr texture = m_factoryTexture.createObjectT();

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
			LOGGER_ERROR( m_serviceProvider )("MarmaladeRenderSystem::createImage invalid initialize"
				);

			return nullptr;
		}

		GLenum wrapS = GL_CLAMP_TO_EDGE;
		GLenum wrapT = GL_CLAMP_TO_EDGE;
		GLenum minFilter = GL_LINEAR;
		GLenum magFilter = GL_LINEAR;
				
		texture->setMagFilter( magFilter );
		texture->setMinFilter( minFilter );
		texture->setWrapS( wrapS );
		texture->setWrapT( wrapT );

		return texture;
	}
	//////////////////////////////////////////////////////////////////////////
	bool MarmaladeRenderSystem::lockRenderTarget( const RenderImageInterfacePtr & _renderTarget )
	{
		//TODO
		return false;
	}
	//////////////////////////////////////////////////////////////////////////
	bool MarmaladeRenderSystem::unlockRenderTarget()
	{
		//TODO
		return false;
	}
    //////////////////////////////////////////////////////////////////////////
    bool MarmaladeRenderSystem::beginScene()
    {
		this->clearFrameBuffer( FBT_COLOR, 0, 1.f, 0 );

        return true;
    }
	//////////////////////////////////////////////////////////////////////////
	void MarmaladeRenderSystem::endScene()
	{		
	}
	//////////////////////////////////////////////////////////////////////////
	void MarmaladeRenderSystem::swapBuffers()
	{
        GLCALL( m_serviceProvider, IwGLSwapBuffers, () );
	}
	//////////////////////////////////////////////////////////////////////////
	void MarmaladeRenderSystem::clearFrameBuffer( uint32_t _frameBufferTypes, uint32_t _color, float _depth, uint32_t _stencil )
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
	void MarmaladeRenderSystem::changeWindowMode( const Resolution & _resolution, bool _fullscreen )
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
	bool MarmaladeRenderSystem::supportTextureFormat( PixelFormat _format ) const
	{
		return s_toGLInternalFormat( _format ) != 0;
	}
	//////////////////////////////////////////////////////////////////////////
	void MarmaladeRenderSystem::onWindowMovedOrResized()
	{

	}
	//////////////////////////////////////////////////////////////////////////
	void MarmaladeRenderSystem::onWindowClose()
	{

	}
	//////////////////////////////////////////////////////////////////////////
	void MarmaladeRenderSystem::setVSync( bool _vSync )
	{
		//m_windowContext->setVSync( _vSync );
	}
    //////////////////////////////////////////////////////////////////////////
    void MarmaladeRenderSystem::clear( uint32_t _color )
    {
    
    }
    //////////////////////////////////////////////////////////////////////////
    RenderImageInterfacePtr MarmaladeRenderSystem::createRenderTargetImage( uint32_t _width, uint32_t _height, uint32_t _channels, uint32_t _depth, PixelFormat _format )
    {
        return nullptr;
    }
	//////////////////////////////////////////////////////////////////////////
	RenderImageInterfacePtr MarmaladeRenderSystem::createDynamicImage( uint32_t _width, uint32_t _height, uint32_t _channels, uint32_t _depth, PixelFormat _format )
	{
		uint32_t hwChannels = 0;
		PixelFormat hwFormat = PF_UNKNOWN;
		this->findFormatFromChannels_( _format, _channels, hwFormat, hwChannels );

		GLint textureInternalFormat = s_toGLInternalFormat( hwFormat );

		if( textureInternalFormat == 0 )
		{
			LOGGER_ERROR(m_serviceProvider)("MarmaladeRenderSystem::createDynamicImage invalid get GL Texture Internal format for PF %d"
				, hwFormat
				);

			return nullptr;
		}

		GLint textureColorFormat = s_toGLColorFormat( hwFormat );

		if( textureColorFormat == 0 )
		{
			LOGGER_ERROR(m_serviceProvider)("MarmaladeRenderSystem::createDynamicImage invalid get GL Texture Color format for PF %d"
				, hwFormat
				);

			return nullptr;
		}

		GLint textureColorDataType = s_getGLColorDataType( hwFormat );

		if( textureColorDataType == 0 )
		{
			LOGGER_ERROR( m_serviceProvider )("MarmaladeRenderSystem::createDynamicImage invalid get GL Color Data Type for PF %d"
				, hwFormat
				);

			return nullptr;
		}

		MarmaladeTexturePtr texture = m_factoryTexture.createObjectT();

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
			LOGGER_ERROR( m_serviceProvider )("MarmaladeRenderSystem::createDynamicImage invalid initialize"
				);

			return nullptr;
		}

		GLenum wrapS = GL_CLAMP_TO_EDGE;
		GLenum wrapT = GL_CLAMP_TO_EDGE;
		GLenum minFilter = GL_LINEAR;
		GLenum magFilter = GL_LINEAR;

		texture->setMagFilter( magFilter );
		texture->setMinFilter( minFilter );
		texture->setWrapS( wrapS );
		texture->setWrapT( wrapT );

		return texture;
	}
	//////////////////////////////////////////////////////////////////////////
    void MarmaladeRenderSystem::makeProjectionOrthogonal( mt::mat4f & _projectionMatrix, const Viewport & _viewport, float _near, float _far )
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
	void MarmaladeRenderSystem::makeProjectionPerspective( mt::mat4f & _projectionMatrix, float _fov, float _aspect, float _zn, float _zf )
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
	void MarmaladeRenderSystem::makeProjectionFrustum( mt::mat4f & _projectionMatrix, const Viewport & _viewport, float _near, float _far )
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
    void MarmaladeRenderSystem::makeViewMatrixFromViewport( mt::mat4f & _viewMatrix, const Viewport & _viewport )
    {
        mt::mat4f wm;
        mt::ident_m4( wm );

        mt::inv_m4( _viewMatrix, wm );
    }
    //////////////////////////////////////////////////////////////////////////
    void MarmaladeRenderSystem::makeViewMatrixLookAt( mt::mat4f & _viewMatrix, const mt::vec3f & _eye, const mt::vec3f & _dir, const mt::vec3f & _up, float _sign )
    {
        mt::make_lookat_m4( _viewMatrix, _eye, _dir, _up, _sign );
    }
}	// namespace Menge
