#	pragma once

#	include "interfaces.h"

#	include <d3d9.h>
#	include <d3dx9.h>

#	include "math/vec3.h"

struct D3D9Vertex
{
	mt::vec3f	position;
	float rhw;
    D3DCOLOR		mColor;   
	mt::vec2f	tcoor;
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
	const D3D9Vertex*	_getD3D9V4() const;
	LPDIRECT3DTEXTURE9	_getTexPointer() const;
	bool				_isAlpha() const;

private:
	float				mWidth;
	float				mHeight;
	bool				mHaveAlpha;
	D3D9Vertex			mVertices[4];
	LPDIRECT3DTEXTURE9	mTexPointer;
};