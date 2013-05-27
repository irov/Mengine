#	include "DX8RenderTexture.h"

namespace Menge
{
	//////////////////////////////////////////////////////////////////////////
	DX8RenderTexture::DX8RenderTexture()
		: m_depthInterface(NULL)
		, m_dirty(true)
	{
	}
	//////////////////////////////////////////////////////////////////////////
	DX8RenderTexture::~DX8RenderTexture()
	{
        if( m_depthInterface )
        {
            m_depthInterface->Release();
            m_depthInterface = nullptr;
        }
	}
    //////////////////////////////////////////////////////////////////////////
    void DX8RenderTexture::setSurface( IDirect3DSurface8 * _depthInterface )
    {
        m_depthInterface = _depthInterface;
    }
	//////////////////////////////////////////////////////////////////////////
	bool DX8RenderTexture::isDirty() const
	{
		return m_dirty;
	}
	//////////////////////////////////////////////////////////////////////////
	void DX8RenderTexture::setDirty( bool _dirty )
	{
		m_dirty = _dirty;
	}
	//////////////////////////////////////////////////////////////////////////
	IDirect3DSurface8* DX8RenderTexture::getDepthInterface()
	{
		return m_depthInterface;
	}
	//////////////////////////////////////////////////////////////////////////
	void DX8RenderTexture::setTexInterface( IDirect3DTexture8* _d3dInterface )
	{
		m_d3dInterface = _d3dInterface;
	}
	//////////////////////////////////////////////////////////////////////////
	void DX8RenderTexture::setDepthInterface( IDirect3DSurface8* _d3dInterface )
	{
		m_depthInterface = _d3dInterface;
	}
}
