#include "SDLFileMappedStream.h"

#include "Interface/MemoryInterface.h"

#include "Kernel/Logger.h"

namespace Mengine
{
    //////////////////////////////////////////////////////////////////////////
    SDLFileMappedStream::SDLFileMappedStream()
    {
    }
    //////////////////////////////////////////////////////////////////////////
    SDLFileMappedStream::~SDLFileMappedStream()
    {
    }
    //////////////////////////////////////////////////////////////////////////
    bool SDLFileMappedStream::open( const FilePath & _relationPath, const FilePath & _folderPath, const FilePath & _filePath )
    {
        (void)_relationPath;
        (void)_folderPath;
        (void)_filePath;

        return false;
    }
    //////////////////////////////////////////////////////////////////////////
    InputStreamInterfacePtr SDLFileMappedStream::createFileStream()
    {
        return nullptr;
    }
    //////////////////////////////////////////////////////////////////////////
    bool SDLFileMappedStream::openFileStream( const InputStreamInterfacePtr & _stream, size_t _offset, size_t _size, void ** _memory )
    {
        (void)_stream;
        (void)_offset;
        (void)_size;
        (void)_memory;
        return false;
    }
}	// namespace Mengine
