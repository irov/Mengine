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

#ifdef MENGINE_DEBUG
    public:
        virtual const FilePath & getRelationPath() const = 0;
        virtual const FilePath & getFolderPath() const = 0;
        virtual const FilePath & getFilePath() const = 0;
#endif
    };
    //////////////////////////////////////////////////////////////////////////
    typedef IntrusivePtr<FileInputStreamInterface> FileInputStreamInterfacePtr;
    //////////////////////////////////////////////////////////////////////////
}


