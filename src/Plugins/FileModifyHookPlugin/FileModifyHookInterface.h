#pragma once

#include "Interface/ServiceInterface.h"

#include "Interface/FileGroupInterface.h"

#include "Kernel/FilePath.h"

#include "Config/Lambda.h"

namespace Mengine
{
    class FileModifyHookServiceInterface
        : public ServiceInterface
    {
        SERVICE_DECLARE( "FileModifyHookService" )

    public:
        typedef Lambda<void()> LambdaFileModifyHook;
        virtual bool setFileModifyHook( const FileGroupInterfacePtr & _fileGroup, const FilePath & _filePath, const LambdaFileModifyHook & _lambda ) = 0;
        virtual void removeFileModifyHook( const FileGroupInterfacePtr & _fileGroup, const FilePath & _filePath ) = 0;
    };
}
//////////////////////////////////////////////////////////////////////////
#define FILEMODIFYHOOK_SERVICE()\
    ((Mengine::FileModifyHookServiceInterface *)SERVICE_GET(Mengine::FileModifyHookServiceInterface))
//////////////////////////////////////////////////////////////////////////