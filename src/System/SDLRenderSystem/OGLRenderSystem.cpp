#	include "OGLRenderSystem.h"
#	include <SDL_syswm.h>
#	include "OGLTexture.h"

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
//////////////////////////////////////////////////////////////////////////
OGLRenderSystem::OGLRenderSystem()
: m_inRender( false )
, m_layer( 1.0f )
{
}
//////////////////////////////////////////////////////////////////////////
OGLRenderSystem::~OGLRenderSystem()
{
	m_SDLWindow.destroy();

	SDL_Quit();
}
//////////////////////////////////////////////////////////////////////////
void OGLRenderSystem::swapBuffers()
{
	m_SDLWindow.swapBuffers(false);
}
//////////////////////////////////////////////////////////////////////////
int OGLRenderSystem::getNumDIP() const
{
	return 1;
}
//////////////////////////////////////////////////////////////////////////
bool OGLRenderSystem::initialize( Menge::LogSystemInterface* _logSystem )
{
	bool initialized = true;

	Uint32 sdlFlags = SDL_INIT_VIDEO;

	if(SDL_Init(sdlFlags) < 0)
	{
		return false;
	}

	return initialized;
}
//////////////////////////////////////////////////////////////////////////
void glEnable2D()   
{   
	GLint viewport[4];   

	glGetIntegerv( GL_VIEWPORT, viewport );   

	glMatrixMode( GL_PROJECTION );   
	glPushMatrix();   
	glLoadIdentity();   

	glOrtho( viewport[0], viewport[0] + viewport[2],   
		viewport[1] + viewport[3], viewport[1], -1, 1 );   
 
	glMatrixMode( GL_MODELVIEW );   
	glPushMatrix();   
	glLoadIdentity();   

	glPushAttrib( GL_DEPTH_BUFFER_BIT | GL_LIGHTING_BIT );   
	glDisable( GL_DEPTH_TEST );  
	glDisable( GL_LIGHTING );  
}   
//////////////////////////////////////////////////////////////////////////
void glDisable2D()   
{   
	glPopAttrib();   

	glMatrixMode( GL_PROJECTION );   
	glPopMatrix();   

	glMatrixMode( GL_MODELVIEW );   
	glPopMatrix();   
}  
//////////////////////////////////////////////////////////////////////////
GLint s_blendMengeToOGL(Menge::EBlendFactor _blend )
{
	switch(_blend)
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
	};
	
	return GL_ONE;
}
//////////////////////////////////////////////////////////////////////////
bool OGLRenderSystem::createRenderWindow( std::size_t _width, std::size_t _height, int _bits, bool _fullscreen, Menge::WindowHandle _winHandle, int _FSAAType, int _FSAAQuality )
{
	NameValuePairList values;
	values.insert(std::make_pair("colourDepth","32"));
	// и как же передать тайтл? :)
	m_SDLWindow.create("TEST",_width,_height,_fullscreen,&values);

	//glEnable2D();

	glEnable(GL_TEXTURE_2D);
	glEnable(GL_BLEND);

	//glDepthFunc(GL_LESS);

	return true;
}

//////////////////////////////////////////////////////////////////////////
const std::vector<int> & OGLRenderSystem::getResolutionList()
{
	static std::vector<int> list;
	list.clear();

	const SDL_VideoInfo * vi = SDL_GetVideoInfo();
	m_videoModes = SDL_ListModes(vi->vfmt, SDL_FULLSCREEN | SDL_OPENGL);

	for (size_t i = 0; m_videoModes[i]; ++i)
	{
		list.push_back( m_videoModes[i]->w );
		list.push_back( m_videoModes[i]->h );
	}

	return list;
}
//////////////////////////////////////////////////////////////////////////
void OGLRenderSystem::screenshot( Menge::RenderImageInterface* _image, const float * _rect /*= 0 */ )
{
	glReadBuffer(GL_FRONT);

	int pitch = 0;

	unsigned char * buffer = _image->lock(&pitch,false);

	glTexImage2D(GL_TEXTURE_2D, 0, GL_RGB, (GLsizei)_rect[2],
		(GLsizei)_rect[3], 0, GL_RGB, GL_UNSIGNED_BYTE, NULL);

	glCopyTexSubImage2D(GL_TEXTURE_2D, 0, 0, 0, (GLint)_rect[0], (GLint)_rect[1],
		(GLsizei)_rect[2], (GLsizei)_rect[3]);

	_image->unlock();
}
//////////////////////////////////////////////////////////////////////////
void OGLRenderSystem::render()
{
}
//////////////////////////////////////////////////////////////////////////
void OGLRenderSystem::setContentResolution( const float * _resolution )
{
}
//////////////////////////////////////////////////////////////////////////
void OGLRenderSystem::setProjectionMatrix( const float * _projection )
{
}
//////////////////////////////////////////////////////////////////////////
void makeGLMatrix(GLfloat gl_matrix[16], const float * m)
{
	size_t x = 0;
	for (size_t i = 0; i < 4; i++)
	{
		for (size_t j = 0; j < 4; j++)
		{
			gl_matrix[x++] = m[j+i*4];
		}
	}
}
//////////////////////////////////////////////////////////////////////////
void OGLRenderSystem::setViewMatrix( const float * _view )
{
	GLfloat mat[16];
	makeGLMatrix( mat, _view );
	glMatrixMode(GL_MODELVIEW);
	glLoadMatrixf(mat);
}
//////////////////////////////////////////////////////////////////////////
void OGLRenderSystem::setWorldMatrix( const float * _world )
{

}
//////////////////////////////////////////////////////////////////////////
Menge::RenderImageInterface * OGLRenderSystem::createImage( const Menge::String & _name,
														   float _width, float _height )
{
	OGLTexture * texture = new OGLTexture(_name,_width, _height);
	m_textureMap.insert( std::make_pair( _name, static_cast<Menge::RenderImageInterface*>( texture ) ) );
	texture->incRef();
	return texture;
}
//////////////////////////////////////////////////////////////////////////
Menge::RenderImageInterface * OGLRenderSystem::createRenderTargetImage( const Menge::String & _name, float _width, float _height )
{
	assert(0);
/*	OGLTexture* texture = new OGLTexture( _name, _width, _height );
	RenderTargetInfo rtgtInfo;
	rtgtInfo.dirty = true;
	rtgtInfo.texture = texture;
	m_targetMap.insert( std::make_pair( _name, rtgtInfo ) );
	return texture;*/
	return 0;
}
//////////////////////////////////////////////////////////////////////////
Menge::RenderImageInterface * OGLRenderSystem::loadImage( const Menge::TextureDesc& _desc )
{
	OGLTexture * texture = new OGLTexture();
	texture->load(_desc);
	m_textureMap.insert( std::make_pair( _desc.name, texture ) );
	texture->incRef();

	return texture;
}
//////////////////////////////////////////////////////////////////////////
void OGLRenderSystem::releaseImage( Menge::RenderImageInterface * _image )
{
	OGLTexture * texture = static_cast<OGLTexture*>( _image );

	if( !_image )
	{
		return;
	}

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
	const OGLTexture * tex = static_cast<const OGLTexture*>( _image );

	GLint srcBlend = s_blendMengeToOGL( _srcBlend );
	GLint dstBlend = s_blendMengeToOGL( _dstBlend );

	glBlendFunc(srcBlend,dstBlend);

	glBindTexture(GL_TEXTURE_2D, tex->getGLTexture());

	int r = (_color>>16) & 0xff;
	int g = (_color>>8) & 0xff;
	int b = (_color) & 0xff;
	int a = (_color>>24) & 0xff;

	glBegin(GL_QUADS);
	
	glColor4f(r/255.f,g/255.f,b/255.f,1.f);

	glTexCoord2f(_uv[0],_uv[1]);
	glVertex2f(_renderVertex[0], _renderVertex[1]);

	glTexCoord2f(_uv[2],_uv[1]);
	glVertex2f(_renderVertex[2], _renderVertex[3]);

	glTexCoord2f(_uv[2],_uv[3]);
	glVertex2f(_renderVertex[4], _renderVertex[5]);

	glTexCoord2f(_uv[0],_uv[3]);
	glVertex2f(_renderVertex[6], _renderVertex[7]);

	glEnd();
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
	const OGLTexture * tex = static_cast<const OGLTexture*>( _image );

	GLint srcBlend = s_blendMengeToOGL( _srcBlend );
	GLint dstBlend = s_blendMengeToOGL( _dstBlend );
	glBlendFunc(srcBlend,dstBlend);

	glBindTexture(GL_TEXTURE_2D, tex->getGLTexture());

	int r = (_color>>16) & 0xff;
	int g = (_color>>8) & 0xff;
	int b = (_color) & 0xff;
	int a = (_color>>24) & 0xff;

	glBegin(GL_TRIANGLES);

	glColor4ub(r,g,b,a);

	glTexCoord2f(_uv0[0],_uv0[1]);
	glVertex2f(_a[0], _a[1]);

	glTexCoord2f(_uv1[0],_uv1[1]);
	glVertex2f(_b[0], _b[1]);

	glTexCoord2f(_uv2[0],_uv2[1]);
	glVertex2f(_c[0], _c[1]);

	glEnd();
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
	//m_layer = 1.0f;
	glClearColor(1,1,1,1.0f);
	glClear(GL_COLOR_BUFFER_BIT);
}
//////////////////////////////////////////////////////////////////////////
void OGLRenderSystem::endScene()
{
}
//////////////////////////////////////////////////////////////////////////
void OGLRenderSystem::beginLayer2D()
{
	glEnable2D();
}
//////////////////////////////////////////////////////////////////////////
void OGLRenderSystem::endLayer2D()
{
	glDisable2D();
	//нах?
	//m_layer -= 0.001f;
}
//////////////////////////////////////////////////////////////////////////
void OGLRenderSystem::renderText(const Menge::String & _text, const float * _pos, unsigned long _color)
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
void OGLRenderSystem::setFullscreenMode( std::size_t _width, std::size_t _height, bool _fullscreen )
{
	int flags = SDL_OPENGL | SDL_RESIZABLE;

	if (_fullscreen)
	{
		flags |= SDL_FULLSCREEN;
	}

	m_screen = SDL_SetVideoMode(_width, _height, m_screen->format->BitsPerPixel, flags);
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
}
//////////////////////////////////////////////////////////////////////////
void OGLRenderSystem::setRenderTarget( const Menge::String& _name, bool _clear )
{
	assert(0);
/*	TTargetMap::iterator it = m_targetMap.find( _name );
	if( it != m_targetMap.end() )
	{
		if( m_inRender )
		{
			m_hge->Gfx_EndScene( false );
		}
		else
		{
			m_inRender = true;
		}
		m_currentRenderTarget = _name;
		m_hge->Gfx_BeginScene( it->second.handle );
		if( it->second.dirty && _clear )
		{
			m_hge->Gfx_Clear( m_clearColor );
			it->second.dirty = false;
		}

	}
	else
	{
		MENGE_LOG_WARNING( "Warning: Invalid Render Target name \"%s\""
			, _name.c_str() );
	}
*/
/*	TTargetMap::iterator it = m_targetMap.find( _name );
	if( it != m_targetMap.end() )
	{
		if( m_inRender )
		{
			endScene();
		}
		else
		{
			m_inRender = true;
		}

		m_currentRenderTarget = _name;

		if (it->second.texture!=0)
		{
			glBindTexture(GL_TEXTURE_2D, it->second.texture->getGLTexture());

			// Copy Our ViewPort To The Texture
			glCopyTexSubImage2D(GL_TEXTURE_2D, 0, 0, 0, 0, 0,
				it->second.texture->getWidth(),
				it->second.texture->getHeight());

			glViewport(0, 0, m_screen->w, m_screen->h);
		}

		glViewport(0, 0, it->second.texture->getWidth(), it->second.texture->getHeight());

		beginScene();
	}*/
}
//////////////////////////////////////////////////////////////////////////
void OGLRenderSystem::setRenderArea( const float* _renderArea )
{
}
//////////////////////////////////////////////////////////////////////////
