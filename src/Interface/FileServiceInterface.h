#pragma once

#include "Interface/ServiceInterface.h"
#include "Interface/InputStreamInterface.h"

#include "Interface/FileGroupInterface.h"

#include "Kernel/Factory.h"

#include "Kernel/ConstString.h"
#include "Kernel/FilePath.h"

namespace Mengine
{
    //////////////////////////////////////////////////////////////////////////
    class FileServiceInterface
        : public ServiceInterface
    {
        SERVICE_DECLARE( "FileService" )

    public:
        virtual bool mountFileGroup( const ConstString & _name, const FileGroupInterfacePtr & _category, const FilePath & _path, const ConstString & _type, FileGroupInterfacePtr * _fileGroup, const Char * _doc ) = 0;
        virtual bool unmountFileGroup( const ConstString & _name ) = 0;

    public:
        virtual bool hasFileGroup( const ConstString & _name, FileGroupInterfacePtr * _fileGroup ) const = 0;
        virtual const FileGroupInterfacePtr & getFileGroup( const ConstString & _name ) const = 0;

        virtual const FileGroupInterfacePtr & getDefaultFileGroup() const = 0;

    public:
        virtual InputStreamInterfacePtr openInputFile( const FileGroupInterfacePtr & _fileGroup, const FilePath & _fileName, bool _streamable, const Char * _doc ) = 0;
        virtual OutputStreamInterfacePtr openOutputFile( const FileGroupInterfacePtr & _fileGroup, const FilePath & _fileName, const Char * _doc ) = 0;
    };
    //////////////////////////////////////////////////////////////////////////
#define FILE_SERVICE()\
    ((Mengine::FileServiceInterface *)SERVICE_GET(Mengine::FileServiceInterface))
}


