#	include "SDLRenderSystem.h"
#	include <SDL_syswm.h>

//////////////////////////////////////////////////////////////////////////
bool initInterfaceSystem( Menge::RenderSystemInterface ** _ptrInterface )
{
	try
	{
		*_ptrInterface = new SDLRenderSystem();
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
	delete static_cast<SDLRenderSystem*>(_ptrInterface);
}
//////////////////////////////////////////////////////////////////////////
SDLRenderSystem::SDLRenderSystem()
: m_inRender( false )
, m_layer( 1.0f )
{
}
//////////////////////////////////////////////////////////////////////////
SDLRenderSystem::~SDLRenderSystem()
{
	if (m_screen)
	{
		SDL_FreeSurface(m_screen);
	}

	SDL_Quit();
}
//////////////////////////////////////////////////////////////////////////
void SDLRenderSystem::swapBuffers()
{
	SDL_GL_SwapBuffers();
}
//////////////////////////////////////////////////////////////////////////
int SDLRenderSystem::getNumDIP() const
{
	return 1;
}
//////////////////////////////////////////////////////////////////////////
bool SDLRenderSystem::initialize( Menge::LogSystemInterface* _logSystem )
{
	bool initialized = true;

	Uint32 sdlFlags = SDL_INIT_VIDEO | SDL_INIT_TIMER;

	if(SDL_Init(sdlFlags) < 0)
	{
		return false;
	}

	return initialized;
}
//////////////////////////////////////////////////////////////////////////
bool SDLRenderSystem::createRenderWindow( std::size_t _width, std::size_t _height, int _bits, bool _fullscreen, Menge::WindowHandle _winHandle, int _FSAAType, int _FSAAQuality )
{
	bool status = true;  
	
	if(_bits == -1)
	{
		_bits = SDL_GetVideoInfo()->vfmt->BitsPerPixel;  
	}

	int rgb_size[3];

	if(_bits == 8)
	{
		rgb_size[0] = 3;
		rgb_size[1] = 3;
		rgb_size[2] = 2;
	}
	else if(_bits == 15 || _bits == 16)
	{
		rgb_size[0] = 5;
		rgb_size[1] = 5;
		rgb_size[2] = 5;
	//	SDL_GL_SetAttribute(SDL_GL_ALPHA_SIZE, 1);
	} 
	else
	{
		rgb_size[0] = 8;
		rgb_size[1] = 8;
		rgb_size[2] = 8;
	//	SDL_GL_SetAttribute(SDL_GL_ALPHA_SIZE, 8);
	}

	SDL_GL_SetAttribute(SDL_GL_RED_SIZE, rgb_size[0]);
	SDL_GL_SetAttribute(SDL_GL_GREEN_SIZE, rgb_size[1]);
	SDL_GL_SetAttribute(SDL_GL_BLUE_SIZE, rgb_size[2]);

	SDL_GL_SetAttribute(SDL_GL_DOUBLEBUFFER, 1);
	SDL_GL_SetAttribute(SDL_GL_DEPTH_SIZE, _bits);   

	if(_FSAAQuality > 1) 
	{
		SDL_GL_SetAttribute(SDL_GL_MULTISAMPLEBUFFERS,1);
		SDL_GL_SetAttribute(SDL_GL_MULTISAMPLESAMPLES,_FSAAQuality);
	}

	if (_bits == 32)
	{
		SDL_GL_SetAttribute(SDL_GL_STENCIL_SIZE, 8);
	}

	int SDLflags = SDL_OPENGL;

	if (_fullscreen)
	{
		SDLflags |= SDL_FULLSCREEN;
	}

	m_screen = SDL_SetVideoMode(_width, _height, _bits, SDLflags);

	if (!_fullscreen)
	{
		SDL_WM_SetCaption("SDL TEST", 0);
	}

	if(!m_screen)
	{
		SDL_Quit();
		return false;
	}


	glMatrixMode(GL_PROJECTION);
	glPushMatrix();
	glLoadIdentity();

	glOrtho(0, (GLfloat)m_screen->w, 0, (GLfloat)m_screen->h, -1, 1);
	glMatrixMode(GL_MODELVIEW);
	glPushMatrix();
	glLoadIdentity();

	glPushAttrib(GL_ENABLE_BIT);
	glDisable(GL_DEPTH_TEST);
	glDisable(GL_CULL_FACE);
	glEnable(GL_TEXTURE_2D);

	glEnable(GL_BLEND);
	glBlendFunc(GL_SRC_ALPHA,GL_ONE_MINUS_SRC_ALPHA);

	return true;
}



//////////////////////////////////////////////////////////////////////////
const std::vector<int> & SDLRenderSystem::getResolutionList()
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
void SDLRenderSystem::screenshot( Menge::RenderImageInterface* _image, const float * _rect /*= 0 */ )
{
}
//////////////////////////////////////////////////////////////////////////
void SDLRenderSystem::render()
{
}
//////////////////////////////////////////////////////////////////////////
void SDLRenderSystem::setContentResolution( const float * _resolution )
{
}
//////////////////////////////////////////////////////////////////////////
void SDLRenderSystem::setProjectionMatrix( const float * _projection )
{
}
//////////////////////////////////////////////////////////////////////////
void SDLRenderSystem::setViewMatrix( const float * _view )
{
}
//////////////////////////////////////////////////////////////////////////
void SDLRenderSystem::setWorldMatrix( const float * _world )
{
}
//////////////////////////////////////////////////////////////////////////
Menge::RenderImageInterface * SDLRenderSystem::createImage( const Menge::String & _name,
														   float _width, float _height )
{
	SDLTexture * texture = new SDLTexture( _name, ::floorf( _width + 0.5f ), ::floorf( _height + 0.5f ) );
	m_textureMap.insert( std::make_pair( _name, static_cast<Menge::RenderImageInterface*>( texture ) ) );
	texture->incRef();
	return texture;
}
//////////////////////////////////////////////////////////////////////////
Menge::RenderImageInterface * SDLRenderSystem::createRenderTargetImage( const Menge::String & _name, float _width, float _height )
{
	SDLTexture* texture = new SDLTexture( _name, _width, _height );
	RenderTargetInfo rtgtInfo;
	rtgtInfo.dirty = true;
	rtgtInfo.texture = texture;
	m_targetMap.insert( std::make_pair( _name, rtgtInfo ) );
	return texture;
	return NULL;
}
//////////////////////////////////////////////////////////////////////////
Menge::RenderImageInterface * SDLRenderSystem::loadImage( const Menge::TextureDesc& _desc )
{
	SDLTexture * sdl_texture = new SDLTexture();
	sdl_texture->load(_desc);
	m_textureMap.insert( std::make_pair( _desc.name, static_cast<Menge::RenderImageInterface*>( sdl_texture ) ) );
	sdl_texture->incRef();

	return sdl_texture;
}
//////////////////////////////////////////////////////////////////////////
void SDLRenderSystem::releaseImage( Menge::RenderImageInterface * _image )
{
	SDLTexture * texture = static_cast<SDLTexture*>( _image );

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
Menge::RenderImageInterface* SDLRenderSystem::getImage( const Menge::String& _desc ) const 
{
	TTextureMap::const_iterator it = m_textureMap.find( _desc );
	if( it != m_textureMap.end() )
	{
		static_cast<SDLTexture*>( it->second )->incRef();
		return it->second;
	}
	return NULL;
}
//////////////////////////////////////////////////////////////////////////
void SDLRenderSystem::renderImage(const float * _renderVertex, 
								  const float * _uv, 
								  unsigned int _color,  
								  const Menge::RenderImageInterface * _image, 
								  Menge::EBlendFactor _srcBlend, 
								  Menge::EBlendFactor _dstBlend )
{
	const SDLTexture * tex = static_cast<const SDLTexture*>( _image );

	if(tex)
	{

		glBindTexture(GL_TEXTURE_2D, tex->getGLTexture());

		int r = (_color>>16) & 0xff;
		int g = (_color>>8) & 0xff;
		int b = (_color) & 0xff;
		int a = (_color>>24) & 0xff;

		glBegin(GL_QUADS);
		
		glColor4f(r/255.f,g/255.f,b/255.f,1.f);

		glTexCoord2f(_uv[0],_uv[1]);
		glVertex3f(_renderVertex[0], _renderVertex[1], m_layer);

		glTexCoord2f(_uv[2],_uv[1]);
		glVertex3f(_renderVertex[2], _renderVertex[3], m_layer);

		glTexCoord2f(_uv[2],_uv[3]);
		glVertex3f(_renderVertex[4], _renderVertex[5], m_layer);

		glTexCoord2f(_uv[0],_uv[3]);
		glVertex3f(_renderVertex[6], _renderVertex[7], m_layer);

		glEnd();
	}
}

//////////////////////////////////////////////////////////////////////////
void SDLRenderSystem::renderTriple(
								   const float * _a, 
								   const float * _b, 
								   const float * _c, 
								   const float * _uv0, 
								   const float * _uv1,
								   const float * _uv2,
								   unsigned int _color,  
								   const Menge::RenderImageInterface * _image, 
								   Menge::EBlendFactor _src, 
								   Menge::EBlendFactor _dst )
{
	const SDLTexture * tex = static_cast<const SDLTexture*>( _image );

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
	glVertex2f(_b[2], _b[3]);

	glTexCoord2f(_uv2[0],_uv2[1]);
	glVertex2f(_c[4], _c[5]);

	glEnd();
}
//////////////////////////////////////////////////////////////////////////
void SDLRenderSystem::renderLine( unsigned int _color, 
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
void SDLRenderSystem::beginScene()
{
	m_layer = 1.0f;

/*	bool zBuffer = false;

	GLbitfield mask = 0;

	glClearColor(1,1,1,1);

	mask |= GL_COLOR_BUFFER_BIT;
	
	if (zBuffer)
	{
		glDepthMask(GL_TRUE);
		mask |= GL_DEPTH_BUFFER_BIT;
	}

	glClear(mask);
*/
	glClearColor(1,1,1,1.0f);
	glClear(GL_COLOR_BUFFER_BIT);
	glLoadIdentity();

//	glLoadIdentity();
}
//////////////////////////////////////////////////////////////////////////
void SDLRenderSystem::endScene()
{
}
//////////////////////////////////////////////////////////////////////////
void SDLRenderSystem::beginLayer2D()
{
}
//////////////////////////////////////////////////////////////////////////
void SDLRenderSystem::endLayer2D()
{
	m_layer -= 0.001f;
}
//////////////////////////////////////////////////////////////////////////
void SDLRenderSystem::renderText(const Menge::String & _text, const float * _pos, unsigned long _color)
{
}
//////////////////////////////////////////////////////////////////////////
void SDLRenderSystem::beginLayer3D()
{
}
//////////////////////////////////////////////////////////////////////////
void SDLRenderSystem::endLayer3D()
{

}
//////////////////////////////////////////////////////////////////////////
void SDLRenderSystem::setFullscreenMode( std::size_t _width, std::size_t _height, bool _fullscreen )
{
	int flags = SDL_OPENGL | SDL_RESIZABLE;

	if (_fullscreen)
	{
		flags |= SDL_FULLSCREEN;
	}

	m_screen = SDL_SetVideoMode(_width, _height, m_screen->format->BitsPerPixel, flags);
}
//////////////////////////////////////////////////////////////////////////
Menge::CameraInterface * SDLRenderSystem::createCamera( const Menge::String & _name )
{
	return NULL;
}
//////////////////////////////////////////////////////////////////////////
Menge::EntityInterface * SDLRenderSystem::createEntity( const Menge::String & _name, const Menge::String & _meshName )
{
	return NULL;
}
//////////////////////////////////////////////////////////////////////////
Menge::LightInterface * SDLRenderSystem::createLight( const Menge::String & _name )
{
	return NULL;
}
//////////////////////////////////////////////////////////////////////////
void SDLRenderSystem::releaseCamera( Menge::CameraInterface * _camera )
{

}
//////////////////////////////////////////////////////////////////////////
void SDLRenderSystem::releaseEntity( Menge::EntityInterface * _entity )
{

}
//////////////////////////////////////////////////////////////////////////
void SDLRenderSystem::releaseLight( Menge::LightInterface * _light )
{

}
//////////////////////////////////////////////////////////////////////////
void SDLRenderSystem::setTextureFiltering( bool _filter )
{

}
//////////////////////////////////////////////////////////////////////////
void SDLRenderSystem::onWindowMovedOrResized()
{

}
//////////////////////////////////////////////////////////////////////////
void SDLRenderSystem::onWindowClose()
{
}
//////////////////////////////////////////////////////////////////////////
void SDLRenderSystem::renderMesh( const Menge::TVertex* _vertices, std::size_t _verticesNum,
								 const Menge::uint16*	_indices, std::size_t _indicesNum,
								 Menge::TMaterial* _material )
{
}
//////////////////////////////////////////////////////////////////////////
void SDLRenderSystem::setRenderTarget( const Menge::String& _name, bool _clear )
{
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
	TTargetMap::iterator it = m_targetMap.find( _name );
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
	}
}
//////////////////////////////////////////////////////////////////////////
void SDLRenderSystem::setRenderArea( const float* _renderArea )
{
}
//////////////////////////////////////////////////////////////////////////
