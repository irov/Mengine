#	include "RenderFont.h"

#	include <map>
#	include <assert.h>


D3D9Font::D3D9Font(RenderSystemInterface * _rs, LPDIRECT3DDEVICE9 _devD3D9, const FontDesc&	_fontDesc)
{
	std::copy(
		_fontDesc.chars,
		_fontDesc.chars+256,
		stdext::checked_array_iterator<FontCharDesc*>(m_chars,256));

	m_color = _fontDesc.color;
	m_height = _fontDesc.height;

	m_rmi = _rs->loadImage(_fontDesc.texDesc);
}

D3D9Font::~D3D9Font()
{}

RenderImageInterface*	D3D9Font::getRenderImage() const
{
	return m_rmi;
}

float D3D9Font::getHeight() const
{
	return m_height;
}

unsigned int D3D9Font::getColor() const
{
	return m_color;
}

float	D3D9Font::getCharWidth(char id) const
{
	return m_chars[id].width;
}

FontCharDesc&	D3D9Font::_getChar(char id)
{
	return m_chars[id];
}