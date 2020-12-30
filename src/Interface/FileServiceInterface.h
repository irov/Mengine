#pragma once

#include "Interface/ServiceInterface.h"
#include "Interface/FileGroupInterface.h"

#include "Kernel/ConstString.h"
#include "Kernel/FilePath.h"

namespace Mengine
{
    class FileServiceInterface
        : public ServiceInterface
    {
        SERVICE_DECLARE( "FileService" )

    public:
        virtual bool mountFileGroup( const ConstString & _name, const FileGroupInterfacePtr & _baseFileGroup, const FileGroupInterfacePtr & _parentFileGroup, const FilePath & _filePath, const ConstString & _type, FileGroupInterfacePtr * const _outFileGroup, bool _create, const DocumentPtr & _doc ) = 0;
        virtual void unmountFileGroup( const ConstString & _name ) = 0;

    public:
        virtual bool hasFileGroup( const ConstString & _name, FileGroupInterfacePtr * const _fileGroup ) const = 0;
        virtual const FileGroupInterfacePtr & getFileGroup( const ConstString & _name ) const = 0;

    public:
        virtual const FileGroupInterfacePtr & getDefaultFileGroup() const = 0;

    public:
        typedef Lambda<void()> LambdaFileModifyHook;
        virtual bool setFileModifyHook( const FileGroupInterfacePtr & _fileGroup, const FilePath & _filePath, const LambdaFileModifyHook & _lambda ) = 0;
        virtual void removeFileModifyHook( const FileGroupInterfacePtr & _fileGroup, const FilePath & _filePath ) = 0;
    };
}
//////////////////////////////////////////////////////////////////////////
#define FILE_SERVICE()\
    ((Mengine::FileServiceInterface *)SERVICE_GET(Mengine::FileServiceInterface))
//////////////////////////////////////////////////////////////////////////


