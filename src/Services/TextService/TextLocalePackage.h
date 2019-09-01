#pragma once

#include "Interface/MemoryInterface.h"
#include "Interface/FileGroupInterface.h"

#include "Kernel/ConstString.h"
#include "Kernel/FilePath.h"
#include "Kernel/Factorable.h"

#include "Config/String.h"

namespace Mengine
{
    class TextLocalePackage
        : public Factorable
    {
    public:
        TextLocalePackage();
        ~TextLocalePackage() override;

    public:
        bool initialize( const FileGroupInterfacePtr & _fileGroup, const FilePath & _filePath );

    public:
        const FileGroupInterfacePtr & getFileGroup() const;
        const FilePath & getFilePath() const;

    public:
        MemoryInterfacePtr getXmlBuffer() const;

    protected:
        FileGroupInterfacePtr m_fileGroup;
        FilePath m_filePath;

        MemoryBufferInterfacePtr m_memory;
    };
    //////////////////////////////////////////////////////////////////////////
    typedef IntrusivePtr<TextLocalePackage> TextLocalePackagePtr;
}
