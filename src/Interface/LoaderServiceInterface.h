#pragma once

#include "Interface/ServiceInterface.h"
#include "Interface/FileGroupInterface.h"
#include "Interface/LoaderInterface.h"

#include "Kernel/ConstString.h"
#include "Kernel/FilePath.h"

//////////////////////////////////////////////////////////////////////////
namespace Metabuf
{
    class Metaparse;
}
//////////////////////////////////////////////////////////////////////////
namespace Mengine
{
    class LoaderServiceInterface
        : public ServiceInterface
    {
        SERVICE_DECLARE( "LoaderService" )

    public:
        virtual void setProtocolPath( const FilePath & _protocolPath ) = 0;
        virtual const FilePath & getProtocolPath() const = 0;

    public:
        virtual bool load( const FileGroupInterfacePtr & _fileGroup, const FilePath & _filePath, Metabuf::Metaparse * _metadata, uint32_t _metaVersion, bool * const _exist ) const = 0;
        virtual bool validation( const FileGroupInterfacePtr & _fileGroup, const FilePath & _filePath, uint32_t _metaVersion ) const = 0;
    };
}
//////////////////////////////////////////////////////////////////////////
#define LOADER_SERVICE()\
    ((Mengine::LoaderServiceInterface *)SERVICE_GET(Mengine::LoaderServiceInterface))
//////////////////////////////////////////////////////////////////////////