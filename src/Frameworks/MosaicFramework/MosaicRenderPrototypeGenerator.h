#pragma once

#include "MosaicInterface.h"

#include "Kernel/DefaultPrototypeGenerator.h"

#include "MosaicRender.h"

namespace Mengine
{
    class MosaicRenderPrototypeGenerator
        : public DefaultPrototypeGenerator<MosaicRender, 16>
    {
    public:
        MosaicRenderPrototypeGenerator();
        ~MosaicRenderPrototypeGenerator() override;
    };
    //////////////////////////////////////////////////////////////////////////
    typedef IntrusivePtr<MosaicRenderPrototypeGenerator, PrototypeGeneratorInterface> MosaicRenderPrototypeGeneratorPtr;
    //////////////////////////////////////////////////////////////////////////
}
