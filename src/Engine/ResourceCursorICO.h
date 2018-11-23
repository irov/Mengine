#pragma once

#include "ResourceCursor.h"

namespace Mengine
{
    class ResourceCursorICO
        : public ResourceCursor
    {
        DECLARE_VISITABLE( ResourceCursor );

    public:
        ResourceCursorICO();
        ~ResourceCursorICO() override;

    public:
        void setFilePath( const FilePath & _filePath );

    public:
        const FilePath & getFilePath() const override;
        const MemoryInterfacePtr & getBuffer() const override;

    public:
        bool _compile() override;
        void _release() override;

    protected:
        FilePath m_filePath;

        MemoryInterfacePtr m_buffer;
    };
    //////////////////////////////////////////////////////////////////////////
    typedef IntrusivePtr<ResourceCursorICO> ResourceCursorICOPtr;
    //////////////////////////////////////////////////////////////////////////
}