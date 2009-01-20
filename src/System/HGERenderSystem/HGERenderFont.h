#	pragma once

#	include "Config/Typedef.h"
#	include "hge.h"

class HGERenderFont
{
public:
	HGERenderFont(HGE * _hge);
	~HGERenderFont();

	void fontGenerate( const Menge::String& _name, int _size, bool _bold, bool _italic, bool _antialias);
	void renderText( float _x, float _y, unsigned long _color, const Menge::String& _text);

private:
	HGE * m_hge;
	float m_height;
	HTEXTURE m_texture;

	struct CharDesc
	{
		float x;
		float y;
		float w;
		float h;
		float a;
		float c;
		float u0;
		float v0;
		float u1;
		float v1;
	};

	//FIX ME:
	CharDesc	m_letters[256];
	
	HTEXTURE _fontGenerate( const Menge::String& _name, int _size, bool _bold, bool _italic, bool _antialias, int _leftRange, int _rigthRange);
	bool _placeSymbols( int _width, int _height, int leftRange,  int rigthRange );
};
