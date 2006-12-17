#pragma once

#	include <string>

#	include "math/mat3.h"
#	include "math/vec4.h"

class RenderImageInterface
{
public:
	virtual float getWidth() const = 0;
	virtual float getHeight() const = 0;
};

class RenderFontInterface
{
public:
	virtual float			getHeight() const = 0;
	virtual unsigned int	getColor() const = 0;
	virtual float			getCharWidth(char id) const = 0;
};

struct	TextureDesc
{
	void*	buffer;
	size_t	size;
	bool	haveAlpha;
};

struct FontCharDesc
{
	mt::vec4f		uv;
	float			width;
};

struct FontDesc
{
	TextureDesc		texDesc;
	FontCharDesc	chars[256];
	unsigned long	color;
	float			height;
};

class	RenderSystemInterface
{
public:
	virtual bool	createDisplay(unsigned int _width, unsigned int _height, unsigned int _bits, bool _fullScreen) = 0;
	virtual bool	beginSceneDrawing(unsigned long _color) = 0;
	virtual bool	endSceneDrawing() = 0;

	virtual void    drawLine(const mt::vec2f& p1, const mt::vec2f& p2, float width, unsigned long color) = 0;

	virtual void	renderImage(const mt::mat3f& _transform, unsigned int _mixedColor, RenderImageInterface* _rmi) = 0;
	virtual void	renderImageOffset(const mt::mat3f& _transform, const mt::vec2f& _offset, unsigned int _mixedColor, RenderImageInterface * _rmi) = 0;

	virtual	void	releaseRenderImage(RenderImageInterface* _rmi) = 0;
	virtual	void	renderText(mt::vec2f _pos, RenderFontInterface* _font, const std::string& _text) = 0;
	virtual	void	releaseRenderFont(RenderFontInterface* _fnt) = 0;

	virtual RenderImageInterface* loadImage(const TextureDesc&	_desc) = 0;
	virtual RenderFontInterface* loadFont(const FontDesc&	_desc) = 0;
};

bool initInterfaceSystem(RenderSystemInterface** _ptrRenderSystem);
void releaseInterfaceSystem(RenderSystemInterface* _ptrRenderSystem);