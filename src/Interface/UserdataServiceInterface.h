#pragma once

#include "Interface/ServiceInterface.h"
#include "Interface/MemoryInterface.h"
#include "Interface/FileGroupInterface.h"

#include "Kernel/ConstString.h"
#include "Kernel/FilePath.h"

#include "Kernel/Magic.h"

namespace Mengine
{
    //////////////////////////////////////////////////////////////////////////
    DECLARE_MAGIC_NUMBER( MAGIC_USER_DATA, 'G', 'M', 'D', '1', 1 );
    //////////////////////////////////////////////////////////////////////////
    class UserdataServiceInterface
        : public ServiceInterface
    {
        SERVICE_DECLARE( "UserdataService" );

    public:
        virtual bool addUserdata( const ConstString & _name, const FileGroupInterfacePtr & _fileGroup, const FilePath & path, const DocumentInterfacePtr & _doc ) = 0;
        virtual bool removeUserdata( const ConstString & _name ) = 0;
        virtual bool hasUserdata( const ConstString & _name ) const = 0;

    public:
        virtual MemoryInterfacePtr loadUserdata( const ConstString & _name, const DocumentInterfacePtr & _doc ) const = 0;
        virtual bool writeUserdata( const ConstString & _name, const void * _data, size_t _size ) const = 0;
    };
}
//////////////////////////////////////////////////////////////////////////
#define USERDATA_SERVICE()\
    ((Mengine::UserdataServiceInterface*)SERVICE_GET(Mengine::UserdataServiceInterface))
//////////////////////////////////////////////////////////////////////////
