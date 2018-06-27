#	pragma once

#	include "Interface/FileMappedStreamInterface.h"

namespace Mengine
{
    class SDLFileMappedStream
        : public FileMappedStreamInterface
    {
    public:
        SDLFileMappedStream();
        ~SDLFileMappedStream() override;

    public:
        bool open( const FilePath & _relationPath, const FilePath & _folderPath, const FilePath & _filePath ) override;

    public:
        InputStreamInterfacePtr createFileStream() override;
        bool openFileStream( const InputStreamInterfacePtr & _stream, size_t _offset, size_t _size, void ** _memory ) override;
    };
}	// namespace Mengine
