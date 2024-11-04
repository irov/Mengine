#pragma once

#include "Interface/ServiceInterface.h"
#include "Interface/FileGroupInterface.h"

#include "Kernel/ConstString.h"
#include "Kernel/FilePath.h"

namespace Mengine
{    
    class FileSystemInterface
        : public ServiceInterface
    {
        SERVICE_DECLARE( "FileSystem" )

    public:
        virtual bool existDirectory( const Char * _directoryPath, const Char * _directory ) const = 0;
        virtual bool createDirectory( const Char * _directoryPath, const Char * _directory ) = 0;

    public:
        virtual bool existFile( const Char * _filePath ) = 0;
        virtual bool removeFile( const Char * _filePath ) = 0;
        virtual bool moveFile( const Char * _oldFilePath, const Char * _newFilePath ) = 0;

    public:
        virtual bool findFiles( const Char * _base, const Char * _path, const Char * _mask, const LambdaFilePath & _lambda ) const = 0;

    public:
        virtual uint64_t getFileTime( const Char * _filePath ) const = 0;
    };
}
//////////////////////////////////////////////////////////////////////////
#define FILE_SYSTEM()\
    ((Mengine::FileSystemInterface *)SERVICE_GET(Mengine::FileSystemInterface))
//////////////////////////////////////////////////////////////////////////


