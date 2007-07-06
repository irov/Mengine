#	include "d3d9IndexData.h"

Direct3d9IndexData::Direct3d9IndexData(IDirect3DDevice9* pd3dDevice)
	: m_pd3dDevice(pd3dDevice)
	, m_IB(0)
	, m_indexCount(0)
	, m_indexStart(0)
{}

Direct3d9IndexData::~Direct3d9IndexData()
{
	if(m_IB)
	{
		m_IB->Release(); 
		m_IB = NULL;
	}
}

void	Direct3d9IndexData::lock(unsigned short* &_indecies, size_t _offset, size_t _count)
{
	m_indexCount = _count;
	m_indexStart = _offset * 3;
	m_IB->Lock(
		m_indexStart * sizeof(unsigned short), 
		m_indexCount * sizeof(unsigned short), 
		(void**)&_indecies, 
		D3DLOCK_NOOVERWRITE);
}

void	Direct3d9IndexData::unlock()
{
	m_IB->Unlock();
}

void	Direct3d9IndexData::createIndexBuffer(size_t _count, size_t _start)
{
	m_indexCount = _count;
	m_indexStart = _start;

	HRESULT hr;

	hr = m_pd3dDevice->CreateIndexBuffer(
		m_indexCount * sizeof(WORD), 
		0, 
		D3DFMT_INDEX16,
		D3DPOOL_MANAGED, 
		&m_IB,
		NULL );
}

LPDIRECT3DINDEXBUFFER9	Direct3d9IndexData::_get() const
{
	return m_IB;
}

size_t Direct3d9IndexData::getIndexStart() const
{
	return m_indexStart;
}

size_t Direct3d9IndexData::getIndexCount() const
{
	return m_indexCount;
}