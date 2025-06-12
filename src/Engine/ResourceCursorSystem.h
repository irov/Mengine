#pragma once

#include "Kernel/ResourceCursor.h"

namespace Mengine
{
    //////////////////////////////////////////////////////////////////////////
    class ResourceCursorSystem
        : public ResourceCursor
    {
        DECLARE_FACTORABLE( ResourceCursorSystem );
        DECLARE_VISITABLE( ResourceCursor );

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