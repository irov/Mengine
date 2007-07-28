#	include	"d3d9BatchRender.h"	
#	include	"d3d9RenderImage.h"	
//////////////////////////////////////////////////////////////////////////
void	D3D9BatchRender::init(IDirect3DDevice9 * _dev)
{
	m_pID3DDevice = _dev;

	m_size1Vert = sizeof(D3D9Vertex);

	HRESULT hr = m_pID3DDevice->CreateVertexBuffer(
		BATCH_BUFFER_SIZE * m_size1Vert, 
		D3DUSAGE_WRITEONLY,
		D3DFVF_TLVERTEX,
		D3DPOOL_MANAGED,
		&m_batchVertexBuffer, NULL);

	if(hr != S_OK)
	{
		assert(!"Can't create vertex buffer for batching");
		return;
	}

	hr = m_pID3DDevice->CreateIndexBuffer(
		BATCH_BUFFER_SIZE * 3, 
		D3DUSAGE_WRITEONLY,
		D3DFMT_INDEX16,
		D3DPOOL_MANAGED,
		&m_batchIndexBuffer, NULL);

	if(hr != S_OK)
	{
		assert(!"Can't create index buffer for batching");
		return;
	}

	int		index = 0;
	short *	indices = NULL;

	hr = m_batchIndexBuffer->Lock(0, BATCH_BUFFER_SIZE*6/4*sizeof(WORD), (VOID**)&indices, 0);
	
	if(hr != S_OK)
	{
		assert(!"Can't lock index buffer for batching");
		return;
	}

	int n = 0;

	for (int i = 0; i < BATCH_BUFFER_SIZE/4; i++)
	{
		indices[index++] = n + 0;
		indices[index++] = n + 1;
		indices[index++] = n + 2;
		indices[index++] = n + 2;
		indices[index++] = n + 3;
		indices[index++] = n + 0;
		n+=4;
	}

	hr = m_batchIndexBuffer->Unlock();

	if(hr != S_OK)
	{
		assert(!"Can't unlock index buffer for batching");
		return;
	}
}
//////////////////////////////////////////////////////////////////////////
void	D3D9BatchRender::destroy()
{
	if(m_batchVertexBuffer)
	{
		m_batchVertexBuffer->Release(); 
		m_batchVertexBuffer = NULL;
	}

	if(m_batchIndexBuffer)
	{
		m_batchIndexBuffer->Release(); 
		m_batchIndexBuffer = NULL;
	}
}
//////////////////////////////////////////////////////////////////////////
void	D3D9BatchRender::insertVertices(D3D9Vertex * _begin, D3D9Vertex * _end)
{
	m_vertices.insert(m_vertices.end(), _begin, _end);
}
//////////////////////////////////////////////////////////////////////////
void	D3D9BatchRender::begin(RenderImageInterface* _rmi, int _blend)
{
	if( m_batches.empty() )
	{
		_newBatch(0,_rmi,_blend);
	}

	Batch & current_b = m_batches.back();

	if( current_b.image != _rmi || current_b.blend != _blend)
	{
		_newBatch(current_b.end,_rmi,_blend);
	}
}
//////////////////////////////////////////////////////////////////////////
void	D3D9BatchRender::end()
{
	m_batches.back().end = m_vertices.size();
}
//////////////////////////////////////////////////////////////////////////
void	D3D9BatchRender::_newBatch(size_t _begin, RenderImageInterface * _rmi, int _blend)
{
	Batch b = {_begin, 0, _blend, _rmi};
	m_batches.push_back( b );
}
//////////////////////////////////////////////////////////////////////////
void	D3D9BatchRender::flush()
{
	if( m_vertices.empty() )
	{
		return;
	}

	m_pID3DDevice->SetVertexShader(NULL);
	m_pID3DDevice->SetFVF(D3DFVF_TLVERTEX);
	m_pID3DDevice->SetIndices(m_batchIndexBuffer);
	
	static D3D9Vertex *	batchVertices = NULL;

	m_pID3DDevice->SetRenderState(D3DRS_ALPHABLENDENABLE, TRUE);

	m_pID3DDevice->SetStreamSource(0, m_batchVertexBuffer, 0, m_size1Vert);

	m_batchVertexBuffer->Lock(0, (UINT)m_vertices.size() * m_size1Vert, (VOID**)&batchVertices, 0);
	memcpy(batchVertices, &m_vertices[0], (UINT)m_vertices.size() * m_size1Vert);
	m_batchVertexBuffer->Unlock();

	UINT startIndex = 0;

	for each( const Batch & b in m_batches )
	{
		D3D9RenderImage * imaged3d9ptype = static_cast<D3D9RenderImage*>(b.image);
		m_pID3DDevice->SetRenderState(D3DRS_ALPHABLENDENABLE, imaged3d9ptype->_isAlpha());
		m_pID3DDevice->SetTexture(0, imaged3d9ptype->_get());

		UINT num_vertices = (UINT)b.end - (UINT)b.begin;
		UINT num_primitives = num_vertices/2;
		m_pID3DDevice->DrawIndexedPrimitive(D3DPT_TRIANGLELIST, 0, 0, num_vertices, startIndex, num_primitives);
		startIndex += num_primitives * 3;
	}

	m_vertices.clear();
	m_batches.clear();
}
//////////////////////////////////////////////////////////////////////////