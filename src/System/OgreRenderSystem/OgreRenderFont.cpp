#	include "OgreRenderFont.h"
//////////////////////////////////////////////////////////////////////////
OgreRenderFont::OgreRenderFont( const Ogre::FontPtr & _font, const Ogre::String & _name )
: m_font(_font)
, m_description( _name )
{
	//Ogre::FontManager::getSingletonPtr()->load( m_description, "fonts");
	
	//Ogre::FontManager::getSingletonPtr()->load("Fontik.fontdef" ,"Fonts");
	//Ogre::FontManager::getSingletonPtr()->getByHandle(
}
//////////////////////////////////////////////////////////////////////////
OgreRenderFont::~OgreRenderFont()
{
	Ogre::FontManager::getSingletonPtr()->remove( m_description );
}
//////////////////////////////////////////////////////////////////////////
Ogre::Font * OgreRenderFont::getFont() const
{
	return m_font.getPointer();
}
//////////////////////////////////////////////////////////////////////////
float OgreRenderFont::getCharWidth( char _id ) const
{
	static float standartHeight = 12.0f;
	return m_font->getGlyphAspectRatio( _id ) * standartHeight * 2.0f;
}
//////////////////////////////////////////////////////////////////////////
float OgreRenderFont::getHeight() const
{
	static float standartHeight = 12.0f;
	return standartHeight;
}
//////////////////////////////////////////////////////////////////////////