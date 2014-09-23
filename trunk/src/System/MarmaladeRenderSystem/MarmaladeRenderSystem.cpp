
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
	bool MarmaladeRenderSystem::createRenderWindow( const Resolution & _resolution, uint32_t _bits,
												bool _fullscreen, WindowHandle _winHandle, 
												bool _waitForVSync, int _FSAAType, int _FSAAQuality )
	{
        m_resolution = _resolution;

		for( uint32_t i = 0; i < MENGE_MAX_TEXTURE_STAGES; ++i )
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

		glActiveTexture( GL_TEXTURE1 );
		glEnable( GL_TEXTURE_2D ); 
		
        glDepthMask( GL_FALSE );
		glPixelStorei( GL_UNPACK_ALIGNMENT, 1 );
		        
        m_depthMask = false;
        		
        return true;
	}
	//////////////////////////////////////////////////////////////////////////
	bool MarmaladeRenderSystem::screenshot( const RenderImageInterfacePtr & _image, const mt::vec4f & _rect )
	{
		(_rect);
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
	void MarmaladeRenderSystem::setTextureMatrix( uint32_t _stage, const float* _texture )
	{
        // To Do
	}
    //////////////////////////////////////////////////////////////////////////
    void MarmaladeRenderSystem::setWorldMatrix( const mt::mat4f & _world )
    {
        // To Do
    }
	//////////////////////////////////////////////////////////////////////////
	VBHandle MarmaladeRenderSystem::createVertexBuffer( uint32_t _verticesNum, uint32_t _vertexSize, bool _dynamic )
	{
		size_t size = _verticesNum * _vertexSize;
		unsigned char * memory = new unsigned char[size];

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

		if( _vbHandle == m_currentVertexBuffer )
		{
			glBindBuffer( GL_ARRAY_BUFFER, 0 );
			m_currentVertexBuffer = 0;
		}

		glDeleteBuffers( 1, &range->bufId );
		gl_check_error();

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
	IBHandle MarmaladeRenderSystem::createIndexBuffer( uint32_t _indiciesNum, bool _dynamic )
	{
		size_t size = _indiciesNum * sizeof( uint16 );
		unsigned char * memory = new unsigned char[size];

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

		if( _ibHandle == m_currentIndexBuffer )
		{
			glBindBuffer( GL_ELEMENT_ARRAY_BUFFER, 0 );
			m_currentIndexBuffer = 0;
		}

		delete [] range->pMem;

		glDeleteBuffers( 1, &range->bufId );
		gl_check_error();

		m_iBuffersMemory.erase( _ibHandle );		
	}
	//////////////////////////////////////////////////////////////////////////
	void * MarmaladeRenderSystem::lockIndexBuffer( IBHandle _ibHandle, uint32_t _offset, uint32_t _size, EBufferLockFlag _flags )
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
	void MarmaladeRenderSystem::drawIndexedPrimitive( EPrimitiveType _type, 
		uint32_t _baseVertexIndex,  uint32_t _minIndex, 
		uint32_t _verticesNum, uint32_t _startIndex, uint32_t _indexCount )
	{
		for( uint32_t i = 0; i != MENGE_MAX_TEXTURE_STAGES; ++i )
		{
			const TextureStage & textureStage = m_textureStage[i];

			if( textureStage.texture == 0 )
			{
				glActiveTexture(GL_TEXTURE0 + i);
				glDisable(GL_TEXTURE_2D);
				break;
			}

			glActiveTexture(GL_TEXTURE0 + i);
			glEnable(GL_TEXTURE_2D);

			

			glTexParameteri( GL_TEXTURE_2D, GL_TEXTURE_WRAP_S, textureStage.wrapS );
			glTexParameteri( GL_TEXTURE_2D, GL_TEXTURE_WRAP_T, textureStage.wrapT );
			glTexParameteri( GL_TEXTURE_2D, GL_TEXTURE_MIN_FILTER, textureStage.minFilter );
			glTexParameteri( GL_TEXTURE_2D, GL_TEXTURE_MAG_FILTER, textureStage.magFilter );

			glBindTexture( GL_TEXTURE_2D, textureStage.texture );
			
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
		glVertexPointer( 3, GL_FLOAT, 32, reinterpret_cast<const GLvoid *>( 0 ) );

		glEnableClientState( GL_COLOR_ARRAY );
		glColorPointer( 4, GL_UNSIGNED_BYTE, 32,  reinterpret_cast<const GLvoid *>( 12 ) );

		glEnableClientState( GL_TEXTURE_COORD_ARRAY );

		glClientActiveTexture( GL_TEXTURE0 );
		glTexCoordPointer( 2, GL_FLOAT, 32, reinterpret_cast<const GLvoid *>( 16 ) );
		glClientActiveTexture( GL_TEXTURE1 );
		glTexCoordPointer( 2, GL_FLOAT, 32, reinterpret_cast<const GLvoid *>( 24 ) );
		
        GLenum mode = s_getGLPrimitiveMode( _type );
		const uint16_t * baseIndex = nullptr;
		const uint16_t * offsetIndex = baseIndex + _startIndex;
		glDrawElements( mode, _indexCount, GL_UNSIGNED_SHORT, reinterpret_cast<const GLvoid *>(offsetIndex) );

		gl_check_error();
		
		glDisableClientState( GL_VERTEX_ARRAY );
		glDisableClientState( GL_COLOR_ARRAY );
		glDisableClientState( GL_TEXTURE_COORD_ARRAY );
		
		glBindBuffer( GL_ARRAY_BUFFER, 0 );
		glBindBuffer( GL_ELEMENT_ARRAY_BUFFER, 0 );

		gl_check_error();
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
	void MarmaladeRenderSystem::setAlphaCmpFunc( ECompareFunction _alphaFunc, uint8_t _alpha )
	{
		GLenum cmpFunc = s_toGLCmpFunc[_alphaFunc];
		GLclampf ref = (GLclampf)(static_cast<float>( _alpha ) / 255.0f);

		glAlphaFunc( cmpFunc, ref );
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
    RenderShaderInterface * MarmaladeRenderSystem::createShader( const void * _buffer, size_t _size )
    {
        return nullptr;
    }
    //////////////////////////////////////////////////////////////////////////
    void MarmaladeRenderSystem::setShader( RenderShaderInterface * _shader )
    {

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
	RenderImageInterfacePtr MarmaladeRenderSystem::createImage( uint32_t _width, uint32_t _height, uint32_t _channels, uint32_t _depth, PixelFormat _format )
	{
        uint32_t hwChannels;
        PixelFormat hwFormat;
		this->findFormatFromChannels_( _format, _channels, hwFormat, hwChannels );

		GLint internalFormat = s_toGLInternalFormat( hwFormat );

		if( internalFormat == 0 )
		{
			LOGGER_ERROR(m_serviceProvider)("MarmaladeRenderSystem::createImage invalid get GL Texture format for PF %d"
				, hwFormat
				);

			return nullptr;
		}

		GLint textureFormat = s_toGLColorFormat( hwFormat );
		GLint textureType = s_getGLColorDataType( hwFormat );

		GLuint tuid = 0;
		glGenTextures( 1, &tuid );

        MarmaladeTexture * texture = m_factoryOGLTexture.createObjectT();

		texture->initialize( 
			m_serviceProvider
            , tuid
			, ERIM_NORMAL
            , _width
            , _height
            , hwChannels
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
        IwGLSwapBuffers();
	}
	//////////////////////////////////////////////////////////////////////////
	void MarmaladeRenderSystem::clearFrameBuffer( uint32_t _frameBufferTypes, uint32_t _color, float _depth, uint32_t _stencil )
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
