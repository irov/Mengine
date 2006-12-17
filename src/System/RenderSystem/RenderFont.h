#	pragma once

#	include "interfaces.h"
#	include "RenderImage.h"

class	D3D9Font 
	: public RenderFontInterface
{
public:
	D3D9Font(LPDIRECT3DDEVICE9 dev, const FontDesc&	_fontDesc);
	~D3D9Font();

public:
	float			getHeight() const;
    unsigned int	getColor() const;
	float			getCharWidth(char id) const;
public:
	FontCharDesc&		_getChar(char id);
	LPDIRECT3DTEXTURE9	_getTexPointer() const;
private:
	FontCharDesc		m_chars[256];
	LPDIRECT3DTEXTURE9	m_texPointer;
	unsigned int		m_height;
	unsigned int		m_color;
};