#pragma once

#include "Interface/ServantInterface.h"
#include "Interface/RenderEnumInterface.h"
#include "Interface/RenderProgramInterface.h"
#include "Interface/RenderTextureInterface.h"

#include "Kernel/Mixin.h"

namespace Mengine
{
    //////////////////////////////////////////////////////////////////////////
    class RenderMaterialStageCacheInterface
        : public Interface
    {
    };
    //////////////////////////////////////////////////////////////////////////
    typedef IntrusivePtr<RenderMaterialStageCacheInterface> RenderMaterialStageCacheInterfacePtr;
    //////////////////////////////////////////////////////////////////////////
    struct RenderTextureStage
    {
        ETextureFilter mipmap = TF_NONE;
        ETextureFilter magnification = TF_POINT;
        ETextureFilter minification = TF_POINT;

        ETextureAddressMode addressU = TAM_CLAMP;
        ETextureAddressMode addressV = TAM_CLAMP;
        uint32_t addressBorder = 0x00000000;
    };
    //////////////////////////////////////////////////////////////////////////
    struct RenderMaterialStage
    {
        uint32_t id = 0;

        RenderProgramInterfacePtr program;
        RenderMaterialStageCacheInterfacePtr stageCache;

        RenderTextureStage textureStages[MENGINE_MAX_TEXTURE_STAGES];

        EBlendFactor blendSrc = BF_ONE;
        EBlendFactor blendDst = BF_ZERO;
        EBlendOp blendOp = BOP_ADD;

        EBlendFactor separateAlphaBlendSrc = BF_ONE;
        EBlendFactor separateAlphaBlendDst = BF_ZERO;
        EBlendOp separateAlphaBlendOp = BOP_ADD;

        bool separateAlphaBlendEnable = false;
        bool alphaBlendEnable = false;
        bool depthBufferTestEnable = false;
        bool depthBufferWriteEnable = false;
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