#include "Win32FileGroupDirectoryFactory.h"

#include "Win32FileGroupDirectory.h"

#include "Kernel/Typename.h"

namespace Mengine
{
    //////////////////////////////////////////////////////////////////////////
    Win32FileGroupDirectoryFactory::Win32FileGroupDirectoryFactory( const FilePath & _relationPath )
        : m_relationPath( _relationPath )
    {
    }
    //////////////////////////////////////////////////////////////////////////
    Win32FileGroupDirectoryFactory::~Win32FileGroupDirectoryFactory()
    {
    }
    //////////////////////////////////////////////////////////////////////////
    Factorable * Win32FileGroupDirectoryFactory::_createObject()
    {
        Win32FileGroupDirectory * t = Helper::newT<Win32FileGroupDirectory>();

        t->setRelationPath( m_relationPath );

        return t;
    }
    //////////////////////////////////////////////////////////////////////////
    void Win32FileGroupDirectoryFactory::_destroyObject( Factorable * _obj )
    {
        Helper::deleteT( static_cast<Win32FileGroupDirectory *>(_obj) );
    }
    //////////////////////////////////////////////////////////////////////////
}