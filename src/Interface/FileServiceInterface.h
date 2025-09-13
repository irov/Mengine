#pragma once

#include "Interface/ServiceInterface.h"
#include "Interface/FileGroupInterface.h"

#include "Kernel/ConstString.h"
#include "Kernel/FilePath.h"

#include "Config/UniqueId.h"

namespace Mengine
{    
    class FileServiceInterface
        : public ServiceInterface
    {
        SERVICE_DECLARE( "FileService" )

    public:
        virtual bool mountFileGroup( const ConstString & _fileGroupName, const FileGroupInterfacePtr & _baseFileGroup, const FileGroupInterfacePtr & _parentFileGroup, const FilePath & _filePath, const ConstString & _type, FileGroupInterfacePtr * const _outFileGroup, bool _create, const DocumentInterfacePtr & _doc ) = 0;
        virtual void unmountFileGroup( const ConstString & _fileGroupName ) = 0;

    public:
        virtual bool hasFileGroup( const ConstString & _fileGroupName, FileGroupInterfacePtr * const _fileGroup ) const = 0;
        virtual const FileGroupInterfacePtr & getFileGroup( const ConstString & _fileGroupName ) const = 0;

#if defined(MENGINE_DEBUG_FILE_PATH_ENABLE)
    public:
        virtual void addDebugFilePath( UniqueId _id, const FilePath & _relationPath, const FilePath & _folderPath, const FilePath & _filePath ) = 0;        
        virtual void removeDebugFilePath( UniqueId _id ) = 0;

    public:
        virtual const FilePath & getDebugRelationPath( UniqueId _id ) const = 0;
        virtual const FilePath & getDebugFolderPath( UniqueId _id ) const = 0;
        virtual const FilePath & getDebugFilePath( UniqueId _id ) const = 0;
#endif
    };
}
//////////////////////////////////////////////////////////////////////////
#define FILE_SERVICE()\
    ((Mengine::FileServiceInterface *)SERVICE_GET(Mengine::FileServiceInterface))
//////////////////////////////////////////////////////////////////////////


