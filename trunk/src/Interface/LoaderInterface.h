#   pragma once

#   include "Interface/ServiceInterface.h"

#   include "Core/ConstString.h"
#   include "Core/FilePath.h"

namespace Metabuf
{
    class Metadata;
}

namespace Menge
{
    class LoaderServiceInterface
        : public ServiceInterface
    {
        SERVICE_DECLARE("LoaderService")

    public:
        virtual bool load( const ConstString & _pak, const FilePath & _path, Metabuf::Metadata * _metadata, bool & _exist ) = 0;
    };

#   define LOADER_SERVICE( serviceProvider )\
    (Menge::getService<Menge::LoaderServiceInterface>(serviceProvider))
}