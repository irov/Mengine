#	pragma once
#	include "Interface\RenderSystemInterface.h"
#	include <d3d9.h>

class	d3d9Texture : public Texture
{
public:
	d3d9Texture(IDirect3DTexture9* _pointer);
	~d3d9Texture();
	IDirect3DTexture9*	_get() const;
public:
	int		getWidth() const;
	int		getHeight() const;
	bool	_isAlpha() const;
private:
	IDirect3DTexture9*	m_texture;
	int	m_width;
	int	m_height;
};