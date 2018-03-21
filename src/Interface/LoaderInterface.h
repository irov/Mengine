#   pragma once

#   include "Interface/ServiceInterface.h"

#   include "Core/ConstString.h"
#   include "Core/FilePath.h"

namespace Metabuf
{
    class Metadata;
}

namespace Mengine
{
    class LoaderServiceInterface
        : public ServiceInterface
    {
        SERVICE_DECLARE("LoaderService")

	public:
		virtual void setProtocolPath( const FilePath & _protocolPath ) = 0;

    public:
        virtual bool load( const ConstString & _pak, const FilePath & _path, Metabuf::Metadata * _metadata, bool & _exist ) const = 0;
		virtual bool validation( const ConstString & _pak, const FilePath & _path, const Metabuf::Metadata * _metadata ) const = 0;
    };

#   define LOADER_SERVICE()\
    ((LoaderServiceInterface *)SERVICE_GET(Mengine::LoaderServiceInterface))
}