#pragma once

#include "Interface/MemoryInterface.h"
#include "Interface/FileGroupInterface.h"

#include "Kernel/ConstString.h"
#include "Kernel/FilePath.h"
#include "Kernel/Factorable.h"

#include "Config/String.h"

namespace Mengine
{
    class TextLocalePack
        : public Factorable
    {
    public:
        TextLocalePack();
        ~TextLocalePack() override;

    public:
        bool initialize( const FileGroupInterfacePtr & _fileGroup, const FilePath & _path );

    public:
        const FileGroupInterfacePtr & getFileGroup() const;
        const FilePath & getPath() const;

    public:
        MemoryInterfacePtr getXmlBuffer() const;

    protected:
        FileGroupInterfacePtr m_fileGroup;
        FilePath m_path;

        MemoryBufferInterfacePtr m_memory;
    };
    //////////////////////////////////////////////////////////////////////////
    typedef IntrusivePtr<TextLocalePack> TextLocalePackPtr;
}
