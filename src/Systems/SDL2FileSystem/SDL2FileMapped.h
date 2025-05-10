#pragma once

#include "Interface/FileMappedInterface.h"

#include "Kernel/Factorable.h"

namespace Mengine
{
    //////////////////////////////////////////////////////////////////////////
    class SDL2FileMapped
        : public FileMappedInterface
        , public Factorable
    {
        DECLARE_FACTORABLE( SDL2FileMapped );

    public:
        SDL2FileMapped();
        ~SDL2FileMapped() override;

    public:
        bool open( const FilePath & _relationPath, const FilePath & _folderPath, const FilePath & _filePath, bool _share ) override;
        bool close() override;

    public:
        InputStreamInterfacePtr createInputStream( const DocumentInterfacePtr & _doc ) override;
        bool openInputStream( const InputStreamInterfacePtr & _stream, size_t _offset, size_t _size ) override;
    };
    //////////////////////////////////////////////////////////////////////////
    typedef IntrusivePtr<SDL2FileMapped, FileMappedInterface> SDL2FileMappedPtr;
    //////////////////////////////////////////////////////////////////////////
}