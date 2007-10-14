#	include "OgreRenderFont.h"

//////////////////////////////////////////////////////////////////////////
OgreRenderFont::OgreRenderFont( const std::string&  _name )
{
	m_font = dynamic_cast<Ogre::Font*>(Ogre::FontManager::getSingleton().getByName(_name).getPointer());

	if ( m_font == NULL )
	{
		assert(0);
	}

	m_font->load();
}
//////////////////////////////////////////////////////////////////////////
Ogre::Font * OgreRenderFont::getFont()
{
	return m_font;
}
//////////////////////////////////////////////////////////////////////////