#	include "d3d9Texture.h"


d3d9Texture::d3d9Texture()
	: m_texture(0)
{}

d3d9Texture::~d3d9Texture()
{}

IDirect3DTexture9*	d3d9Texture::_get() const
{
	return m_texture;
}