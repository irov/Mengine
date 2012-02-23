
#	include "OGLRenderSystem.h"

#	include <cmath>

#	include "Interface/LogSystemInterface.h"
#	include "OGLTexture.h"
//#	include "OGLWindowContext.h"

#	define GET_A_FLOAT_FROM_ARGB32( argb ) ( ((float)(argb >> 24)) / 255.0f )
#	define GET_R_FLOAT_FROM_ARGB32( argb ) ( ((float)((argb >> 16) & 0xFF)) / 255.0f )
#	define GET_G_FLOAT_FROM_ARGB32( argb ) ( ((float)((argb >> 8) & 0xFF)) / 255.0f )
#	define GET_B_FLOAT_FROM_ARGB32( argb ) ( (float)(argb & 0xFF) / 255.0f )

//////////////////////////////////////////////////////////////////////////
bool initInterfaceSystem( Menge::RenderSystemInterface ** _ptrInterface )
{
	try
	{
		*_ptrInterface = new Menge::OGLRenderSystem();
	}
	catch (...)
	{
		return false;
	}

	return true;
}
//////////////////////////////////////////////////////////////////////////
void releaseInterfaceSystem( Menge::RenderSystemInterface* _ptrInterface )
{
	delete static_cast<Menge::OGLRenderSystem*>(_ptrInterface);
}
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
	static GLenum s_toGLMinMipFilter( ETextureFilter _minFilter, ETextureFilter _mipFilter )
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
			}
			break;
		}
		return GL_NEAREST;
	}
	//////////////////////////////////////////////////////////////////////////
	static int s_toGLInternalFormat( PixelFormat _format )
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
			//return GL_ALPHA8;
			return GL_ALPHA;
		}
		return 0;
	}
	//////////////////////////////////////////////////////////////////////////
	static GLenum s_toGLColorFormat( PixelFormat _format )
	{
		switch( _format )
		{
		case Menge::PF_X8R8G8B8:
		case Menge::PF_A8R8G8B8:
			return GL_BGRA;
			//return GL_RGBA;
		case Menge::PF_A8:
			return GL_ALPHA;
		}
		return 0;
	}
	//////////////////////////////////////////////////////////////////////////
	static GLenum s_getGLColorDataType( PixelFormat _format )
	{
		switch( _format )
		{
		case Menge::PF_X8R8G8B8:
		case Menge::PF_A8R8G8B8:
			return GL_UNSIGNED_BYTE;
		case Menge::PF_A8:
			return GL_UNSIGNED_BYTE;
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
	static void s_toGLMatrix( GLfloat gl_matrix[16], const float* _matrix )
	{
		// transpose
		for( int i = 0; i < 16; ++i )
		{
			gl_matrix[i] = _matrix[(i%4)*4 + i/4];
		}
	}
	//////////////////////////////////////////////////////////////////////////
	OGLRenderSystem::OGLRenderSystem()
		: m_logSystem( NULL )
		//, m_windowContext( NULL )
		, m_supportNPOT( false )
		, m_currentVertexBuffer( 0 )
		, m_currentIndexBuffer( 0 )
		, m_srcBlendFactor( GL_ONE )
		, m_dstBlendFactor( GL_ZERO )
		, m_activeTextureStage( 0 )
		, m_activeTexture( 0 )
		, m_activeRenderTarget( NULL )
	{
	}
	//////////////////////////////////////////////////////////////////////////
	OGLRenderSystem::~OGLRenderSystem()
	{
		//releaseWindowContext( m_windowContext );
	}
	//////////////////////////////////////////////////////////////////////////
	bool OGLRenderSystem::initialize( LogSystemInterface* _logSystem, RenderSystemListener* _listener )
	{
		m_logSystem = _logSystem;

		LOGGER_INFO(m_logSystem)( "Initializing OpenGL RenderSystem..." );

		return true;
	}
	//////////////////////////////////////////////////////////////////////////
	bool OGLRenderSystem::createRenderWindow( std::size_t _width, std::size_t _height, int _bits,
												bool _fullscreen, WindowHandle _winHandle, 
												bool _waitForVSync, int _FSAAType, int _FSAAQuality )
	{
#if 0
		createWindowContext( &m_windowContext );
		if( m_windowContext == NULL )
		{
			LOGGER_ERROR(m_logSystem)( "Error: failed to create platform window context" );
			return false;
		}

		m_winWidth = _width;
		m_winHeight = _height;
		if( m_windowContext->initialize( _width, _height, _bits, _fullscreen, _winHandle, _waitForVSync ) == false )
		{
			LOGGER_ERROR(m_logSystem)( "Error: failed to initialize window context" );
			return false;
		}
		m_windowContext->setFullscreenMode( _width, _height, _fullscreen );
		m_winContextWidth = _width;
		m_winContextHeight = _height;

		const char* str = (const char*)glGetString( GL_VERSION );
		String mstr = "None";
		if( str != NULL )
		{
			mstr = str;
		}
		LOGGER_INFO(m_logSystem)( "OpenGL Version: %s", mstr.c_str() );
		mstr = "None";
		str = (const char*)glGetString( GL_VENDOR );
		if( str != NULL )
		{
			mstr = str;
		}
		LOGGER_INFO(m_logSystem)( "Vendor: %s", mstr.c_str() );
		str = (const char*)glGetString( GL_RENDERER );
		//LOG( "Renderer: " + Menge::String( str ) );
		m_ext = (const char*)glGetString( GL_EXTENSIONS );
		//LOG( "Extensions:" );
		//LOG( m_ext );
		// check for NPOT
		String extSubStr = "texture_non_power_of_two";
		String::size_type pos =  m_ext.find( extSubStr );
		/*if( pos != String::npos && ( m_ext[pos+extSubStr.size()] == '\0' || m_ext[pos+extSubStr.size()] == ' ' ) )	// it seems to be supported
		{
			// try to create NPOT to be sure
			GLuint npotTex;
			GLint size = 65;
			glGenTextures( 1, &npotTex );
			glBindTexture( GL_TEXTURE_2D, npotTex );
			//glTexParameteri( GL_TEXTURE_2D, GL_TEXTURE_MAX_LEVEL, 0 );
			glTexParameteri( GL_TEXTURE_2D, GL_TEXTURE_WRAP_S, GL_CLAMP_TO_EDGE);
			glTexParameteri( GL_TEXTURE_2D, GL_TEXTURE_WRAP_T, GL_CLAMP_TO_EDGE);
			glTexParameteri( GL_TEXTURE_2D, GL_TEXTURE_MAG_FILTER, GL_LINEAR);
			glTexParameteri( GL_TEXTURE_2D, GL_TEXTURE_MIN_FILTER, GL_LINEAR);
			glTexImage2D( GL_PROXY_TEXTURE_2D, 0, GL_RGBA8, size, size, 0, GL_BGRA, GL_UNSIGNED_BYTE, NULL );
			GLint width = 0;
			glGetTexLevelParameteriv( GL_PROXY_TEXTURE_2D, 0, GL_TEXTURE_WIDTH, &width );
			if( width == size )
			{
				m_supportNPOT = true;
			}
			glBindTexture( GL_TEXTURE_2D, 0 );
			glDeleteTextures( 1, &npotTex );
		}*/

		extSubStr = "pixel_buffer_object";
		pos = m_ext.find( extSubStr );
		if( pos != String::npos && ( m_ext[pos+extSubStr.size()] == '\0' || m_ext[pos+extSubStr.size()] == ' ' ) )	// it seems to be supported
		{
			LOGGER_INFO(m_logSystem)( "Supports PBO" );
		}
		
#endif

		for( int i = 0; i < MENGE_MAX_TEXTURE_STAGES; ++i )
		{
			glActiveTexture( GL_TEXTURE0 + i );
			glTexEnvi( GL_TEXTURE_ENV, GL_TEXTURE_ENV_MODE, GL_COMBINE );
			glTexEnvi( GL_TEXTURE_ENV, GL_OPERAND0_RGB, GL_SRC_COLOR );
			glTexEnvi( GL_TEXTURE_ENV, GL_OPERAND1_RGB, GL_SRC_COLOR );
			glTexEnvi( GL_TEXTURE_ENV, GL_OPERAND0_ALPHA, GL_SRC_ALPHA );
			glTexEnvi( GL_TEXTURE_ENV, GL_OPERAND1_ALPHA, GL_SRC_ALPHA );
			m_textureStage[i].enabled = false;
			glDisable( GL_TEXTURE_2D );
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
	void OGLRenderSystem::getResolutions( TVectorResolutions & _resolutions )
	{

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
		/*if( _image == NULL )
		{
			LOGGER_ERROR(m_logSystem)( "Warning: _image == NULL in OGLRenderSystem::screenshot" );
			return;
		}
		int srcX = 0;
		int srcY = 0;
		int srcWidth = m_winWidth;
		int srcHeight = m_winHeight;
		if( _rect != NULL )
		{
			srcX = (std::max<int>)( 0, _rect[0] );
			srcY = (std::max<int>)( 0, _rect[1] );
			srcWidth = std::min<int>( m_winWidth, _rect[2] ) - srcX;
			srcHeight = std::min<int>( m_winHeight, _rect[3] ) - srcY;
		}

		int dstX = 0;
		int dstY = 0;
		int dstWidth = srcWidth;
		int dstHeight = srcHeight;

		GLint iWidth;
		GLint iHeight;
		OGLTexture* oglTexture = static_cast<OGLTexture*>( _image );
		glBindTexture( GL_TEXTURE_2D, oglTexture->uid );
		glGetTexLevelParameteriv( GL_TEXTURE_2D, 0, GL_TEXTURE_WIDTH, &iWidth );
		glGetTexLevelParameteriv( GL_TEXTURE_2D, 0, GL_TEXTURE_HEIGHT, &iHeight );
		glBindTexture( GL_TEXTURE_2D, m_activeTexture );

		if( dstWidth > iWidth )
		{
			dstWidth = iWidth;
		}
		if( dstHeight > iHeight )
		{
			dstHeight = iHeight;
		}

		int pitch = 0;
		unsigned char* imageData = _image->lock( &pitch, false );
		glReadBuffer( GL_BACK );

		int bufDataPitch = srcWidth * 4;
		unsigned char* bufferData = new unsigned char[bufDataPitch*srcHeight];
		glPixelStorei( GL_PACK_ALIGNMENT, 1 );
		glReadPixels( srcX, m_winHeight - srcY - srcHeight, srcWidth, srcHeight, GL_BGRA, GL_UNSIGNED_BYTE, bufferData );	// why is our buffer flipped by y????
		glPixelStorei( GL_PACK_ALIGNMENT, 4 );
		if( srcWidth == dstWidth && srcHeight == dstHeight )
		{
			unsigned char* buf = bufferData + bufDataPitch * ( srcHeight - 1 );
			for( std::size_t i = 0; i < srcHeight; i++ )
			{
				std::copy( buf, buf + bufDataPitch, imageData );
				imageData += pitch;
				buf -= bufDataPitch; // flip by y
			}
		}
		else	// resample image
		{

			// srcdata stays at beginning of slice, pdst is a moving pointer
			unsigned char* srcdata = bufferData;
			unsigned char* pdst = imageData + ( dstHeight - 1 ) * pitch;	// flip by y

			// sx_48,sy_48 represent current position in source
			// using 16/48-bit fixed precision, incremented by steps
			Menge::uint64 stepx = ((Menge::uint64)srcWidth << 48) / dstWidth;
			Menge::uint64 stepy = ((Menge::uint64)srcHeight << 48) / dstHeight;

			// bottom 28 bits of temp are 16/12 bit fixed precision, used to
			// adjust a source coordinate backwards by half a pixel so that the
			// integer bits represent the first sample (eg, sx1) and the
			// fractional bits are the blend weight of the second sample
			unsigned int temp;

			Menge::uint64 sy_48 = (stepy >> 1) - 1;
			for (size_t y = 0; y < iHeight; y++, sy_48 += stepy) 
			{
				temp = sy_48 >> 36;
				temp = (temp > 0x800)? temp - 0x800: 0;
				unsigned int syf = temp & 0xFFF;
				size_t sy1 = temp >> 12;
				size_t sy2 = (std::min<size_t>)( sy1 + 1, srcHeight - 1 );
				size_t syoff1 = sy1 * srcWidth;//bufDataPitch;
				size_t syoff2 = sy2 * srcWidth;//bufDataPitch;

				Menge::uint64 sx_48 = (stepx >> 1) - 1;
				for (size_t x = 0; x < dstWidth; x++, sx_48+=stepx) 
				{
					temp = sx_48 >> 36;
					temp = (temp > 0x800)? temp - 0x800 : 0;
					unsigned int sxf = temp & 0xFFF;
					size_t sx1 = temp >> 12;
					size_t sx2 = (std::min<size_t>)(sx1+1, srcWidth-1);

					unsigned int sxfsyf = sxf*syf;
					for (unsigned int k = 0; k < 4; k++) 
					{
						unsigned int accum =
							srcdata[(sx1 + syoff1)*4+k]*(0x1000000-(sxf<<12)-(syf<<12)+sxfsyf) +
							srcdata[(sx2 + syoff1)*4+k]*((sxf<<12)-sxfsyf) +
							srcdata[(sx1 + syoff2)*4+k]*((syf<<12)-sxfsyf) +
							srcdata[(sx2 + syoff2)*4+k]*sxfsyf;
						// accum is computed using 8/24-bit fixed-point math
						// (maximum is 0xFF000000; rounding will not cause overflow)
						*pdst++ = (accum + 0x800000) >> 24;
					}
				}
				pdst -= pitch * 2;
				//pdst += 4*dst.getRowSkip();
			}

		}
		delete[] bufferData;
		_image->unlock();*/
	}
	//////////////////////////////////////////////////////////////////////////
	void OGLRenderSystem::setProjectionMatrix( const mt::mat4f & _projection )
	{
		/*GLfloat mat[16];
		s_toGLMatrix( mat, ( float * )&_projection );

		glMatrixMode( GL_PROJECTION );

		glLoadMatrixf( mat );

		glMatrixMode( GL_MODELVIEW );*/
	}
	//////////////////////////////////////////////////////////////////////////
	void OGLRenderSystem::setModelViewMatrix( const mt::mat4f & _view )
	{
		/*GLfloat mat[16];
		s_toGLMatrix( mat, ( float * )&_view );
		glLoadMatrixf( mat );*/
	}
	//////////////////////////////////////////////////////////////////////////
	void OGLRenderSystem::setTextureMatrix( size_t _stage, const float* _texture )
	{
		if( m_activeTextureStage != _stage )
		{
			glActiveTexture( GL_TEXTURE0  + _stage );
			m_activeTextureStage = _stage;
		}

		glMatrixMode( GL_TEXTURE );
		if( _texture != NULL )
		{
			//GLfloat mat[16];
			//s_toGLMatrix( mat, _texture );
			//glMultMatrixf( mat );
			glTranslatef( _texture[8], _texture[9], 0.0f );
			glScalef( _texture[0], _texture[5], _texture[10] );
		}
		/*else
		{
			glLoadIdentity();
		}*/
		glMatrixMode( GL_MODELVIEW );
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

		/*if( !GLEE_EXT_vertex_array_bgra )
		{
			// swap r/b
			unsigned char* pVBufferMem = static_cast<unsigned char*>( memRange.pMem );
			unsigned char* pVBufferMemEnd = static_cast<unsigned char*>( memRange.pMem ) + memRange.size;
			while( pVBufferMem < pVBufferMemEnd )
			{
				unsigned char temp = *(pVBufferMem + 12);		// r
				*(pVBufferMem + 12) = *(pVBufferMem + 12 + 2);	// g
				*(pVBufferMem + 12 + 2) = temp;
				pVBufferMem += 24;
			}
		}*/

		glBufferSubData( GL_ARRAY_BUFFER, memRange.offset, memRange.size, memRange.pMem + memRange.offset );
		/*GLenum error = glGetError();
		if( error != GL_NO_ERROR )
			error = error;*/
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
		/*GLuint bufId = 0;
		glGenBuffers( 1, &bufId );
		glBindBuffer( GL_ELEMENT_ARRAY_BUFFER, bufId );
		glBufferData( GL_ELEMENT_ARRAY_BUFFER, _indiciesNum * sizeof( uint16 ), NULL, GL_STATIC_DRAW );
		glBindBuffer( GL_ELEMENT_ARRAY_BUFFER, m_currentIndexBuffer );
		return static_cast<IBHandle>( bufId );*/
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
		/*GLuint bufId = static_cast<GLuint>( _ibHandle );
		if( bufId == m_currentIndexBuffer )
		{
			m_currentIndexBuffer = 0;
		}
		glDeleteBuffers( 1, &bufId );*/
	}
	//////////////////////////////////////////////////////////////////////////
	uint16* OGLRenderSystem::lockIndexBuffer( IBHandle _ibHandle )
	{
		/*GLuint bufId = static_cast<GLuint>( _ibHandle );
		glBindBuffer( GL_ELEMENT_ARRAY_BUFFER, bufId );
		void* pMem = glMapBuffer( GL_ELEMENT_ARRAY_BUFFER, GL_WRITE_ONLY );
		glBindBuffer( GL_ELEMENT_ARRAY_BUFFER, m_currentIndexBuffer );
		return static_cast<uint16*>( pMem );*/
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
		/*GLuint bufId = static_cast<GLuint>( _ibHandle );
		glBindBuffer( GL_ELEMENT_ARRAY_BUFFER, bufId );
		GLboolean result = glUnmapBuffer( GL_ELEMENT_ARRAY_BUFFER );
		glBindBuffer( GL_ELEMENT_ARRAY_BUFFER, m_currentIndexBuffer );
		return result == GL_TRUE;*/
	}
	//////////////////////////////////////////////////////////////////////////
	void OGLRenderSystem::setIndexBuffer( IBHandle _ibHandle, size_t _baseVertexIndex )
	{
		GLuint bufId = static_cast<GLuint>( _ibHandle );
		glBindBuffer( GL_ELEMENT_ARRAY_BUFFER, bufId );
		m_currentIndexBuffer = bufId;

		if( _baseVertexIndex != 0 )
			_baseVertexIndex = _baseVertexIndex;
		//m_baseVertexIndex = _baseVertexIndex;
	}
	//////////////////////////////////////////////////////////////////////////
	void OGLRenderSystem::setVertexDeclaration( size_t _vertexSize, uint32 _declaration )
	{

	}
	//////////////////////////////////////////////////////////////////////////
	void OGLRenderSystem::drawIndexedPrimitive( EPrimitiveType _type, 
		std::size_t _baseVertexIndex,  std::size_t _minIndex, 
		std::size_t _verticesNum, std::size_t _startIndex, std::size_t _indexCount )
	{
		GLenum mode = s_toGLPrimitiveMode[ _type ];

		//return;

		/*{
			static int drawnum = 0;

			wchar_t out[ 256 ];
			wsprintf( out, L"drawnum = %i _baseVertexIndex = %i _verticesNum = %i _startIndex = %i\n", drawnum, _baseVertexIndex, _verticesNum, _startIndex );
			OutputDebugString( out );

			drawnum++;
		}*/

		/*glDrawRangeElements( mode, _minIndex, _minIndex + _verticesNum - 1,
			_indexCount, GL_UNSIGNED_SHORT, reinterpret_cast<const GLvoid *>( _startIndex * sizeof( uint16 ) ) );*/
		glDrawElements( mode, _indexCount, GL_UNSIGNED_SHORT, reinterpret_cast<const GLvoid *>( _startIndex * sizeof( uint16 ) ) );
		
	}
	//////////////////////////////////////////////////////////////////////////
	void OGLRenderSystem::setTexture( std::size_t _stage, RenderImageInterface* _texture )
	{
		if( m_activeTextureStage != _stage )
		{
			glActiveTexture( GL_TEXTURE0  + _stage );
			m_activeTextureStage = _stage;
		}

		OGLTexture* texture = static_cast<OGLTexture*>( _texture );
		if( texture != NULL )
		{
			m_activeTexture = texture->uid;
			glBindTexture( GL_TEXTURE_2D, m_activeTexture );
			TextureStage& tStage = m_textureStage[_stage];
			if( texture->wrapS != tStage.wrapS )
			{
				texture->wrapS = tStage.wrapS;
				glTexParameteri( GL_TEXTURE_2D, GL_TEXTURE_WRAP_S, texture->wrapS );
			}
			if( texture->wrapT != tStage.wrapT )
			{
				texture->wrapT = tStage.wrapT;
				glTexParameteri( GL_TEXTURE_2D, GL_TEXTURE_WRAP_T, texture->wrapT );
			}
			if( texture->minFilter != tStage.minFilter )
			{
				texture->minFilter = tStage.minFilter;
				glTexParameteri( GL_TEXTURE_2D, GL_TEXTURE_MIN_FILTER, texture->minFilter );
			}
			if( texture->magFilter != tStage.magFilter )
			{
				texture->magFilter = tStage.magFilter;
				glTexParameteri( GL_TEXTURE_2D, GL_TEXTURE_MAG_FILTER, texture->magFilter );
			}
			glMatrixMode( GL_TEXTURE );
			glLoadIdentity();
			if( texture->m_isRenderTarget == true )
			{
				glScalef( 1.0f, -1.0f, 1.0f );
				glTranslatef( 0.0f, -1.0f, 0.0f );
			}
			glMatrixMode( GL_MODELVIEW );

		}
		else
		{
			m_activeTexture = 0;
			glBindTexture( GL_TEXTURE_2D, m_activeTexture );
		}
	}
	//////////////////////////////////////////////////////////////////////////
	void OGLRenderSystem::setTextureAddressing( std::size_t _stage, ETextureAddressMode _modeU, ETextureAddressMode _modeV )
	{
		if( m_activeTextureStage != _stage )
		{
			glActiveTexture( GL_TEXTURE0  + _stage );
			m_activeTextureStage = _stage;
		}

		GLenum modeUGL = s_toGLAddressMode[ _modeU ];
		GLenum modeVGL = s_toGLAddressMode[ _modeV ];
		if( m_textureStage[_stage].wrapS !=  modeUGL )
		{
			m_textureStage[_stage].wrapS = modeUGL;
			glTexParameteri( GL_TEXTURE_2D, GL_TEXTURE_WRAP_S, modeUGL );
		}
		if( m_textureStage[_stage].wrapT != modeVGL )
		{
			m_textureStage[_stage].wrapT = modeVGL;
			glTexParameteri( GL_TEXTURE_2D, GL_TEXTURE_WRAP_T, modeVGL );
		}
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
		enable ? glEnable( GL_CULL_FACE ) : glDisable( GL_CULL_FACE );
	}
	//////////////////////////////////////////////////////////////////////////
	void OGLRenderSystem::setDepthBufferTestEnable( bool _depthTest )
	{
		_depthTest ? glEnable( GL_DEPTH_TEST ) : glDisable( GL_DEPTH_TEST );
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
		_alphaTest ? glEnable( GL_ALPHA_TEST ) : glDisable( GL_ALPHA_TEST );
	}
	//////////////////////////////////////////////////////////////////////////
	void OGLRenderSystem::setAlphaBlendEnable( bool _alphaBlend )
	{
		_alphaBlend ? glEnable( GL_BLEND ) : glDisable( GL_BLEND );
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
		_light ? glEnable( GL_LIGHTING ) : glDisable( GL_LIGHTING );
	}
	//////////////////////////////////////////////////////////////////////////
	void OGLRenderSystem::setTextureStageColorOp( size_t _stage, ETextureOp _textrueOp,  ETextureArgument _arg1, ETextureArgument _arg2 )
	{
		if( m_activeTextureStage != _stage )
		{
			glActiveTexture( GL_TEXTURE0  + _stage );
			m_activeTextureStage = _stage;
		}

		if( _textrueOp == Menge::TOP_DISABLE )
		{
			m_textureStage[_stage].enabled = false;
			glDisable( GL_TEXTURE_2D );
			return;
		}

		if( m_textureStage[_stage].enabled == false )
		{
			m_textureStage[_stage].enabled = true;
			glEnable( GL_TEXTURE_2D );
		}

		GLenum textureOpGL = s_toGLTextureOp[ _textrueOp ];
		GLenum arg1GL = s_toGLTextureArg[ _arg1 ];
		GLenum arg2GL = s_toGLTextureArg[ _arg2 ];
		glTexEnvi( GL_TEXTURE_ENV, GL_COMBINE_RGB, textureOpGL );
		if( _textrueOp == Menge::TOP_SELECTARG2 )
		{
			arg1GL = arg2GL;
		}
		//glTexEnvi( GL_TEXTURE_ENV, GL_SOURCE0_RGB, arg1GL );
		//glTexEnvi( GL_TEXTURE_ENV, GL_SOURCE1_RGB, arg2GL );
	}
	//////////////////////////////////////////////////////////////////////////
	void OGLRenderSystem::setTextureStageAlphaOp( size_t _stage, ETextureOp _textrueOp,  ETextureArgument _arg1, ETextureArgument _arg2 )
	{
		if( m_activeTextureStage != _stage )
		{
			glActiveTexture( GL_TEXTURE0  + _stage );
			m_activeTextureStage = _stage;
		}

		GLenum textureOpGL = s_toGLTextureOp[ _textrueOp ];
		GLenum arg1GL = s_toGLTextureArg[ _arg1 ];
		GLenum arg2GL = s_toGLTextureArg[ _arg2 ];
		glTexEnvi( GL_TEXTURE_ENV, GL_COMBINE_ALPHA, textureOpGL );
		if( _textrueOp == Menge::TOP_SELECTARG2 )
		{
			arg1GL = arg2GL;
		}
		//glTexEnvi( GL_TEXTURE_ENV, GL_SOURCE0_ALPHA, arg1GL );
		//glTexEnvi( GL_TEXTURE_ENV, GL_SOURCE1_ALPHA, arg2GL );
	}
	//////////////////////////////////////////////////////////////////////////
	void OGLRenderSystem::setTextureStageFilter( size_t _stage, ETextureFilterType _filterType, ETextureFilter _filter )
	{
		if( m_activeTextureStage != _stage )
		{
			glActiveTexture( GL_TEXTURE0  + _stage );
			m_activeTextureStage = _stage;
		}

		TextureStage& tStage = m_textureStage[_stage];

		if( _filterType == TFT_MINIFICATION )
		{
			tStage.mengeMinFilter = _filter;
			GLenum filterGL = s_toGLMinMipFilter( tStage.mengeMinFilter, tStage.mengeMipFilter );
			if( tStage.minFilter != filterGL )
			{
				tStage.minFilter = filterGL;
				glTexParameteri( GL_TEXTURE_2D, GL_TEXTURE_MIN_FILTER, filterGL );
			}
		}
		else if( _filterType == TFT_MIPMAP )
		{
			tStage.mengeMipFilter = _filter;
			GLenum filterGL = s_toGLMinMipFilter( tStage.mengeMinFilter, tStage.mengeMipFilter );
			if( tStage.minFilter != filterGL )
			{
				tStage.minFilter = filterGL;
				glTexParameteri( GL_TEXTURE_2D, GL_TEXTURE_MIN_FILTER, filterGL );
			}
		}
		else if( _filterType == TFT_MAGNIFICATION )
		{
			GLenum filterGL = s_toGLFilter[ _filter ];
			if( tStage.magFilter != filterGL )
			{
				tStage.magFilter = filterGL;
				glTexParameteri( GL_TEXTURE_2D, GL_TEXTURE_MAG_FILTER, filterGL );
			}
		}
	}
	//////////////////////////////////////////////////////////////////////////
	RenderImageInterface* OGLRenderSystem::createImage( std::size_t _width, std::size_t _height, std::size_t & _realWidth, std::size_t & _realHeight, PixelFormat& _format )
	{
		if( _format == Menge::PF_R8G8B8 )
		{
			_format = Menge::PF_X8R8G8B8;
		}

		GLsizei requestedWidth = _width;
		GLsizei requestedHeight = _height;

		if( ( _width & ( _width - 1 ) ) != 0
			|| ( _height & ( _height - 1 ) ) != 0 )
		{
			bool npot = false;//m_supportNPOT;//();
			if( npot == false )	// we're all gonna die
			{
				_width = s_firstPO2From( _width );
				_height = s_firstPO2From( _height );
			}
		}

		_realWidth = _width;
		_realHeight = _height;

		int numColors = 4;
		if( _format == PF_A8 )
		{
			numColors = 1;
		}

		GLuint tuid = 0;
		glGenTextures( 1, &tuid );
		OGLTexture* texture = new OGLTexture( tuid, this );
		texture->m_isRenderTarget = false;
		texture->width = _width;
		texture->height = _height;
		texture->level = 0;
		texture->border = 0;
		texture->internalFormat = s_toGLInternalFormat( _format );
		texture->format = s_toGLColorFormat( _format );
		texture->type = s_getGLColorDataType( _format );
		texture->wrapS = GL_CLAMP_TO_EDGE;
		texture->wrapT = GL_CLAMP_TO_EDGE;
		texture->minFilter = GL_LINEAR;
		texture->magFilter = GL_LINEAR;
		texture->m_lock = new unsigned char[_width*_height*numColors];
		texture->pitch = _width * numColors;
		texture->requestedWidth = requestedWidth;
		texture->requestedHeight = requestedHeight;

		glBindTexture( GL_TEXTURE_2D, tuid );
		glTexParameteri( GL_TEXTURE_2D, GL_TEXTURE_WRAP_S, texture->wrapS );
		glTexParameteri( GL_TEXTURE_2D, GL_TEXTURE_WRAP_T, texture->wrapT );
		glTexParameteri( GL_TEXTURE_2D, GL_TEXTURE_MAG_FILTER, texture->minFilter );
		glTexParameteri( GL_TEXTURE_2D, GL_TEXTURE_MIN_FILTER, texture->magFilter );
		//glTexParameteri( GL_TEXTURE_2D, GL_TEXTURE_MAX_LEVEL, 0 );
		glTexParameteri( GL_TEXTURE_2D, GL_GENERATE_MIPMAP, GL_FALSE );
		glTexImage2D( GL_TEXTURE_2D, texture->level, texture->internalFormat,
						texture->width, texture->height, texture->border, texture->format,
						texture->type, NULL );
		glBindTexture( GL_TEXTURE_2D, m_activeTexture );

		return texture;
	}
	//////////////////////////////////////////////////////////////////////////
	/*RenderImageInterface* OGLRenderSystem::createRenderTargetImage( std::size_t& _width, std::size_t& _height )
	{
		PixelFormat format = PF_A8R8G8B8;
		RenderImageInterface* image = createImage( _width, _height, format );
		OGLTexture* oglTexture = static_cast<OGLTexture*>( image );
		if( oglTexture != NULL )
		{
			oglTexture->m_isRenderTarget = true;
		}
		return image;
	}*/
	//////////////////////////////////////////////////////////////////////////
	void OGLRenderSystem::releaseImage( RenderImageInterface * _image )
	{
		OGLTexture* texture = static_cast<OGLTexture*>( _image );
		if( m_activeTexture == texture->uid )
		{
			m_activeTexture = 0;
		}
		if( texture != NULL )
		{
			delete[] texture->m_lock;
		}
		glDeleteTextures( 1, &texture->uid );
		delete texture;
	}
	//////////////////////////////////////////////////////////////////////////
	bool OGLRenderSystem::beginScene()
	{
		glEnableClientState( GL_VERTEX_ARRAY );
		glEnableClientState( GL_COLOR_ARRAY );
		glEnableClientState( GL_TEXTURE_COORD_ARRAY );  

		GLint colorSize = 4;
		/*if( GLEE_EXT_vertex_array_bgra )
		{
			colorSize = GL_BGRA;
		}*/

		/*glVertexPointer( 3, GL_FLOAT, 24,  0 );
		glColorPointer( colorSize, GL_UNSIGNED_BYTE, 24,  reinterpret_cast<const GLvoid *>( 12 ) );

		glClientActiveTexture( GL_TEXTURE0 );
		glTexCoordPointer( 2, GL_FLOAT, 24, reinterpret_cast<const GLvoid *>( 16 ) );
		glClientActiveTexture( GL_TEXTURE1 );
		glTexCoordPointer( 2, GL_FLOAT, 24, reinterpret_cast<const GLvoid *>( 16 ) );*/

		glVertexPointer( 4, GL_FLOAT, 28,  0 );
		glColorPointer( colorSize, GL_UNSIGNED_BYTE, 28,  reinterpret_cast<const GLvoid *>( 16 ) );

		glClientActiveTexture( GL_TEXTURE0 );
		glTexCoordPointer( 2, GL_FLOAT, 28, reinterpret_cast<const GLvoid *>( 20 ) );
		glClientActiveTexture( GL_TEXTURE1 );
		glTexCoordPointer( 2, GL_FLOAT, 28, reinterpret_cast<const GLvoid *>( 20 ) );

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
	void OGLRenderSystem::clearFrameBuffer( uint32 _frameBufferTypes,
											uint32 _color /*= 0  */,
											float _depth /*= 1.0f  */,
											uint16 _stencil /*= 0 */ )
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
#if TARGET_OS_MAC && !TARGET_OS_IPHONE
            glClearDepth( _depth );
#else
			glClearDepthf( _depth );
#endif
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
	void OGLRenderSystem::beginLayer2D()
	{
	}
	//////////////////////////////////////////////////////////////////////////
	void OGLRenderSystem::endLayer2D()
	{
		return;
		glDisable( GL_TEXTURE_2D );

		uint16 * ind = lockIndexBuffer( 1 );
		ind[ 0 ] = 0;
		ind[ 1 ] = 1;
		ind[ 2 ] = 3;
		ind[ 3 ] = 1;
		ind[ 4 ] = 2;
		ind[ 5 ] = 3;
		unlockIndexBuffer( 1 );

		struct Vertex2D
		{
			float pos[4];
			uint32 color;
			float uv[2];
		};

		Vertex2D * v = ( Vertex2D * )lockVertexBuffer( 2, 0, sizeof( Vertex2D ) * 4, 0 );
		if( v )
		{
			v[ 0 ].pos[ 0 ] = 0;
			v[ 0 ].pos[ 1 ] = 0;
			v[ 0 ].pos[ 2 ] = 0;
			v[ 0 ].pos[ 3 ] = 1;

			v[ 0 ].color = 0xFF0000FF;

			v[ 1 ].pos[ 0 ] = 768;
			v[ 1 ].pos[ 1 ] = 0;
			v[ 1 ].pos[ 2 ] = 0;
			v[ 1 ].pos[ 3 ] = 1;

			v[ 1 ].color = 0xFF00FF00;

			v[ 2 ].pos[ 0 ] = 768;
			v[ 2 ].pos[ 1 ] = 1024;
			v[ 2 ].pos[ 2 ] = 0;
			v[ 2 ].pos[ 3 ] = 1;

			v[ 2 ].color = 0xFFFF0000;

			v[ 3 ].pos[ 0 ] = 0;
			v[ 3 ].pos[ 1 ] = 1024;
			v[ 3 ].pos[ 2 ] = 0;
			v[ 3 ].pos[ 3 ] = 1;

			v[ 3 ].color = 0xFFFFFFFF;

			unlockVertexBuffer( 2 );

			setIndexBuffer( 1, 0 );
			setVertexBuffer( 2 );
			
			glVertexPointer( 4, GL_FLOAT, 28,  0 );
			glColorPointer( 4, GL_UNSIGNED_BYTE, 28,  reinterpret_cast<const GLvoid *>( 16 ) );
			
			glClientActiveTexture( GL_TEXTURE0 );
			glTexCoordPointer( 2, GL_FLOAT, 28, reinterpret_cast<const GLvoid *>( 20 ) );
			glClientActiveTexture( GL_TEXTURE1 );
			glTexCoordPointer( 2, GL_FLOAT, 28, reinterpret_cast<const GLvoid *>( 20 ) );
			
			glViewport( 0, 0, 768, 1024 );
			glMatrixMode( GL_PROJECTION );
			glLoadIdentity();
#if TARGET_OS_MAC && !TARGET_OS_IPHONE
            glOrtho( 0, 768, 0, 1024, -9999., 9999. );
#else
			glOrthof( 0, 768, 0, 1024, -9999., 9999. );
#endif
			glMatrixMode( GL_MODELVIEW );
			glLoadIdentity();

			glDrawElements( GL_TRIANGLES, 6, GL_UNSIGNED_SHORT, 0 );
		}

		/*glDisable( GL_TEXTURE_2D );

		glBegin( GL_TRIANGLE_FAN );
		glColor3f( 1, 0, 0 );
		glVertex2f( 0, 0 );

		glColor3f( 0, 1, 0 );
		glVertex2f( 100, 0 );

		glColor3f( 0, 0, 1 );
		glVertex2f( 100, 100 );

		glColor3f( 1, 1, 1 );
		glVertex2f( 0, 100 );
		glEnd();*/
	}
	//////////////////////////////////////////////////////////////////////////
	void OGLRenderSystem::beginLayer3D()
	{

	}
	//////////////////////////////////////////////////////////////////////////
	void OGLRenderSystem::endLayer3D()
	{

	}
	//////////////////////////////////////////////////////////////////////////
	void OGLRenderSystem::setRenderViewport( const Viewport & _viewport )
	{
		//float dx = static_cast<float>( m_winContextWidth ) / m_winWidth;
		//float dy = static_cast<float>( m_winContextHeight ) / m_winHeight;
		float w = _viewport.getWidth() + 0.5f;
		float h = _viewport.getHeight() + 0.5f;


		//glViewport( (int)_viewport.begin.x, m_winContextHeight - (int)_viewport.begin.y - h, w, h );
#if TARGET_OS_MAC && !TARGET_OS_IPHONE
        glViewport( _viewport.begin.x, _viewport.begin.y, w, h );		
#else
        glViewport( 768 - (_viewport.begin.y + h), 1024 - (_viewport.begin.x + w), h, w );
#endif
		//glScissor( (int)_viewport.begin.x, m_winContextHeight - (int)_viewport.begin.y - h, w, h );

		glMatrixMode( GL_PROJECTION );
		glLoadIdentity();
		//glOrthof( _viewport.begin.x, _viewport.begin.x + w, _viewport.begin.y, _viewport.begin.y + h, -9999., 9999. );
#if TARGET_OS_MAC && !TARGET_OS_IPHONE
        glScalef( 1.f, -1.f, 1.f );
		glOrtho( _viewport.begin.x - 0.5f, _viewport.end.x - 0.5f, _viewport.begin.y - 0.5f, _viewport.end.y - 0.5f, -9999., 9999. );
#else
        glScalef( 1.f, -1.f, 1.f );
        glOrthof( _viewport.begin.y - 0.5f, _viewport.end.y - 0.5f, _viewport.begin.x - 0.5f, _viewport.end.x - 0.5f, -9999., 9999. );
        //glOrthof( 0.f, h, 0.f, w, -9999., 9999. );
		glRotatef( 90.f, 0.f, 0.f, 1.f );
        
		glTranslatef( 0.f, -768, 0.f );
#endif
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
			glBindTexture( GL_TEXTURE_2D, m_activeRenderTarget->uid );
			glCopyTexImage2D( GL_TEXTURE_2D, 0, m_activeRenderTarget->internalFormat, 0, m_winContextHeight-m_winHeight + m_activeRenderTarget->requestedHeight - m_activeRenderTarget->height, 
				m_activeRenderTarget->width, m_activeRenderTarget->height, 0 );
			glBindTexture( GL_TEXTURE_2D, m_activeTexture );
		}

		m_activeRenderTarget = oglTexture;

		if( m_activeRenderTarget != NULL )
		{
			m_winWidth = m_activeRenderTarget->requestedWidth;
			m_winHeight = m_activeRenderTarget->requestedHeight;
		}
		else
		{
			m_winWidth = m_winContextWidth;
			m_winHeight = m_winContextHeight;
		}
		clearFrameBuffer( FBT_COLOR | FBT_DEPTH );
	}

	bool OGLRenderSystem::supportTextureFormat( PixelFormat _format )
	{
		return s_toGLInternalFormat( _format );
	}

	//////////////////////////////////////////////////////////////////////////
	LightInterface * OGLRenderSystem::createLight( const String & _name )
	{
		return NULL;
	}
	//////////////////////////////////////////////////////////////////////////
	void OGLRenderSystem::releaseLight( LightInterface * _light )
	{
		//
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
	void OGLRenderSystem::unlockTexture( GLuint _uid, GLint _internalFormat,
		GLsizei _width, GLsizei _height, GLenum _format, GLenum _type, const GLvoid* _data )
	{
		glBindTexture( GL_TEXTURE_2D, _uid );

		glTexImage2D( GL_TEXTURE_2D, 0, _internalFormat, _width, _height, 0, _format, _type, _data );

		glBindTexture( GL_TEXTURE_2D, m_activeTexture );
	}
	//////////////////////////////////////////////////////////////////////////
	void OGLRenderSystem::makeProjection2D( float _left, float _right, 
											float _top, float _bottom, 
											float _near, float _far,  
											float* _outMatrix )
	{
		float tx = 1.0f / ( _right - _left );
		float ty = 1.0f / ( _top - _bottom );
		float tz = 1.0f / ( _far - _near );
		_outMatrix[0] = 2.0f * tx;
		_outMatrix[1] = 0.0f;
		_outMatrix[2] = 0.0f;
		_outMatrix[3] = 0.0f;
		_outMatrix[4] = 0.0f;
		_outMatrix[5] = -2.0f * ty;
		_outMatrix[6] = 0.0f;
		_outMatrix[7] = 0.0f;
		_outMatrix[8] = 0.0f;
		_outMatrix[9] = 0.0f;
		_outMatrix[10] = 2.0f * tz;
		_outMatrix[11] = 0.0f;
		_outMatrix[12] = -(_right + _left) * tx;
		_outMatrix[13] = (_top + _bottom) * ty;
		_outMatrix[14] = -(_far + _near) * tz;
		_outMatrix[15] = 1.0f;
	}
	//////////////////////////////////////////////////////////////////////////
}	// namespace Menge
