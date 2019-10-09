#pragma once

#include "Kernel/ResourceImage.h"

#include "Config/Typedef.h"
#include "Kernel/Color.h"

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
    typedef IntrusiveResourcePtr<ResourceImageSolid> ResourceImageSolidPtr;
    //////////////////////////////////////////////////////////////////////////
}
