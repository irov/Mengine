#	pragma once

#	include "RenderFont.h"
#	include "RenderImage.h"

#	include <map>
#	include <vector>
#	include <list>
#	include <algorithm>

class	Direct3d9RenderSystem 
	: public RenderSystemInterface
{
public:
	Direct3d9RenderSystem();
	~Direct3d9RenderSystem();

public:
	bool	createDisplay(int _width, int _height, int _bits, bool _fullScreen);
	bool	beginSceneDrawing(unsigned long _color);
	bool	endSceneDrawing();

	void	drawLine(const mt::vec2f& p1, const mt::vec2f& p2, float width, unsigned long color) override;

	void	releaseRenderImage(RenderImageInterface* _rmi);
    void	renderText(mt::vec2f _pos, RenderFontInterface* _font, const std::string& _text);
	void	releaseRenderFont(RenderFontInterface* _fnt);

	void	renderImage(
					const mt::mat3f& _transform, 
					const mt::vec2f& _offset,
					const mt::vec4f& _uv,
					const mt::vec2f& _size,
					unsigned int _mixedColor, 
					RenderImageInterface* _rmi);

	RenderImageInterface* loadImage(const TextureDesc&	_desc);
	RenderFontInterface* loadFont(const FontDesc& _desc);

private:
	static const int BATCH_BUFFER_SIZE = 9000;

	LPDIRECT3D9					m_direct3d9;
	LPDIRECT3DDEVICE9			m_deviceD3D9;

	UINT						m_size4Verts;
	UINT						m_size1Vert;

	std::vector<D3D9Vertex>		m_fontVerts;

	LPDIRECT3DVERTEXBUFFER9		m_batchFontVB;
	LPDIRECT3DINDEXBUFFER9		m_batchFontIB;
	UINT						m_numBatchVertices;
	LPDIRECT3DTEXTURE9			m_currentTex;

	LPD3DXLINE					m_pLine;

	int							m_width;
	int							m_height;
	int							m_bits;
	bool						m_fullScreen;
	bool						m_deviceLost;

	D3DPRESENT_PARAMETERS		m_present;

	std::vector<D3D9Vertex>		m_vertices;

	struct Batch
	{
		size_t	begin;
		size_t	end;
		int		blend;
		RenderImageInterface * image;
	};

	std::list<Batch>			m_batches;

	LPDIRECT3DVERTEXBUFFER9		m_batchVB;
	LPDIRECT3DINDEXBUFFER9		m_batchIB;

	bool						_initRenderSystem();
	bool						_initBatching();
	void						_renderBatches();
	void						_prepareBatch(RenderImageInterface* _rmi, int _blend);
	void						_newBatch(size_t _begin, RenderImageInterface* _rmi, int _blend);
};