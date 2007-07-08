#	pragma once
#	include "..\..\Menge\RenderSystemInterface.h"
#	include <d3d9.h>
#	include <d3dx9.h>
#	include <vector>

class d3d9VertexDeclaration : public VertexDeclaration
{
public:
	d3d9VertexDeclaration(IDirect3DDevice9 * pd3dDevice);
	~d3d9VertexDeclaration();
	void	insert(size_t stream, size_t offset, DECLTYPE type, DECLUSAGE usage, size_t index);
public:
	LPDIRECT3DVERTEXDECLARATION9 _get() const;
private:
	mutable LPDIRECT3DVERTEXDECLARATION9 m_pVertexDeclaration;
	IDirect3DDevice9 * m_pd3dDevice;
	std::vector<D3DVERTEXELEMENT9>	m_decl;
	mutable bool	m_isChanged;
};
