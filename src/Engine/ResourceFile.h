#pragma once

#include "Kernel/Resource.h"
#include "Kernel/FilePath.h"

namespace Mengine
{
    //////////////////////////////////////////////////////////////////////////
    class ResourceFile
        : public Resource
    {
        DECLARE_VISITABLE( Resource );

    public:
        ResourceFile();
        ~ResourceFile() override;
    };
    //////////////////////////////////////////////////////////////////////////
    typedef IntrusiveResourcePtr<ResourceFile> ResourceFilePtr;
    //////////////////////////////////////////////////////////////////////////
}
