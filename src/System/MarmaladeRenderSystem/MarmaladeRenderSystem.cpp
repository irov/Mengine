
#	include "MarmaladeRenderSystem.h"

#	include <cmath>

#	include "Interface/LogSystemInterface.h"
//#	include "OGLWindowContext.h"

#   include "Logger/Logger.h"

#	define GET_A_FLOAT_FROM_ARGB32( argb ) ( ((float)(argb >> 24)) / 255.0f )
#	define GET_R_FLOAT_FROM_ARGB32( argb ) ( ((float)((argb >> 16) & 0xFF)) / 255.0f )
#	define GET_G_FLOAT_FROM_ARGB32( argb ) ( ((float)((argb >> 8) & 0xFF)) / 255.0f )
#	define GET_B_FLOAT_FROM_ARGB32( argb ) ( (float)(argb & 0xFF) / 255.0f )
//////////////////////////////////////////////////////////////////////////
SERVICE_FACTORY(RenderSystem, Menge::RenderSystemInterface, Menge::MarmaladeRenderSystem);
//////////////////////////////////////////////////////////////////////////
namespace Menge
{
	static void gl_check_error()
	{
		GLenum err = glGetError(); // GL_INVALID_OPERATION

		if( err == GL_NO_ERROR )
		{
			return;
		}

		printf("!!!!!!!!!!!!!!!GL ERROR %d\n"
			, err
			);
	}
    //////////////////////////////////////////////////////////////////////////
    static const GLenum s_toGLBlendFactor[] = 
    {
		    GL_ONE					// BF_ONE
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
		    GL_FLAT	// SHT_FLAT
	    , GL_SMOOTH	// SHT_GOURAUD
	    , GL_SMOOTH	// SHT_PHONG
    };
    //////////////////////////////////////////////////////////////////////////
    static const GLenum s_toGLFilter[] = 
    {
		    GL_NEAREST	// TF_NONE
	    , GL_NEAREST	// TF_POINT
	    , GL_LINEAR		// TF_LINEAR
	    , GL_LINEAR		// TF_ANISOTROPIC
	    , GL_LINEAR		// TF_FLATCUBIC
	    , GL_LINEAR		// TF_GAUSSIANCUBIC
    };
    //////////////////////////////////////////////////////////////////////////
    static GLenum s_toGLMinMipFilter( Menge::ETextureFilter _minFilter, Menge::ETextureFilter _mipFilter )
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
	    return GL_NEAREST;
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
	    default:;
	    }
	    return 0;
    }
    //////////////////////////////////////////////////////////////////////////
    static GLenum s_toGLColorFormat( Menge::PixelFormat _format )
    {
	    switch( _format )
	    {
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
	    default:;
	    }
	    return 0;
    }
    //////////////////////////////////////////////////////////////////////////
    static GLenum s_getGLColorDataType( Menge::PixelFormat _format )
    {
	    switch( _format )
	    {
	    case Menge::PF_X8R8G8B8:
	    case Menge::PF_A8R8G8B8:
		    return GL_UNSIGNED_BYTE;
	    case Menge::PF_A8:
		    return GL_UNSIGNED_BYTE;
	    default:;
	    }
	    return 0;
    }
    //////////////////////////////////////////////////////////////////////////
    static const GLenum s_toGLPrimitiveMode[] = 
    {
		    GL_POINTS			// PT_POINTLIST
	    , GL_LINES			// PT_LINELIST
	    , GL_LINE_STRIP		// PT_LINESTRIP
	    , GL_TRIANGLES		// PT_TRIANGLELIST
	    , GL_TRIANGLE_STRIP	// PT_TRIANGLESTRIP
	    , GL_TRIANGLE_FAN	// PT_TRIANGLEFAN
    };
    //////////////////////////////////////////////////////////////////////////
    static const GLenum s_toGLAddressMode[] = 
    {
		    GL_CLAMP_TO_EDGE		// TAM_CLAMP
	    , GL_REPEAT				// TAM_WRAP
	    //, GL_MIRRORED_REPEAT	// TAM_MIRROR
    };
    //////////////////////////////////////////////////////////////////////////
    static const GLenum s_toGLTextureOp[] = 
    {
		    GL_REPLACE	// TOP_DISABLE
	    , GL_REPLACE	// TOP_SELECTARG1
	    , GL_REPLACE	// TOP_SELECTARG2
	    , GL_MODULATE	// TOP_MODULATE
	    , GL_ADD		// TOP_ADD
	    , GL_SUBTRACT	// TOP_SUBSTRACT
    };
    //////////////////////////////////////////////////////////////////////////
    static const GLenum s_toGLTextureArg[] = 
    {
		    GL_PREVIOUS		// TARG_CURRENT
	    , GL_PRIMARY_COLOR	// TARG_DIFFUSE
	    , GL_PRIMARY_COLOR	// TARG_SPECULAR
	    , GL_TEXTURE		// TARG_TEXTURE
	    , GL_CONSTANT		// TARG_TFACTOR
    };
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
		, m_activeRenderTarget(nullptr)
		, m_VBHandleGenerator(0)
		, m_IBHandleGenerator(0)
	{
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
		LOGGER_INFO(m_serviceProvider)( "Initializing OpenGL RenderSystem..." );

        if( IwGLInit() == false )
        {
            return false;
        }

		return true;
	}
	//////////////////////////////////////////////////////////////////////////
    void MarmaladeRenderSystem::finalize()
    {
		IwGLTerminate();
    }
    //////////////////////////////////////////////////////////////////////////
    void MarmaladeRenderSystem::setRenderListener( RenderSystemListener * _listener )
    {
        m_listener = _listener;
    }
	//////////////////////////////////////////////////////////////////////////
	bool MarmaladeRenderSystem::createRenderWindow( const Resolution & _resolution, int _bits,
												bool _fullscreen, WindowHandle _winHandle, 
												bool _waitForVSync, int _FSAAType, int _FSAAQuality )
	{
        m_resolution = _resolution;

		for( int i = 0; i < MENGE_MAX_TEXTURE_STAGES; ++i )
		{
			glActiveTexture( GL_TEXTURE0 + i );
			glTexEnvi( GL_TEXTURE_ENV, GL_TEXTURE_ENV_MODE, GL_COMBINE );
			glTexEnvi( GL_TEXTURE_ENV, GL_OPERAND0_RGB, GL_SRC_COLOR );
			glTexEnvi( GL_TEXTURE_ENV, GL_OPERAND1_RGB, GL_SRC_COLOR );
			glTexEnvi( GL_TEXTURE_ENV, GL_OPERAND0_ALPHA, GL_SRC_ALPHA );
			glTexEnvi( GL_TEXTURE_ENV, GL_OPERAND1_ALPHA, GL_SRC_ALPHA );			
			glDisable( GL_TEXTURE_2D );

            m_textureStage[i] = TextureStage();
		}
        
		glFrontFace( GL_CW );
		glDisable( GL_DEPTH_TEST );
		glDisable( GL_CULL_FACE );
		glDisable( GL_LIGHTING );
		glEnable( GL_BLEND );
		glActiveTexture( GL_TEXTURE0 );
		glEnable( GL_TEXTURE_2D ); 
		
        glDepthMask( GL_FALSE );
		glPixelStorei( GL_UNPACK_ALIGNMENT, 1 );

        m_textureStage[0].enabled = false;
        m_depthMask = false;
        		
        return true;
	}
	//////////////////////////////////////////////////////////////////////////
	bool MarmaladeRenderSystem::screenshot( const RenderImageInterfacePtr & _image, const float * _rect )
	{
        //NOT SUPPORT!
		
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

        glViewport( (GLsizei)xb, height - (GLsizei)(yb), (GLsizei)w, (GLsizei)h );
    }
	//////////////////////////////////////////////////////////////////////////
	void MarmaladeRenderSystem::setProjectionMatrix( const mt::mat4f & _projection )
	{
        glMatrixMode( GL_PROJECTION );

        const float * matrix = _projection.buff();
        glLoadMatrixf( matrix );
	}
	//////////////////////////////////////////////////////////////////////////
	void MarmaladeRenderSystem::setModelViewMatrix( const mt::mat4f & _view )
	{
        glMatrixMode( GL_MODELVIEW );

        const float * matrix = _view.buff();
        glLoadMatrixf( matrix );
	}
	//////////////////////////////////////////////////////////////////////////
	void MarmaladeRenderSystem::setTextureMatrix( size_t _stage, const float* _texture )
	{
        // To Do
	}
    //////////////////////////////////////////////////////////////////////////
    void MarmaladeRenderSystem::setWorldMatrix( const mt::mat4f & _world )
    {
        // To Do
    }
	//////////////////////////////////////////////////////////////////////////
	VBHandle MarmaladeRenderSystem::createVertexBuffer( size_t _verticesNum, size_t _vertexSize, bool _dynamic )
	{
		size_t size = _verticesNum * _vertexSize;

		MemoryRange memRange;
		memRange.pMem = new unsigned char[size];
		memRange.size = size;
		memRange.offset = 0;
		memRange.flags = 0;
        
        GLenum usage = GL_STATIC_DRAW;

        if( _dynamic == true )
        {
            usage = GL_DYNAMIC_DRAW;
        }

		GLuint bufId = 0;
		glGenBuffers( 1, &bufId );

		glBindBuffer( GL_ARRAY_BUFFER, bufId );
		glBufferData( GL_ARRAY_BUFFER, memRange.size, NULL, usage );
		glBindBuffer( GL_ARRAY_BUFFER, 0 );

		memRange.bufId = bufId;
		
        VBHandle vbHandle = ++m_VBHandleGenerator;
		m_vBuffersMemory.insert( vbHandle, memRange );

		gl_check_error();
		
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
		
		delete [] range->pMem;

		glDeleteBuffers( 1, &range->bufId );
		gl_check_error();
		
        if( _vbHandle == m_currentVertexBuffer )
		{
			m_currentVertexBuffer = 0;
		}

		m_vBuffersMemory.erase( _vbHandle );
	}
	//////////////////////////////////////////////////////////////////////////
	void * MarmaladeRenderSystem::lockVertexBuffer( VBHandle _vbHandle, size_t _offset, size_t _size, uint32 _flags )
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
	
		glBindBuffer( GL_ARRAY_BUFFER, range->bufId );
		glBufferSubData( GL_ARRAY_BUFFER, range->offset, range->size, range->pMem );        
		glBindBuffer( GL_ARRAY_BUFFER, 0 );

		gl_check_error();

		m_vBuffersLocks.erase( _vbHandle );		

		return true;
	}
	//////////////////////////////////////////////////////////////////////////
	void MarmaladeRenderSystem::setVertexBuffer( VBHandle _vbHandle )
	{
		m_currentVertexBuffer = _vbHandle;
	}
	//////////////////////////////////////////////////////////////////////////
	IBHandle MarmaladeRenderSystem::createIndexBuffer( size_t _indiciesNum, bool _dynamic )
	{
		size_t size = _indiciesNum * sizeof( uint16 );

        MemoryRange memRange;
		memRange.pMem = new unsigned char[size];
		memRange.size = size;
		memRange.offset = 0;
		memRange.flags = 0;

        GLenum usage = GL_STATIC_DRAW;

        if( _dynamic == true )
        {
            usage = GL_DYNAMIC_DRAW;
        }

		GLuint bufId = 0;
		glGenBuffers( 1, &bufId );

		glBindBuffer( GL_ELEMENT_ARRAY_BUFFER, bufId );

		glBufferData( GL_ELEMENT_ARRAY_BUFFER, memRange.size, NULL, usage );
		glBindBuffer( GL_ELEMENT_ARRAY_BUFFER, 0 );

		gl_check_error();

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

		delete [] range->pMem;
		glDeleteBuffers( 1, &range->bufId );

        if( _ibHandle == m_currentIndexBuffer )
		{
			m_currentIndexBuffer = 0;
		}

		m_iBuffersMemory.erase( _ibHandle );		
	}
	//////////////////////////////////////////////////////////////////////////
	void * MarmaladeRenderSystem::lockIndexBuffer( IBHandle _ibHandle, size_t _offset, size_t _size, uint32 _flags )
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
		
        void * mem = reinterpret_cast<void *>(memRange.pMem);

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
		
		glBindBuffer( GL_ELEMENT_ARRAY_BUFFER, range->bufId );
		glBufferSubData( GL_ELEMENT_ARRAY_BUFFER, range->offset, range->size, range->pMem );
		glBindBuffer( GL_ELEMENT_ARRAY_BUFFER, 0 );

		gl_check_error();
		
		m_iBuffersLocks.erase( _ibHandle );		

		return true;
	}
	//////////////////////////////////////////////////////////////////////////
	void MarmaladeRenderSystem::setIndexBuffer( IBHandle _ibHandle, size_t _baseVertexIndex )
	{
		m_currentIndexBuffer = _ibHandle;        
	}
	//////////////////////////////////////////////////////////////////////////
	void MarmaladeRenderSystem::setVertexDeclaration( size_t _vertexSize, uint32 _declaration )
	{
        // To Do
	}
	//////////////////////////////////////////////////////////////////////////
	void MarmaladeRenderSystem::drawIndexedPrimitive( EPrimitiveType _type, 
		size_t _baseVertexIndex,  size_t _minIndex, 
		size_t _verticesNum, size_t _startIndex, size_t _indexCount )
	{
		for( size_t i = 0; i != MENGE_MAX_TEXTURE_STAGES; ++i )
		{
			const TextureStage & textureStage = m_textureStage[i];

			if( textureStage.enabled == false )
			{
				glActiveTexture(GL_TEXTURE0 + i);
				glDisable(GL_TEXTURE_2D);
				break;
			}

			glActiveTexture(GL_TEXTURE0 + i);
			glEnable(GL_TEXTURE_2D);

			glBindTexture(GL_TEXTURE_2D, textureStage.texture);

			glTexParameteri( GL_TEXTURE_2D, GL_TEXTURE_WRAP_S, textureStage.wrapS );
			glTexParameteri( GL_TEXTURE_2D, GL_TEXTURE_WRAP_T, textureStage.wrapT );
			glTexParameteri( GL_TEXTURE_2D, GL_TEXTURE_MIN_FILTER, textureStage.minFilter );
			glTexParameteri( GL_TEXTURE_2D, GL_TEXTURE_MAG_FILTER, textureStage.magFilter );

			glTexEnvi(GL_TEXTURE_ENV, GL_TEXTURE_ENV_MODE, GL_COMBINE);

			glTexEnvi(GL_TEXTURE_ENV, GL_COMBINE_RGB, textureStage.colorOp);
			glTexEnvi(GL_TEXTURE_ENV, GL_SRC0_RGB, textureStage.colorArg1);
			glTexEnvi(GL_TEXTURE_ENV, GL_SRC1_RGB, textureStage.colorArg2);
			glTexEnvi(GL_TEXTURE_ENV, GL_OPERAND0_RGB, GL_SRC_COLOR);
			glTexEnvi(GL_TEXTURE_ENV, GL_OPERAND1_RGB, GL_SRC_COLOR);

			glTexEnvi(GL_TEXTURE_ENV, GL_COMBINE_ALPHA, textureStage.alphaOp);
			glTexEnvi(GL_TEXTURE_ENV, GL_SRC0_ALPHA, textureStage.alphaArg1);
			glTexEnvi(GL_TEXTURE_ENV, GL_SRC1_ALPHA, textureStage.alphaArg2);
			glTexEnvi(GL_TEXTURE_ENV, GL_OPERAND0_ALPHA, GL_SRC_ALPHA);
			glTexEnvi(GL_TEXTURE_ENV, GL_OPERAND1_ALPHA, GL_SRC_ALPHA);
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
		
		glBindBuffer( GL_ARRAY_BUFFER, vb_range->bufId );
		glBindBuffer( GL_ELEMENT_ARRAY_BUFFER, ib_range->bufId );

		gl_check_error();

		glEnableClientState( GL_VERTEX_ARRAY );
		glEnableClientState( GL_COLOR_ARRAY );
		glEnableClientState( GL_TEXTURE_COORD_ARRAY );

		glVertexPointer( 3, GL_FLOAT, 32, 0 );
		glColorPointer( 4, GL_UNSIGNED_BYTE, 32,  reinterpret_cast<const GLvoid *>( 12 ) );        

		glClientActiveTexture( GL_TEXTURE0 );
		glTexCoordPointer( 2, GL_FLOAT, 32, reinterpret_cast<const GLvoid *>( 16 ) );
		glClientActiveTexture( GL_TEXTURE1 );
		glTexCoordPointer( 2, GL_FLOAT, 32, reinterpret_cast<const GLvoid *>( 24 ) );
		
        GLenum mode = s_toGLPrimitiveMode[ _type ];
		const uint16 * baseIndex = 0;
		const uint16 * offsetIndex = baseIndex + _startIndex;
		glDrawElements( mode, _indexCount, GL_UNSIGNED_SHORT, reinterpret_cast<const GLvoid *>(offsetIndex) );

		gl_check_error();
		
		glDisableClientState( GL_TEXTURE_COORD_ARRAY );
		glDisableClientState( GL_COLOR_ARRAY ); 
		glDisableClientState( GL_VERTEX_ARRAY );
		
		glBindBuffer( GL_ARRAY_BUFFER, 0 );
		glBindBuffer( GL_ELEMENT_ARRAY_BUFFER, 0 );

		gl_check_error();
	}
	//////////////////////////////////////////////////////////////////////////
	void MarmaladeRenderSystem::setTexture( size_t _stage, const RenderImageInterfacePtr & _texture )
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
	void MarmaladeRenderSystem::setTextureAddressing( size_t _stage, ETextureAddressMode _modeU, ETextureAddressMode _modeV )
	{
        GLenum modeUGL = s_toGLAddressMode[ _modeU ];
        GLenum modeVGL = s_toGLAddressMode[ _modeV ];
        m_textureStage[_stage].wrapS = modeUGL;
        m_textureStage[_stage].wrapT = modeVGL;
	}
	//////////////////////////////////////////////////////////////////////////
	void MarmaladeRenderSystem::setTextureFactor( uint32 _color )
	{

	}
	//////////////////////////////////////////////////////////////////////////
	void MarmaladeRenderSystem::setSrcBlendFactor( EBlendFactor _src )
	{
		m_srcBlendFactor = s_toGLBlendFactor[ _src ];
		glBlendFunc( m_srcBlendFactor, m_dstBlendFactor );
	}
	//////////////////////////////////////////////////////////////////////////
	void MarmaladeRenderSystem::setDstBlendFactor( EBlendFactor _dst )
	{
		m_dstBlendFactor = s_toGLBlendFactor[ _dst ];
		glBlendFunc( m_srcBlendFactor, m_dstBlendFactor );
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
		
        glCullFace( face );
		
        if( enable == true )
        {
            glEnable( GL_CULL_FACE );
        }
        else
        {
            glDisable( GL_CULL_FACE );
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
		    glEnable( GL_DEPTH_TEST );
        }
        else
        {
            glDisable( GL_DEPTH_TEST );
        }
	}
	//////////////////////////////////////////////////////////////////////////
	void MarmaladeRenderSystem::setDepthBufferWriteEnable( bool _depthWrite )
	{
		m_depthMask = _depthWrite;
		glDepthMask( m_depthMask ? GL_TRUE : GL_FALSE );
	}
	//////////////////////////////////////////////////////////////////////////
	void MarmaladeRenderSystem::setDepthBufferCmpFunc( ECompareFunction _depthFunction )
	{
		GLenum cmpFunc = s_toGLCmpFunc[ _depthFunction ];
		glDepthFunc( cmpFunc );
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
		glColorMask( _r ? GL_TRUE : GL_FALSE,
			_g ? GL_TRUE : GL_FALSE,
			_b ? GL_TRUE : GL_FALSE,
			_a ? GL_TRUE : GL_FALSE );
	}
	//////////////////////////////////////////////////////////////////////////
	void MarmaladeRenderSystem::setShadeType( EShadeType _sType )
	{
		GLenum model = s_toGLShadeMode[ _sType ];
		glShadeModel( model );
	}
	//////////////////////////////////////////////////////////////////////////
	void MarmaladeRenderSystem::setAlphaTestEnable( bool _alphaTest )
	{
        if( _alphaTest == true )
        {
            glEnable( GL_ALPHA_TEST );
        }
        else
        {
            glDisable( GL_ALPHA_TEST );
        }
	}
	//////////////////////////////////////////////////////////////////////////
	void MarmaladeRenderSystem::setAlphaBlendEnable( bool _alphaBlend )
	{
		if( _alphaBlend == true )
        {
            glEnable( GL_BLEND );
        }
        else
        {
            glDisable( GL_BLEND );
        }
	}
	//////////////////////////////////////////////////////////////////////////
	void MarmaladeRenderSystem::setAlphaCmpFunc( ECompareFunction _alphaFunc, uint8 _alpha )
	{
		GLenum cmpFunc = s_toGLCmpFunc[ _alphaFunc ];
		glAlphaFunc( cmpFunc, static_cast<float>( _alpha ) / 255.0f );
	}
	//////////////////////////////////////////////////////////////////////////
	void MarmaladeRenderSystem::setLightingEnable( bool _light )
	{
		if( _light == true )
        {
            glEnable( GL_LIGHTING );
        }
        else
        {
            glDisable( GL_LIGHTING );
        }
	}
	//////////////////////////////////////////////////////////////////////////
	void MarmaladeRenderSystem::setTextureStageColorOp( size_t _stage, ETextureOp _textrueOp,  ETextureArgument _arg1, ETextureArgument _arg2 )
	{
        if( _textrueOp == Menge::TOP_DISABLE )
        {
            m_textureStage[_stage].enabled = false;
            return;
        }

        m_textureStage[_stage].enabled = true;

        if( _textrueOp == Menge::TOP_SELECTARG2 )
        {
            _arg1 = _arg2;
            _textrueOp = Menge::TOP_SELECTARG1;
        }

        m_textureStage[_stage].colorOp = s_toGLTextureOp[ _textrueOp ];
        m_textureStage[_stage].colorArg1 = s_toGLTextureArg[ _arg1 ];
        m_textureStage[_stage].colorArg2 = s_toGLTextureArg[ _arg2 ];
	}
	//////////////////////////////////////////////////////////////////////////
	void MarmaladeRenderSystem::setTextureStageAlphaOp( size_t _stage, ETextureOp _textrueOp,  ETextureArgument _arg1, ETextureArgument _arg2 )
	{
        if( _textrueOp == Menge::TOP_DISABLE )
        {
            m_textureStage[_stage].enabled = false;
            return;
        }

        m_textureStage[_stage].enabled = true;

        if( _textrueOp == Menge::TOP_SELECTARG2 )
        {
            _arg1 = _arg2;
            _textrueOp = Menge::TOP_SELECTARG1;
        }

        m_textureStage[_stage].alphaOp = s_toGLTextureOp[ _textrueOp ];
        m_textureStage[_stage].alphaArg1 = s_toGLTextureArg[ _arg1 ];
        m_textureStage[_stage].alphaArg2 = s_toGLTextureArg[ _arg2 ];
	}
    void MarmaladeRenderSystem::setTextureStageTexCoordIndex( size_t _stage, size_t _index )
    {
        //m_textureStage[_stage].texCoordIndex = _index;
    }
	//////////////////////////////////////////////////////////////////////////
	void MarmaladeRenderSystem::setTextureStageFilter( size_t _stage, ETextureFilterType _filterType, ETextureFilter _filter )
	{
        TextureStage& tStage = m_textureStage[_stage];
        if( _filterType == Menge::TFT_MINIFICATION )
        {
            tStage.mengeMinFilter = _filter;
            tStage.minFilter = s_toGLMinMipFilter( tStage.mengeMinFilter, tStage.mengeMipFilter );
        }
        else if( _filterType == Menge::TFT_MIPMAP )
        {
            tStage.mengeMipFilter = _filter;
            tStage.minFilter = s_toGLMinMipFilter( tStage.mengeMinFilter, tStage.mengeMipFilter );
        }
        else if( _filterType == Menge::TFT_MAGNIFICATION )
        {
            tStage.magFilter = s_toGLFilter[ _filter ];
        }
	}
    //////////////////////////////////////////////////////////////////////////
    RenderShaderInterface * MarmaladeRenderSystem::createShader( const void * _code, size_t _len )
    {
        return nullptr;
    }
    //////////////////////////////////////////////////////////////////////////
    void MarmaladeRenderSystem::setShader( RenderShaderInterface * _shader )
    {

    }
    //////////////////////////////////////////////////////////////////////////
    PixelFormat MarmaladeRenderSystem::findFormatFromChannels_( size_t & _channels, PixelFormat _format ) const
    {
        PixelFormat hwFormat = PF_UNKNOWN;

        switch( _format )
        {
        case PF_UNKNOWN:
            {
                if( _channels == 1 )
                {
                    hwFormat = PF_X8R8G8B8;
                    _channels = 4;
                }
                else if( _channels == 3 )
                {
                    hwFormat = PF_X8R8G8B8;
                    _channels = 4;
                }
                else if( _channels == 4 )
                {
                    hwFormat = PF_A8R8G8B8;
                }
            }break;
        default:
            {
                hwFormat = _format;
            }break;
        }

        return hwFormat;
    }
	//////////////////////////////////////////////////////////////////////////
	RenderImageInterfacePtr MarmaladeRenderSystem::createImage( size_t _width, size_t _height, size_t _channels, PixelFormat _format )
	{
        size_t hwChannels = _channels;

        PixelFormat hwFormat = this->findFormatFromChannels_( hwChannels, _format );

		GLuint tuid = 0;
		glGenTextures( 1, &tuid );

		GLint internalFormat = s_toGLInternalFormat( hwFormat );
		GLint textureFormat = s_toGLColorFormat( hwFormat );
		GLint textureType = s_getGLColorDataType( hwFormat );
		GLint texturePitch = _width * hwChannels;

		if( hwFormat == Menge::PF_ETC1 || 
			hwFormat == Menge::PF_PVRTC4_RGB || 
			hwFormat == Menge::PF_PVRTC4_RGBA )
		{
			GLuint size = (_width * _height) >> 1;
			texturePitch = size / _height;
		}

        MarmaladeTexture * texture = m_factoryOGLTexture.createObjectT();

		texture->initialize( 
            tuid
			, ERIM_NORMAL
            , _width
            , _height
            , hwChannels
            , texturePitch
            , hwFormat
            , internalFormat
            , textureFormat
            , textureType
            , false );

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
    bool MarmaladeRenderSystem::beginScene()
    {
		this->clearFrameBuffer( FBT_COLOR );

        return true;
    }
	//////////////////////////////////////////////////////////////////////////
	void MarmaladeRenderSystem::endScene()
	{		
	}
	//////////////////////////////////////////////////////////////////////////
	void MarmaladeRenderSystem::swapBuffers()
	{
        IwGLSwapBuffers();
	}
	//////////////////////////////////////////////////////////////////////////
	void MarmaladeRenderSystem::clearFrameBuffer( uint32 _frameBufferTypes, uint32 _color, float _depth, uint16 _stencil )
	{
		GLbitfield frameBufferFlags = 0;
		
        if( ( _frameBufferTypes & FBT_COLOR ) != 0 )
		{
			frameBufferFlags |= GL_COLOR_BUFFER_BIT;
			glClearColor( GET_R_FLOAT_FROM_ARGB32( _color ),
				GET_G_FLOAT_FROM_ARGB32( _color ),
				GET_B_FLOAT_FROM_ARGB32( _color ),
				GET_A_FLOAT_FROM_ARGB32( _color ) );
		}

		if( ( _frameBufferTypes & FBT_DEPTH ) != 0 )
		{
			frameBufferFlags |= GL_DEPTH_BUFFER_BIT;
			if( m_depthMask == false )
			{
				glDepthMask( GL_TRUE );
			}

			glClearDepthf( _depth );
		}

		if( (_frameBufferTypes & FBT_STENCIL) != 0 )
		{
			frameBufferFlags |= GL_STENCIL_BUFFER_BIT;
			glClearStencil( _stencil );
		}

		glClear( frameBufferFlags );

		if( m_depthMask == false )
		{
			glDepthMask( GL_FALSE );
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
	bool MarmaladeRenderSystem::setRenderTarget( const RenderImageInterfacePtr & _renderTarget, bool _clear )
	{
        return true;
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
    void MarmaladeRenderSystem::clear( uint32 _color )
    {
    
    }
    //////////////////////////////////////////////////////////////////////////
    RenderImageInterfacePtr MarmaladeRenderSystem::createRenderTargetImage( size_t _width, size_t _height, size_t _channels, PixelFormat _format )
    {
        return nullptr;
    }
	//////////////////////////////////////////////////////////////////////////
	RenderImageInterfacePtr MarmaladeRenderSystem::createDynamicImage( size_t _width, size_t _height, size_t _channels, PixelFormat _format )
	{
		return nullptr;
	}
	//////////////////////////////////////////////////////////////////////////
    void MarmaladeRenderSystem::makeProjectionOrthogonal( mt::mat4f & _projectionMatrix, const Viewport & _viewport, float _near, float _far )
	{
        mt::mat4f scale;
        mt::make_scale_m4( scale, 1.f, 1.f, 1.f );

        //D3DXMatrixTranslation(&tmp, -0.5f, +0.5f, 0.0f);
        mt::mat4f translation;
        mt::make_translation_m4( translation, 0.f, 0.f, 0.f );

        //D3DXMatrixMultiply(&matProj, &matProj, &tmp);
        mt::mat4f transform;
        mt::mul_m4_m4( transform, scale, translation );

        //D3DXMatrixOrthoOffCenterLH(&tmp, (float)vp.X, (float)(vp.X+vp.Width), -((float)(vp.Y+vp.Height)), -((float)vp.Y), vp.MinZ, vp.MaxZ);
        mt::mat4f ortho;
        mt::make_projection_ortho_lh_m4(ortho, _viewport.begin.x, _viewport.end.x, _viewport.begin.y, _viewport.end.y, _near, _far );

        //D3DXMatrixMultiply(&matProj, &matProj, &tmp);
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
		//D3DXMatrixScaling(&matProj, 1.0f, -1.0f, 1.0f);
		mt::mat4f scale;
		mt::make_scale_m4( scale, 1.0f, 1.0f, 1.0f );

		//D3DXMatrixTranslation(&tmp, -0.5f, +0.5f, 0.0f);
		mt::mat4f translation;
		mt::make_translation_m4( translation, -0.5f, -0.5f, 0.0f );

		//D3DXMatrixMultiply(&matProj, &matProj, &tmp);
		mt::mat4f transform;
		mt::mul_m4_m4( transform, scale, translation );

		//D3DXMatrixOrthoOffCenterLH(&tmp, (float)vp.X, (float)(vp.X+vp.Width), -((float)(vp.Y+vp.Height)), -((float)vp.Y), vp.MinZ, vp.MaxZ);
		mt::mat4f frustum;
		mt::make_projection_frustum_m4(frustum, _viewport.begin.x, _viewport.end.x, _viewport.begin.y, _viewport.end.y, _near, _far );

		//D3DXMatrixMultiply(&matProj, &matProj, &tmp);
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
