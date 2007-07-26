#	include "d3d9VertexData.h"
#	include "d3d9VertexDeclaration.h"

D3D9VertexData::D3D9VertexData(IDirect3DDevice9 * _pd3dDevice)
	: m_pd3dDevice(_pd3dDevice)
	, m_vertexCount(0)
	, m_vertexStart(0)
	, m_vertexSize(0)
{}

D3D9VertexData::~D3D9VertexData()
{
	if(m_VB)
	{
		m_VB->Release(); 
		m_VB = NULL;
	}
}

void	D3D9VertexData::lock(float* &_vertices, size_t _offset, size_t _count, size_t _vertexSizeInBytes)
{
	m_vertexCount = _count;
	m_vertexSize = _vertexSizeInBytes;
	m_vertexStart = _offset;

	HRESULT hr = m_VB->Lock( _offset * _vertexSizeInBytes, _count * _vertexSizeInBytes, (void**)&_vertices, D3DLOCK_NOOVERWRITE );
	if(hr != S_OK)
	{
		assert(0);
	}
}
void	D3D9VertexData::unlock()
{
	HRESULT hr = m_VB->Unlock();
	if(hr != S_OK)
	{
		assert(0);
	}
}

void	D3D9VertexData::createVertexBuffer(size_t _count, size_t _vertexSizeInBytes, size_t _start)
{
	m_vertexCount = _count;
	m_vertexStart = _start;
	m_vertexSize = _vertexSizeInBytes;

	HRESULT hr = m_pd3dDevice->CreateVertexBuffer( _count * _vertexSizeInBytes,
                                        0, 0,
                                        D3DPOOL_MANAGED, &m_VB, NULL );
	if(hr != S_OK)
	{
		assert(0);
	}
}

void	D3D9VertexData::setVertexDeclaration(VertexDeclaration * _vd)
{
	m_VD = _vd;
}

LPDIRECT3DVERTEXBUFFER9	D3D9VertexData::_get() const
{
	return m_VB;
}

LPDIRECT3DVERTEXDECLARATION9 D3D9VertexData::_getVertexDecl() const
{
	return static_cast<D3D9VertexDeclaration*>(m_VD)->_get();
}

size_t D3D9VertexData::getVertexSize() const
{
	return m_vertexSize;
}

size_t D3D9VertexData::getVertexStart() const
{
	return m_vertexStart;
}

size_t D3D9VertexData::getVertexCount() const
{
	return m_vertexCount;
}