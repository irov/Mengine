#	pragma once

#	include "Config/Typedef.h"
#	include "hge.h"

class HGERenderFont
{
public:
	HGERenderFont(const Menge::String& _name, HGE * _hge);
	~HGERenderFont();

	void renderText( float _x, float _y, unsigned long _color, const Menge::String& _text);

private:
	HGE * m_hge;
	float m_height;
	HTEXTURE	m_texture;

	struct CHAR_DESC
	{
		int x, y, w, h;
		int a, c;
	};

	CHAR_DESC	m_letters[256];
	
	HTEXTURE _fontGenerate( const Menge::String& _name, int _size, bool _bold, bool _italic, bool _antialias, int _leftRange, int _rigthRange);
	bool _placeSymbols( int _width, int _height, int leftRange,  int rigthRange );
};