#pragma once

#include "Interface/OutputStreamInterface.h"

#include "Kernel/FilePath.h"

namespace Mengine
{
    //////////////////////////////////////////////////////////////////////////
    class FileOutputStreamInterface
        : public OutputStreamInterface
    {
    public:
        virtual bool open( const FilePath & _relationPath, const FilePath & _folderPath, const FilePath & _filePath, bool _withTemp ) = 0;
        virtual bool close() = 0;

#ifdef MENGINE_DEBUG
    public:
        virtual const FilePath & getRelationPath() const = 0;
        virtual const FilePath & getFolderPath() const = 0;
        virtual const FilePath & getFilePath() const = 0;
#endif
    };
    //////////////////////////////////////////////////////////////////////////
    typedef IntrusivePtr<FileOutputStreamInterface, OutputStreamInterface> FileOutputStreamInterfacePtr;
    //////////////////////////////////////////////////////////////////////////
}


