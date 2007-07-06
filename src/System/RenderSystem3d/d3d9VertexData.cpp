#	include "d3d9VertexData.h"
#	include "d3d9VertexDeclaration.h"

Direct3d9VertexData::Direct3d9VertexData(IDirect3DDevice9* pd3dDevice)
	: m_pd3dDevice(pd3dDevice)
	, m_vertexCount(0)
	, m_vertexStart(0)
	, m_vertexSize(0)
{}

Direct3d9VertexData::~Direct3d9VertexData()
{
	if(m_VB)
	{
		m_VB->Release(); 
		m_VB = NULL;
	}
}

void	Direct3d9VertexData::lock(float* &_vertices, size_t _offset, size_t _count, size_t _vertexSizeInBytes)
{
	m_vertexCount = _count;
	m_vertexSize = _vertexSizeInBytes;
	m_vertexStart = _offset;

	HRESULT hr = m_VB->Lock( _offset * _vertexSizeInBytes, _count * _vertexSizeInBytes, (void**)&_vertices, D3DLOCK_NOOVERWRITE );
}
void	Direct3d9VertexData::unlock()
{
	HRESULT hr = m_VB->Unlock();
}

void	Direct3d9VertexData::createVertexBuffer(size_t _count, size_t _vertexSizeInBytes, size_t _start)
{
	m_vertexCount = _count;
	m_vertexStart = _start;
	m_vertexSize = _vertexSizeInBytes;

	HRESULT hr = m_pd3dDevice->CreateVertexBuffer( _count * _vertexSizeInBytes,
                                        0, 0,
                                        D3DPOOL_MANAGED, &m_VB, NULL );
}

void	Direct3d9VertexData::setVertexDeclaration(VertexDeclaration * _vd)
{
	m_VD = _vd;
}

LPDIRECT3DVERTEXBUFFER9	Direct3d9VertexData::_get() const
{
	return m_VB;
}

LPDIRECT3DVERTEXDECLARATION9 Direct3d9VertexData::_getVertexDecl() const
{
	return static_cast<d3d9VertexDeclaration*>(m_VD)->_get();
}

size_t Direct3d9VertexData::getVertexSize() const
{
	return m_vertexSize;
}

size_t Direct3d9VertexData::getVertexStart() const
{
	return m_vertexStart;
}

size_t Direct3d9VertexData::getVertexCount() const
{
	return m_vertexCount;
}