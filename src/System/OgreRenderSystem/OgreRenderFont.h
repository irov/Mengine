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

public:
	Ogre::Font * getFont();

private:
	Ogre::Font* m_font;
};