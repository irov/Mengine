#	pragma once

#	include "Interface/RenderSystemInterface.h"

#	include "Ogre.h"

#	include "OgreFont.h"
#	include "OgreFontManager.h"

class OgreRenderFont
	: public RenderFontInterface
{
public:
	OgreRenderFont( const std::string&  _name );
	~OgreRenderFont();

public:
	Ogre::Font * getFont();

	const std::string& getID();
	
private:
	Ogre::FontPtr m_font;
	Ogre::String m_ID;
};