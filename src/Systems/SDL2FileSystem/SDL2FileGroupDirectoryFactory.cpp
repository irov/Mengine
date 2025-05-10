#include "SDL2FileGroupDirectoryFactory.h"
#include "SDL2FileGroupDirectory.h"

#include "Kernel/MemoryAllocator.h"

namespace Mengine
{
    //////////////////////////////////////////////////////////////////////////
    SDL2FileGroupDirectoryFactory::SDL2FileGroupDirectoryFactory( const FilePath & _relationPath )
        : m_relationPath( _relationPath )
    {
    }
    //////////////////////////////////////////////////////////////////////////
    SDL2FileGroupDirectoryFactory::~SDL2FileGroupDirectoryFactory()
    {
    }
    //////////////////////////////////////////////////////////////////////////
    Factorable * SDL2FileGroupDirectoryFactory::_createObject()
    {
        SDL2FileGroupDirectory * t = Helper::newT<SDL2FileGroupDirectory>();

        t->setRelationPath( m_relationPath );

        return t;
    }
    //////////////////////////////////////////////////////////////////////////
    void SDL2FileGroupDirectoryFactory::_destroyObject( Factorable * _obj )
    {
        Helper::deleteT( static_cast<SDL2FileGroupDirectory *>(_obj) );
    }
    //////////////////////////////////////////////////////////////////////////
}