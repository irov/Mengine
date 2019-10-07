#pragma once

#include "Interface/ServantInterface.h"
#include "Interface/RenderEnumInterface.h"
#include "Interface/RenderProgramInterface.h"
#include "Interface/RenderTextureInterface.h"

#include "Kernel/Mixin.h"

namespace Mengine
{
    //////////////////////////////////////////////////////////////////////////
    struct RenderTextureStage
    {
        RenderTextureStage() noexcept
            : mipmap( TF_NONE )
            , magnification( TF_LINEAR )
            , minification( TF_LINEAR )
            , addressU( TAM_CLAMP )
            , addressV( TAM_CLAMP )
            , addressBorder( 0x00000000 )
        {
        }

        ETextureFilter mipmap;
        ETextureFilter magnification;
        ETextureFilter minification;

        ETextureAddressMode addressU;
        ETextureAddressMode addressV;
        uint32_t addressBorder;
    };
    //////////////////////////////////////////////////////////////////////////
    struct RenderMaterialStage
    {
        RenderMaterialStage() noexcept
            : id( 0 )
            , blendSrc( BF_SOURCE_ALPHA )
            , blendDst( BF_ONE_MINUS_SOURCE_ALPHA )
            , blendOp( BOP_ADD )
            , alphaBlendEnable( false )
            , depthBufferTestEnable( false )
            , depthBufferWriteEnable( false )
        {
        }

        uint32_t id;

        RenderTextureStage textureStage[MENGINE_MAX_TEXTURE_STAGES];

        RenderProgramInterfacePtr program;

        EBlendFactor blendSrc;
        EBlendFactor blendDst;
        EBlendOp blendOp;

        bool alphaBlendEnable;
        bool depthBufferTestEnable;
        bool depthBufferWriteEnable;
    };
    //////////////////////////////////////////////////////////////////////////
    class RenderMaterialInterface
        : public ServantInterface
    {
    public:
        virtual const ConstString & getName() const = 0;
        virtual uint32_t getId() const = 0;

    public:
        virtual void update( float _time, float _timing ) = 0;

    public:
        virtual EPrimitiveType getPrimitiveType() const = 0;
        virtual uint32_t getTextureCount() const = 0;
        virtual const RenderTextureInterfacePtr & getTexture( uint32_t _index ) const = 0;
        virtual const RenderMaterialStage * getStage() const = 0;
    };
    //////////////////////////////////////////////////////////////////////////
    typedef IntrusivePtr<RenderMaterialInterface> RenderMaterialInterfacePtr;
    //////////////////////////////////////////////////////////////////////////
}