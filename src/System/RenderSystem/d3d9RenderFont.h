#	pragma once
#	include "Interface\RenderSystemInterface.h"
#	include <d3d9.h>
#	include <d3dx9.h>

#	include	"d3d9RenderImage.h"

class	D3D9Font 
	: public RenderFontInterface
{
public:
	D3D9Font(RenderSystemInterface * _rs, LPDIRECT3DDEVICE9 dev, const FontDesc&	_fontDesc);
	~D3D9Font();

public:
	float			getHeight() const;
    unsigned int	getColor() const;
	float			getCharWidth(char id) const;
public:
	FontCharDesc&		_getChar(char id);
	RenderImageInterface*	getRenderImage() const; 	
private:
	RenderImageInterface*	m_rmi;
	FontCharDesc		m_chars[256];
	float				m_height;
	unsigned int		m_color;
};