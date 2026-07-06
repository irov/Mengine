#pragma once

#include "Interface/ServiceInterface.h"
#include "Interface/ContentInterface.h"
#include "Interface/MetabufLoaderInterface.h"

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
    class MetabufLoaderServiceInterface
        : public ServiceInterface
    {
        SERVICE_DECLARE( "MetabufLoaderService" )

    public:
        virtual void setProtocolPath( const FilePath & _protocolPath ) = 0;
        virtual const FilePath & getProtocolPath() const = 0;

    public:
        virtual bool load( const ContentInterfacePtr & _content, Metabuf::Metaparse * _metadata, uint32_t _metaVersion, bool * const _exist, const DocumentInterfacePtr & _doc ) const = 0;
        virtual bool validation( const ContentInterfacePtr & _content, uint32_t _metaVersion ) const = 0;
    };
}
//////////////////////////////////////////////////////////////////////////
#define METABUF_LOADER_SERVICE()\
    ((Mengine::MetabufLoaderServiceInterface *)SERVICE_GET(Mengine::MetabufLoaderServiceInterface))
//////////////////////////////////////////////////////////////////////////