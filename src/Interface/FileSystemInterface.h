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

    };
}
//////////////////////////////////////////////////////////////////////////
#define FILE_SYSTEM()\
    ((Mengine::FileSystemInterface *)SERVICE_GET(Mengine::FileSystemInterface))
//////////////////////////////////////////////////////////////////////////


