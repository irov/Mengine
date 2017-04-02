#	pragma once

#	include "Interface/FileSystemInterface.h"

namespace Menge
{
    class SDLFileMapped
        : public FileMappedInterface
    {
    public:
        SDLFileMapped();
        ~SDLFileMapped();

    public:
        bool open( const FilePath & _folder, const FilePath & _fileName ) override;

    public:
        InputStreamInterfacePtr createFileStream() override;
        bool openFileStream( const InputStreamInterfacePtr & _stream, size_t _offset, size_t _size, void ** _memory ) override;
    };
}	// namespace Menge
