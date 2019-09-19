#pragma once

#include "Kernel/Resource.h"
#include "Kernel/Content.h"

#include "Kernel/FilePath.h"

namespace Mengine
{
    class ResourceFile
        : public Resource
        , public Content
    {
        DECLARE_VISITABLE( Resource );
        DECLARE_CONTENTABLE();

    public:
        ResourceFile();
        ~ResourceFile() override;
    };
    //////////////////////////////////////////////////////////////////////////
    typedef IntrusivePtr<ResourceFile> ResourceFilePtr;
    //////////////////////////////////////////////////////////////////////////
}
