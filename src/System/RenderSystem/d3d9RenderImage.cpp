#include "d3d9RenderImage.h"
#include <assert.h>
//////////////////////////////////////////////////////////////////////////
D3D9RenderImage::D3D9RenderImage(LPDIRECT3DTEXTURE9	_texPointer, const TextureDesc& _desc)
	: m_texPointer(_texPointer)
{ 
    D3DSURFACE_DESC surfaceDesc;
 
    if(FAILED(m_texPointer->GetLevelDesc(0, &surfaceDesc)))
    {
        assert(!"can't get desc!");
    }

	m_width = (float)surfaceDesc.Width;
	m_height = (float)surfaceDesc.Height;
}
//////////////////////////////////////////////////////////////////////////
D3D9RenderImage::~D3D9RenderImage()
{
	if(m_texPointer)
	{
		m_texPointer->Release();
		m_texPointer = NULL;
	}
}
//////////////////////////////////////////////////////////////////////////
int	D3D9RenderImage::getWidth() const
{
	return m_width;
}
//////////////////////////////////////////////////////////////////////////
int	D3D9RenderImage::getHeight() const
{
	return m_height;
}
//////////////////////////////////////////////////////////////////////////
LPDIRECT3DTEXTURE9	D3D9RenderImage::_get() const
{
	return m_texPointer;
}
//////////////////////////////////////////////////////////////////////////
bool	D3D9RenderImage::_isAlpha() const
{
	return m_haveAlpha;
}
//////////////////////////////////////////////////////////////////////////
