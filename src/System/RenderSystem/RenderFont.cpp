#	include "RenderFont.h"

#	include <map>
#	include <assert.h>


D3D9Font::D3D9Font(LPDIRECT3DDEVICE9	_devD3D9, const FontDesc&	_fontDesc)
{
	std::copy(
		_fontDesc.chars,
		_fontDesc.chars+256,
		stdext::checked_array_iterator<FontCharDesc*>(m_chars,256));

	m_color = _fontDesc.color;
	m_height = _fontDesc.height;

    if (FAILED(D3DXCreateTextureFromFileInMemoryEx(_devD3D9, _fontDesc.texDesc.buffer, (UINT)_fontDesc.texDesc.size,
								D3DX_DEFAULT, D3DX_DEFAULT,
								1, 0,					
								_fontDesc.texDesc.haveAlpha ? D3DFMT_A8R8G8B8 : D3DFMT_R8G8B8,	
								D3DPOOL_MANAGED,	
								D3DX_FILTER_NONE,	
								D3DX_DEFAULT,		
								0,					
								NULL, NULL,
								&m_texPointer ) ) )

	{
		assert(!"D3DXCreateTextureFromFileInMemoryEx");
	}

	D3DSURFACE_DESC		descTexture;

	if(FAILED(m_texPointer->GetLevelDesc(0, &descTexture)))
	{
		assert(!"can't get desc!");
	}
}

D3D9Font::~D3D9Font()
{}

LPDIRECT3DTEXTURE9	D3D9Font::_getTexPointer() const
{
	return	m_texPointer;
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