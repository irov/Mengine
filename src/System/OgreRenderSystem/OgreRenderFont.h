#	pragma once

#	include "Interface/RenderSystemInterface.h"

#	include "Ogre.h"

#	include "OgreFontManager.h"

class OgreRenderFont
	: public RenderFontInterface
{
public:
	OgreRenderFont( const std::string&  _name );
	~OgreRenderFont();

	float getCharWidth( char _id ) const;
	float getHeight() const;

public:
	Ogre::Font * getFont() const;
	
private:
	Ogre::FontPtr m_font;
	Ogre::String m_description;
};