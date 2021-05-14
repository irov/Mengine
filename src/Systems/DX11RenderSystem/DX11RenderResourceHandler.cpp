#include "DX11RenderResourceHandler.h"

namespace Mengine
{
    //////////////////////////////////////////////////////////////////////////
    DX11RenderResourceHandler::DX11RenderResourceHandler()
        : m_pD3DDevice( nullptr )
    {
    }
    //////////////////////////////////////////////////////////////////////////
    DX11RenderResourceHandler::~DX11RenderResourceHandler()
    {
    }
    //////////////////////////////////////////////////////////////////////////
    void DX11RenderResourceHandler::setDirect3D11Device( ID3D11Device * _pD3DDevice )
    {
        m_pD3DDevice = _pD3DDevice;
    }
    //////////////////////////////////////////////////////////////////////////
    ID3D11Device * DX11RenderResourceHandler::getDirect3D11Device() const
    {
        return m_pD3DDevice;
    }
    //////////////////////////////////////////////////////////////////////////
    ID3D11DeviceContextPtr DX11RenderResourceHandler::getDirect3D11ImmediateContext() const
    {
        ID3D11DeviceContext * pImmediateContext;
        ID3D11Device * pD3DDevice = this->getDirect3D11Device();
        pD3DDevice->GetImmediateContext( &pImmediateContext );

        return ID3D11DeviceContextPtr( pImmediateContext );
    }
    //////////////////////////////////////////////////////////////////////////
}