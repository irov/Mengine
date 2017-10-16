#	include "SDLFileMapped.h"

#	include "Interface/MemoryInterface.h"
#	include "Interface/UnicodeInterface.h"

#   include "Logger/Logger.h"

namespace Menge
{
    //////////////////////////////////////////////////////////////////////////
    SDLFileMapped::SDLFileMapped()
    {
    }
    //////////////////////////////////////////////////////////////////////////
    SDLFileMapped::~SDLFileMapped()
    {
    }
    //////////////////////////////////////////////////////////////////////////
    bool SDLFileMapped::open( const FilePath & _relationPath, const FilePath & _folderPath, const FilePath & _filePath )
    {
        (void)_relationPath;
        (void)_folderPath;
        (void)_filePath;

        return false;
    }
    //////////////////////////////////////////////////////////////////////////
    InputStreamInterfacePtr SDLFileMapped::createFileStream()
    {
        return nullptr;
    }
    //////////////////////////////////////////////////////////////////////////
    bool SDLFileMapped::openFileStream( const InputStreamInterfacePtr & _stream, size_t _offset, size_t _size, void ** _memory )
    {
        (void)_stream;
        (void)_offset;
        (void)_size;
        (void)_memory;
        return false;
    }
}	// namespace Menge
