#pragma once

#include "Interface/FileMappedInterface.h"

#include "Kernel/Factorable.h"

namespace Mengine
{
    //////////////////////////////////////////////////////////////////////////
    class SDL3FileMapped
        : public FileMappedInterface
        , public Factorable
    {
        DECLARE_FACTORABLE( SDL3FileMapped );

    public:
        SDL3FileMapped();
        ~SDL3FileMapped() override;

    public:
        bool open( const FilePath & _relationPath, const FilePath & _folderPath, const FilePath & _filePath, bool _share ) override;
        void close() override;

    public:
        InputStreamInterfacePtr createInputStream( const DocumentInterfacePtr & _doc ) override;
        bool openInputStream( const InputStreamInterfacePtr & _stream, size_t _offset, size_t _size ) override;
    };
    //////////////////////////////////////////////////////////////////////////
    typedef IntrusivePtr<SDL3FileMapped, FileMappedInterface> SDL3FileMappedPtr;
    //////////////////////////////////////////////////////////////////////////
}

