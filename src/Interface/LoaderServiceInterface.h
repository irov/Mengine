#pragma once

#include "Interface/ServiceInterface.h"
#include "Interface/FileGroupInterface.h"
#include "Interface/LoaderInterface.h"

#include "Kernel/ConstString.h"
#include "Kernel/FilePath.h"

//////////////////////////////////////////////////////////////////////////
namespace Metabuf
{
    class Metadata;
}
//////////////////////////////////////////////////////////////////////////
namespace Mengine
{
	//////////////////////////////////////////////////////////////////////////
    class LoaderServiceInterface
        : public ServiceInterface
    {
        SERVICE_DECLARE( "LoaderService" )

    public:
        virtual void setProtocolPath( const FilePath & _protocolPath ) = 0;
		virtual const FilePath & getProtocolPath() const = 0;

    public:
        virtual bool load( const FileGroupInterfacePtr & _pak, const FilePath & _path, Metabuf::Metadata * _metadata, bool & _exist ) const = 0;
        virtual bool validation( const FileGroupInterfacePtr & _pak, const FilePath & _path, const Metabuf::Metadata * _metadata ) const = 0;

	public:
		virtual bool addLoader( const ConstString & _type, const LoaderInterfacePtr & _loader ) = 0;
		virtual void removeLoader( const ConstString & _type ) = 0;
		virtual const LoaderInterfacePtr & getLoader( const ConstString & _type ) const = 0;
    };
}
//////////////////////////////////////////////////////////////////////////
#define LOADER_SERVICE()\
    ((Mengine::LoaderServiceInterface *)SERVICE_GET(Mengine::LoaderServiceInterface))
//////////////////////////////////////////////////////////////////////////