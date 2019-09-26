#pragma once

#include "Kernel/Resource.h"

#include "spine/spine.h"

namespace Mengine
{
    class ResourceSpineAtlas
        : public Resource
    {
        DECLARE_VISITABLE( Resource );

    public:
        virtual spAtlas * getSpineAtlas() const = 0;
    };
    //////////////////////////////////////////////////////////////////////////
    typedef IntrusiveResourcePtr<ResourceSpineAtlas> ResourceSpineAtlasPtr;
    //////////////////////////////////////////////////////////////////////////
}
