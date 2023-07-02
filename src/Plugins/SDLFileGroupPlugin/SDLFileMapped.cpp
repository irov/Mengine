#include "SDLFileMapped.h"

#include "Interface/MemoryInterface.h"

#include "Kernel/Logger.h"
#include "Kernel/AssertionNotImplemented.h"

namespace Mengine
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
    bool SDLFileMapped::open( const FilePath & _relationPath, const FilePath & _folderPath, const FilePath & _filePath, bool _share )
    {
        MENGINE_UNUSED( _relationPath );
        MENGINE_UNUSED( _folderPath );
        MENGINE_UNUSED( _filePath );
        MENGINE_UNUSED( _share );

        MENGINE_ASSERTION_NOT_IMPLEMENTED();

        return false;
    }
    //////////////////////////////////////////////////////////////////////////
    bool SDLFileMapped::close()
    {
        MENGINE_ASSERTION_NOT_IMPLEMENTED();

        return false;
    }
    //////////////////////////////////////////////////////////////////////////
    InputStreamInterfacePtr SDLFileMapped::createInputStream( const DocumentInterfacePtr & _doc )
    {
        MENGINE_UNUSED( _doc );

        MENGINE_ASSERTION_NOT_IMPLEMENTED();

        return nullptr;
    }
    //////////////////////////////////////////////////////////////////////////
    bool SDLFileMapped::openInputStream( const InputStreamInterfacePtr & _stream, size_t _offset, size_t _size )
    {
        MENGINE_UNUSED( _stream );
        MENGINE_UNUSED( _offset );
        MENGINE_UNUSED( _size );
        
        MENGINE_ASSERTION_NOT_IMPLEMENTED();

        return false;
    }
    //////////////////////////////////////////////////////////////////////////
}
