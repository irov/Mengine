#	include "d3d9VertexDeclaration.h"
#	include <assert.h>

d3d9VertexDeclaration::d3d9VertexDeclaration(IDirect3DDevice9* pd3dDevice)
	: m_pd3dDevice(pd3dDevice)
	, m_pVertexDeclaration(0)
	, m_isChanged(true)
{}

d3d9VertexDeclaration::~d3d9VertexDeclaration()
{
	if(m_pVertexDeclaration)
	{
		m_pVertexDeclaration->Release(); 
		m_pVertexDeclaration = NULL;
	}
}

void	d3d9VertexDeclaration::insert(size_t stream, size_t offset, DECLTYPE type, DECLUSAGE usage, size_t index)
{
	D3DDECLTYPE _type = D3DDECLTYPE_FLOAT3;
	D3DDECLUSAGE _usage = D3DDECLUSAGE_POSITION;

	if(type == DECLFLOAT2) _type = D3DDECLTYPE_FLOAT2;
	if(type == DECLFLOAT4) _type = D3DDECLTYPE_FLOAT4;

	if(usage == DECLTEX) _usage = D3DDECLUSAGE_TEXCOORD;
	if(usage == DECLNRM) _usage = D3DDECLUSAGE_NORMAL;
	if(usage == DECLCOL) _usage = D3DDECLUSAGE_COLOR;

	D3DVERTEXELEMENT9 elem = {stream, offset, _type, D3DDECLMETHOD_DEFAULT, _usage, index};
	m_decl.push_back(elem);
	m_isChanged = true;
}

LPDIRECT3DVERTEXDECLARATION9 d3d9VertexDeclaration::_get() const
{
	if(m_decl.empty())
	{
		assert(!"declaration is empty! minimum must be POSITION setted");
	}

	if(m_isChanged)
	{
		std::vector<D3DVERTEXELEMENT9>	decl;

		std::copy(m_decl.begin(),m_decl.end(),std::back_inserter(decl));
		D3DVERTEXELEMENT9 end = D3DDECL_END();
		decl.push_back(end);
		m_pd3dDevice->CreateVertexDeclaration( &*decl.begin(), &m_pVertexDeclaration );
		m_isChanged = false;
	}

	return m_pVertexDeclaration;
}