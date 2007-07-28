#	pragma once

#	include "Interface\RenderSystemInterface.h"
#	include <d3d9.h>
#	include <d3dx9.h>

class	D3D9RenderImage 
	: public RenderImageInterface
{
public:
	D3D9RenderImage(LPDIRECT3DTEXTURE9 _texPointer, const TextureDesc& _desc);
	~D3D9RenderImage();

public:
	int	getWidth() const;
	int	getHeight() const;

public:
	LPDIRECT3DTEXTURE9	_get() const;
	bool				_isAlpha() const;

private:
	int				m_width;
	int				m_height;
	bool				m_haveAlpha;
	LPDIRECT3DTEXTURE9	m_texPointer;
};