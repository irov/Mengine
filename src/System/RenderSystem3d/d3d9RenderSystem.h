#	pragma once
#	include "RenderSystem3dInterface.h"
#	include <d3d9.h>

class	Direct3d9RenderSystem : public RenderSystem
{
public:
	Direct3d9RenderSystem();
	~Direct3d9RenderSystem();
public:
	bool	createDisplay(int _width, int _height, int _bits, bool _fullScreen);
	void	setRenderCallback(renderFunction _rf);
	void	drawPrimitive(const PrimitiveData& _pd);
	void	update();
	VertexData * createVertexData();
	IndexData * createIndexData();
	VertexDeclaration * createVertexDeclaration();
	Texture * createTexture();
public:
	void	_setDevice(IDirect3DDevice9 * _pd3dDevice);
	IDirect3DDevice9 * _getDevice();
private:
	renderFunction m_renderFunction;
	IDirect3DDevice9 * m_pd3dDevice;
};