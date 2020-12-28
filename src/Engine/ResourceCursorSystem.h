#pragma once

#include "ResourceCursor.h"

#include "Kernel/BaseContent.h"

namespace Mengine
{
    //////////////////////////////////////////////////////////////////////////
    class ResourceCursorSystem
        : public ResourceCursor
        , private BaseContent
    {
        DECLARE_VISITABLE( ResourceCursor );
        DECLARE_CONTENTABLE();

    public:
        ResourceCursorSystem();
        ~ResourceCursorSystem() override;

    public:
        const MemoryInterfacePtr & getBuffer() const override;
    };
    //////////////////////////////////////////////////////////////////////////
    typedef IntrusiveResourcePtr<ResourceCursorSystem> ResourceCursorSystemPtr;
    //////////////////////////////////////////////////////////////////////////
}