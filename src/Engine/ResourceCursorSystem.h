#pragma once

#include "ResourceCursor.h"

#include "Kernel/Content.h"

namespace Mengine
{
    class ResourceCursorSystem
        : public ResourceCursor
        , public Content
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
    typedef IntrusivePtr<ResourceCursorSystem> ResourceCursorSystemPtr;
    //////////////////////////////////////////////////////////////////////////
}