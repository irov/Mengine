#pragma once

#include "ResourceCursor.h"

#include "Kernel/Content.h"

namespace Mengine
{
    class ResourceCursorICO
        : public ResourceCursor
        , public Content
    {
        DECLARE_VISITABLE( ResourceCursor );
        DECLARE_CONTENTABLE();

    public:
        ResourceCursorICO();
        ~ResourceCursorICO() override;

    public:
        const MemoryInterfacePtr & getBuffer() const override;

    public:
        bool _compile() override;
        void _release() override;

    protected:
        MemoryInterfacePtr m_buffer;
    };
    //////////////////////////////////////////////////////////////////////////
    typedef IntrusivePtr<ResourceCursorICO> ResourceCursorICOPtr;
    //////////////////////////////////////////////////////////////////////////
}