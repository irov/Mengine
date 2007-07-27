#	pragma once
#	include "Interface\WinRenderSystemInterface.h"

#	include <vector>
#	include <list>

#	include <d3d9.h>
#	include <d3dx9.h>

struct D3D9Vertex
{
	mt::vec3f pos;
	float rhw;
	D3DCOLOR color;   
	mt::vec2f tcoor;
};

class	D3D9BatchRender
{
public:
	void	init(IDirect3DDevice9 * _dev);
	void	destroy();
	void	insertVertices(D3D9Vertex * _begin, D3D9Vertex * _end);
	void	begin(RenderImageInterface* _rmi, int _blend);
	void	flush();
	void	end();
private:
	LPDIRECT3DVERTEXBUFFER9	m_batchVertexBuffer;
	LPDIRECT3DINDEXBUFFER9	m_batchIndexBuffer;
	UINT					m_size1Vert;

	static const DWORD D3DFVF_TLVERTEX = D3DFVF_XYZRHW | D3DFVF_DIFFUSE | D3DFVF_TEX1;
	static const int BATCH_BUFFER_SIZE = 9000;

	std::vector<D3D9Vertex>	m_vertices;

	struct Batch
	{
		size_t	begin;
		size_t	end;
		int		blend;
		RenderImageInterface * image;
	};

	std::list<Batch> m_batches;

	IDirect3DDevice9 * m_pID3DDevice;

	void	_newBatch(size_t _begin, RenderImageInterface * _rmi, int _blend);
};