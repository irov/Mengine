#	include "DX9RenderTexture.h"

namespace Menge
{
	//////////////////////////////////////////////////////////////////////////
	DX9RenderTexture::DX9RenderTexture()
		: m_depthInterface(NULL)
		, m_dirty(true)
	{
	}
	//////////////////////////////////////////////////////////////////////////
	DX9RenderTexture::~DX9RenderTexture()
	{
        if( m_depthInterface )
        {
            m_depthInterface->Release();
            m_depthInterface = nullptr;
        }
	}
    //////////////////////////////////////////////////////////////////////////
    void DX9RenderTexture::setSurface( IDirect3DSurface9 * _depthInterface )
    {
        m_depthInterface = _depthInterface;
    }
	//////////////////////////////////////////////////////////////////////////
	bool DX9RenderTexture::isDirty() const
	{
		return m_dirty;
	}
	//////////////////////////////////////////////////////////////////////////
	void DX9RenderTexture::setDirty( bool _dirty )
	{
		m_dirty = _dirty;
	}
	//////////////////////////////////////////////////////////////////////////
	IDirect3DSurface9 * DX9RenderTexture::getDepthInterface()
	{
		return m_depthInterface;
	}
	//////////////////////////////////////////////////////////////////////////
	void DX9RenderTexture::setTexInterface( IDirect3DTexture9 * _d3dInterface )
	{
		m_d3dInterface = _d3dInterface;
	}
	//////////////////////////////////////////////////////////////////////////
	void DX9RenderTexture::setDepthInterface( IDirect3DSurface9 * _d3dInterface )
	{
		m_depthInterface = _d3dInterface;
	}
}
