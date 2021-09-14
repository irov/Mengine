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
    };
    //////////////////////////////////////////////////////////////////////////
    typedef IntrusivePtr<FileOutputStreamInterface> FileOutputStreamInterfacePtr;
    //////////////////////////////////////////////////////////////////////////
}


