#include "AppleFileGroupDirectoryFactory.h"
#include "AppleFileGroupDirectory.h"

#include "Kernel/MemoryAllocator.h"

namespace Mengine
{
    //////////////////////////////////////////////////////////////////////////
    AppleFileGroupDirectoryFactory::AppleFileGroupDirectoryFactory( const FilePath & _relationPath )
        : m_relationPath( _relationPath )
    {
    }
    //////////////////////////////////////////////////////////////////////////
    AppleFileGroupDirectoryFactory::~AppleFileGroupDirectoryFactory()
    {
    }
    //////////////////////////////////////////////////////////////////////////
    Factorable * AppleFileGroupDirectoryFactory::_createObject()
    {
        AppleFileGroupDirectory * t = Helper::newT<AppleFileGroupDirectory>();

        t->setRelationPath( m_relationPath );

        return t;
    }
    //////////////////////////////////////////////////////////////////////////
    void AppleFileGroupDirectoryFactory::_destroyObject( Factorable * _obj )
    {
        Helper::deleteT( static_cast<AppleFileGroupDirectory *>(_obj) );
    }
    //////////////////////////////////////////////////////////////////////////
}
