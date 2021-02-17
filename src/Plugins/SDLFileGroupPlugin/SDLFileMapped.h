#pragma once

#include "Interface/FileMappedInterface.h"

#include "Kernel/Factorable.h"

namespace Mengine
{
    //////////////////////////////////////////////////////////////////////////
    class SDLFileMapped
        : public FileMappedInterface
        , public Factorable
    {
    public:
        SDLFileMapped();
        ~SDLFileMapped() override;

    public:
        bool open( const FilePath & _relationPath, const FilePath & _folderPath, const FilePath & _filePath, bool _share ) override;
        bool close() override;

    public:
        InputStreamInterfacePtr createInputStream( const DocumentPtr & _doc ) override;
        bool openInputStream( const InputStreamInterfacePtr & _stream, size_t _offset, size_t _size, void ** const _memory ) override;
    };
    //////////////////////////////////////////////////////////////////////////
    typedef IntrusivePtr<SDLFileMapped> SDLFileMappedPtr;
    //////////////////////////////////////////////////////////////////////////
}
