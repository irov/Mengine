#pragma once

#include "Interface/ServiceInterface.h"
#include "Interface/ContentInterface.h"
#include "Interface/MetabufLoaderInterface.h"

#include "Kernel/ConstString.h"

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
        virtual bool load( const ContentInterfacePtr & _content, Metabuf::Metaparse * _metadata, bool * const _exist, const DocumentInterfacePtr & _doc ) const = 0;
    };
}
//////////////////////////////////////////////////////////////////////////
#define METABUF_LOADER_SERVICE()\
    ((Mengine::MetabufLoaderServiceInterface *)SERVICE_GET(Mengine::MetabufLoaderServiceInterface))
//////////////////////////////////////////////////////////////////////////
