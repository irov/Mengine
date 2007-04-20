#	pragma once

#	include	"RenderCommon.h"

class	D3D9RenderImage 
	: public RenderImageInterface
{
public:
	D3D9RenderImage(LPDIRECT3DDEVICE9 _dev, const TextureDesc& _desc);
	~D3D9RenderImage();

public:
	float	getWidth() const;
	float	getHeight() const;

public:
	D3D9Vertex*	_getD3D9V4();
	LPDIRECT3DTEXTURE9	_getTexPointer() const;
	bool				_isAlpha() const;

private:
	float				m_width;
	float				m_height;
	bool				m_haveAlpha;
	D3D9Vertex			m_verts[4];
	LPDIRECT3DTEXTURE9	m_texPointer;
};