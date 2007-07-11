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