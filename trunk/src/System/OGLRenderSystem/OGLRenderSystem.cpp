
#	include "OGLRenderSystem.h"

#	include <cmath>

#	include "Interface/LogSystemInterface.h"
#	include "OGLTexture.h"
#	include "OGLWindowContext.h"

#	ifndef MENGE_MASTER_RELEASE
#		define MENGE_LOG log_
#	else
#		define MENGE_LOG
#	endif

#	define MENGE_LOG_ERROR log_error_

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
	static GLenum s_toGLBlendFactor( EBlendFactor _blend )
	{
		switch( _blend )
		{
		case Menge::BF_ONE:
			return GL_ONE;
		case Menge::BF_ZERO:
			return GL_ZERO;
		case Menge::BF_DEST_COLOUR:
			return GL_DST_COLOR;
		case Menge::BF_SOURCE_COLOUR:
			return GL_SRC_COLOR;
		case Menge::BF_ONE_MINUS_DEST_COLOUR:
			return GL_ONE_MINUS_DST_COLOR;
		case Menge::BF_ONE_MINUS_SOURCE_COLOUR:
			return GL_ONE_MINUS_SRC_COLOR;
		case Menge::BF_DEST_ALPHA:
			return GL_DST_ALPHA;
		case Menge::BF_SOURCE_ALPHA:
			return GL_SRC_ALPHA;
		case Menge::BF_ONE_MINUS_DEST_ALPHA:
			return GL_ONE_MINUS_DST_ALPHA;
		case Menge::BF_ONE_MINUS_SOURCE_ALPHA:
			return GL_ONE_MINUS_SRC_ALPHA;
		}
		return GL_ZERO;
	}
	//////////////////////////////////////////////////////////////////////////
	static GLenum s_toGLCmpFunc( ECompareFunction _func )
	{
		switch( _func )
		{
		case CMPF_ALWAYS_FAIL:
			return GL_NEVER;
		case CMPF_LESS:
			return GL_LESS;
		case CMPF_EQUAL:
			return GL_EQUAL;
		case CMPF_LESS_EQUAL:
			return GL_LEQUAL;
		case CMPF_GREATER:
			return GL_GREATER;
		case CMPF_NOT_EQUAL:
			return GL_NOTEQUAL;
		case CMPF_GREATER_EQUAL:
			return GL_GEQUAL;
		case CMPF_ALWAYS_PASS:
			return GL_ALWAYS;
		}
		return GL_NEVER;
	}
	//////////////////////////////////////////////////////////////////////////
	static GLenum s_toGLFillMode( EFillMode _mode )
	{
		switch( _mode )
		{
		case FM_POINT:
			return GL_POINT;
		case FM_WIREFRAME:
			return GL_LINE;
		case FM_SOLID:
			return GL_FILL;
		}
		return GL_POINT;
	}
	//////////////////////////////////////////////////////////////////////////
	static GLenum s_toGLShadeMode( EShadeType _type )
	{
		if( _type == SHT_FLAT )
		{
			return GL_FLAT;
		}
		return GL_SMOOTH;
	}
	//////////////////////////////////////////////////////////////////////////
	static GLenum s_toGLFilter( ETextureFilter _filter )
	{
		switch( _filter )
		{
		case Menge::TF_NONE:
		case Menge::TF_POINT:
			return GL_NEAREST;
		case Menge::TF_ANISOTROPIC:
		case Menge::TF_LINEAR:
		case Menge::TF_FLATCUBIC:
		case Menge::TF_GAUSSIANCUBIC:
			return GL_LINEAR;
		}
		return GL_NEAREST;
	}
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
			return GL_RGBA8;
		case Menge::PF_A8:
			return GL_ALPHA8;
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
	static GLenum s_toGLPrimitiveMode( EPrimitiveType _primType )
	{
		switch( _primType )
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
		}
		return GL_POINTS;
	}
	//////////////////////////////////////////////////////////////////////////
	static GLenum s_toGLAddressMode( ETextureAddressMode _mode )
	{
		switch( _mode )
		{
		case Menge::TAM_CLAMP:
			return GL_CLAMP_TO_EDGE;
		case Menge::TAM_WRAP:
			return GL_REPEAT;
		case Menge::TAM_MIRROR:
			return GL_MIRRORED_REPEAT;
		}
		return GL_REPEAT;
	}
	//////////////////////////////////////////////////////////////////////////
	static GLenum s_toGLTextureOp( ETextureOp _textureOp )
	{
		switch( _textureOp )
		{
		case Menge::TOP_SELECTARG1:
		case Menge::TOP_SELECTARG2:
			return GL_REPLACE;
		case Menge::TOP_MODULATE:
			return GL_MODULATE;
		case Menge::TOP_ADD:
			return GL_ADD;
		case Menge::TOP_SUBSTRACT:
			return GL_SUBTRACT;
		}
		return GL_REPLACE;
	}
	//////////////////////////////////////////////////////////////////////////
	static GLenum s_toGLTextureArg( ETextureArgument _textureArg )
	{
		switch( _textureArg )
		{
		case Menge::TARG_CURRENT:
			return GL_PREVIOUS;
		case Menge::TARG_DIFFUSE:
			return GL_PRIMARY_COLOR;
		case Menge::TARG_SPECULAR:
			return GL_PRIMARY_COLOR;
		case Menge::TARG_TEXTURE:
			return GL_TEXTURE;
		case Menge::TARG_TFACTOR:
			return GL_CONSTANT;
		}
		return GL_TEXTURE;
	}
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
		, m_windowContext( NULL )
		, m_supportNPOT( false )
		, m_currentVertexBuffer( 0 )
		, m_currentIndexBuffer( 0 )
		, m_srcBlendFactor( GL_ONE )
		, m_dstBlendFactor( GL_ZERO )
		, m_activeTextureStage( 0 )
		, m_activeTexture( 0 )
	{
	}
	//////////////////////////////////////////////////////////////////////////
	OGLRenderSystem::~OGLRenderSystem()
	{
		releaseWindowContext( m_windowContext );
	}
	//////////////////////////////////////////////////////////////////////////
	bool OGLRenderSystem::initialize( LogSystemInterface* _logSystem, RenderSystemListener* _listener )
	{
		m_logSystem = _logSystem;

		MENGE_LOG( "Initializing OpenGL RenderSystem..." );

		return true;
	}
	//////////////////////////////////////////////////////////////////////////
	bool OGLRenderSystem::createRenderWindow( std::size_t _width, std::size_t _height, int _bits,
												bool _fullscreen, WindowHandle _winHandle, 
												bool _waitForVSync, int _FSAAType, int _FSAAQuality )
	{
		createWindowContext( &m_windowContext );
		if( m_windowContext == NULL )
		{
			MENGE_LOG_ERROR( "Error: failed to create platform window context" );
			return false;
		}

		m_winWidth = _width;
		m_winHeight = _height;
		if( m_windowContext->initialize( _width, _height, _bits, _fullscreen, _winHandle, _waitForVSync ) == false )
		{
			MENGE_LOG_ERROR( "Error: failed to initialize window context" );
			return false;
		}
		m_windowContext->setFullscreenMode( _width, _height, _fullscreen );

		const char* str = (const char*)glGetString( GL_VERSION );
		String mstr = "None";
		if( str != NULL )
		{
			mstr = str;
		}
		MENGE_LOG( "OpenGL Version: %s", mstr.c_str() );
		mstr = "None";
		str = (const char*)glGetString( GL_VENDOR );
		if( str != NULL )
		{
			mstr = str;
		}
		MENGE_LOG( "Vendor: %s", mstr.c_str() );
		str = (const char*)glGetString( GL_RENDERER );
		//LOG( "Renderer: " + Menge::String( str ) );
		m_ext = (const char*)glGetString( GL_EXTENSIONS );
		//LOG( "Extensions:" );
		//LOG( m_ext );
		// check for NPOT
		String extSubStr = "texture_non_power_of_two";
		String::size_type pos =  m_ext.find( extSubStr );
		if( pos != String::npos && ( m_ext[pos+extSubStr.size()] == '\0' || m_ext[pos+extSubStr.size()] == ' ' ) )	// it seems to be supported
		{
			// try to create NPOT to be sure
			GLuint npotTex;
			GLint size = 65;
			glGenTextures( 1, &npotTex );
			glBindTexture( GL_TEXTURE_2D, npotTex );
			glTexParameteri( GL_TEXTURE_2D, GL_TEXTURE_MAX_LEVEL, 0 );
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
		}

		extSubStr = "pixel_buffer_object";
		pos = m_ext.find( extSubStr );
		if( pos != String::npos && ( m_ext[pos+extSubStr.size()] == '\0' || m_ext[pos+extSubStr.size()] == ' ' ) )	// it seems to be supported
		{
			MENGE_LOG( "Supports PBO" );
		}

		for( int i = 0; i < MENGE_MAX_TEXTURE_STAGES; ++i )
		{
			glActiveTexture( GL_TEXTURE0 + i );
			glTexEnvi( GL_TEXTURE_ENV, GL_TEXTURE_ENV_MODE, GL_COMBINE );
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
		glEnable( GL_SCISSOR_TEST );
		m_depthMask = false;
		glDepthMask( GL_FALSE );
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
		int pitch = 0;
		if( _image == NULL )
		{
			MENGE_LOG_ERROR( "Warning: _image == NULL in OGLRenderSystem::screenshot" );
			return;
		}
		unsigned char* imageData = _image->lock( &pitch, false );
		glReadBuffer( GL_BACK );
		int x = 0;
		int y = m_winHeight;
		int width =  m_winWidth; 
		int height = m_winHeight; 
		if( _rect != NULL )
		{
			x = _rect[0];
			y -= _rect[3];
			width = ::floorf( _rect[2] - _rect[0] + 0.5f );
			height = ::floorf( _rect[3] - _rect[1] + 0.5f );
		}
		//std::size_t iWidth = _image->getWidth();
		//std::size_t iHeight = _image->getHeight();
		int iWidth;
		int iHeight;
		OGLTexture* oglTexture = static_cast<OGLTexture*>( _image );
		glBindTexture( GL_TEXTURE_2D, oglTexture->uid );
		glGetTexLevelParameteriv( GL_TEXTURE_2D, 0, GL_TEXTURE_WIDTH, &iWidth );
		glGetTexLevelParameteriv( GL_TEXTURE_2D, 0, GL_TEXTURE_HEIGHT, &iHeight );
		glBindTexture( GL_TEXTURE_2D, m_activeTexture );

		int bufDataPitch = width * 4;
		unsigned char* bufferData = new unsigned char[bufDataPitch*height];
		glPixelStorei( GL_PACK_ALIGNMENT, 1 );
		glReadPixels( x, y, width, height, GL_BGRA, GL_UNSIGNED_BYTE, bufferData );	// why is our buffer flipped by y????
		glPixelStorei( GL_PACK_ALIGNMENT, 4 );
		if( iWidth == width && iHeight == height )
		{
			unsigned char* buf = bufferData + bufDataPitch * ( height - 1 );
			for( std::size_t i = 0; i < height; i++ )
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
			unsigned char* pdst = imageData + ( iHeight - 1 ) * pitch;	// flip by y

			// sx_48,sy_48 represent current position in source
			// using 16/48-bit fixed precision, incremented by steps
			Menge::uint64 stepx = ((Menge::uint64)width << 48) / iWidth;
			Menge::uint64 stepy = ((Menge::uint64)height << 48) / iHeight;

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
				size_t sy2 = (std::min<size_t>)( sy1 + 1, height - 1 );
				size_t syoff1 = sy1 * width;//bufDataPitch;
				size_t syoff2 = sy2 * width;//bufDataPitch;

				Menge::uint64 sx_48 = (stepx >> 1) - 1;
				for (size_t x = 0; x < iWidth; x++, sx_48+=stepx) 
				{
					temp = sx_48 >> 36;
					temp = (temp > 0x800)? temp - 0x800 : 0;
					unsigned int sxf = temp & 0xFFF;
					size_t sx1 = temp >> 12;
					size_t sx2 = (std::min<size_t>)(sx1+1, width-1);

					unsigned int sxfsyf = sxf*syf;
					for (unsigned int k = 0; k < 4; k++) 
					{
						unsigned int accum =
							srcdata[(sx1 + syoff1)*3+k]*(0x1000000-(sxf<<12)-(syf<<12)+sxfsyf) +
							srcdata[(sx2 + syoff1)*3+k]*((sxf<<12)-sxfsyf) +
							srcdata[(sx1 + syoff2)*3+k]*((syf<<12)-sxfsyf) +
							srcdata[(sx2 + syoff2)*3+k]*sxfsyf;
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
		_image->unlock();
	}
	//////////////////////////////////////////////////////////////////////////
	void OGLRenderSystem::render()
	{
		// deprecated
	}
	//////////////////////////////////////////////////////////////////////////
	void OGLRenderSystem::setContentResolution( const std::size_t * _resolution )
	{
		// deprecated
	}
	//////////////////////////////////////////////////////////////////////////
	void OGLRenderSystem::setProjectionMatrix( const float * _projection )
	{
		//GLfloat mat[16];
		//s_toGLMatrix( mat, _projection );
		glMatrixMode( GL_PROJECTION );
		//glLoadMatrixf( mat );
		//glLoadIdentity();
		//glOrtho( 0,  1024, 768, 0, 0, -1 ); 
		glLoadMatrixf( _projection );
		//glScalef( 1.0f, 1.0f, -1.0f );
		//glTranslatef( 0.0f, 0.0f, -0.5f );
		//glMultMatrixf( mat );
		glMatrixMode( GL_MODELVIEW );
	}
	//////////////////////////////////////////////////////////////////////////
	void OGLRenderSystem::setModelViewMatrix( const float * _modelview )
	{
		//GLfloat mat[16];
		//s_toGLMatrix( mat, _modelview );
		glLoadMatrixf( _modelview );
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
			GLfloat mat[16];
			s_toGLMatrix( mat, _texture );
			glLoadMatrixf( mat );
		}
		else
		{
			glLoadIdentity();
		}
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

		if( !GL_EXT_vertex_array_bgra )
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
		}

		glBufferSubData( GL_ARRAY_BUFFER, memRange.offset, memRange.size, memRange.pMem );
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
		glBufferData( GL_ELEMENT_ARRAY_BUFFER, _indiciesNum * sizeof( uint16 ), NULL, GL_STATIC_DRAW );
		glBindBuffer( GL_ELEMENT_ARRAY_BUFFER, m_currentIndexBuffer );
		return static_cast<IBHandle>( bufId );
	}
	//////////////////////////////////////////////////////////////////////////
	void OGLRenderSystem::releaseIndexBuffer( IBHandle _ibHandle )
	{
		GLuint bufId = static_cast<GLuint>( _ibHandle );
		if( bufId == m_currentIndexBuffer )
		{
			m_currentIndexBuffer = 0;
		}
		glDeleteBuffers( 1, &bufId );
	}
	//////////////////////////////////////////////////////////////////////////
	uint16* OGLRenderSystem::lockIndexBuffer( IBHandle _ibHandle )
	{
		GLuint bufId = static_cast<GLuint>( _ibHandle );
		glBindBuffer( GL_ELEMENT_ARRAY_BUFFER, bufId );
		void* pMem = glMapBuffer( GL_ELEMENT_ARRAY_BUFFER, GL_WRITE_ONLY );
		glBindBuffer( GL_ELEMENT_ARRAY_BUFFER, m_currentIndexBuffer );
		return static_cast<uint16*>( pMem );
	}
	//////////////////////////////////////////////////////////////////////////
	bool OGLRenderSystem::unlockIndexBuffer( IBHandle _ibHandle )
	{
		GLuint bufId = static_cast<GLuint>( _ibHandle );
		glBindBuffer( GL_ELEMENT_ARRAY_BUFFER, bufId );
		GLboolean result = glUnmapBuffer( GL_ELEMENT_ARRAY_BUFFER );
		glBindBuffer( GL_ELEMENT_ARRAY_BUFFER, m_currentIndexBuffer );
		return result == GL_TRUE;
	}
	//////////////////////////////////////////////////////////////////////////
	void OGLRenderSystem::setIndexBuffer( IBHandle _ibHandle )
	{
		GLuint bufId = static_cast<GLuint>( _ibHandle );
		glBindBuffer( GL_ELEMENT_ARRAY_BUFFER, bufId );
		m_currentIndexBuffer = bufId;
	}
	//////////////////////////////////////////////////////////////////////////
	void OGLRenderSystem::setVertexDeclaration( uint32 _declaration )
	{

	}
	//////////////////////////////////////////////////////////////////////////
	void OGLRenderSystem::drawIndexedPrimitive( EPrimitiveType _type, 
		std::size_t _baseVertexIndex,  std::size_t _minIndex, 
		std::size_t _verticesNum, std::size_t _startIndex, std::size_t _indexCount )
	{
		GLenum mode = s_toGLPrimitiveMode( _type );
		GLint colorSize = 4;
		if( GL_EXT_vertex_array_bgra )
		{
			colorSize = GL_BGRA;
		}

		glVertexPointer( 3, GL_FLOAT, 24,  0 );
		glEnableClientState(GL_VERTEX_ARRAY);

		glColorPointer( colorSize, GL_UNSIGNED_BYTE, 24,  reinterpret_cast<const GLvoid *>( 12 ) );
		glEnableClientState(GL_COLOR_ARRAY);

		glTexCoordPointer( 2, GL_FLOAT, 24, reinterpret_cast<const GLvoid *>( 16 ) );
		glEnableClientState( GL_TEXTURE_COORD_ARRAY );  

		glDrawRangeElements( mode, _minIndex, _minIndex + _verticesNum,
			_indexCount, GL_UNSIGNED_SHORT, reinterpret_cast<const GLvoid *>( _startIndex * sizeof( uint16 ) ) );
		
		glDisableClientState(GL_TEXTURE_COORD_ARRAY );
		glDisableClientState(GL_COLOR_ARRAY); 
		glDisableClientState(GL_VERTEX_ARRAY);
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

		GLenum modeUGL = s_toGLAddressMode( _modeU );
		GLenum modeVGL = s_toGLAddressMode( _modeV );
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
		m_srcBlendFactor = s_toGLBlendFactor( _src );
		glBlendFunc( m_srcBlendFactor, m_dstBlendFactor );
	}
	//////////////////////////////////////////////////////////////////////////
	void OGLRenderSystem::setDstBlendFactor( EBlendFactor _dst )
	{
		m_dstBlendFactor = s_toGLBlendFactor( _dst );
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
		GLenum cmpFunc = s_toGLCmpFunc( _depthFunction );
		glDepthFunc( cmpFunc );
	}
	//////////////////////////////////////////////////////////////////////////
	void OGLRenderSystem::setFillMode( EFillMode _mode )
	{
		GLenum mode = s_toGLFillMode( _mode );
		glPolygonMode( GL_FRONT_AND_BACK, mode );
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
		GLenum model = s_toGLShadeMode( _sType );
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
		GLenum cmpFunc = s_toGLCmpFunc( _alphaFunc );
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

		GLenum textureOpGL = s_toGLTextureOp( _textrueOp );
		GLenum arg1GL = s_toGLTextureArg( _arg1 );
		GLenum arg2GL = s_toGLTextureArg( _arg2 );
		glTexEnvi( GL_TEXTURE_ENV, GL_COMBINE_RGB, textureOpGL );
		if( _textrueOp == Menge::TOP_SELECTARG2 )
		{
			arg1GL = arg2GL;
		}
		glTexEnvi( GL_TEXTURE_ENV, GL_SOURCE0_RGB, arg1GL );
		glTexEnvi( GL_TEXTURE_ENV, GL_SOURCE1_RGB, arg2GL );
	}
	//////////////////////////////////////////////////////////////////////////
	void OGLRenderSystem::setTextureStageAlphaOp( size_t _stage, ETextureOp _textrueOp,  ETextureArgument _arg1, ETextureArgument _arg2 )
	{
		if( m_activeTextureStage != _stage )
		{
			glActiveTexture( GL_TEXTURE0  + _stage );
			m_activeTextureStage = _stage;
		}

		GLenum textureOpGL = s_toGLTextureOp( _textrueOp );
		GLenum arg1GL = s_toGLTextureArg( _arg1 );
		GLenum arg2GL = s_toGLTextureArg( _arg2 );
		glTexEnvi( GL_TEXTURE_ENV, GL_COMBINE_ALPHA, textureOpGL );
		if( _textrueOp == Menge::TOP_SELECTARG2 )
		{
			arg1GL = arg2GL;
		}
		glTexEnvi( GL_TEXTURE_ENV, GL_SOURCE0_ALPHA, arg1GL );
		glTexEnvi( GL_TEXTURE_ENV, GL_SOURCE1_ALPHA, arg2GL );
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
			GLenum filterGL = s_toGLFilter( _filter );
			if( tStage.magFilter != filterGL )
			{
				tStage.magFilter = filterGL;
				glTexParameteri( GL_TEXTURE_2D, GL_TEXTURE_MAG_FILTER, filterGL );
			}
		}
	}
	//////////////////////////////////////////////////////////////////////////
	RenderImageInterface* OGLRenderSystem::createImage( std::size_t& _width, std::size_t& _height, PixelFormat& _format )
	{
		if( _format == Menge::PF_R8G8B8 )
		{
			_format = Menge::PF_X8R8G8B8;
		}

		if( ( _width & ( _width - 1 ) ) != 0
			|| ( _height & ( _height - 1 ) ) != 0 )
		{
			bool npot = m_supportNPOT;//();
			if( npot == false )	// we're all gonna die
			{
				_width = s_firstPO2From( _width );
				_height = s_firstPO2From( _height );
			}
		}

		int numColors = 4;
		if( _format == PF_A8 )
		{
			numColors = 1;
		}

		GLuint tuid = 0;
		glGenTextures( 1, &tuid );
		OGLTexture* texture = new OGLTexture( tuid, this );
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

		glBindTexture( GL_TEXTURE_2D, tuid );
		glTexParameteri( GL_TEXTURE_2D, GL_TEXTURE_WRAP_S, texture->wrapS );
		glTexParameteri( GL_TEXTURE_2D, GL_TEXTURE_WRAP_T, texture->wrapT );
		glTexParameteri( GL_TEXTURE_2D, GL_TEXTURE_MAG_FILTER, texture->minFilter );
		glTexParameteri( GL_TEXTURE_2D, GL_TEXTURE_MIN_FILTER, texture->magFilter );
		glTexParameteri( GL_TEXTURE_2D, GL_TEXTURE_MAX_LEVEL, 0 );
		glTexParameteri( GL_TEXTURE_2D, GL_GENERATE_MIPMAP, GL_FALSE );
		glTexImage2D( GL_TEXTURE_2D, texture->level, texture->internalFormat,
						texture->width, texture->height, texture->border, texture->format,
						texture->type, NULL );
		glBindTexture( GL_TEXTURE_2D, m_activeTexture );

		return texture;
	}
	//////////////////////////////////////////////////////////////////////////
	RenderImageInterface* OGLRenderSystem::createRenderTargetImage( std::size_t& _width, std::size_t& _height )
	{
		return NULL;
	}
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
		return true;
	}
	//////////////////////////////////////////////////////////////////////////
	void OGLRenderSystem::endScene()
	{

	}
	//////////////////////////////////////////////////////////////////////////
	void OGLRenderSystem::swapBuffers()
	{
		m_windowContext->swapBuffers();
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
			glClearDepth( _depth );
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
	void OGLRenderSystem::setRenderArea( const float* _renderArea )
	{
		int w = static_cast<int>( _renderArea[2] - _renderArea[0] );
		int h = static_cast<int>( _renderArea[3] - _renderArea[1] );

		glViewport( (int)_renderArea[0], m_winHeight - (int)_renderArea[1] - h, w, h );
		glScissor( (int)_renderArea[0], m_winHeight - (int)_renderArea[1] - h, w, h );
		//glViewport( (int)_renderArea[0], (int)_renderArea[1], w, h );
		//glScissor( (int)_renderArea[0], (int)_renderArea[1], w, h );
	}
	//////////////////////////////////////////////////////////////////////////
	void OGLRenderSystem::setFullscreenMode( std::size_t _width, std::size_t _height, bool _fullscreen )
	{
		m_windowContext->setFullscreenMode( _width, _height, _fullscreen );
		glViewport( 0, 0, _width, _height );
		m_winWidth = _width;
		m_winHeight = _height;
	}
	//////////////////////////////////////////////////////////////////////////
	void OGLRenderSystem::setRenderTarget( RenderImageInterface* _renderTarget, bool _clear )
	{

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
		m_windowContext->setVSync( _vSync );
	}
	//////////////////////////////////////////////////////////////////////////
	void OGLRenderSystem::log_( const char* _message, ... )
	{
		if( m_logSystem == NULL )
		{
			return;
		}

		va_list argList;

		va_start(argList, _message);

		char str[1024];

		vsnprintf( str, 1024, _message, argList );

		va_end(argList);

		String message( str );
		message += '\n';

		m_logSystem->logMessage( message, LM_LOG );
	}
	//////////////////////////////////////////////////////////////////////////
	void OGLRenderSystem::log_error_( const char* _message, ... )
	{
		if( m_logSystem == NULL )
		{
			return;
		}

		va_list argList;

		va_start(argList, _message);

		char str[1024];

		vsnprintf( str, 1024, _message, argList );

		va_end(argList);

		String message( str );
		message += '\n';

		m_logSystem->logMessage( message, LM_ERROR );
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
	//////////////////////////////////////////////////////////////////////////
	//bool OGLRenderSystem::initialize( Menge::LogSystemInterface* _logSystem )
	//{
	//	m_logSystem = _logSystem;
	//	LOG( "Starting OpenGL Render System..." );
	//	bool initialized = true;
	//
	//	OGLUtils::identity(m_worldMatrix);
	//	OGLUtils::identity(m_viewMatrix);
	//
	//	return initialized;
	//}
	////////////////////////////////////////////////////////////////////////////
	//void OGLRenderSystem::swapBuffers()
	//{
	//#if MENGE_PLATFORM_WIN32
	//	SwapBuffers(m_hdc);
	//#elif MENGE_PLATFORM_MACOSX
	//	if(m_aglContext != aglGetCurrentContext())
	//		aglSetCurrentContext(m_aglContext);
	//			
	//	aglSwapBuffers(m_aglContext);
	//#endif
	//	
	//}
	////////////////////////////////////////////////////////////////////////////
	//int OGLRenderSystem::getNumDIP() const
	//{
	//	return 1;
	//}
	////////////////////////////////////////////////////////////////////////////
	//bool OGLRenderSystem::createRenderWindow( std::size_t _width, std::size_t _height, int _bits, bool _fullscreen, Menge::WindowHandle _winHandle, int _FSAAType, int _FSAAQuality )
	//{
	//#if MENGE_PLATFORM_WIN32
	//	m_hdc = GetDC((HWND)_winHandle);
	//	PIXELFORMATDESCRIPTOR pfd;
	//	memset(&pfd, 0, sizeof(pfd));
	//	pfd.nSize = sizeof(pfd);
	//	pfd.nVersion = 1;
	//	pfd.dwFlags = PFD_DRAW_TO_WINDOW | PFD_SUPPORT_OPENGL | PFD_DOUBLEBUFFER;
	//	pfd.iPixelType = PFD_TYPE_RGBA;
	//	pfd.cColorBits = (_bits > 16)? 24 : _bits;
	//	pfd.cAlphaBits = (_bits > 16)? 8 : 0;
	//	pfd.cDepthBits = 24;
	//	pfd.cStencilBits = 8;
	//
	//	int format = 0;
	//	format = ChoosePixelFormat(m_hdc, &pfd);
	//	if( format == 0 )
	//	{
	//		LOG_ERROR( "OpenGL error: failed choose pixel format" );
	//		return false;
	//	}
	//	BOOL res = SetPixelFormat(m_hdc, format, &pfd);
	//	if( res == FALSE )
	//	{
	//		LOG_ERROR( "OpenGL error: failed set pixel format" );
	//		return false;
	//	}
	//	m_glrc = wglCreateContext(m_hdc);
	//	if( m_glrc == 0 )
	//	{
	//		LOG_ERROR( "OpenGL error: failed to create OpenGL context" );
	//		return false;
	//	}
	//	wglMakeCurrent(m_hdc, m_glrc);
	//
	//	GLenum err = glewInit();
	//	if (GLEW_OK != err)	
	//	{
	//
	//	}
	//#elif MENGE_PLATFORM_MACOSX
	//	int i = 0;
	//	AGLPixelFormat pixelFormat;
	//	GLint attribs[] = { AGL_NO_RECOVERY, GL_TRUE, AGL_ACCELERATED, GL_TRUE, AGL_RGBA, AGL_DOUBLEBUFFER, AGL_FULLSCREEN, /*AGL_ALPHA_SIZE, 8
	//						,AGL_RED_SIZE, 8, AGL_STENCIL_SIZE, 8, AGL_DEPTH_SIZE, 24,*/ AGL_NONE };
	//	
	//	GDHandle gdhDisplay;
	//	m_mainDisplayID = CGMainDisplayID();
	//	m_desktopDisplayMode = CGDisplayCurrentMode( m_mainDisplayID );
	//	
	//	m_fullscreen = _fullscreen;
	//	/*if( m_fullscreen )
	//	{
	//		CFDictionaryRef refDisplayMode = 0;
	//		refDisplayMode = CGDisplayBestModeForParameters( CGMainDisplayID(), _bits, _width, _height, NULL );
	//		CGDisplaySwitchToMode( CGMainDisplayID(), refDisplayMode ); 
	//	}
	//	
	//	if( DMGetGDeviceByDisplayID( (DisplayIDType)m_mainDisplayID, &gdhDisplay, false ) != noErr )
	//	{
	//		return false;
	//	}*/
	//	
	//	
	//	pixelFormat = aglChoosePixelFormat( 0, 0, attribs );
	//
	//	// Note: when using a single context with AGL one must ensure that there is always a context attached to a window to ensure
	//	// the accelerated surface is not destroyed.  We use buffer naming and use a dummy context to hold accelerated surface in place.
	//	// The following code points both contexts to the same buffer name (thus buffer) and then leaves the dummy context attached to the
	//	// window.
	//	//m_aglDummyContext = aglCreateContext( pixelFormat, NULL );
	//	//GLint bufferName = 1;
	//	//aglSetInteger( m_aglDummyContext, AGL_BUFFER_NAME, &bufferName ); // set buffer name for this window context
	//	m_aglContext = aglCreateContext(pixelFormat, NULL);
	//	if( m_aglContext == NULL )
	//	{
	//		return false;
	//	}
	//	//aglSetInteger( m_aglContext, AGL_BUFFER_NAME, &bufferName); // set same buffer name to share hardware buffers
	//
	//	aglDestroyPixelFormat( pixelFormat );
	//	
	//	m_windowRef = (WindowRef)_winHandle;
	//	
	//    GrafPtr portSave = NULL;
	//    GetPort(&portSave);
	//	SetPort((GrafPtr) GetWindowPort(m_windowRef));
	//			
	//	/*if( !aglSetDrawable( m_aglDummyContext, GetWindowPort(m_windowRef) ) ) // force creation of buffers for window
	//	{
	//		return false;
	//	}*/
	//	//aglSetDrawable( m_aglDummyContext, GetWindowPort(m_windowRef) );
	//	
	//	if( m_fullscreen )
	//	{
	//		aglSetFullScreen( m_aglContext, 0, 0, 0, 0 );
	//	}
	//	else
	//	{
	//		aglSetDrawable(m_aglContext, GetWindowPort(m_windowRef) );
	//	}
	//	aglSetCurrentContext(m_aglContext);
	//	GLint val = 1;
	//	aglSetInteger( m_aglContext, AGL_STATE_VALIDATION, &val );
	//	
	//	SetPort(portSave);
	//#endif
	//	m_windowWidth = _width;
	//	m_windowHeight = _height;
	//	m_viewport[0] = m_viewport[1] = 0;
	//	m_viewport[2] = _width;
	//	m_viewport[3] = _height;
	//
	//	const char* str = (const char*)glGetString( GL_VERSION );
	//	Menge::String mstr = "None";
	//	if( str != NULL )
	//	{
	//		mstr = str;
	//	}
	//	LOG( "OpenGL Version: " + mstr );
	//	mstr = "None";
	//	str = (const char*)glGetString( GL_VENDOR );
	//	if( str != NULL )
	//	{
	//		mstr = str;
	//	}
	//	LOG( "Vendor: " + Menge::String( str ) );
	//	str = (const char*)glGetString( GL_RENDERER );
	//	//LOG( "Renderer: " + Menge::String( str ) );
	//	//m_ext = (const char*)glGetString( GL_EXTENSIONS );
	//	//LOG( "Extensions:" );
	//	//LOG( m_ext );
	//	// check for NPOT
	//	Menge::String extSubStr = "texture_non_power_of_two";
	//	Menge::String::size_type pos =  m_ext.find( extSubStr );
	//	if( pos != Menge::String::npos && ( m_ext[pos+extSubStr.size()] == '\0' || m_ext[pos+extSubStr.size()] == ' ' ) )	// it seems to be supported
	//	{
	//		// try to create NPOT to be sure
	//		GLuint npotTex;
	//		GLint size = 65;
	//		glGenTextures( 1, &npotTex );
	// 		glBindTexture( GL_TEXTURE_2D, npotTex );
	//		glTexParameteri( GL_TEXTURE_2D, GL_TEXTURE_MAX_LEVEL, 0 );
	//		glTexParameteri( GL_TEXTURE_2D, GL_TEXTURE_WRAP_S, GL_CLAMP_TO_EDGE);
	//		glTexParameteri( GL_TEXTURE_2D, GL_TEXTURE_WRAP_T, GL_CLAMP_TO_EDGE);
	//		glTexParameteri( GL_TEXTURE_2D, GL_TEXTURE_MAG_FILTER, GL_LINEAR);
	//		glTexParameteri( GL_TEXTURE_2D, GL_TEXTURE_MIN_FILTER, GL_LINEAR);
	//		glTexImage2D( GL_PROXY_TEXTURE_2D, 0, GL_RGBA8, size, size, 0, GL_BGRA, GL_UNSIGNED_BYTE, NULL );
	//		GLint width = 0;
	//		glGetTexLevelParameteriv( GL_PROXY_TEXTURE_2D, 0, GL_TEXTURE_WIDTH, &width );
	//		if( width == size )
	//		{
	//			m_supportNPOT = true;
	//		}
	//	}
	//	extSubStr = "pixel_buffer_object";
	//	pos = m_ext.find( extSubStr );
	//	if( pos != Menge::String::npos && ( m_ext[pos+extSubStr.size()] == '\0' || m_ext[pos+extSubStr.size()] == ' ' ) )	// it seems to be supported
	//	{
	//		LOG( "Supports PBO" );
	//	}
	//	
	//	initBatching();
	//	glFrontFace( GL_CW );
	//
	//	return true;
	//}
	//
	////////////////////////////////////////////////////////////////////////////
	//const std::vector<int> & OGLRenderSystem::getResolutionList()
	//{
	//	static std::vector<int> list;
	//	list.clear();
	//
	//	return list;
	//}
	////////////////////////////////////////////////////////////////////////////
	//void OGLRenderSystem::screenshot( Menge::RenderImageInterface* _image, const float * _rect /*= 0 */ )
	//{
	//	int pitch = 0;
	//	if( _image == NULL )
	//	{
	//		LOG_ERROR( "Warning: _image == NULL in OGLRenderSystem::screenshot" );
	//		return;
	//	}
	//	unsigned char* imageData = _image->lock( &pitch, false );
	//	glReadBuffer( GL_BACK );
	//	int x = 0;
	//	int y = m_windowHeight;
	//	std::size_t width =  m_windowWidth; 
	//	std::size_t height = m_windowHeight; 
	//	if( _rect != NULL )
	//	{
	//		x = _rect[0];
	//		y -= _rect[3];
	//		width = ::floorf( _rect[2] - _rect[0] + 0.5f );
	//		height = ::floorf( _rect[3] - _rect[1] + 0.5f );
	//	}
	//	std::size_t iWidth = _image->getWidth();
	//	std::size_t iHeight = _image->getHeight();
	//
	//	int bufDataPitch = width * 3;
	//	unsigned char* bufferData = new unsigned char[bufDataPitch*height];
	//	glPixelStorei( GL_PACK_ALIGNMENT, 1 );
	//	glReadPixels( x, y, width, height, GL_BGR, GL_UNSIGNED_BYTE, bufferData );	// why is our buffer flipped by y????
	//	glPixelStorei( GL_PACK_ALIGNMENT, 4 );
	//	if( iWidth == width && iHeight == height )
	//	{
	//		unsigned char* buf = bufferData + bufDataPitch * ( height - 1 );
	//		for( std::size_t i = 0; i < height; i++ )
	//		{
	//			std::copy( buf, buf + bufDataPitch, imageData );
	//			imageData += pitch;
	//			buf -= bufDataPitch; // flip by y
	//		}
	//	}
	//	else	// resample image
	//	{
	//
	//		// srcdata stays at beginning of slice, pdst is a moving pointer
	//		unsigned char* srcdata = bufferData;
	//		unsigned char* pdst = imageData + ( iHeight - 1 ) * pitch;	// flip by y
	//
	//		// sx_48,sy_48 represent current position in source
	//		// using 16/48-bit fixed precision, incremented by steps
	//		Menge::uint64 stepx = ((Menge::uint64)width << 48) / iWidth;
	//		Menge::uint64 stepy = ((Menge::uint64)height << 48) / iHeight;
	//
	//		// bottom 28 bits of temp are 16/12 bit fixed precision, used to
	//		// adjust a source coordinate backwards by half a pixel so that the
	//		// integer bits represent the first sample (eg, sx1) and the
	//		// fractional bits are the blend weight of the second sample
	//		unsigned int temp;
	//
	//		Menge::uint64 sy_48 = (stepy >> 1) - 1;
	//		for (size_t y = 0; y < iHeight; y++, sy_48 += stepy) 
	//		{
	//			temp = sy_48 >> 36;
	//			temp = (temp > 0x800)? temp - 0x800: 0;
	//			unsigned int syf = temp & 0xFFF;
	//			size_t sy1 = temp >> 12;
	//			size_t sy2 = (std::min)( sy1 + 1, height - 1 );
	//			size_t syoff1 = sy1 * width;//bufDataPitch;
	//			size_t syoff2 = sy2 * width;//bufDataPitch;
	//
	//			Menge::uint64 sx_48 = (stepx >> 1) - 1;
	//			for (size_t x = 0; x < iWidth; x++, sx_48+=stepx) 
	//			{
	//				temp = sx_48 >> 36;
	//				temp = (temp > 0x800)? temp - 0x800 : 0;
	//				unsigned int sxf = temp & 0xFFF;
	//				size_t sx1 = temp >> 12;
	//				size_t sx2 = (std::min)(sx1+1, width-1);
	//
	//				unsigned int sxfsyf = sxf*syf;
	//				for (unsigned int k = 0; k < 4; k++) 
	//				{
	//					unsigned int accum =
	//						srcdata[(sx1 + syoff1)*3+k]*(0x1000000-(sxf<<12)-(syf<<12)+sxfsyf) +
	//						srcdata[(sx2 + syoff1)*3+k]*((sxf<<12)-sxfsyf) +
	//						srcdata[(sx1 + syoff2)*3+k]*((syf<<12)-sxfsyf) +
	//						srcdata[(sx2 + syoff2)*3+k]*sxfsyf;
	//					// accum is computed using 8/24-bit fixed-point math
	//					// (maximum is 0xFF000000; rounding will not cause overflow)
	//					*pdst++ = (accum + 0x800000) >> 24;
	//				}
	//			}
	//			pdst -= pitch * 2;
	//			//pdst += 4*dst.getRowSkip();
	//		}
	//
	//	}
	//	delete[] bufferData;
	//	_image->unlock();
	//}
	////////////////////////////////////////////////////////////////////////////
	//void OGLRenderSystem::render()
	//{
	//	// deprecated
	//}
	////////////////////////////////////////////////////////////////////////////
	//void OGLRenderSystem::setContentResolution( const std::size_t * _resolution )
	//{
	//	// deprecated
	//}
	////////////////////////////////////////////////////////////////////////////
	//void OGLRenderSystem::setProjectionMatrix( const float * _projection )
	//{
	//	glMatrixMode(GL_PROJECTION);
	//	glLoadMatrixf(m_projMatrix);
	//	glMatrixMode(GL_MODELVIEW);
	//}
	////////////////////////////////////////////////////////////////////////////
	//void OGLRenderSystem::setViewMatrix( const float * _view )
	//{
	//	glLoadMatrixf(m_viewMatrix);
	//}
	////////////////////////////////////////////////////////////////////////////
	//void OGLRenderSystem::setWorldMatrix( const float * _world )
	//{
	//	renderBatch();
	//
	//	std::copy( _world, _world + 16, m_worldMatrix );
	//
	//	float WV[16];
	//	OGLUtils::multiplyMatrices(WV,m_worldMatrix,m_viewMatrix);
	//	GLfloat mogl[16];
	//	OGLUtils::makeGLMatrix( mogl, WV );
	//	glMatrixMode(GL_MODELVIEW);
	//	glLoadMatrixf(mogl);
	//	//glRotatef(-180,0,0,0);
	//	/*std::copy( _world, _world + 16, m_worldMatrix );
	//	glMatrixMode( GL_MODELVIEW );
	//	glLoadMatrixf( m_worldMatrix );
	//	glMultMatrixf( m_viewMatrix );*/
	//}
	////////////////////////////////////////////////////////////////////////////
	//Menge::RenderImageInterface * OGLRenderSystem::createImage( const Menge::String & _name,
	//														   std::size_t _width, std::size_t _height
	//														   , Menge::PixelFormat _format )
	//{
	//	OGLTexture * texture = new OGLTexture();
	//
	//	std::size_t image_width = _width;
	//	std::size_t image_height = _height;
	//	if( ( _width & ( _width - 1 ) ) != 0
	//		|| ( _height & ( _height - 1 ) ) != 0 )
	//	{
	//		bool npot = supportNPOT();
	//		if( npot == false )	// we're all gonna die
	//		{
	//			_width = s_firstPO2From( _width );
	//			_height = s_firstPO2From( _height );
	//		}
	//	}
	//
	//	std::size_t memSize = Menge::PixelUtil::getMemorySize( _width, _height, 1, _format );
	//	Menge::TextureDesc _desc = {_name,0,0,memSize,_width,_height,_format};
	//
	//	texture->load( image_width, image_height, _desc );
	//	m_textureMap.insert( std::make_pair( _desc.name, texture ) );
	//	texture->incRef();
	//
	//	return texture;
	//}
	////////////////////////////////////////////////////////////////////////////
	//Menge::RenderImageInterface * OGLRenderSystem::createRenderTargetImage( const Menge::String & _name, std::size_t _width, std::size_t _height )
	//{
	//	OGLRenderTextureFBO * texture = new OGLRenderTextureFBO();
	//	
	//	std::size_t image_width = _width;
	//	std::size_t image_height = _height;
	//	if( ( _width & ( _width - 1 ) ) != 0
	//		|| ( _height & ( _height - 1 ) ) != 0 )
	//	{
	//		bool npot = supportNPOT();
	//		if( npot == false )	// we're all gonna die
	//		{
	//			_width = s_firstPO2From( _width );
	//			_height = s_firstPO2From( _height );
	//		}
	//	}
	//	
	//	if( texture->create( _name, image_width, image_height, _width, _height ) == false )
	//	{
	//		LOG_ERROR( "Error while creating RenderTargetImage. RenderTargetImage not created" );
	//		delete texture;
	//		return NULL;
	//	}
	//
	//	RenderTargetInfo rtgtInfo;
	//	rtgtInfo.dirty = true;
	//	rtgtInfo.texture = texture;
	//
	//	m_targetMap.insert( std::make_pair( _name, rtgtInfo ) );
	//
	//	return texture;
	//}
	////////////////////////////////////////////////////////////////////////////
	//bool OGLRenderSystem::supportNPOT()
	//{
	//	//return m_supportNPOT;
	//	return false;
	//}
	////////////////////////////////////////////////////////////////////////////
	//void OGLRenderSystem::releaseImage( Menge::RenderImageInterface * _image )
	//{
	//	OGLTexture * texture = static_cast<OGLTexture*>( _image );
	//
	//	if( !_image )
	//	{
	//		return;
	//	}
	//
	//	TTargetMap::iterator it = m_targetMap.find( texture->getDescription() );
	//	if( it != m_targetMap.end() )
	//	{
	//		m_targetMap.erase( it );
	//		delete texture;
	//	}
	//	else
	//	{
	//		TTextureMap::iterator itt = m_textureMap.find( texture->getDescription() );
	//		if( itt != m_textureMap.end() )
	//		{
	//			if( texture->decRef() == 0 )
	//			{
	//				m_textureMap.erase( itt );
	//				delete texture;
	//			}
	//		}
	//	}
	//	
	//}
	////////////////////////////////////////////////////////////////////////////
	//void OGLRenderSystem::renderLine( unsigned int _color, 
	//								 const float * _begin, 
	//								 const float * _end )
	//{
	//	int r = (_color>>16) & 0xff;
	//	int g = (_color>>8) & 0xff;
	//	int b = (_color) & 0xff;
	//	int a = (_color>>24) & 0xff;
	//
	//	glDisable(GL_TEXTURE_2D);
	//
	//	glBegin(GL_LINES);
	//
	//	glColor4ub(r,g,b,a);
	//	glVertex2f(_begin[0],_begin[1]);
	//	glVertex2f(_end[0],_end[1]);
	//	glEnd();
	//
	//	glEnable(GL_TEXTURE_2D);
	//}
	////////////////////////////////////////////////////////////////////////////
	//void OGLRenderSystem::beginScene()
	//{
	//	m_layer = 1.0f;
	//	glViewport( 0, 0, m_windowWidth, m_windowHeight );
	//#ifdef _DEBUG
	//	glClearColor(0.0f, 0.0f,1.0f,1.0f);
	//#else
	//	glClearColor(0.0f,0.0f,0.0f,1.0f);
	//#endif
	//	glClear(GL_COLOR_BUFFER_BIT | GL_DEPTH_BUFFER_BIT);
	//	n_prim = 0;
	//}
	////////////////////////////////////////////////////////////////////////////
	//void OGLRenderSystem::endScene()
	//{
	//	renderBatch();
	//	glFlush();
	//}
	////////////////////////////////////////////////////////////////////////////
	//void OGLRenderSystem::setFullscreenMode( std::size_t _width, std::size_t _height, bool _fullscreen )
	//{
	//#if MENGE_PLATFORM_WIN32
	//	if ( _fullscreen )
	//	{
	//		DEVMODE dm;
	//		dm.dmSize = sizeof(DEVMODE);
	//		dm.dmPelsWidth = _width;
	//		dm.dmPelsHeight = _height;
	//		dm.dmFields = DM_PELSWIDTH | DM_PELSHEIGHT;
	//
	//		if (ChangeDisplaySettings(&dm, CDS_FULLSCREEN) != DISP_CHANGE_SUCCESSFUL)
	//		{
	//			LOG_ERROR( "Error: OpenGL failed to switch into fullscreen mode " );
	//		}
	//	}
	//	else
	//	{
	//		ChangeDisplaySettings(NULL, 0);
	//	}
	//#elif MENGE_PLATFORM_MACOSX
	//	if( _fullscreen )
	//	{
	//		aglSetDrawable( m_aglContext, NULL );
	//		aglSetFullScreen( m_aglContext, _width, _height, 0, 0 );
	//	}
	//	else
	//	{
	//		aglSetDrawable( m_aglContext, NULL );
	//		aglSetDrawable( m_aglContext, GetWindowPort( m_windowRef ) );
	//	}
	//#endif
	//	m_windowWidth = _width;
	//	m_windowHeight = _height;
	//	m_fullscreen = _fullscreen;
	//	
	//	glViewport( 0, 0, _width, _height );
	//}
	////////////////////////////////////////////////////////////////////////////
	//void OGLRenderSystem::setRenderTarget( const Menge::String& _name, bool _clear )
	//{
	//	renderBatch();
	//	//glFlush();
	//
	//	TTargetMap::iterator itp = m_targetMap.find( m_currentRenderTarget );
	//
	//	if( itp != m_targetMap.end() )
	//	{
	//		itp->second.texture->flush();
	//		//aglSetCurrentContext( NULL );
	//		//aglSetCurrentContext( m_aglContext );
	//	}
	//
	//	if( _name == "defaultCamera" && m_currentRenderTarget != "defaultCamera" )
	//	{
	//	
	//		// Back to window
	//		glBindFramebufferEXT( GL_FRAMEBUFFER_EXT, 0 ); 
	//		//glDrawBuffer( GL_BACK );
	//		//glReadBuffer( GL_BACK );
	//		//aglSetDrawable( m_aglContext, NULL );
	//		/*if( m_fullscreen )
	//		{
	//			CGLContextObj mCGLContext;
	//			if( aglGetCGLContext( m_aglContext, (void**)&mCGLContext ) == GL_FALSE )
	//			{
	//				LOG_ERROR( "ERROR\n" );
	//			}
	//			if( CGLFlushDrawable(mCGLContext) != kCGLNoError )
	//			{
	//				LOG_ERROR( "ERROR\n");
	//			}
	//			CGLContextObj curCtx = CGLGetCurrentContext();
	//			//if(curCtx != mCGLContext)
	//			{
	//				CGLSetCurrentContext(mCGLContext);
	//				CGLClearDrawable(mCGLContext);
	//				CGLSetFullScreen(mCGLContext);
	//			}
	//
	//		//	aglSetFullScreen( m_aglContext, 0, 0, 0, 0 );
	//		}
	//		else
	//		{
	//			aglSetDrawable(m_aglContext, GetWindowPort(m_windowRef) );
	//		}*/
	//		//aglSetCurrentContext( m_aglContext );
	//		m_currentRenderTarget = _name;
	//	}
	//
	//	TTargetMap::iterator it = m_targetMap.find( _name );
	//
	//	if( it != m_targetMap.end() )
	//	{
	//		m_currentRenderTarget = _name;
	//		if( it->second.texture->enable() == false )
	//		{
	//			LOG_ERROR( "Error switching render target" );
	//		}
	//		if( it->second.dirty && _clear )
	//		{
	//			glClearColor( 0.0f, 0.0f, 0.0f, 1.0f );
	//			glClear(GL_COLOR_BUFFER_BIT | GL_DEPTH_BUFFER_BIT);
	//		}
	//
	//	}
	//}
	////////////////////////////////////////////////////////////////////////////
	//void OGLRenderSystem::setRenderArea( const float* _renderArea )
	//{
	//	for( int i = 0; i < 4; i++ )
	//	{
	//		m_viewport[i] = _renderArea[i];
	//	}
	//
	//	if( ( m_viewport[0] | m_viewport[1] | m_viewport[2] | m_viewport[3] ) == 0 )
	//	{
	//		m_viewport[2] = m_windowWidth;
	//		m_viewport[3] = m_windowHeight;
	//	}
	//
	//	GLsizei w = m_viewport[2] - m_viewport[0];
	//	GLsizei h = m_viewport[3] - m_viewport[1];
	//
	//	glViewport( m_viewport[0], m_viewport[1], w, h );
	//
	//	//_glEnable2D();
	//	glMatrixMode( GL_PROJECTION );
	//	glLoadIdentity();
	//	if( m_currentRenderTarget == "defaultCamera" )
	//	{
	//		glOrtho( m_viewport[0],  m_viewport[2], m_viewport[3], m_viewport[1], -1, 1 );
	//	}
	//	else
	//	{
	//		glOrtho( m_viewport[0],  m_viewport[2], m_viewport[1], m_viewport[3], -1, 1 );
	//	}
	//}
	//////////////////////////////////////////////////////////////////////////
	
}	// namespace Menge
