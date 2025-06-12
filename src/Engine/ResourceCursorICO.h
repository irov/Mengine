#pragma once

#include "Kernel/ResourceCursor.h"

namespace Mengine
{
    //////////////////////////////////////////////////////////////////////////
    class ResourceCursorICO
        : public ResourceCursor
    {
        DECLARE_FACTORABLE( ResourceCursorICO );
        DECLARE_VISITABLE( ResourceCursor );

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
    typedef IntrusiveResourcePtr<ResourceCursorICO> ResourceCursorICOPtr;
    //////////////////////////////////////////////////////////////////////////
}