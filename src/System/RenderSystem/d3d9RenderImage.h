#	pragma once

#	include "Interface\RenderSystemInterface.h"
#	include <d3d9.h>
#	include <d3dx9.h>

struct D3D9Vertex
{
	mt::vec3f position;
	float rhw;
	D3DCOLOR color;   
	mt::vec2f tcoor;
};

const DWORD D3DFVF_TLVERTEX = D3DFVF_XYZRHW | D3DFVF_DIFFUSE | D3DFVF_TEX1;

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