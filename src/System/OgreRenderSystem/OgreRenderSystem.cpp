#	include "OgreRenderSystem.h"

#	include "OgreRenderSpriteManager.h"
#	include "OgreRenderImage.h"
#	include "OgreRenderFont.h"

//////////////////////////////////////////////////////////////////////////
bool initInterfaceSystem( RenderSystemInterface ** _ptrInterface )
{
	try
	{
		*_ptrInterface = new OgreRenderSystem();
	}
	catch (...)
	{
		return false;
	}

	return true;
}
//////////////////////////////////////////////////////////////////////////
void releaseInterfaceSystem( RenderSystemInterface* _ptrInterface )
{
	delete static_cast<OgreRenderSystem*>(_ptrInterface);
}
//////////////////////////////////////////////////////////////////////////
OgreRenderSystem::OgreRenderSystem()
	: m_root(0)
	, m_renderWindow(0)
	, m_viewport(0)
	, m_sceneMgr(0)
	, m_spriteMgr(0)
{
}
//////////////////////////////////////////////////////////////////////////
OgreRenderSystem::~OgreRenderSystem()
{
	m_spriteMgr->end();
	delete m_spriteMgr;
}
//////////////////////////////////////////////////////////////////////////
bool OgreRenderSystem::init( Ogre::Root * _root, Ogre::RenderWindow * _renderWindow )
{
	m_root = _root;
	m_renderWindow = _renderWindow;

	m_sceneMgr = m_root->createSceneManager( Ogre::ST_GENERIC, "defaultSceneManager" );
	m_renderSys = m_root->getRenderSystem();

	Ogre::Camera * camera = m_sceneMgr->createCamera("defaultCamera");
 	camera->setNearClipDistance(5);

 	m_viewport = m_renderWindow->addViewport( camera );
 	
	float width = m_viewport->getActualWidth();
	float height = m_viewport->getActualHeight();
	float aspect = width / height;

 	camera->setAspectRatio( aspect );

 	Ogre::TextureManager::getSingleton().setDefaultNumMipmaps(0);

	m_spriteMgr = new OgreRenderSpriteManager();
	m_spriteMgr->init( m_sceneMgr, m_renderSys, m_viewport, Ogre::RENDER_QUEUE_OVERLAY, true);

	return true;
}
//////////////////////////////////////////////////////////////////////////
void OgreRenderSystem::render()
{
	//m_renderWindow->update();
	//m_root->renderOneFrame();
}
//////////////////////////////////////////////////////////////////////////
void OgreRenderSystem::setProjectionMatrix( const float * _projection )
{
	m_renderSys->_setProjectionMatrix( *(Ogre::Matrix4 *)_projection );
}
//////////////////////////////////////////////////////////////////////////
void OgreRenderSystem::setViewMatrix( const float * _view )
{
	m_renderSys->_setViewMatrix( *(Ogre::Matrix4 *)_view );
}
//////////////////////////////////////////////////////////////////////////
void OgreRenderSystem::setWorldMatrix( const float * _world )
{
	m_renderSys->_setWorldMatrix( *(Ogre::Matrix4 *)_world );
}
//////////////////////////////////////////////////////////////////////////
RenderImageInterface* OgreRenderSystem::loadImage( const TextureDesc&	_desc )
{
	return new OgreRenderImage( _desc );
}
//////////////////////////////////////////////////////////////////////////
void OgreRenderSystem::releaseImage( RenderImageInterface* _rii )
{
	delete static_cast<OgreRenderImage*>( _rii );
}
//////////////////////////////////////////////////////////////////////////
void OgreRenderSystem::renderImage(		
				 const float * _transform, 
				 const float * _offset,
				 const float * _uv,
				 const float * _size,
				 unsigned int _mixedColor, 
				 const RenderImageInterface* _image)
{
	const OgreRenderImage * image = static_cast<const OgreRenderImage *>( _image );
	Ogre::Texture * texture = image->getTexture();

	m_spriteMgr->spriteBltFull( texture
		, *(Ogre::Matrix3*)_transform
		, *(Ogre::Vector2*)_offset
		, *(Ogre::Vector4*)_uv
		, *(Ogre::Vector2*)_size );
}
//////////////////////////////////////////////////////////////////////////
RenderFontInterface* OgreRenderSystem::loadFont( const FontDesc & _desc )
{
	Ogre::String name( _desc.name ); 

	Ogre::FontPtr font = Ogre::FontManager::getSingletonPtr()->getByName( name );

	if( font.isNull() )
	{
		return 0;
	}
	
	font->load();
	
	return new OgreRenderFont( font, name );
}
//////////////////////////////////////////////////////////////////////////
void	OgreRenderSystem::releaseRenderFont( RenderFontInterface* _fnt )
{
	delete static_cast<OgreRenderFont*>(_fnt);
}
//////////////////////////////////////////////////////////////////////////
void	OgreRenderSystem::renderText( const RenderFontInterface* _font, const char * _text, const float * _transform )
{
	const OgreRenderFont * font = static_cast<const OgreRenderFont *>( _font );	
	Ogre::Font * ogreFont = font->getFont();

	float width = m_viewport->getActualWidth();
	float heigth = m_viewport->getActualHeight();

	float left = _transform[6];
	float top = _transform[7];

	float spaceWidth = 0;

	float charHeight = font->getHeight();

	RenderSprite spr;

	if ( spaceWidth == 0 )
	{
		spaceWidth = ogreFont->getGlyphAspectRatio( 'A' ) * charHeight * 2.0;
	}

	Ogre::String textureName = ogreFont->getName() + "Texture";

	Ogre::ResourcePtr texPtr = Ogre::TextureManager::getSingletonPtr()
		->getByName(textureName);

	if( texPtr.isNull() )
	{
		printf("Warning: render font can't find texture '%s'\n"
			, textureName.c_str()
			);

		return;
	}

	Ogre::Texture * tex = static_cast<Ogre::Texture *>( texPtr.getPointer() );

	std::string	caption(_text);

	for( std::string::const_iterator i = caption.begin(); i != caption.end(); ++i )
	{
		if ( *i == ' ' )
		{
			left += spaceWidth;
			continue;
		}

		spr.texHandle = tex->getHandle();

		float aspectRatio = ogreFont->getGlyphAspectRatio( *i ) ;

		const Ogre::Font::UVRect & rect = ogreFont->getGlyphTexCoords( *i );
 
		Ogre::Vector4	uv( rect.left, rect.top, rect.right, rect.bottom );
 
		float height = charHeight * 2.0;
		float width = aspectRatio * height;

		Ogre::Vector2	offset( left, top );
		Ogre::Vector2	size( width, height );

		m_spriteMgr->spriteBltFull( tex, *(Ogre::Matrix3*)_transform, offset, uv, size );

		left += width;
	}
}
//////////////////////////////////////////////////////////////////////////
void	OgreRenderSystem::beginLayer()
{
	m_spriteMgr->beginLayer();
}
//////////////////////////////////////////////////////////////////////////
void	OgreRenderSystem::endLayer()
{
	m_spriteMgr->endLayer();
}
//////////////////////////////////////////////////////////////////////////