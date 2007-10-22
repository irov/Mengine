#	include "OgreRenderFont.h"
//////////////////////////////////////////////////////////////////////////
OgreRenderFont::OgreRenderFont( const std::string&  _name )
	: m_description( _name )
{
	m_font = Ogre::FontManager::getSingletonPtr()->getByName( m_description );
	m_font->load();
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
	static float standartHeight = 0.02f;
	return m_font->getGlyphAspectRatio( _id ) * standartHeight * 2.0f;
}
//////////////////////////////////////////////////////////////////////////
float OgreRenderFont::getHeight() const
{
	static float standartHeight = 0.02f;
	return standartHeight;
}
//////////////////////////////////////////////////////////////////////////