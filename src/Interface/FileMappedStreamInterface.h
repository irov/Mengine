#pragma once

#include "Interface/InputStreamInterface.h"

#include "Kernel/FilePath.h"
#include "Kernel/Mixin.h"

namespace Mengine
{
    //////////////////////////////////////////////////////////////////////////
    class FileMappedStreamInterface
        : public Mixin
    {
    public:
        virtual bool open( const FilePath & _relationPath, const FilePath & _folderPath, const FilePath & _filePath ) = 0;

    public:
        virtual InputStreamInterfacePtr createFileStream() = 0;
        virtual bool openFileStream( const InputStreamInterfacePtr & _stream, size_t _offset, size_t _size, void ** _memory ) = 0;
    };
    //////////////////////////////////////////////////////////////////////////
    typedef IntrusivePtr<FileMappedStreamInterface> FileMappedStreamInterfacePtr;
    //////////////////////////////////////////////////////////////////////////
}


