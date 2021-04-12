#include "SDLFileGroupDirectoryFactory.h"
#include "SDLFileGroupDirectory.h"

#include "Kernel/MemoryAllocator.h"

namespace Mengine
{
    //////////////////////////////////////////////////////////////////////////
    SDLFileGroupDirectoryFactory::SDLFileGroupDirectoryFactory( const FilePath & _relationPath )
        : m_relationPath( _relationPath )
    {
    }
    //////////////////////////////////////////////////////////////////////////
    SDLFileGroupDirectoryFactory::~SDLFileGroupDirectoryFactory()
    {
    }
    //////////////////////////////////////////////////////////////////////////
    Factorable * SDLFileGroupDirectoryFactory::_createObject()
    {
        SDLFileGroupDirectory * t = Helper::newT<SDLFileGroupDirectory>();

        t->setRelationPath( m_relationPath );

        return t;
    }
    //////////////////////////////////////////////////////////////////////////
    void SDLFileGroupDirectoryFactory::_destroyObject( Factorable * _obj )
    {
        Helper::deleteT( static_cast<SDLFileGroupDirectory *>(_obj) );
    }
    //////////////////////////////////////////////////////////////////////////
}