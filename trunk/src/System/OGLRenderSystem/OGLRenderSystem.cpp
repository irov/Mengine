#	include "Config/Typedef.h"

#	include "OGLRenderSystem.h"

#	include "Interface/LogSystemInterface.h"

#	include "OGLTexture.h"
#	include "OGLUtils.h"

#	include "OGLRenderTextureFBO.h"

#	include "Menge/PixelFormat.h"

#	include <assert.h>
#	include <OpenGL/OpenGL.h>

#	ifndef MENGE_MASTER_RELEASE
#		define LOG( message )\
	if( m_logSystem ) m_logSystem->logMessage( message + Menge::StringA("\n"), Menge::LM_LOG );
#	else
#		define LOG( message )
#	endif

#	define LOG_ERROR( message )\
	if( m_logSystem ) m_logSystem->logMessage( message + Menge::StringA("\n"), Menge::LM_ERROR );

//////////////////////////////////////////////////////////////////////////
bool initInterfaceSystem( Menge::RenderSystemInterface ** _ptrInterface )
{
	try
	{
		*_ptrInterface = new OGLRenderSystem();
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
	delete static_cast<OGLRenderSystem*>(_ptrInterface);
}

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
OGLRenderSystem::OGLRenderSystem()
: m_layer( 1.0f )
, m_layer3D( false )
, m_currentRenderTarget( "defaultCamera" )
, m_supportNPOT( false )
, m_fullscreen( false )
#if MENGE_PLATFORM_WIN32
, m_hdc( 0 )
, m_glrc( 0 )
#elif MENGE_PLATFORM_MACOSX
, m_aglContext( NULL )
, m_aglDummyContext( NULL )
#endif
{
}
//////////////////////////////////////////////////////////////////////////
OGLRenderSystem::~OGLRenderSystem()
{
#if MENGE_PLATFORM_MACOSX
	if( m_aglContext != NULL )
	{
		aglDestroyContext( m_aglContext );
	}
#endif
	deleteBatching();
}
//////////////////////////////////////////////////////////////////////////
bool OGLRenderSystem::initialize( Menge::LogSystemInterface* _logSystem )
{
	m_logSystem = _logSystem;
	LOG( "Starting OpenGL Render System..." );
	bool initialized = true;

	OGLUtils::identity(m_worldMatrix);
	OGLUtils::identity(m_viewMatrix);

	return initialized;
}
//////////////////////////////////////////////////////////////////////////
void OGLRenderSystem::swapBuffers()
{
#if MENGE_PLATFORM_WIN32
	SwapBuffers(m_hdc);
#elif MENGE_PLATFORM_MACOSX
	if(m_aglContext != aglGetCurrentContext())
		aglSetCurrentContext(m_aglContext);
			
	aglSwapBuffers(m_aglContext);
#endif
	
}
//////////////////////////////////////////////////////////////////////////
int OGLRenderSystem::getNumDIP() const
{
	return 1;
}
//////////////////////////////////////////////////////////////////////////
bool OGLRenderSystem::createRenderWindow( std::size_t _width, std::size_t _height, int _bits, bool _fullscreen, Menge::WindowHandle _winHandle, int _FSAAType, int _FSAAQuality )
{
#if MENGE_PLATFORM_WIN32
	m_hdc = GetDC((HWND)_winHandle);
	PIXELFORMATDESCRIPTOR pfd;
	memset(&pfd, 0, sizeof(pfd));
	pfd.nSize = sizeof(pfd);
	pfd.nVersion = 1;
	pfd.dwFlags = PFD_DRAW_TO_WINDOW | PFD_SUPPORT_OPENGL | PFD_DOUBLEBUFFER;
	pfd.iPixelType = PFD_TYPE_RGBA;
	pfd.cColorBits = (_bits > 16)? 24 : _bits;
	pfd.cAlphaBits = (_bits > 16)? 8 : 0;
	pfd.cDepthBits = 24;
	pfd.cStencilBits = 8;

	int format = 0;
	format = ChoosePixelFormat(m_hdc, &pfd);
	if( format == 0 )
	{
		LOG_ERROR( "OpenGL error: failed choose pixel format" );
		return false;
	}
	BOOL res = SetPixelFormat(m_hdc, format, &pfd);
	if( res == FALSE )
	{
		LOG_ERROR( "OpenGL error: failed set pixel format" );
		return false;
	}
	m_glrc = wglCreateContext(m_hdc);
	if( m_glrc == 0 )
	{
		LOG_ERROR( "OpenGL error: failed to create OpenGL context" );
		return false;
	}
	wglMakeCurrent(m_hdc, m_glrc);

	GLenum err = glewInit();
	if (GLEW_OK != err)	
	{

	}
#elif MENGE_PLATFORM_MACOSX
	int i = 0;
	AGLPixelFormat pixelFormat;
	GLint attribs[] = { AGL_NO_RECOVERY, GL_TRUE, AGL_ACCELERATED, GL_TRUE, AGL_RGBA, AGL_DOUBLEBUFFER, AGL_FULLSCREEN, /*AGL_ALPHA_SIZE, 8
						,AGL_RED_SIZE, 8, AGL_STENCIL_SIZE, 8, AGL_DEPTH_SIZE, 24,*/ AGL_NONE };
	
	GDHandle gdhDisplay;
	m_mainDisplayID = CGMainDisplayID();
	m_desktopDisplayMode = CGDisplayCurrentMode( m_mainDisplayID );
	
	m_fullscreen = _fullscreen;
	/*if( m_fullscreen )
	{
		CFDictionaryRef refDisplayMode = 0;
		refDisplayMode = CGDisplayBestModeForParameters( CGMainDisplayID(), _bits, _width, _height, NULL );
		CGDisplaySwitchToMode( CGMainDisplayID(), refDisplayMode ); 
	}
	
	if( DMGetGDeviceByDisplayID( (DisplayIDType)m_mainDisplayID, &gdhDisplay, false ) != noErr )
	{
		return false;
	}*/
	
	
	pixelFormat = aglChoosePixelFormat( 0, 0, attribs );

	// Note: when using a single context with AGL one must ensure that there is always a context attached to a window to ensure
	// the accelerated surface is not destroyed.  We use buffer naming and use a dummy context to hold accelerated surface in place.
	// The following code points both contexts to the same buffer name (thus buffer) and then leaves the dummy context attached to the
	// window.
	//m_aglDummyContext = aglCreateContext( pixelFormat, NULL );
	//GLint bufferName = 1;
	//aglSetInteger( m_aglDummyContext, AGL_BUFFER_NAME, &bufferName ); // set buffer name for this window context
	m_aglContext = aglCreateContext(pixelFormat, NULL);
	if( m_aglContext == NULL )
	{
		return false;
	}
	//aglSetInteger( m_aglContext, AGL_BUFFER_NAME, &bufferName); // set same buffer name to share hardware buffers

	aglDestroyPixelFormat( pixelFormat );
	
	m_windowRef = (WindowRef)_winHandle;
	
    GrafPtr portSave = NULL;
    GetPort(&portSave);
	SetPort((GrafPtr) GetWindowPort(m_windowRef));
			
	/*if( !aglSetDrawable( m_aglDummyContext, GetWindowPort(m_windowRef) ) ) // force creation of buffers for window
	{
		return false;
	}*/
	//aglSetDrawable( m_aglDummyContext, GetWindowPort(m_windowRef) );
	
	if( m_fullscreen )
	{
		aglSetFullScreen( m_aglContext, 0, 0, 0, 0 );
	}
	else
	{
		aglSetDrawable(m_aglContext, GetWindowPort(m_windowRef) );
	}
	aglSetCurrentContext(m_aglContext);
	GLint val = 1;
	aglSetInteger( m_aglContext, AGL_STATE_VALIDATION, &val );
	
	SetPort(portSave);
#endif
	m_windowWidth = _width;
	m_windowHeight = _height;
	m_viewport[0] = m_viewport[1] = 0;
	m_viewport[2] = _width;
	m_viewport[3] = _height;

	const char* str = (const char*)glGetString( GL_VERSION );
	Menge::String mstr = "None";
	if( str != NULL )
	{
		mstr = str;
	}
	LOG( "OpenGL Version: " + mstr );
	mstr = "None";
	str = (const char*)glGetString( GL_VENDOR );
	if( str != NULL )
	{
		mstr = str;
	}
	LOG( "Vendor: " + Menge::String( str ) );
	str = (const char*)glGetString( GL_RENDERER );
	//LOG( "Renderer: " + Menge::String( str ) );
	//m_ext = (const char*)glGetString( GL_EXTENSIONS );
	//LOG( "Extensions:" );
	//LOG( m_ext );
	// check for NPOT
	Menge::String extSubStr = "texture_non_power_of_two";
	Menge::String::size_type pos =  m_ext.find( extSubStr );
	if( pos != Menge::String::npos && ( m_ext[pos+extSubStr.size()] == '\0' || m_ext[pos+extSubStr.size()] == ' ' ) )	// it seems to be supported
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
	}
	extSubStr = "pixel_buffer_object";
	pos = m_ext.find( extSubStr );
	if( pos != Menge::String::npos && ( m_ext[pos+extSubStr.size()] == '\0' || m_ext[pos+extSubStr.size()] == ' ' ) )	// it seems to be supported
	{
		LOG( "Supports PBO" );
	}
	
	initBatching();
	glFrontFace( GL_CW );

	return true;
}

//////////////////////////////////////////////////////////////////////////
const std::vector<int> & OGLRenderSystem::getResolutionList()
{
	static std::vector<int> list;
	list.clear();

	return list;
}
//////////////////////////////////////////////////////////////////////////
void OGLRenderSystem::screenshot( Menge::RenderImageInterface* _image, const float * _rect /*= 0 */ )
{
	int pitch = 0;
	if( _image == NULL )
	{
		LOG_ERROR( "Warning: _image == NULL in OGLRenderSystem::screenshot" );
		return;
	}
	unsigned char* imageData = _image->lock( &pitch, false );
	glReadBuffer( GL_BACK );
	int x = 0;
	int y = m_windowHeight;
	std::size_t width =  m_windowWidth; 
	std::size_t height = m_windowHeight; 
	if( _rect != NULL )
	{
		x = _rect[0];
		y -= _rect[3];
		width = ::floorf( _rect[2] - _rect[0] + 0.5f );
		height = ::floorf( _rect[3] - _rect[1] + 0.5f );
	}
	std::size_t iWidth = _image->getWidth();
	std::size_t iHeight = _image->getHeight();

	int bufDataPitch = width * 3;
	unsigned char* bufferData = new unsigned char[bufDataPitch*height];
	glPixelStorei( GL_PACK_ALIGNMENT, 1 );
	glReadPixels( x, y, width, height, GL_BGR, GL_UNSIGNED_BYTE, bufferData );	// why is our buffer flipped by y????
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
			size_t sy2 = (std::min)( sy1 + 1, height - 1 );
			size_t syoff1 = sy1 * width;//bufDataPitch;
			size_t syoff2 = sy2 * width;//bufDataPitch;

			Menge::uint64 sx_48 = (stepx >> 1) - 1;
			for (size_t x = 0; x < iWidth; x++, sx_48+=stepx) 
			{
				temp = sx_48 >> 36;
				temp = (temp > 0x800)? temp - 0x800 : 0;
				unsigned int sxf = temp & 0xFFF;
				size_t sx1 = temp >> 12;
				size_t sx2 = (std::min)(sx1+1, width-1);

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
}
//////////////////////////////////////////////////////////////////////////
void OGLRenderSystem::setContentResolution( const std::size_t * _resolution )
{
	m_contentResolution[0] = _resolution[0];
	m_contentResolution[1] = _resolution[1];
}
//////////////////////////////////////////////////////////////////////////
void OGLRenderSystem::setProjectionMatrix( const float * _projection )
{
	renderBatch();

	for(int i = 0; i < 16;i++)
	{
		m_projMatrix[i] = _projection[i];
	}
	
	//OGLUtils::makeGLMatrix(m_projMatrix, _projection);

	/*if(m_currentRenderTarget != "defaultCamera" )
	{
		m_projMatrix[1] = -m_projMatrix[1];
		m_projMatrix[5] = -m_projMatrix[5];
		m_projMatrix[9] = -m_projMatrix[9];
		m_projMatrix[13] = -m_projMatrix[13];
	}*/

	//m_projMatrix[12]*=-1;

	glMatrixMode(GL_PROJECTION);   
	glLoadMatrixf(m_projMatrix);
	glMatrixMode(GL_MODELVIEW);
}
//////////////////////////////////////////////////////////////////////////
void OGLRenderSystem::setViewMatrix( const float * _view )
{
	renderBatch();

	std::copy( _view, _view + 16, m_viewMatrix );

	//float WV[16];
	//OGLUtils::multiplyMatrices(WV,m_worldMatrix,m_viewMatrix);
	//GLfloat mogl[16];
	//OGLUtils::makeGLMatrix( mogl, WV );
	glMatrixMode(GL_MODELVIEW);
	glLoadMatrixf(m_viewMatrix);
	/*std::copy( _view, _view + 16, m_viewMatrix );
	glMatrixMode( GL_MODELVIEW );
	glLoadMatrixf( m_worldMatrix );
	glMultMatrixf( m_viewMatrix );*/
}
//////////////////////////////////////////////////////////////////////////
void OGLRenderSystem::setWorldMatrix( const float * _world )
{
	renderBatch();

	std::copy( _world, _world + 16, m_worldMatrix );

	float WV[16];
	OGLUtils::multiplyMatrices(WV,m_worldMatrix,m_viewMatrix);
	GLfloat mogl[16];
	OGLUtils::makeGLMatrix( mogl, WV );
	glMatrixMode(GL_MODELVIEW);
	glLoadMatrixf(mogl);
	//glRotatef(-180,0,0,0);
	/*std::copy( _world, _world + 16, m_worldMatrix );
	glMatrixMode( GL_MODELVIEW );
	glLoadMatrixf( m_worldMatrix );
	glMultMatrixf( m_viewMatrix );*/
}
//////////////////////////////////////////////////////////////////////////
Menge::RenderImageInterface * OGLRenderSystem::createImage( const Menge::String & _name,
														   std::size_t _width, std::size_t _height
														   , Menge::PixelFormat _format )
{
	OGLTexture * texture = new OGLTexture();

	std::size_t image_width = _width;
	std::size_t image_height = _height;
	if( ( _width & ( _width - 1 ) ) != 0
		|| ( _height & ( _height - 1 ) ) != 0 )
	{
		bool npot = supportNPOT();
		if( npot == false )	// we're all gonna die
		{
			_width = s_firstPO2From( _width );
			_height = s_firstPO2From( _height );
		}
	}

	std::size_t memSize = Menge::PixelUtil::getMemorySize( _width, _height, 1, _format );
	Menge::TextureDesc _desc = {_name,0,0,memSize,_width,_height,_format};

	texture->load( image_width, image_height, _desc );
	m_textureMap.insert( std::make_pair( _desc.name, texture ) );
	texture->incRef();

	return texture;
}
//////////////////////////////////////////////////////////////////////////
Menge::RenderImageInterface * OGLRenderSystem::loadImage( const Menge::String& _name, std::size_t _width, std::size_t _height, const Menge::TextureDesc& _desc )
{
	OGLTexture * texture = new OGLTexture();
	texture->load( _width, _height, _desc );
	m_textureMap.insert( std::make_pair( _desc.name, texture ) );
	texture->incRef();

	return texture;
}
//////////////////////////////////////////////////////////////////////////
Menge::RenderImageInterface * OGLRenderSystem::createRenderTargetImage( const Menge::String & _name, std::size_t _width, std::size_t _height )
{
	OGLRenderTextureFBO * texture = new OGLRenderTextureFBO();
	
	std::size_t image_width = _width;
	std::size_t image_height = _height;
	if( ( _width & ( _width - 1 ) ) != 0
		|| ( _height & ( _height - 1 ) ) != 0 )
	{
		bool npot = supportNPOT();
		if( npot == false )	// we're all gonna die
		{
			_width = s_firstPO2From( _width );
			_height = s_firstPO2From( _height );
		}
	}
	
	if( texture->create( _name, image_width, image_height, _width, _height ) == false )
	{
		LOG_ERROR( "Error while creating RenderTargetImage. RenderTargetImage not created" );
		delete texture;
		return NULL;
	}

	RenderTargetInfo rtgtInfo;
	rtgtInfo.dirty = true;
	rtgtInfo.texture = texture;

	m_targetMap.insert( std::make_pair( _name, rtgtInfo ) );

	return texture;
}
//////////////////////////////////////////////////////////////////////////
bool OGLRenderSystem::supportNPOT()
{
	//return m_supportNPOT;
	return false;
}
//////////////////////////////////////////////////////////////////////////
void OGLRenderSystem::releaseImage( Menge::RenderImageInterface * _image )
{
	OGLTexture * texture = static_cast<OGLTexture*>( _image );

	if( !_image )
	{
		return;
	}

	TTargetMap::iterator it = m_targetMap.find( texture->getDescription() );
	if( it != m_targetMap.end() )
	{
		m_targetMap.erase( it );
		delete texture;
	}
	else
	{
		TTextureMap::iterator itt = m_textureMap.find( texture->getDescription() );
		if( itt != m_textureMap.end() )
		{
			if( texture->decRef() == 0 )
			{
				m_textureMap.erase( itt );
				delete texture;
			}
		}
	}
	
}
//////////////////////////////////////////////////////////////////////////
Menge::RenderImageInterface* OGLRenderSystem::getImage( const Menge::String& _desc ) const 
{
	TTextureMap::const_iterator it = m_textureMap.find( _desc );
	if( it != m_textureMap.end() )
	{
		static_cast<OGLTexture*>( it->second )->incRef();
		return it->second;
	}
	return NULL;
}
//////////////////////////////////////////////////////////////////////////
void OGLRenderSystem::renderImage(const float * _renderVertex, 
								  const float * _uv, 
								  unsigned int _color,  
								  const Menge::RenderImageInterface * _image, 
								  Menge::EBlendFactor _srcBlend, 
								  Menge::EBlendFactor _dstBlend )
{
	// swap color to ABGR
	unsigned int b = ( _color & 0x000000FF ) << 16;
	unsigned int r = ( _color & 0x00FF0000 ) >> 16;
	_color = ( _color & 0xFF00FF00 ) + ( b | r );

	const OGLTexture * tex = static_cast<const OGLTexture*>( _image );
	GLint glTex = 0;
	float uvMask[2] = { 1.0f, 1.0f };
	if( tex != NULL	)
	{
		glTex = tex->getGLTexture();
		uvMask[0] = tex->getUVMask()[0];
		uvMask[1] = tex->getUVMask()[1];
	}

	GLint srcBlend = OGLUtils::s_blendMengeToOGL( _srcBlend );
	GLint dstBlend = OGLUtils::s_blendMengeToOGL( _dstBlend );

	Menge::TVertex quad[4];

	quad[0].pos[0] = _renderVertex[0];
	quad[0].pos[1] = _renderVertex[1];
	quad[0].pos[2] = m_layer;
	quad[0].col = _color;
	quad[0].n[0] = 0.0f; 
	quad[0].n[1] = 0.0f; 
	quad[0].n[2] = 1.0f;

	quad[1].pos[0] = _renderVertex[2];
	quad[1].pos[1] = _renderVertex[3];
	quad[1].pos[2] = m_layer;
	quad[1].col = _color;
	quad[1].n[0] = 0.0f; 
	quad[1].n[1] = 0.0f; 
	quad[1].n[2] = 1.0f;

	quad[2].pos[0] = _renderVertex[4];
	quad[2].pos[1] = _renderVertex[5];
	quad[2].pos[2] = m_layer;
	quad[2].col = _color;
	quad[2].n[0] = 0.0f;
	quad[2].n[1] = 0.0f; 
	quad[2].n[2] = 1.0f;

	quad[3].pos[0] = _renderVertex[6];
	quad[3].pos[1] = _renderVertex[7];
	quad[3].pos[2] = m_layer;
	quad[3].col = _color;
	quad[3].n[0] = 0.0f; 
	quad[3].n[1] = 0.0f; 
	quad[3].n[2] = 1.0f;

	quad[0].uv[0] = _uv[0] * uvMask[0];
	quad[0].uv[1] = _uv[1] * uvMask[1];

	quad[1].uv[0] = _uv[2] * uvMask[0];
	quad[1].uv[1] = _uv[1] * uvMask[1];

	quad[2].uv[0] = _uv[2] * uvMask[0];
	quad[2].uv[1] = _uv[3] * uvMask[1];

	quad[3].uv[0] = _uv[0] * uvMask[0];
	quad[3].uv[1] = _uv[3] * uvMask[1];

	Gfx_RenderQuad( quad, glTex, srcBlend, dstBlend );
}

//////////////////////////////////////////////////////////////////////////
void OGLRenderSystem::renderTriple(
								   const float * _a, 
								   const float * _b, 
								   const float * _c, 
								   const float * _uv0, 
								   const float * _uv1,
								   const float * _uv2,
								   unsigned int _color,  
								   const Menge::RenderImageInterface * _image, 
								   Menge::EBlendFactor _srcBlend, 
								   Menge::EBlendFactor _dstBlend )
{
	// swap color to ABGR
	unsigned int b = ( _color & 0x000000FF ) << 16;
	unsigned int r = ( _color & 0x00FF0000 ) >> 16;
	_color = ( _color & 0xFF00FF00 ) + ( b | r );

	const OGLTexture * tex = static_cast<const OGLTexture*>( _image );
	GLint glTex = 0;
	float uvMask[2] = { 1.0f, 1.0f };
	if( tex != NULL	)
	{
		glTex = tex->getGLTexture();
		uvMask[0] = tex->getUVMask()[0];
		uvMask[1] = tex->getUVMask()[1];
	}

	GLint srcBlend = OGLUtils::s_blendMengeToOGL( _srcBlend );
	GLint dstBlend = OGLUtils::s_blendMengeToOGL( _dstBlend );

	Menge::TVertex quad[3];

	quad[0].pos[0] = _a[0];
	quad[0].pos[1] = _a[1];
	quad[0].pos[2] = m_layer;
	quad[0].col = _color;
	quad[0].n[0] = 0.0f; 
	quad[0].n[1] = 0.0f; 
	quad[0].n[2] = 1.0f;

	quad[1].pos[0] = _b[0];
	quad[1].pos[1] = _b[1];
	quad[1].pos[2] = m_layer;
	quad[1].col = _color;
	quad[1].n[0] = 0.0f; 
	quad[1].n[1] = 0.0f;
	quad[1].n[2] = 1.0f;

	quad[2].pos[0] = _c[0];
	quad[2].pos[1] = _c[1];
	quad[2].pos[2] = m_layer;
	quad[2].col = _color;
	quad[2].n[0] = 0.0f;
	quad[2].n[1] = 0.0f; 
	quad[2].n[2] = 1.0f;

	quad[0].uv[0] = _uv0[0] * uvMask[0];
	quad[0].uv[1] = _uv0[1] * uvMask[1];

	quad[1].uv[0] = _uv1[0] * uvMask[0];
	quad[1].uv[1] = _uv1[1] * uvMask[1];

	quad[2].uv[0] = _uv2[0] * uvMask[0];
	quad[2].uv[1] = _uv2[1] * uvMask[1];

	Gfx_RenderTriple(quad,glTex,srcBlend,dstBlend);
}
//////////////////////////////////////////////////////////////////////////
void OGLRenderSystem::renderLine( unsigned int _color, 
								 const float * _begin, 
								 const float * _end )
{
	int r = (_color>>16) & 0xff;
	int g = (_color>>8) & 0xff;
	int b = (_color) & 0xff;
	int a = (_color>>24) & 0xff;

	glDisable(GL_TEXTURE_2D);

	glBegin(GL_LINES);

	glColor4ub(r,g,b,a);
	glVertex2f(_begin[0],_begin[1]);
	glVertex2f(_end[0],_end[1]);
	glEnd();

	glEnable(GL_TEXTURE_2D);
}
//////////////////////////////////////////////////////////////////////////
void OGLRenderSystem::beginScene()
{
	m_layer = 1.0f;
	glViewport( 0, 0, m_windowWidth, m_windowHeight );
#ifdef _DEBUG
	glClearColor(0.0f, 0.0f,1.0f,1.0f);
#else
	glClearColor(0.0f,0.0f,0.0f,1.0f);
#endif
	glClear(GL_COLOR_BUFFER_BIT | GL_DEPTH_BUFFER_BIT);
	n_prim = 0;
}
//////////////////////////////////////////////////////////////////////////
void OGLRenderSystem::endScene()
{
	renderBatch();
	glFlush();
}
//////////////////////////////////////////////////////////////////////////
void OGLRenderSystem::beginLayer2D()
{
	if( m_layer3D == false ) 
	{
		return;	// already 2D
	}
	
	_glEnable2D();
	m_layer3D = false;
	
}
//////////////////////////////////////////////////////////////////////////
void OGLRenderSystem::endLayer2D()
{
	m_layer -= 0.001f;
}
//////////////////////////////////////////////////////////////////////////
void OGLRenderSystem::beginLayer3D()
{
	if( m_layer3D ) 
	{
		return;	// already 3D
	}

	_glEnable3D();
	m_layer3D = true;
}
//////////////////////////////////////////////////////////////////////////
void OGLRenderSystem::endLayer3D()
{
}
//////////////////////////////////////////////////////////////////////////
void OGLRenderSystem::renderText(const Menge::String & _text, const float * _pos, unsigned long _color)
{
}
//////////////////////////////////////////////////////////////////////////
void OGLRenderSystem::setFullscreenMode( std::size_t _width, std::size_t _height, bool _fullscreen )
{
#if MENGE_PLATFORM_WIN32
	if ( _fullscreen )
	{
		DEVMODE dm;
		dm.dmSize = sizeof(DEVMODE);
		dm.dmPelsWidth = _width;
		dm.dmPelsHeight = _height;
		dm.dmFields = DM_PELSWIDTH | DM_PELSHEIGHT;

		if (ChangeDisplaySettings(&dm, CDS_FULLSCREEN) != DISP_CHANGE_SUCCESSFUL)
		{
			LOG_ERROR( "Error: OpenGL failed to switch into fullscreen mode " );
		}
	}
	else
	{
		ChangeDisplaySettings(NULL, 0);
	}
#elif MENGE_PLATFORM_MACOSX
	if( _fullscreen )
	{
		aglSetDrawable( m_aglContext, NULL );
		aglSetFullScreen( m_aglContext, _width, _height, 0, 0 );
	}
	else
	{
		aglSetDrawable( m_aglContext, NULL );
		aglSetDrawable( m_aglContext, GetWindowPort( m_windowRef ) );
	}
#endif
	m_windowWidth = _width;
	m_windowHeight = _height;
	m_fullscreen = _fullscreen;
	
	glViewport( 0, 0, _width, _height );
}
//////////////////////////////////////////////////////////////////////////
Menge::CameraInterface * OGLRenderSystem::createCamera( const Menge::String & _name )
{
	return NULL;
}
//////////////////////////////////////////////////////////////////////////
Menge::EntityInterface * OGLRenderSystem::createEntity( const Menge::String & _name, const Menge::String & _meshName )
{
	return NULL;
}
//////////////////////////////////////////////////////////////////////////
Menge::LightInterface * OGLRenderSystem::createLight( const Menge::String & _name )
{
	return NULL;
}
//////////////////////////////////////////////////////////////////////////
void OGLRenderSystem::releaseCamera( Menge::CameraInterface * _camera )
{

}
//////////////////////////////////////////////////////////////////////////
void OGLRenderSystem::releaseEntity( Menge::EntityInterface * _entity )
{

}
//////////////////////////////////////////////////////////////////////////
void OGLRenderSystem::releaseLight( Menge::LightInterface * _light )
{

}
//////////////////////////////////////////////////////////////////////////
void OGLRenderSystem::setTextureFiltering( bool _filter )
{

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
void OGLRenderSystem::renderMesh( const Menge::TVertex* _vertices, std::size_t _verticesNum,
								 const Menge::uint16*	_indices, std::size_t _indicesNum,
								 Menge::TMaterial* _material )
{
	//glPolygonMode(GL_FRONT_AND_BACK, GL_LINE);

	const OGLTexture * tex = static_cast<const OGLTexture*>( _material->texture );
	GLint glTex = 0;
	float uvMask[2] = { 1.0f, 1.0f };
	if( tex != NULL )
	{
		glTex = tex->getGLTexture();
		uvMask[0] = tex->getUVMask()[0];
		uvMask[1] = tex->getUVMask()[1];
	}
	
	glBindTexture( GL_TEXTURE_2D, glTex );
	glMatrixMode( GL_TEXTURE );
	GLfloat texMatrix[16];
	std::fill( texMatrix, texMatrix + 16, 0.0f );
	texMatrix[0] = uvMask[0];
	texMatrix[5] = uvMask[1];
	texMatrix[10] = texMatrix[15] = 1.0f;
	glLoadMatrixf( texMatrix );

	glEnableClientState( GL_VERTEX_ARRAY );
	glColor4ubv( (GLubyte*)&(_material->color) );
	//glEnableClientState(GL_COLOR_ARRAY);
	glEnableClientState(GL_VERTEX_ARRAY);
	glEnableClientState(GL_TEXTURE_COORD_ARRAY );
	glEnableClientState(GL_NORMAL_ARRAY );

	// convert colors to gl color format.

	glClientActiveTextureARB(GL_TEXTURE0);

	//glColorPointer(4, GL_UNSIGNED_BYTE, sizeof(Menge::TVertex), &_vertices[0].col);
	glNormalPointer(GL_FLOAT, sizeof(Menge::TVertex), &_vertices[0].n);
	glTexCoordPointer(2, GL_FLOAT, sizeof(Menge::TVertex), &_vertices[0].uv);
	glVertexPointer(3, GL_FLOAT, sizeof(Menge::TVertex),  &_vertices[0].pos);

	glDrawElements(GL_TRIANGLES, _indicesNum, GL_UNSIGNED_SHORT, _indices);

	glDisableClientState( GL_VERTEX_ARRAY );
	//glDisableClientState(GL_COLOR_ARRAY);
	glDisableClientState(GL_VERTEX_ARRAY);
	glDisableClientState(GL_TEXTURE_COORD_ARRAY );
	glDisableClientState(GL_NORMAL_ARRAY );
	
	glLoadIdentity();
}
//////////////////////////////////////////////////////////////////////////
void OGLRenderSystem::setRenderTarget( const Menge::String& _name, bool _clear )
{
	renderBatch();
	//glFlush();

	TTargetMap::iterator itp = m_targetMap.find( m_currentRenderTarget );

	if( itp != m_targetMap.end() )
	{
		itp->second.texture->flush();
		//aglSetCurrentContext( NULL );
		//aglSetCurrentContext( m_aglContext );
	}

	if( _name == "defaultCamera" && m_currentRenderTarget != "defaultCamera" )
	{
	
		// Back to window
		glBindFramebufferEXT( GL_FRAMEBUFFER_EXT, 0 ); 
		//glDrawBuffer( GL_BACK );
		//glReadBuffer( GL_BACK );
		//aglSetDrawable( m_aglContext, NULL );
		/*if( m_fullscreen )
		{
			CGLContextObj mCGLContext;
			if( aglGetCGLContext( m_aglContext, (void**)&mCGLContext ) == GL_FALSE )
			{
				LOG_ERROR( "ERROR\n" );
			}
			if( CGLFlushDrawable(mCGLContext) != kCGLNoError )
			{
				LOG_ERROR( "ERROR\n");
			}
			CGLContextObj curCtx = CGLGetCurrentContext();
			//if(curCtx != mCGLContext)
			{
				CGLSetCurrentContext(mCGLContext);
				CGLClearDrawable(mCGLContext);
				CGLSetFullScreen(mCGLContext);
			}

		//	aglSetFullScreen( m_aglContext, 0, 0, 0, 0 );
		}
		else
		{
			aglSetDrawable(m_aglContext, GetWindowPort(m_windowRef) );
		}*/
		//aglSetCurrentContext( m_aglContext );
		m_currentRenderTarget = _name;
	}

	TTargetMap::iterator it = m_targetMap.find( _name );

	if( it != m_targetMap.end() )
	{
		m_currentRenderTarget = _name;
		if( it->second.texture->enable() == false )
		{
			LOG_ERROR( "Error switching render target" );
		}
		if( it->second.dirty && _clear )
		{
			glClearColor( 0.0f, 0.0f, 0.0f, 1.0f );
			glClear(GL_COLOR_BUFFER_BIT | GL_DEPTH_BUFFER_BIT);
		}

	}
}
//////////////////////////////////////////////////////////////////////////
void OGLRenderSystem::setRenderArea( const float* _renderArea )
{
	for( int i = 0; i < 4; i++ )
	{
		m_viewport[i] = _renderArea[i];
	}

	if( ( m_viewport[0] | m_viewport[1] | m_viewport[2] | m_viewport[3] ) == 0 )
	{
		m_viewport[2] = m_windowWidth;
		m_viewport[3] = m_windowHeight;
	}

	GLsizei w = m_viewport[2] - m_viewport[0];
	GLsizei h = m_viewport[3] - m_viewport[1];

	glViewport( m_viewport[0], m_viewport[1], w, h );

	//_glEnable2D();
	glMatrixMode( GL_PROJECTION );
	glLoadIdentity();
	if( m_currentRenderTarget == "defaultCamera" )
	{
		glOrtho( m_viewport[0],  m_viewport[2], m_viewport[3], m_viewport[1], -1, 1 );
	}
	else
	{
		glOrtho( m_viewport[0],  m_viewport[2], m_viewport[1], m_viewport[3], -1, 1 );
	}
}
//////////////////////////////////////////////////////////////////////////