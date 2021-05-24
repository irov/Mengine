#include "DX11RenderResourceHandler.h"

namespace Mengine
{
    //////////////////////////////////////////////////////////////////////////
    DX11RenderResourceHandler::DX11RenderResourceHandler()
    {
    }
    //////////////////////////////////////////////////////////////////////////
    DX11RenderResourceHandler::~DX11RenderResourceHandler()
    {
    }
    //////////////////////////////////////////////////////////////////////////
    void DX11RenderResourceHandler::setDirect3D11Device( const ID3D11DevicePtr & _pD3DDevice )
    {
        m_pD3DDevice = _pD3DDevice;
    }
    //////////////////////////////////////////////////////////////////////////
    const ID3D11DevicePtr & DX11RenderResourceHandler::getDirect3D11Device() const
    {
        return m_pD3DDevice;
    }
    //////////////////////////////////////////////////////////////////////////
    ID3D11DeviceContextPtr DX11RenderResourceHandler::getDirect3D11ImmediateContext() const
    {
        ID3D11DeviceContext * pImmediateContext;
        m_pD3DDevice->GetImmediateContext( &pImmediateContext );

        ID3D11DeviceContextPtr immediateContext;
        immediateContext.Attach( pImmediateContext );

        return immediateContext;
    }
    //////////////////////////////////////////////////////////////////////////
}