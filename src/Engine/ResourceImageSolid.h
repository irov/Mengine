#pragma once

#include "Kernel/ResourceImage.h"

#include "Config/Typedef.h"
#include "Kernel/ColourValue.h"

namespace Mengine
{
    class ResourceImageSolid
        : public ResourceImage
    {
        DECLARE_VISITABLE( ResourceImage );

    public:
        ResourceImageSolid();
        ~ResourceImageSolid() override;

    protected:
        bool _compile() override;
    };
    //////////////////////////////////////////////////////////////////////////
    typedef IntrusivePtr<ResourceImageSolid> ResourceImageSolidPtr;
    //////////////////////////////////////////////////////////////////////////
}
