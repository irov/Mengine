#include "SDL3FileMapped.h"

#include "Interface/MemoryInterface.h"

#include "Kernel/Logger.h"
#include "Kernel/AssertionNotImplemented.h"

namespace Mengine
{
    //////////////////////////////////////////////////////////////////////////
    SDL3FileMapped::SDL3FileMapped()
    {
    }
    //////////////////////////////////////////////////////////////////////////
    SDL3FileMapped::~SDL3FileMapped()
    {
    }
    //////////////////////////////////////////////////////////////////////////
    bool SDL3FileMapped::open( const FilePath & _relationPath, const FilePath & _folderPath, const FilePath & _filePath, bool _share )
    {
        MENGINE_UNUSED( _relationPath );
        MENGINE_UNUSED( _folderPath );
        MENGINE_UNUSED( _filePath );
        MENGINE_UNUSED( _share );

        MENGINE_ASSERTION_NOT_IMPLEMENTED();

        return false;
    }
    //////////////////////////////////////////////////////////////////////////
    void SDL3FileMapped::close()
    {
        MENGINE_ASSERTION_NOT_IMPLEMENTED();
    }
    //////////////////////////////////////////////////////////////////////////
    InputStreamInterfacePtr SDL3FileMapped::createInputStream( const DocumentInterfacePtr & _doc )
    {
        MENGINE_UNUSED( _doc );

        MENGINE_ASSERTION_NOT_IMPLEMENTED();

        return nullptr;
    }
    //////////////////////////////////////////////////////////////////////////
    bool SDL3FileMapped::openInputStream( const InputStreamInterfacePtr & _stream, size_t _offset, size_t _size )
    {
        MENGINE_UNUSED( _stream );
        MENGINE_UNUSED( _offset );
        MENGINE_UNUSED( _size );
        
        MENGINE_ASSERTION_NOT_IMPLEMENTED();

        return false;
    }
    //////////////////////////////////////////////////////////////////////////
}

