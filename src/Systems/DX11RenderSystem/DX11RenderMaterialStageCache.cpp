#include "DX11RenderMaterialStageCache.h"

#include "DX11RenderErrorHelper.h"
#include "DX11RenderEnum.h"

namespace Mengine
{
    //////////////////////////////////////////////////////////////////////////
    DX11RenderMaterialStageCache::DX11RenderMaterialStageCache()
    {
    }
    //////////////////////////////////////////////////////////////////////////
    DX11RenderMaterialStageCache::~DX11RenderMaterialStageCache()
    {
    }
    //////////////////////////////////////////////////////////////////////////
    bool DX11RenderMaterialStageCache::initialize( const ID3D11DevicePtr & _pD3DDevice, const RenderMaterialStage * _stage )
    {
        D3D11_DEPTH_STENCIL_DESC depthStencilStateDesc;
        ZeroMemory( &depthStencilStateDesc, sizeof( depthStencilStateDesc ) );

        // Set up the description of the stencil state.
        if( _stage->depthBufferWriteEnable == true )
        {
            depthStencilStateDesc.DepthEnable = TRUE;
        }
        else
        {
            depthStencilStateDesc.DepthEnable = FALSE;
        }

        depthStencilStateDesc.DepthWriteMask = D3D11_DEPTH_WRITE_MASK_ALL;

        if( _stage->depthBufferTestEnable == true )
        {
            depthStencilStateDesc.DepthFunc = D3D11_COMPARISON_LESS_EQUAL;
        }
        else
        {
            depthStencilStateDesc.DepthFunc = D3D11_COMPARISON_ALWAYS;
        }

        depthStencilStateDesc.StencilEnable = TRUE;
        depthStencilStateDesc.StencilReadMask = 0xFF;
        depthStencilStateDesc.StencilWriteMask = 0xFF;

        // Stencil operations if pixel is front-facing.
        depthStencilStateDesc.FrontFace.StencilFailOp = D3D11_STENCIL_OP_KEEP;
        depthStencilStateDesc.FrontFace.StencilDepthFailOp = D3D11_STENCIL_OP_INCR;
        depthStencilStateDesc.FrontFace.StencilPassOp = D3D11_STENCIL_OP_KEEP;
        depthStencilStateDesc.FrontFace.StencilFunc = D3D11_COMPARISON_ALWAYS;

        // Stencil operations if pixel is back-facing.
        depthStencilStateDesc.BackFace.StencilFailOp = D3D11_STENCIL_OP_KEEP;
        depthStencilStateDesc.BackFace.StencilDepthFailOp = D3D11_STENCIL_OP_DECR;
        depthStencilStateDesc.BackFace.StencilPassOp = D3D11_STENCIL_OP_KEEP;
        depthStencilStateDesc.BackFace.StencilFunc = D3D11_COMPARISON_ALWAYS;

        ID3D11DepthStencilState * depthStencilState;
        IF_DXCALL( _pD3DDevice, CreateDepthStencilState, (&depthStencilStateDesc, &depthStencilState) )
        {
            return false;
        }

        m_depthStencilState.Attach( depthStencilState );

        D3D11_BLEND_DESC blendStateDesc;
        ZeroMemory( &blendStateDesc, sizeof( blendStateDesc ) );

        if( _stage->alphaBlendEnable == true )
        {
            blendStateDesc.RenderTarget[0].BlendEnable = TRUE;
        }
        else
        {
            blendStateDesc.RenderTarget[0].BlendEnable = FALSE;
        }

        // blend state
        blendStateDesc.RenderTarget[0].RenderTargetWriteMask = D3D11_COLOR_WRITE_ENABLE_ALL;

        blendStateDesc.RenderTarget[0].SrcBlend = Helper::toD3DBlendFactor( _stage->blendSrc );
        blendStateDesc.RenderTarget[0].DestBlend = Helper::toD3DBlendFactor( _stage->blendDst );
        blendStateDesc.RenderTarget[0].BlendOp = Helper::toD3DBlendOp( _stage->blendOp );

        if( _stage->separateAlphaBlendEnable == true )
        {
            blendStateDesc.RenderTarget[0].SrcBlendAlpha = Helper::toD3DBlendFactor( _stage->separateAlphaBlendSrc );
            blendStateDesc.RenderTarget[0].DestBlendAlpha = Helper::toD3DBlendFactor( _stage->separateAlphaBlendDst );
            blendStateDesc.RenderTarget[0].BlendOpAlpha = Helper::toD3DBlendOp( _stage->separateAlphaBlendOp );
        }
        else
        {
            blendStateDesc.RenderTarget[0].SrcBlendAlpha = D3D11_BLEND_ONE;
            blendStateDesc.RenderTarget[0].DestBlendAlpha = D3D11_BLEND_ZERO;
            blendStateDesc.RenderTarget[0].BlendOpAlpha = D3D11_BLEND_OP_ADD;
        }

        ID3D11BlendState * blendState;
        IF_DXCALL( _pD3DDevice, CreateBlendState, (&blendStateDesc, &blendState) )
        {
            return false;
        }

        m_blendState.Attach( blendState );

        for( uint32_t index = 0; index != MENGINE_MAX_TEXTURE_STAGES; ++index )
        {
            D3D11_SAMPLER_DESC samplerDesc;
            ZeroMemory( &samplerDesc, sizeof( samplerDesc ) );

            const RenderTextureStage * textureStage = _stage->textureStages + index;

            samplerDesc.Filter = D3D11_FILTER_MIN_MAG_MIP_LINEAR;
            samplerDesc.AddressU = Helper::toD3DTextureAddress( textureStage->addressU );
            samplerDesc.AddressV = Helper::toD3DTextureAddress( textureStage->addressV );
            samplerDesc.AddressW = D3D11_TEXTURE_ADDRESS_WRAP;
            samplerDesc.MaxLOD = FLT_MAX;

            ID3D11SamplerState * samplerState;
            IF_DXCALL( _pD3DDevice, CreateSamplerState, (&samplerDesc, &samplerState) )
            {
                return false;
            }

            m_samplerStates[index] = samplerState;
        }

        return true;
    }
    //////////////////////////////////////////////////////////////////////////
    void DX11RenderMaterialStageCache::finalize()
    {
        m_depthStencilState = nullptr;
        m_blendState = nullptr;

        for( uint32_t index = 0; index != MENGINE_MAX_TEXTURE_STAGES; ++index )
        {
            if( m_samplerStates[index] != nullptr )
            {
                m_samplerStates[index]->Release();
                m_samplerStates[index] = nullptr;
            }
        }
    }
    //////////////////////////////////////////////////////////////////////////
    void DX11RenderMaterialStageCache::begin( const ID3D11DeviceContextPtr & _pD3DDeviceContext ) const
    {
        _pD3DDeviceContext->OMSetDepthStencilState( m_depthStencilState.Get(), 1 );
        _pD3DDeviceContext->OMSetBlendState( m_blendState.Get(), nullptr, 0xffffffff );

        _pD3DDeviceContext->PSSetSamplers( 0, MENGINE_MAX_TEXTURE_STAGES, m_samplerStates );
    }
    //////////////////////////////////////////////////////////////////////////
    void DX11RenderMaterialStageCache::end( const ID3D11DeviceContextPtr & _pD3DDeviceContext ) const
    {
        _pD3DDeviceContext->OMSetDepthStencilState( nullptr, 1 );
        _pD3DDeviceContext->OMSetBlendState( nullptr, nullptr, 0xffffffff );
        
        ID3D11SamplerState * emptySamplerStates[MENGINE_MAX_TEXTURE_STAGES] = {nullptr};
        _pD3DDeviceContext->PSSetSamplers( 0, MENGINE_MAX_TEXTURE_STAGES, emptySamplerStates );
    }
    //////////////////////////////////////////////////////////////////////////
}