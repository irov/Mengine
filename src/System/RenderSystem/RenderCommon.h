#	pragma once

#	include "../../Menge/RenderSystemInterface.h"

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
