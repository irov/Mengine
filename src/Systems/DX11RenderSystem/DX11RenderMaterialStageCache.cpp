#include "DX11RenderMaterialStageCache.h"

#include "DX11RenderErrorHelper.h"
#include "DX11RenderEnum.h"

#include "Config/Limits.h"

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

        bool depthEnable = _stage->depthBufferTestEnable == true || _stage->depthBufferWriteEnable == true;

        depthStencilStateDesc.DepthEnable = depthEnable == true ? TRUE : FALSE;
        depthStencilStateDesc.DepthWriteMask = _stage->depthBufferWriteEnable == true ? D3D11_DEPTH_WRITE_MASK_ALL : D3D11_DEPTH_WRITE_MASK_ZERO;
        depthStencilStateDesc.DepthFunc = _stage->depthBufferTestEnable == true ? D3D11_COMPARISON_LESS_EQUAL : D3D11_COMPARISON_ALWAYS;

        depthStencilStateDesc.StencilEnable = FALSE;
        depthStencilStateDesc.StencilReadMask = D3D11_DEFAULT_STENCIL_READ_MASK;
        depthStencilStateDesc.StencilWriteMask = D3D11_DEFAULT_STENCIL_WRITE_MASK;

        depthStencilStateDesc.FrontFace.StencilFailOp = D3D11_STENCIL_OP_KEEP;
        depthStencilStateDesc.FrontFace.StencilDepthFailOp = D3D11_STENCIL_OP_KEEP;
        depthStencilStateDesc.FrontFace.StencilPassOp = D3D11_STENCIL_OP_KEEP;
        depthStencilStateDesc.FrontFace.StencilFunc = D3D11_COMPARISON_ALWAYS;

        depthStencilStateDesc.BackFace = depthStencilStateDesc.FrontFace;

        ID3D11DepthStencilState * depthStencilState;
        MENGINE_IF_DX11_CALL( _pD3DDevice, CreateDepthStencilState, (&depthStencilStateDesc, &depthStencilState) )
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
        MENGINE_IF_DX11_CALL( _pD3DDevice, CreateBlendState, (&blendStateDesc, &blendState) )
        {
            return false;
        }

        m_blendState.Attach( blendState );

        for( uint32_t index = 0; index != MENGINE_MAX_TEXTURE_STAGES; ++index )
        {
            D3D11_SAMPLER_DESC samplerDesc;
            ZeroMemory( &samplerDesc, sizeof( samplerDesc ) );

            const RenderTextureStage * textureStage = _stage->textureStages + index;

            samplerDesc.Filter = Helper::toD3DTextureFilter( textureStage->minification, textureStage->mipmap, textureStage->magnification );
            samplerDesc.AddressU = Helper::toD3DTextureAddress( textureStage->addressU );
            samplerDesc.AddressV = Helper::toD3DTextureAddress( textureStage->addressV );
            samplerDesc.AddressW = D3D11_TEXTURE_ADDRESS_WRAP;
            samplerDesc.MaxAnisotropy = D3D11_REQ_MAXANISOTROPY;
            samplerDesc.ComparisonFunc = D3D11_COMPARISON_NEVER;
            samplerDesc.MaxLOD = MENGINE_FLT_MAX;

            ID3D11SamplerState * samplerState;
            MENGINE_IF_DX11_CALL( _pD3DDevice, CreateSamplerState, (&samplerDesc, &samplerState) )
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
