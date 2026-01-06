#include "SDL3FileGroupDirectoryFactory.h"
#include "SDL3FileGroupDirectory.h"

#include "Kernel/MemoryAllocator.h"

namespace Mengine
{
    //////////////////////////////////////////////////////////////////////////
    SDL3FileGroupDirectoryFactory::SDL3FileGroupDirectoryFactory( const FilePath & _relationPath )
        : m_relationPath( _relationPath )
    {
    }
    //////////////////////////////////////////////////////////////////////////
    SDL3FileGroupDirectoryFactory::~SDL3FileGroupDirectoryFactory()
    {
    }
    //////////////////////////////////////////////////////////////////////////
    Factorable * SDL3FileGroupDirectoryFactory::_createObject()
    {
        SDL3FileGroupDirectory * t = Helper::newT<SDL3FileGroupDirectory>();

        t->setRelationPath( m_relationPath );

        return t;
    }
    //////////////////////////////////////////////////////////////////////////
    void SDL3FileGroupDirectoryFactory::_destroyObject( Factorable * _obj )
    {
        Helper::deleteT( static_cast<SDL3FileGroupDirectory *>(_obj) );
    }
    //////////////////////////////////////////////////////////////////////////
}



