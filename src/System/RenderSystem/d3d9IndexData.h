#	pragma once
#	include "Interface/RenderSystemInterface.h"
#	include <d3d9.h>

class Direct3d9IndexData : public IndexData
{
public:
    Direct3d9IndexData(IDirect3DDevice9* pd3dDevice);
	~Direct3d9IndexData();
	void	createIndexBuffer(size_t _count, size_t _start = 0);
	size_t	getIndexStart() const;
	size_t	getIndexCount() const;
	void	lock(unsigned short* &_indecies, size_t _offset, size_t _count);
	void	unlock();
public:
	LPDIRECT3DINDEXBUFFER9	_get() const;
private:
	IDirect3DDevice9 * m_pd3dDevice;
	IDirect3DIndexBuffer9 *  m_IB;
	size_t m_indexStart;
	size_t m_indexCount;
};