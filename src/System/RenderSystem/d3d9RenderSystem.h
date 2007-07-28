#	pragma once
#	include "Interface\WinRenderSystemInterface.h"

#	include "d3d9BatchRender.h"

#	include <vector>
#	include <list>

#	include <d3d9.h>
#	include <d3dx9.h>

class	D3D9Font;
class	D3D9RenderImage;

class	Direct3d9RenderSystem
	: public WinRenderSystemInterface
{
public:
	Direct3d9RenderSystem();
	~Direct3d9RenderSystem();
public:
	bool	createDisplay(
		HWND _hWnd, int _width, int _height, int _bits, bool _fullScreen,
		bool _vsync, bool _stencilBuffer, bool _antiAlias, bool _pureSoftware) override;

	bool beginScene(int _color) override;
	bool endScene() override;

	void	setMaterialColor(unsigned char _ambient[4], 
		unsigned char _diffuse[4],
		unsigned char _specular[4]) override;

	void	drawPrimitive(PrimitiveData * _pd) override;
	
	VertexData * createVertexData() override;
	IndexData * createIndexData() override;
	VertexDeclaration * createVertexDeclaration() override;

	void setTexture(RenderImageInterface * _tex) override;

	RenderImageInterface* loadImage(const TextureDesc&	_desc) override;

	void	renderImage(			
		const mt::mat3f& _transform, 
		const mt::vec2f& _offset,
		const mt::vec4f& _uv,
		const mt::vec2f& _size,
		unsigned int _mixedColor, 
		RenderImageInterface* _rmi) override;

	void	releaseRenderImage(RenderImageInterface* _rmi) override;
	RenderFontInterface* loadFont(const FontDesc&	_desc) override;
	void	renderText(const mt::vec2f& _pos, RenderFontInterface* _font, const std::string& _text) override;
	void	releaseRenderFont(RenderFontInterface* _fnt) override;

	void	setProjectionMatrix(const mt::mat4f& _projection) override;
	void	setViewMatrix(const mt::mat4f& _view) override;
	void	setWorldMatrix(const mt::mat4f& _world) override;

	void	drawLine2D(const mt::vec2f& _p1, const mt::vec2f& _p2, unsigned long _color) override;
	void	drawLine3D(const mt::vec3f& _p1, const mt::vec3f& _p2, unsigned long _color) override;
	void	drawBox(const mt::vec3f& _minEdge, const mt::vec3f & _maxEdge, unsigned long _color) override;
private:
	//
	D3D9BatchRender BatchRender;
	//
	void			_resetRenderSystemParams();
	//
	struct HelperVertex2d
	{
		float x, y, z, w;
		unsigned long color; 
	};
	//
	struct HelperVertex3d
	{
		float x, y, z;
		unsigned long color; 
	};
	//
	IDirect3DDevice9 * m_pID3DDevice;
	bool m_deviceLost;
};