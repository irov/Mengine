#include "POSIXFileGroupDirectoryFactory.h"

#include "POSIXFileGroupDirectory.h"

#include "Kernel/MemoryAllocator.h"

namespace Mengine
{
    //////////////////////////////////////////////////////////////////////////
    POSIXFileGroupDirectoryFactory::POSIXFileGroupDirectoryFactory( const FilePath & _relationPath )
        : m_relationPath( _relationPath )
    {
    }
    //////////////////////////////////////////////////////////////////////////
    POSIXFileGroupDirectoryFactory::~POSIXFileGroupDirectoryFactory()
    {
    }
    //////////////////////////////////////////////////////////////////////////
    Factorable * POSIXFileGroupDirectoryFactory::_createObject()
    {
        POSIXFileGroupDirectory * fileGroup = Helper::newT<POSIXFileGroupDirectory>();

        fileGroup->setRelationPath( m_relationPath );

        return fileGroup;
    }
    //////////////////////////////////////////////////////////////////////////
    void POSIXFileGroupDirectoryFactory::_destroyObject( Factorable * _object )
    {
        Helper::deleteT( static_cast<POSIXFileGroupDirectory *>(_object) );
    }
    //////////////////////////////////////////////////////////////////////////
}
