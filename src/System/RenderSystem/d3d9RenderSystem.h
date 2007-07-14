#	pragma once
#	include "Interface\WinRenderSystemInterface.h"
#	include "d3d9RenderImage.h"

#	include <vector>
#	include <list>

class	d3d9RenderFont;

class	Direct3d9RenderSystem
	: public WinRenderSystemInterface
{
public:
	Direct3d9RenderSystem();
	~Direct3d9RenderSystem();
public:
	bool	createDisplay(
		HWND _hWnd, int _width, int _height, int _bits, bool _fullScreen,
		bool _vsync, bool _stencilBuffer, bool _antiAlias, bool _pureSoftware);

	void	drawPrimitive(PrimitiveData * _pd);
	
	VertexData * createVertexData();
	IndexData * createIndexData();
	VertexDeclaration * createVertexDeclaration();

	Texture * createTextureInMemory(const TextureDesc& _desc);

	RenderFontInterface* loadFont(const FontDesc& _desc);
	void	releaseRenderImage(RenderImageInterface* _rmi);
    void	renderText(mt::vec2f _pos, RenderFontInterface* _font, const std::string& _text);
	void	releaseRenderFont(RenderFontInterface* _fnt);
	void	setTexture(Texture * _tex);

	void	releaseTexture(Texture * _tex);

	void	renderImage(
					const mt::mat3f& _transform, 
					const mt::vec2f& _offset,
					const mt::vec4f& _uv,
					const mt::vec2f& _size,
					unsigned int _mixedColor, 
					RenderImageInterface* _rmi);

	RenderImageInterface* loadImage(const TextureDesc&	_desc);

	void	setProjectionMatrix(const mt::mat4f& _projection);
	void	setViewMatrix(const mt::mat4f& _view);
	void	setWorldMatrix(const mt::mat4f& _world);
public:
	void	_setDevice(IDirect3DDevice9 * _pd3dDevice);
	IDirect3DDevice9 * _getDevice();
	
	void			_renderBatches();
	void			_initRenderSystemParams();
	//new
	bool beginScene(bool backBuffer, bool zBuffer, int color);
	bool endScene();
private:

	bool			_initBatching();
	void			_prepareBatch(RenderImageInterface* _rmi, int _blend);
	void			_newBatch(size_t _begin, RenderImageInterface* _rmi, int _blend);
	
	static const int BATCH_BUFFER_SIZE = 9000;

	LPDIRECT3DVERTEXBUFFER9		m_batchFontVB;
	LPDIRECT3DINDEXBUFFER9		m_batchFontIB;
		
	UINT						m_size4Verts;
	UINT						m_size1Vert;

	std::vector<D3D9Vertex>		m_vertices;

	struct Batch
	{
		size_t	begin;
		size_t	end;
		int		blend;
		RenderImageInterface * image;
	};

	std::list<Batch>			m_batches;

private:
	//new
	IDirect3DDevice9* pID3DDevice;
	bool DeviceLost;
};