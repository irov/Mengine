#pragma once

#include "Interface/FileGroupInterface.h"

#include "Kernel/BaseContent.h"
#include "Kernel/FilePath.h"

namespace Mengine
{
    //////////////////////////////////////////////////////////////////////////
    class FileContent
        : public BaseContent
    {
    public:
        void setFileGroup( const FileGroupInterfacePtr & _fileGroup );
        const FileGroupInterfacePtr & getFileGroup() const;

        void setFilePath( const FilePath & _filePath );
        const FilePath & getFilePath() const;

    protected:
        FileGroupInterfacePtr m_fileGroup;
        FilePath m_filePath;
    };
    //////////////////////////////////////////////////////////////////////////
    typedef InterfacePtr<FileContent, ContentInterface> FileContentPtr;
    //////////////////////////////////////////////////////////////////////////
}