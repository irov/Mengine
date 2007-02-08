#	pragma once

#	include "RenderFont.h"

#	include <map>
#	include <vector>

struct	FontBatch
{
	mt::vec2f	pos;
	std::string	text;
	D3D9Font*	font;
};

class	Direct3d9RenderSystem 
	: public RenderSystemInterface
{
public:
	Direct3d9RenderSystem();
	~Direct3d9RenderSystem();

public:
	bool	createDisplay(unsigned int _width, unsigned int _height, unsigned int _bits, bool _fullScreen);
	bool	beginSceneDrawing(unsigned long _color);
	bool	endSceneDrawing();

	void	drawLine(const mt::vec2f& p1, const mt::vec2f& p2, float width, unsigned long color) override;

    void	renderImage(const mt::mat3f& _transform, unsigned int _mixedColor,RenderImageInterface* _rmi);
	void	renderImageOffset(const mt::mat3f& _transform, const mt::vec2f& offset, unsigned int _mixedColor, RenderImageInterface * _rmi);
	void	releaseRenderImage(RenderImageInterface* _rmi);
    void	renderText(mt::vec2f _pos, RenderFontInterface* _font, const std::string& _text);
	void	releaseRenderFont(RenderFontInterface* _fnt);

	RenderImageInterface* loadImage(const TextureDesc&	_desc);
	RenderFontInterface* loadFont(const FontDesc& _desc);

private:
	static const int BATCH_BUFFER_SIZE = 1000;

	LPDIRECT3D9					m_direct3d9;
	LPDIRECT3DDEVICE9			m_deviceD3D9;

	LPDIRECT3DVERTEXBUFFER9		m_vbDynamic;

	UINT m_size4Verts;
	UINT m_size1Vert;

	std::vector<D3D9Vertex>		m_fontVerts;

	LPDIRECT3DVERTEXBUFFER9		m_batchVB;
	LPDIRECT3DINDEXBUFFER9		m_batchIB;
	UINT						m_numBatchVertices;
	LPDIRECT3DTEXTURE9			m_currentTex;

	LPD3DXLINE					m_pLine;

	size_t						m_width;
	size_t						m_height;
	size_t						m_bits;
	bool						m_fullScreen;
	bool						m_deviceLost;

	D3DPRESENT_PARAMETERS		m_present;

	typedef std::multimap<LPDIRECT3DTEXTURE9,FontBatch> TMultimapFontBatch;

	TMultimapFontBatch			m_flushBuffers;

	bool						_initRenderSystem();
	bool						_initBatching();
	void						_flushFonts();
};