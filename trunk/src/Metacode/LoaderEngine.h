#	pragma once

#   include "Interface/LoaderInterface.h"
#   include "Interface/StreamInterface.h"
#   include "Interface/ArchiveInterface.h"

#	include "Core/ConstString.h"
#   include "Core/String.h"

#	include "Utils/Archive/Archive.hpp"

#	include "Config/Typedef.h"

#	include "metabuf/Metabuf.hpp"

namespace Menge
{
	class LoaderEngine
        : public LoaderServiceInterface
	{
	public:
		LoaderEngine();

    public:
        void setServiceProvider( ServiceProviderInterface * _serviceProvider ) override;
        ServiceProviderInterface * getServiceProvider() const override;

	public:
		bool initialize( const ConstString & _protocolPath ) override;

	public:
		bool load( const ConstString & _pak, const FilePath & _path, Metabuf::Metadata * _metadata, bool & _exist ) override;

	public:
		bool import( const ConstString & _pak, const FilePath & _path, Metabuf::Metadata * _metadata, bool & _exist );

	private:
		bool importBin_( const InputStreamInterfacePtr & _bin, Metabuf::Metadata * _metadata, bool * _reimport );
		bool openBin_( const ConstString & _pak, const FilePath & _path, InputStreamInterfacePtr & _file, bool & _exist );

#	ifndef MENGINE_MASTER_RELEASE
		bool makeBin_( const ConstString & _pak, const FilePath & _pathXml, const FilePath & _pathBin );
#	endif

    public:
        const ConstString & getCacheConstString( uint32_t _index ) const;

	protected:
        ServiceProviderInterface * m_serviceProvider;

		ArchivatorInterfacePtr m_archivator;

		ConstString m_protocolPath;
			        
        TVectorConstString m_bufferConstString;
	};
}