#pragma once

#include "Interface/InputStreamInterface.h"

#include "Kernel/FilePath.h"

namespace Mengine
{
    //////////////////////////////////////////////////////////////////////////
    class FileInputStreamInterface
        : public InputStreamInterface
    {
    public:
        virtual bool open( const FilePath & _relationPath, const FilePath & _folderPath, const FilePath & _filePath, size_t _offset, size_t _size, bool _streaming, bool _share ) = 0;
        virtual void close() = 0;
    };
    //////////////////////////////////////////////////////////////////////////
    typedef IntrusivePtr<FileInputStreamInterface, InputStreamInterface> FileInputStreamInterfacePtr;
    //////////////////////////////////////////////////////////////////////////
}


