#	pragma once
#	include "Interface\RenderSystemInterface.h"
#	include <d3d9.h>

class D3D9VertexData : public VertexData
{
public:
    D3D9VertexData(IDirect3DDevice9 * _pd3dDevice);
	~D3D9VertexData();
	void	createVertexBuffer(size_t _count, size_t _vertexSizeInBytes, size_t _start = 0);
	void	setVertexDeclaration(VertexDeclaration * _vd);
	size_t	getVertexSize() const;
	size_t	getVertexStart() const;		
	size_t	getVertexCount() const;
	void	lock(float* &_vertices, size_t _offset, size_t _count, size_t _vertexSizeInBytes);
	void	unlock();
public:
	LPDIRECT3DVERTEXBUFFER9	_get() const;
	LPDIRECT3DVERTEXDECLARATION9 _getVertexDecl() const;
private:
	IDirect3DDevice9 * m_pd3dDevice;
	IDirect3DVertexBuffer9 * m_VB;
	VertexDeclaration * m_VD;
	size_t m_vertexStart;
	size_t m_vertexSize;
	size_t m_vertexCount;
};