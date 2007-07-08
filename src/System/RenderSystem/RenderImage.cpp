#include "RenderImage.h"
#include <assert.h>

D3D9RenderImage::D3D9RenderImage(LPDIRECT3DDEVICE9 _dev, const TextureDesc& _desc)
{ 

	D3DXIMAGE_INFO info;
	if ( FAILED( D3DXGetImageInfoFromFileInMemory( _desc.buffer, (UINT)_desc.size, &info ) ) )
	{
		assert(!"can't get TEXTURE info!");
	}

	if( info.Format == D3DFMT_A8R8G8B8 )
	{
		m_haveAlpha = true;
	}

	if (FAILED(D3DXCreateTextureFromFileInMemoryEx(_dev, _desc.buffer, (UINT)_desc.size,
                                    info.Width, info.Height,
                                    1,                  
                                    0,                  
									info.Format,    
                                    D3DPOOL_MANAGED,    
                                    D3DX_FILTER_NONE,   
                                    D3DX_DEFAULT,       
                                    0,                  
                                    NULL, NULL,
                                    &m_texPointer)))
    {
        assert(!"can't get TEXTURE desc!");
    }
 
    D3DSURFACE_DESC surfaceDesc;
 
    if(FAILED(m_texPointer->GetLevelDesc(0, &surfaceDesc)))
    {
        assert(!"can't get desc!");
    }

	m_width = (float)surfaceDesc.Width;
	m_height = (float)surfaceDesc.Height;

	m_verts[0].tcoor = mt::vec2f(0.0f, 0.0f);
	m_verts[1].tcoor = mt::vec2f(1.0f, 0.0f);
	m_verts[2].tcoor = mt::vec2f(1.0f, 1.0f);
	m_verts[3].tcoor = mt::vec2f(0.0f, 1.0f);

	m_verts[0].position = mt::vec3f(0.0f, 0.0f, 1.0f);
	m_verts[1].position = mt::vec3f(m_width, 0.0f, 1.0f);
	m_verts[2].position = mt::vec3f(m_width, m_height, 1.0f);
	m_verts[3].position = mt::vec3f(0.0f, m_height, 1.0f);

	m_verts[0].rhw = 1.0f;
	m_verts[1].rhw = 1.0f;
	m_verts[2].rhw = 1.0f;
	m_verts[3].rhw = 1.0f;

	m_verts[0].color = 0xFFFFFFFF;
	m_verts[1].color = 0xFFFFFFFF;
	m_verts[2].color = 0xFFFFFFFF;
	m_verts[3].color = 0xFFFFFFFF;
}
  
D3D9RenderImage::~D3D9RenderImage()
{
}

float	D3D9RenderImage::getWidth() const
{
	return m_width;
}

float	D3D9RenderImage::getHeight() const
{
	return m_height;
}

D3D9Vertex*	D3D9RenderImage::_getD3D9V4()
{
	return m_verts;
}

LPDIRECT3DTEXTURE9	D3D9RenderImage::_getTexPointer() const
{
	return m_texPointer;
}

bool	D3D9RenderImage::_isAlpha() const
{
	return m_haveAlpha;
}

