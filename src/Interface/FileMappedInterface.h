#pragma once

#include "Interface/MappedInterface.h"

#include "Kernel/FilePath.h"

namespace Mengine
{
    //////////////////////////////////////////////////////////////////////////
    class FileMappedInterface
        : public MappedInterface
    {
    public:
        virtual bool open( const FilePath & _relationPath, const FilePath & _folderPath, const FilePath & _filePath, bool _share ) = 0;
        virtual void close() = 0;
    };
    //////////////////////////////////////////////////////////////////////////
    typedef IntrusivePtr<FileMappedInterface> FileMappedInterfacePtr;
    //////////////////////////////////////////////////////////////////////////
}


