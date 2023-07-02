#include "SDLFileMappedStream.h"

#include "Interface/MemoryInterface.h"

#include "Kernel/Logger.h"
#include "Kernel/AssertionNotImplemented.h"

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
        MENGINE_UNUSED( _relationPath );
        MENGINE_UNUSED( _folderPath );
        MENGINE_UNUSED( _filePath );

        MENGINE_ASSERTION_NOT_IMPLEMENTED();

        return false;
    }
    //////////////////////////////////////////////////////////////////////////
    InputStreamInterfacePtr SDLFileMappedStream::createFileStream( const DocumentInterfacePtr & _doc )
    {
        MENGINE_UNUSED( _doc );

        MENGINE_ASSERTION_NOT_IMPLEMENTED();

        return nullptr;
    }
    //////////////////////////////////////////////////////////////////////////
    bool SDLFileMappedStream::openFileStream( const InputStreamInterfacePtr & _stream, size_t _offset, size_t _size, void ** const _memory )
    {
        MENGINE_UNUSED( _stream );
        MENGINE_UNUSED( _offset );
        MENGINE_UNUSED( _size );
        MENGINE_UNUSED( _memory );

        MENGINE_ASSERTION_NOT_IMPLEMENTED();

        return false;
    }
    //////////////////////////////////////////////////////////////////////////
}
