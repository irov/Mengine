#pragma once

#include "Interface/ServiceInterface.h"
#include "Interface/FileGroupInterface.h"
#include "Interface/FontEffectInterface.h"

#include "Kernel/ConstString.h"
#include "Kernel/FilePath.h"

namespace Mengine
{
    class FEServiceInterface
        : public ServiceInterface
    {
    public:
        SERVICE_DECLARE( "FEService" )

    public:
        virtual FontEffectInterfacePtr createFontEffect( const FileGroupInterfacePtr & _fileGroup, const FilePath & _filePath, const ConstString & _name, uint32_t _sample ) = 0;
    };
}
//////////////////////////////////////////////////////////////////////////
#define FE_SERVICE()\
    ((Mengine::FEServiceInterface *)SERVICE_GET(Mengine::FEServiceInterface))
//////////////////////////////////////////////////////////////////////////