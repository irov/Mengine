#pragma once

#include "Interface/RenderMaterialInterface.h"

#include "Environment/DirectX11/DirectX11RenderIncluder.h"

namespace Mengine
{
    //////////////////////////////////////////////////////////////////////////
    class DX11RenderMaterialStageCache
        : public RenderMaterialStageCacheInterface
        , public Factorable
    {
        DECLARE_FACTORABLE( DX11RenderMaterialStageCache );

    public:
        DX11RenderMaterialStageCache();
        ~DX11RenderMaterialStageCache() override;

    public:
        bool initialize( const ID3D11DevicePtr & _pD3DDevice, const RenderMaterialStage * _stage );
        void finalize();

    public:
        void begin( const ID3D11DeviceContextPtr & _pD3DDeviceContext ) const;
        void end( const ID3D11DeviceContextPtr & _pD3DDeviceContext ) const;

    protected:
        ID3D11DepthStencilStatePtr m_depthStencilState;
        ID3D11BlendStatePtr m_blendState;
        ID3D11SamplerState * m_samplerStates[MENGINE_MAX_TEXTURE_STAGES] = {nullptr};
    };
    //////////////////////////////////////////////////////////////////////////
    typedef IntrusivePtr<DX11RenderMaterialStageCache, RenderMaterialStageCacheInterface> DX11RenderMaterialStageCachePtr;
    //////////////////////////////////////////////////////////////////////////
}