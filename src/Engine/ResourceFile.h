#pragma once

#include "Kernel/Resource.h"
#include "Kernel/BaseContent.h"

#include "Kernel/FilePath.h"

namespace Mengine
{
    //////////////////////////////////////////////////////////////////////////
    class ResourceFile
        : public Resource
        , private BaseContent
    {
        DECLARE_VISITABLE( Resource );
        DECLARE_CONTENTABLE();

    public:
        ResourceFile();
        ~ResourceFile() override;
    };
    //////////////////////////////////////////////////////////////////////////
    typedef IntrusiveResourcePtr<ResourceFile> ResourceFilePtr;
    //////////////////////////////////////////////////////////////////////////
}
