
#	include "OGLRenderSystem.h"

#	include <cmath>

#	include "Interface/LogSystemInterface.h"
#	include "OGLTexture.h"
//#	include "OGLWindowContext.h"

#   include "Logger/Logger.h"

#	define GET_A_FLOAT_FROM_ARGB32( argb ) ( ((float)(argb >> 24)) / 255.0f )
#	define GET_R_FLOAT_FROM_ARGB32( argb ) ( ((float)((argb >> 16) & 0xFF)) / 255.0f )
#	define GET_G_FLOAT_FROM_ARGB32( argb ) ( ((float)((argb >> 8) & 0xFF)) / 255.0f )
#	define GET_B_FLOAT_FROM_ARGB32( argb ) ( (float)(argb & 0xFF) / 255.0f )

SERVICE_FACTORY( RenderSystem, Menge::RenderSystemInterface, Menge::OGLRenderSystem );
//////////////////////////////////////////////////////////////////////////
namespace Menge
{
    //////////////////////////////////////////////////////////////////////////
    static Menge::uint32 s_firstPO2From( Menge::uint32 n )
    {
	    --n;            
	    n |= n >> 16;
	    n |= n >> 8;
	    n |= n >> 4;
	    n |= n >> 2;
	    n |= n >> 1;
	    ++n;
	    return n;
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
	    case Menge::PF_A8:
		    return GL_ALPHA;
		    //return GL_LUMINANCE;
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
	OGLRenderSystem::OGLRenderSystem()
		: m_serviceProvider(NULL)
		//, m_windowContext(NULL)
		, m_supportNPOT(false)
		, m_currentVertexBuffer(0)
		, m_currentIndexBuffer(0)
		, m_srcBlendFactor(GL_ONE)
		, m_dstBlendFactor(GL_ZERO)
		, m_activeTextureStage(0)
		, m_activeTexture(0)
		, m_activeRenderTarget(NULL)
	{
	}
	//////////////////////////////////////////////////////////////////////////
	OGLRenderSystem::~OGLRenderSystem()
	{
		//releaseWindowContext( m_windowContext );
	}
    //////////////////////////////////////////////////////////////////////////
    void OGLRenderSystem::setServiceProvider( ServiceProviderInterface * _serviceProvider )
    {
        m_serviceProvider = _serviceProvider;
    }
    //////////////////////////////////////////////////////////////////////////
    ServiceProviderInterface * OGLRenderSystem::getServiceProvider() const
    {
        return m_serviceProvider;
    }
	//////////////////////////////////////////////////////////////////////////
	bool OGLRenderSystem::initialize()
	{
		LOGGER_INFO(m_serviceProvider)( "Initializing OpenGL RenderSystem..." );

		return true;
	}
	//////////////////////////////////////////////////////////////////////////
    void OGLRenderSystem::finalize()
    {        
    }
    //////////////////////////////////////////////////////////////////////////
    void OGLRenderSystem::setRenderListener( RenderSystemListener * _listener )
    {
        m_listener = _listener;
    }
	//////////////////////////////////////////////////////////////////////////
	bool OGLRenderSystem::createRenderWindow( std::size_t _width, std::size_t _height, int _bits,
												bool _fullscreen, WindowHandle _winHandle, 
												bool _waitForVSync, int _FSAAType, int _FSAAQuality )
	{
//#if WIN32
//		createWindowContext( &m_windowContext );
//		if( m_windowContext == NULL )
//		{
//			LOGGER_ERROR(m_logService)( "Error: failed to create platform window context" );
//			return false;
//		}
//
//		m_winWidth = _width;
//		m_winHeight = _height;
//		if( m_windowContext->initialize( _width, _height, _bits, _fullscreen, _winHandle, _waitForVSync ) == false )
//		{
//			LOGGER_ERROR(m_logService)( "Error: failed to initialize window context" );
//			return false;
//		}
//		m_windowContext->setFullscreenMode( _width, _height, _fullscreen );
//		m_winContextWidth = _width;
//		m_winContextHeight = _height;
//
//		const char* str = (const char*)glGetString( GL_VERSION );
//		String mstr = "None";
//		if( str != NULL )
//		{
//			mstr = str;
//		}
//		LOGGER_INFO(m_logService)( "OpenGL Version: %s", mstr.c_str() );
//		mstr = "None";
//		str = (const char*)glGetString( GL_VENDOR );
//		if( str != NULL )
//		{
//			mstr = str;
//		}
//		LOGGER_INFO(m_logService)( "Vendor: %s", mstr.c_str() );
//		str = (const char*)glGetString( GL_RENDERER );
//		//LOG( "Renderer: " + Menge::String( str ) );
//		m_ext = (const char*)glGetString( GL_EXTENSIONS );
//		//LOG( "Extensions:" );
//		//LOG( m_ext );
//		// check for NPOT
//		String extSubStr = "texture_non_power_of_two";
//		String::size_type pos =  m_ext.find( extSubStr );
//		/*if( pos != String::npos && ( m_ext[pos+extSubStr.size()] == '\0' || m_ext[pos+extSubStr.size()] == ' ' ) )	// it seems to be supported
//		{
//			// try to create NPOT to be sure
//			GLuint npotTex;
//			GLint size = 65;
//			glGenTextures( 1, &npotTex );
//			glBindTexture( GL_TEXTURE_2D, npotTex );
//			//glTexParameteri( GL_TEXTURE_2D, GL_TEXTURE_MAX_LEVEL, 0 );
//			glTexParameteri( GL_TEXTURE_2D, GL_TEXTURE_WRAP_S, GL_CLAMP_TO_EDGE);
//			glTexParameteri( GL_TEXTURE_2D, GL_TEXTURE_WRAP_T, GL_CLAMP_TO_EDGE);
//			glTexParameteri( GL_TEXTURE_2D, GL_TEXTURE_MAG_FILTER, GL_LINEAR);
//			glTexParameteri( GL_TEXTURE_2D, GL_TEXTURE_MIN_FILTER, GL_LINEAR);
//			glTexImage2D( GL_PROXY_TEXTURE_2D, 0, GL_RGBA8, size, size, 0, GL_BGRA, GL_UNSIGNED_BYTE, NULL );
//			GLint width = 0;
//			glGetTexLevelParameteriv( GL_PROXY_TEXTURE_2D, 0, GL_TEXTURE_WIDTH, &width );
//			if( width == size )
//			{
//				m_supportNPOT = true;
//			}
//			glBindTexture( GL_TEXTURE_2D, 0 );
//			glDeleteTextures( 1, &npotTex );
//		}*/
//
//		extSubStr = "pixel_buffer_object";
//		pos = m_ext.find( extSubStr );
//		if( pos != String::npos && ( m_ext[pos+extSubStr.size()] == '\0' || m_ext[pos+extSubStr.size()] == ' ' ) )	// it seems to be supported
//		{
//			LOGGER_INFO(m_logService)( "Supports PBO" );
//		}
//		
//#endif

		for( int i = 0; i < MENGE_MAX_TEXTURE_STAGES; ++i )
		{
			glActiveTexture( GL_TEXTURE0 + i );
			glTexEnvi( GL_TEXTURE_ENV, GL_TEXTURE_ENV_MODE, GL_COMBINE );
			glTexEnvi( GL_TEXTURE_ENV, GL_OPERAND0_RGB, GL_SRC_COLOR );
			glTexEnvi( GL_TEXTURE_ENV, GL_OPERAND1_RGB, GL_SRC_COLOR );
			glTexEnvi( GL_TEXTURE_ENV, GL_OPERAND0_ALPHA, GL_SRC_ALPHA );
			glTexEnvi( GL_TEXTURE_ENV, GL_OPERAND1_ALPHA, GL_SRC_ALPHA );			
			glDisable( GL_TEXTURE_2D );

            m_textureStage[i].enabled = false;
            m_textureStage[i].minFilter = 0;
            m_textureStage[i].magFilter = 0;
            m_textureStage[i].wrapS = 0;
            m_textureStage[i].wrapT = 0;
            m_textureStage[i].mengeMinFilter = Menge::TF_NONE;
            m_textureStage[i].mengeMipFilter = Menge::TF_NONE;
            m_textureStage[i].texture = 0;
            m_textureStage[i].colorOp = 0;
            m_textureStage[i].colorArg1 = 0;
            m_textureStage[i].colorArg2 = 0;
            m_textureStage[i].alphaOp = 0;
            m_textureStage[i].alphaArg1 = 0;
            m_textureStage[i].alphaArg2 = 0;
		}
        
		glFrontFace( GL_CW );
		glDisable( GL_DEPTH_TEST );
		glDisable( GL_CULL_FACE );
		glDisable( GL_LIGHTING );
		glEnable( GL_BLEND );
		glActiveTexture( GL_TEXTURE0 );
		glEnable( GL_TEXTURE_2D ); 
		m_textureStage[0].enabled = true;
		// glEnable( GL_SCISSOR_TEST ); do not use scissor test
		m_depthMask = false;
		glDepthMask( GL_FALSE );
		glPixelStorei( GL_UNPACK_ALIGNMENT, 1 );
		//clearFrameBuffer( FBT_COLOR | FBT_DEPTH | FBT_STENCIL );
		return true;
	}
	//////////////////////////////////////////////////////////////////////////
	float OGLRenderSystem::getTexelOffsetX() const 
	{
		return 0.0f;
	}
	//////////////////////////////////////////////////////////////////////////
	float OGLRenderSystem::getTexelOffsetY() const 
	{
		return 0.0f;
	}
	//////////////////////////////////////////////////////////////////////////
	void OGLRenderSystem::screenshot( RenderImageInterface* _image, const float * _rect )
	{
        //NOT SUPPORT!
	}
	//////////////////////////////////////////////////////////////////////////
	void OGLRenderSystem::setProjectionMatrix( const mt::mat4f & _projection )
	{

	}
	//////////////////////////////////////////////////////////////////////////
	void OGLRenderSystem::setModelViewMatrix( const mt::mat4f & _view )
	{

	}
	//////////////////////////////////////////////////////////////////////////
	void OGLRenderSystem::setTextureMatrix( size_t _stage, const float* _texture )
	{
        // To Do
	}
    //////////////////////////////////////////////////////////////////////////
    void OGLRenderSystem::setWorldMatrix( const mt::mat4f & _world )
    {
        // To Do
    }
	//////////////////////////////////////////////////////////////////////////
	VBHandle OGLRenderSystem::createVertexBuffer( std::size_t _verticesNum, std::size_t _vertexSize )
	{
		GLuint bufId = 0;
		glGenBuffers( 1, &bufId );
		glBindBuffer( GL_ARRAY_BUFFER, bufId );
        
		MemoryRange memRange;
		memRange.size = _verticesNum * _vertexSize;
		memRange.pMem = new unsigned char[memRange.size];
		
        glBufferData( GL_ARRAY_BUFFER, memRange.size, NULL, GL_DYNAMIC_DRAW );
		glBindBuffer( GL_ARRAY_BUFFER, m_currentVertexBuffer );
		
        VBHandle vbHandle = static_cast<VBHandle>( bufId );
		m_vBuffersMemory.insert( std::make_pair( vbHandle, memRange ) );
		
        return vbHandle;
	}
	//////////////////////////////////////////////////////////////////////////
	void OGLRenderSystem::releaseVertexBuffer( VBHandle _vbHandle )
	{
		TMapVBufferMemory::iterator it_find = m_vBuffersMemory.find( _vbHandle );
		
        if( it_find != m_vBuffersMemory.end() )
		{
			delete[] it_find->second.pMem;
			m_vBuffersMemory.erase( it_find );
		}
		
        GLuint bufId = (GLuint)( _vbHandle );
		
        if( bufId == m_currentVertexBuffer )
		{
			m_currentVertexBuffer = 0;
		}
		
        glDeleteBuffers( 1, &bufId );
	}
	//////////////////////////////////////////////////////////////////////////
	void* OGLRenderSystem::lockVertexBuffer( VBHandle _vbHandle, size_t _offset, size_t _size, uint32 _flags )
	{
		TMapVBufferMemory::iterator it_find = m_vBuffersMemory.find( _vbHandle );
		
        if( it_find == m_vBuffersMemory.end() )
		{
			return NULL;
		}
		
        MemoryRange memRange = { it_find->second.pMem, _size, _offset };
		
        m_vBuffersLocks.insert( std::make_pair( _vbHandle, memRange ) );
		
        return memRange.pMem + _offset;
	}
	//////////////////////////////////////////////////////////////////////////
	bool OGLRenderSystem::unlockVertexBuffer( VBHandle _vbHandle )
	{
		TMapVBufferMemory::iterator it_find = m_vBuffersLocks.find( _vbHandle );
		
        if( it_find == m_vBuffersLocks.end() )
		{
			return false;
		}
	
		GLuint bufId = static_cast<GLuint>( _vbHandle );
		glBindBuffer( GL_ARRAY_BUFFER, bufId );
		
        MemoryRange& memRange = it_find->second;

		glBufferSubData( GL_ARRAY_BUFFER, memRange.offset, memRange.size, memRange.pMem + memRange.offset );
		glBindBuffer( GL_ARRAY_BUFFER, m_currentVertexBuffer );

		m_vBuffersLocks.erase( it_find );

		return true;
	}
	//////////////////////////////////////////////////////////////////////////
	void OGLRenderSystem::setVertexBuffer( VBHandle _vbHandle )
	{
		GLuint bufId = static_cast<GLuint>( _vbHandle );
		glBindBuffer( GL_ARRAY_BUFFER, bufId );
		m_currentVertexBuffer = bufId;
	}
	//////////////////////////////////////////////////////////////////////////
	IBHandle OGLRenderSystem::createIndexBuffer( std::size_t _indiciesNum )
	{
		GLuint bufId = 0;
		glGenBuffers( 1, &bufId );

		glBindBuffer( GL_ELEMENT_ARRAY_BUFFER, bufId );
		
        MemoryRange memRange;
		memRange.size = _indiciesNum * sizeof( uint16 );
		memRange.pMem = new unsigned char[memRange.size];
		
        glBufferData( GL_ELEMENT_ARRAY_BUFFER, memRange.size, NULL, GL_STATIC_DRAW );
		glBindBuffer( GL_ELEMENT_ARRAY_BUFFER, m_currentIndexBuffer );
		
        IBHandle ibHandle = static_cast<IBHandle>( bufId );
		m_iBuffersMemory.insert( std::make_pair( ibHandle, memRange ) );

		return ibHandle;
	}
	//////////////////////////////////////////////////////////////////////////
	void OGLRenderSystem::releaseIndexBuffer( IBHandle _ibHandle )
	{
		TMapIBufferMemory::iterator it_find = m_iBuffersMemory.find( _ibHandle );
		
        if( it_find != m_iBuffersMemory.end() )
		{
			delete[] it_find->second.pMem;
			m_iBuffersMemory.erase( it_find );
		}
		
        GLuint bufId = (GLuint)( _ibHandle );
		
        if( bufId == m_currentIndexBuffer )
		{
			m_currentIndexBuffer = 0;
		}

		glDeleteBuffers( 1, &bufId );
	}
	//////////////////////////////////////////////////////////////////////////
	uint16* OGLRenderSystem::lockIndexBuffer( IBHandle _ibHandle )
	{
		TMapIBufferMemory::iterator it_find = m_iBuffersMemory.find( _ibHandle );

		if( it_find == m_iBuffersMemory.end() )
		{
			return NULL;
		}
		
        MemoryRange memRange = { it_find->second.pMem, it_find->second.size, 0 };
		m_iBuffersLocks.insert( std::make_pair( _ibHandle, memRange ) );
		
        return ( uint16 * )memRange.pMem;
	}
	//////////////////////////////////////////////////////////////////////////
	bool OGLRenderSystem::unlockIndexBuffer( IBHandle _ibHandle )
	{
		TMapIBufferMemory::iterator it_find = m_iBuffersLocks.find( _ibHandle );
		
        if( it_find == m_iBuffersLocks.end() )
		{
			return false;
		}
		
		GLuint bufId = static_cast<GLuint>( _ibHandle );
		glBindBuffer( GL_ELEMENT_ARRAY_BUFFER, bufId );
		MemoryRange& memRange = it_find->second;
		
		glBufferSubData( GL_ELEMENT_ARRAY_BUFFER, memRange.offset, memRange.size, memRange.pMem + memRange.offset );
		glBindBuffer( GL_ELEMENT_ARRAY_BUFFER, m_currentIndexBuffer );
		
		m_iBuffersLocks.erase( it_find );

		return true;
	}
	//////////////////////////////////////////////////////////////////////////
	void OGLRenderSystem::setIndexBuffer( IBHandle _ibHandle, size_t _baseVertexIndex )
	{
		GLuint bufId = static_cast<GLuint>( _ibHandle );
		glBindBuffer( GL_ELEMENT_ARRAY_BUFFER, bufId );
		m_currentIndexBuffer = bufId;
	}
	//////////////////////////////////////////////////////////////////////////
	void OGLRenderSystem::setVertexDeclaration( size_t _vertexSize, uint32 _declaration )
	{
        // To Do
	}
	//////////////////////////////////////////////////////////////////////////
	void OGLRenderSystem::drawIndexedPrimitive( EPrimitiveType _type, 
		std::size_t _baseVertexIndex,  std::size_t _minIndex, 
		std::size_t _verticesNum, std::size_t _startIndex, std::size_t _indexCount )
	{
		GLenum mode = s_toGLPrimitiveMode[ _type ];

        for(size_t i = 0; i < MENGE_MAX_TEXTURE_STAGES; ++i)
        {
            if(!m_textureStage[i].enabled)
            {
                glActiveTexture(GL_TEXTURE0 + i);
                glDisable(GL_TEXTURE_2D);
                break;
            }

            glActiveTexture(GL_TEXTURE0 + i);
            glEnable(GL_TEXTURE_2D);
            glTexParameteri( GL_TEXTURE_2D, GL_TEXTURE_WRAP_S, m_textureStage[i].wrapS );
            glTexParameteri( GL_TEXTURE_2D, GL_TEXTURE_WRAP_T, m_textureStage[i].wrapT );
            glTexParameteri( GL_TEXTURE_2D, GL_TEXTURE_MIN_FILTER, m_textureStage[i].minFilter );
            glTexParameteri( GL_TEXTURE_2D, GL_TEXTURE_MAG_FILTER, m_textureStage[i].magFilter );

            glBindTexture(GL_TEXTURE_2D, m_textureStage[i].texture);
            glTexEnvi(GL_TEXTURE_ENV, GL_TEXTURE_ENV_MODE, GL_COMBINE);

            //blend the texture with the framebuffer(GL_PREVIOUS)
            glTexEnvi(GL_TEXTURE_ENV, GL_COMBINE_RGB, m_textureStage[i].colorOp);
            glTexEnvi(GL_TEXTURE_ENV, GL_SRC0_RGB, m_textureStage[i].colorArg1);
            glTexEnvi(GL_TEXTURE_ENV, GL_SRC1_RGB, m_textureStage[i].colorArg2);
            glTexEnvi(GL_TEXTURE_ENV, GL_OPERAND0_RGB, GL_SRC_COLOR);
            glTexEnvi(GL_TEXTURE_ENV, GL_OPERAND1_RGB, GL_SRC_COLOR);
            //use the texture's alpha channel
            glTexEnvi(GL_TEXTURE_ENV, GL_COMBINE_ALPHA, m_textureStage[i].alphaOp);
            glTexEnvi(GL_TEXTURE_ENV, GL_SRC0_ALPHA, m_textureStage[i].alphaArg1);
            glTexEnvi(GL_TEXTURE_ENV, GL_SRC1_ALPHA, m_textureStage[i].alphaArg2);
            glTexEnvi(GL_TEXTURE_ENV, GL_OPERAND0_ALPHA, GL_SRC_ALPHA);
            glTexEnvi(GL_TEXTURE_ENV, GL_OPERAND1_ALPHA, GL_SRC_ALPHA); 
            //m_textureStage[i].enabled = false;
        }

		glDrawElements( mode, _indexCount, GL_UNSIGNED_SHORT, reinterpret_cast<const GLvoid *>( _startIndex * sizeof( uint16 ) ) );		
	}
	//////////////////////////////////////////////////////////////////////////
	void OGLRenderSystem::setTexture( std::size_t _stage, RenderImageInterface* _texture )
	{
        OGLTexture* texture = static_cast<OGLTexture*>(_texture);

        if( texture != NULL )
        {
            TextureStage& tStage = m_textureStage[_stage];

            tStage.texture = texture->getUId();
            tStage.wrapS = texture->getWrapS();
            tStage.wrapT = texture->getWrapT();
            tStage.minFilter = texture->getMinFilter();
            tStage.magFilter = texture->getMagFilter();
        }
        else
        {
            m_textureStage[_stage].texture = 0;
        }
	}
	//////////////////////////////////////////////////////////////////////////
	void OGLRenderSystem::setTextureAddressing( std::size_t _stage, ETextureAddressMode _modeU, ETextureAddressMode _modeV )
	{
        GLenum modeUGL = s_toGLAddressMode[ _modeU ];
        GLenum modeVGL = s_toGLAddressMode[ _modeV ];
        m_textureStage[_stage].wrapS = modeUGL;
        m_textureStage[_stage].wrapT = modeVGL;
	}
	//////////////////////////////////////////////////////////////////////////
	void OGLRenderSystem::setTextureFactor( uint32 _color )
	{

	}
	//////////////////////////////////////////////////////////////////////////
	void OGLRenderSystem::setSrcBlendFactor( EBlendFactor _src )
	{
		m_srcBlendFactor = s_toGLBlendFactor[ _src ];
		glBlendFunc( m_srcBlendFactor, m_dstBlendFactor );
	}
	//////////////////////////////////////////////////////////////////////////
	void OGLRenderSystem::setDstBlendFactor( EBlendFactor _dst )
	{
		m_dstBlendFactor = s_toGLBlendFactor[ _dst ];
		glBlendFunc( m_srcBlendFactor, m_dstBlendFactor );
	}
	//////////////////////////////////////////////////////////////////////////
	void OGLRenderSystem::setCullMode( ECullMode _mode )
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
    void OGLRenderSystem::setSeparateAlphaBlendMode()
    {
    
    }
	//////////////////////////////////////////////////////////////////////////
	void OGLRenderSystem::setDepthBufferTestEnable( bool _depthTest )
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
	void OGLRenderSystem::setDepthBufferWriteEnable( bool _depthWrite )
	{
		m_depthMask = _depthWrite;
		glDepthMask( m_depthMask ? GL_TRUE : GL_FALSE );
	}
	//////////////////////////////////////////////////////////////////////////
	void OGLRenderSystem::setDepthBufferCmpFunc( ECompareFunction _depthFunction )
	{
		GLenum cmpFunc = s_toGLCmpFunc[ _depthFunction ];
		glDepthFunc( cmpFunc );
	}
	//////////////////////////////////////////////////////////////////////////
	void OGLRenderSystem::setFillMode( EFillMode _mode )
	{
		/*GLenum mode = s_toGLFillMode[ _mode ];
		glPolygonMode( GL_FRONT_AND_BACK, mode );*/
	}
	//////////////////////////////////////////////////////////////////////////
	void OGLRenderSystem::setColorBufferWriteEnable( bool _r, bool _g, bool _b, bool _a )
	{
		glColorMask( _r ? GL_TRUE : GL_FALSE,
			_g ? GL_TRUE : GL_FALSE,
			_b ? GL_TRUE : GL_FALSE,
			_a ? GL_TRUE : GL_FALSE );
	}
	//////////////////////////////////////////////////////////////////////////
	void OGLRenderSystem::setShadeType( EShadeType _sType )
	{
		GLenum model = s_toGLShadeMode[ _sType ];
		glShadeModel( model );
	}
	//////////////////////////////////////////////////////////////////////////
	void OGLRenderSystem::setAlphaTestEnable( bool _alphaTest )
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
	void OGLRenderSystem::setAlphaBlendEnable( bool _alphaBlend )
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
	void OGLRenderSystem::setAlphaCmpFunc( ECompareFunction _alphaFunc, uint8 _alpha )
	{
		GLenum cmpFunc = s_toGLCmpFunc[ _alphaFunc ];
		glAlphaFunc( cmpFunc, static_cast<float>( _alpha ) / 255.0f );
	}
	//////////////////////////////////////////////////////////////////////////
	void OGLRenderSystem::setLightingEnable( bool _light )
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
	void OGLRenderSystem::setTextureStageColorOp( size_t _stage, ETextureOp _textrueOp,  ETextureArgument _arg1, ETextureArgument _arg2 )
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
	void OGLRenderSystem::setTextureStageAlphaOp( size_t _stage, ETextureOp _textrueOp,  ETextureArgument _arg1, ETextureArgument _arg2 )
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
	//////////////////////////////////////////////////////////////////////////
	void OGLRenderSystem::setTextureStageFilter( size_t _stage, ETextureFilterType _filterType, ETextureFilter _filter )
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
    //////////////////////////////////////////////////////////////////////////
    PixelFormat OGLRenderSystem::findFormatFromChannels_( size_t & _channels, PixelFormat _format ) const
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
	RenderImageInterface* OGLRenderSystem::createImage( std::size_t _width, std::size_t _height, size_t _channels, PixelFormat _format )
	{
        std::size_t hwChannels = _channels;

        PixelFormat hwFormat = this->findFormatFromChannels_( hwChannels, _format );

		std::size_t hwWidth = _width;
		std::size_t hwHeight = _height;        

		if( ( _width & ( _width - 1 ) ) != 0
			|| ( _height & ( _height - 1 ) ) != 0 )
		{
			bool npot = false;//m_supportNPOT;//();
			if( npot == false )	// we're all gonna die
			{
				hwWidth = s_firstPO2From( _width );
				hwHeight = s_firstPO2From( _height );
			}
		}

		GLuint tuid = 0;
		glGenTextures( 1, &tuid );

		GLint internalFormat = s_toGLInternalFormat( hwFormat );
		GLint textureFormat = s_toGLColorFormat( hwFormat );
		GLint textureType = s_getGLColorDataType( hwFormat );
		GLint texturePitch = hwWidth * hwChannels;

		OGLTexture* texture = new OGLTexture( 
            tuid
            , hwWidth
            , hwHeight
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

		//glBindTexture( GL_TEXTURE_2D, tuid );
		//glTexParameteri( GL_TEXTURE_2D, GL_TEXTURE_WRAP_S, wrapS );
		//glTexParameteri( GL_TEXTURE_2D, GL_TEXTURE_WRAP_T, wrapT );
		//glTexParameteri( GL_TEXTURE_2D, GL_TEXTURE_MAG_FILTER, minFilter );
		//glTexParameteri( GL_TEXTURE_2D, GL_TEXTURE_MIN_FILTER, magFilter );
		////glTexParameteri( GL_TEXTURE_2D, GL_TEXTURE_MAX_LEVEL, 0 );
		//glTexParameteri( GL_TEXTURE_2D, GL_GENERATE_MIPMAP, GL_FALSE );
		//
		//glTexImage2D( GL_TEXTURE_2D, level, internalFormat,	hwWidth, hwHeight, border, textureFormat, textureType, NULL );

		//glBindTexture( GL_TEXTURE_2D, m_activeTexture );

		return texture;
	}
    //////////////////////////////////////////////////////////////////////////
    bool OGLRenderSystem::beginScene()
    {
        glEnableClientState( GL_VERTEX_ARRAY );
        glEnableClientState( GL_COLOR_ARRAY );
        glEnableClientState( GL_TEXTURE_COORD_ARRAY );  
                
        glVertexPointer( 3, GL_FLOAT, 32,  0 );

        glColorPointer( 4, GL_UNSIGNED_BYTE, 32,  reinterpret_cast<const GLvoid *>( 12 ) );

        glClientActiveTexture( GL_TEXTURE0 );
        glTexCoordPointer( 2, GL_FLOAT, 32, reinterpret_cast<const GLvoid *>( 16 ) );
        glClientActiveTexture( GL_TEXTURE1 );
        glTexCoordPointer( 2, GL_FLOAT, 32, reinterpret_cast<const GLvoid *>( 24 ) );

        return true;
    }
	//////////////////////////////////////////////////////////////////////////
	void OGLRenderSystem::endScene()
	{
		glDisableClientState( GL_TEXTURE_COORD_ARRAY );
		glDisableClientState( GL_COLOR_ARRAY ); 
		glDisableClientState( GL_VERTEX_ARRAY );
	}
	//////////////////////////////////////////////////////////////////////////
	void OGLRenderSystem::swapBuffers()
	{
		//glClearColor( 1.f, 0.f, 0.f, 0.f );
		//glClear( GL_COLOR_BUFFER_BIT );
		//m_windowContext->swapBuffers();
	}
	//////////////////////////////////////////////////////////////////////////
	void OGLRenderSystem::clearFrameBuffer( uint32 _frameBufferTypes, uint32 _color, float _depth, uint16 _stencil )
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

#	if WIN32
			glClearDepth( _depth );
#	elif TARGET_OS_MAC && !TARGET_OS_IPHONE
            glClearDepth( _depth );
#	else
			glClearDepthf( _depth );
#	endif

		}
		if( ( _frameBufferTypes & FBT_STENCIL ) != 0 )
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
	void OGLRenderSystem::setViewport( const Viewport & _viewport )
	{
        float w = _viewport.getWidth();
        float h = _viewport.getHeight();
        
        //glViewport( (int)_viewport.begin.x, m_winContextHeight - (int)_viewport.begin.y - h, w, h );
#if WIN32
        glViewport( _viewport.begin.x, _viewport.begin.y, w, h );
#elif TARGET_OS_MAC && !TARGET_OS_IPHONE
        glViewport( _viewport.begin.x, _viewport.begin.y, w, h );
#else
        float bx = 768.f - (_viewport.begin.y + h);
        float by = 1024.f - (_viewport.begin.x + w);
        
        glViewport( bx, by, h, w );
#endif
        //glScissor( (int)_viewport.begin.x, m_winContextHeight - (int)_viewport.begin.y - h, w, h );
        
        glMatrixMode( GL_PROJECTION );
        glLoadIdentity();
        //glOrthof( _viewport.begin.x, _viewport.end.x, _viewport.begin.y, _viewport.end.y, -9999., 9999. );
        
# if WIN32
        //glScalef( 1.f, -1.f, 1.f );
        glOrtho( _viewport.begin.x, _viewport.end.x, _viewport.begin.y, _viewport.end.y, -9999.f, 9999.f );
# elif TARGET_OS_MAC && !TARGET_OS_IPHONE
        glScalef( 1.f, -1.f, 1.f );
        glOrtho( _viewport.begin.x, _viewport.end.x, _viewport.begin.y, _viewport.end.y, -9999.f, 9999.f );
# else
        glScalef( 1.f, -1.f, 1.f );
        
        //glOrthof( _viewport.begin.y, _viewport.end.y, _viewport.begin.x, _viewport.end.x, -9999.f, 9999.f );
        
        //
        //
        glOrthof( bx, bx + h, by, by + w, -9999.f, 9999.f );
        glRotatef( 90.f, 0.f, 0.f, 1.f );
        glTranslatef( 0.f, -768.f, 0.f );
# endif
        
        glMatrixMode( GL_MODELVIEW );
        glLoadIdentity();
    
    }
	//////////////////////////////////////////////////////////////////////////
	void OGLRenderSystem::changeWindowMode( const Resolution & _resolution, bool _fullscreen )
	{/*
		//m_windowContext->setFullscreenMode( _resolution.getWidth(), _resolution.getHeight(), _fullscreen );
		//glViewport( 0, 0, _resolution.getWidth(), _resolution.getHeight() );
		glViewport( 0, 0, _resolution.getHeight(), _resolution.getWidth() );
		//glScissor( 0, 0, _resolution.getWidth(), _resolution.getHeight() );
		m_winWidth = _resolution.getWidth();
		m_winHeight = _resolution.getHeight();
		m_winContextWidth = _resolution.getWidth();
		m_winContextHeight = _resolution.getHeight();*/

	}
	//////////////////////////////////////////////////////////////////////////
	void OGLRenderSystem::setRenderTarget( RenderImageInterface* _renderTarget, bool _clear )
	{
		OGLTexture* oglTexture = static_cast<OGLTexture*>( _renderTarget );
		if( m_activeRenderTarget != 0 )
		{
			GLuint uid = m_activeRenderTarget->getUId();
			glBindTexture( GL_TEXTURE_2D, uid );

			size_t hwWidth = m_activeRenderTarget->getHWWidth();
			size_t hwHeight = m_activeRenderTarget->getHWHeight();
			//size_t requestedWidth = m_activeRenderTarget->getRequestedWidth();
			//size_t requestedHeight = m_activeRenderTarget->getRequestedHeight();

			GLint x = 0;
            GLint y = 0;
			//GLint y = m_winContextHeight - m_winHeight + requestedHeight - hwHeight;

			PixelFormat hwFormat = m_activeRenderTarget->getHWPixelFormat();
			GLint internalFormat = s_toGLInternalFormat(hwFormat);

			glCopyTexImage2D( GL_TEXTURE_2D, 0, internalFormat, x, y,	hwWidth, hwHeight, 0 );
			glBindTexture( GL_TEXTURE_2D, m_activeTexture );
		}

		m_activeRenderTarget = oglTexture;

		//if( m_activeRenderTarget != NULL )
		//{
		//	m_winWidth = m_activeRenderTarget->getRequestedWidth();
		//	m_winHeight = m_activeRenderTarget->getRequestedHeight();
		//}
		//else
		//{
			m_winWidth = m_winContextWidth;
			m_winHeight = m_winContextHeight;
		//}
		clearFrameBuffer( FBT_COLOR | FBT_DEPTH );
	}

	bool OGLRenderSystem::supportTextureFormat( PixelFormat _format ) const
	{
		return s_toGLInternalFormat( _format ) != 0;
	}
	//////////////////////////////////////////////////////////////////////////
	void OGLRenderSystem::onWindowMovedOrResized()
	{

	}
	//////////////////////////////////////////////////////////////////////////
	void OGLRenderSystem::onWindowClose()
	{

	}
	//////////////////////////////////////////////////////////////////////////
	void OGLRenderSystem::setVSync( bool _vSync )
	{
		//m_windowContext->setVSync( _vSync );
	}
    //////////////////////////////////////////////////////////////////////////
    void OGLRenderSystem::clear( uint32 _color )
    {
    
    }
    //////////////////////////////////////////////////////////////////////////
    RenderImageInterface * OGLRenderSystem::createRenderTargetImage( size_t _width, size_t _height, size_t _channels, PixelFormat _format )
    {
        return NULL;
    }
	//////////////////////////////////////////////////////////////////////////
	RenderImageInterface * OGLRenderSystem::createDynamicImage( size_t _width, size_t _height, size_t _channels, PixelFormat _format )
	{
		return NULL;
	}
	//////////////////////////////////////////////////////////////////////////
    void OGLRenderSystem::makeProjectionOrthogonal( mt::mat4f & _projectionMatrix, const Viewport & _viewport, float _near, float _far )
	{
        mt::mat4f scale;
        mt::make_scale_m4( scale, 1.0f, -1.0f, 1.0f );

        //D3DXMatrixTranslation(&tmp, -0.5f, +0.5f, 0.0f);
        mt::mat4f translation;
        mt::make_translation_m4( translation, -0.5f, -0.5f, 0.0f );

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
	void OGLRenderSystem::makeProjectionPerspective( mt::mat4f & _projectionMatrix, float _fov, float _aspect, float _zn, float _zf )
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
	void OGLRenderSystem::makeProjectionFrustum( mt::mat4f & _projectionMatrix, const Viewport & _viewport, float _near, float _far )
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
    void OGLRenderSystem::makeViewMatrixFromViewport( mt::mat4f & _viewMatrix, const Viewport & _viewport )
    {
        mt::ident_m4( _viewMatrix );
    }
}	// namespace Menge
