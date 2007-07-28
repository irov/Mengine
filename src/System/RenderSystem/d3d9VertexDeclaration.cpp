#	include "d3d9VertexDeclaration.h"
#	include <assert.h>
//////////////////////////////////////////////////////////////////////////
D3D9VertexDeclaration::D3D9VertexDeclaration(IDirect3DDevice9 * _pd3dDevice)
	: m_pd3dDevice(_pd3dDevice)
	, m_pVertexDeclaration(0)
	, m_isChanged(true)
{}
//////////////////////////////////////////////////////////////////////////
D3D9VertexDeclaration::~D3D9VertexDeclaration()
{
	if(m_pVertexDeclaration)
	{
		m_pVertexDeclaration->Release(); 
		m_pVertexDeclaration = NULL;
	}
}
//////////////////////////////////////////////////////////////////////////
void	D3D9VertexDeclaration::insert(size_t _stream, size_t _offset, eDeclType _type, eDeclUsage _usage, size_t _index)
{
	D3DDECLTYPE type = D3DDECLTYPE_FLOAT3;
	D3DDECLUSAGE usage = D3DDECLUSAGE_POSITION;

	if(_type == DECLFLOAT2) type = D3DDECLTYPE_FLOAT2;
	if(_type == DECLFLOAT4) type = D3DDECLTYPE_FLOAT4;

	if(_usage == DECLTEX) usage = D3DDECLUSAGE_TEXCOORD;
	if(_usage == DECLNRM) usage = D3DDECLUSAGE_NORMAL;
	if(_usage == DECLCOL) usage = D3DDECLUSAGE_COLOR;

	D3DVERTEXELEMENT9 elem = {(WORD)_stream, (WORD)_offset, type, D3DDECLMETHOD_DEFAULT, usage, (BYTE)_index};

	m_decl.push_back(elem);
	m_isChanged = true;
}
//////////////////////////////////////////////////////////////////////////
LPDIRECT3DVERTEXDECLARATION9 D3D9VertexDeclaration::_get() const
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
		HRESULT hr = m_pd3dDevice->CreateVertexDeclaration( &*decl.begin(), &m_pVertexDeclaration );
		if(hr != S_OK)
		{
			assert(0);
		}
		m_isChanged = false;
	}

	return m_pVertexDeclaration;
}
//////////////////////////////////////////////////////////////////////////