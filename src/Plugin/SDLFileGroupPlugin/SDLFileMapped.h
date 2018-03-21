#	pragma once

#	include "Interface/FileSystemInterface.h"

namespace Mengine
{
    class SDLFileMapped
        : public FileMappedInterface
    {
    public:
        SDLFileMapped();
        ~SDLFileMapped();

    public:
        bool open( const FilePath & _relationPath, const FilePath & _folderPath, const FilePath & _filePath ) override;

    public:
        InputStreamInterfacePtr createFileStream() override;
        bool openFileStream( const InputStreamInterfacePtr & _stream, size_t _offset, size_t _size, void ** _memory ) override;
    };
}	// namespace Mengine
