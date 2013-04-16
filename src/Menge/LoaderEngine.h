#	pragma once

#   include "Interface/LoaderInterface.h"

#	include "Core/ConstString.h"
#   include "Core/String.h"

#	include "Utils/Archive/Archive.hpp"

#	include "Config/Typedef.h"

#	include "metabuf/Metabuf.hpp"

#	include <map>

namespace Menge
{
	class InputStreamInterface;

	class LoaderEngine
        : public LoaderServiceInterface
	{
	public:
		LoaderEngine();

    public:
        void setServiceProvider( ServiceProviderInterface * _serviceProvider ) override;
        ServiceProviderInterface * getServiceProvider() const override;

	public:
		bool load( const ConstString & _pak, const FilePath & _path, Metabuf::Metadata * _metadata, bool & _exist ) override;

	public:
		bool import( const ConstString & _pak, const FilePath & _path, Metabuf::Metadata * _metadata, bool & _exist );

	private:
		bool importBin_( InputStreamInterface * _bin, Metabuf::Metadata * _metadata, bool * _reimport );
		bool openBin_( const ConstString & _pak, const FilePath & _path, InputStreamInterface ** _file, bool & _exist );

#	ifndef MENGE_MASTER_RELEASE
		bool makeBin_( const ConstString & _pak, const FilePath & _pathXml, const FilePath & _pathBin );
#	endif

    public:
        const ConstString & getCacheConstString( size_t _index ) const;

	protected:
        ServiceProviderInterface * m_serviceProvider;

		Archive m_bufferArchive;
        TVectorConstString m_bufferConstString;
	};
}