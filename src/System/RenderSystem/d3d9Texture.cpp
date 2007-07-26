#	include "d3d9Texture.h"


d3d9Texture::d3d9Texture(IDirect3DTexture9 * _pointer)
	: m_texture(_pointer)
{
	D3DSURFACE_DESC surfaceDesc;
 
    if(FAILED(m_texture->GetLevelDesc(0, &surfaceDesc)))
    {
        assert(!"can't get desc!");
    }

	m_width = (float)surfaceDesc.Width;
	m_height = (float)surfaceDesc.Height;
}

int		d3d9Texture::getWidth() const
{
	return -1;
}

int		d3d9Texture::getHeight() const
{
	return -1;
}

bool	d3d9Texture::_isAlpha() const
{
	return -1;
}

d3d9Texture::~d3d9Texture()
{
	if(m_texture)
	{
		m_texture->Release(); 
		m_texture = NULL;
	}
}

IDirect3DTexture9*	d3d9Texture::_get() const
{
	return m_texture;
}