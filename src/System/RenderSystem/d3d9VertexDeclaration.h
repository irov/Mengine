#	pragma once
#	include "Interface\RenderSystemInterface.h"
#	include <d3d9.h>
#	include <vector>

class D3D9VertexDeclaration
	: public VertexDeclaration
{
public:
	D3D9VertexDeclaration(IDirect3DDevice9 * _pd3dDevice);
	~D3D9VertexDeclaration();
	void	insert(size_t _stream, size_t _offset, eDeclType _type, eDeclUsage _usage, size_t _index);
public:
	LPDIRECT3DVERTEXDECLARATION9 _get() const;
private:
	mutable LPDIRECT3DVERTEXDECLARATION9 m_pVertexDeclaration;
	IDirect3DDevice9 * m_pd3dDevice;
	std::vector<D3DVERTEXELEMENT9>	m_decl;
	mutable bool   	m_isChanged;
};      