#	pragma once

#	include "Interface/MovieKeyFrameInterface.h"
#	include "Interface/DataInterface.h"
#   include "Interface/ArchiveInterface.h"

#	include "MovieFramePack.h"

#	include "Config/Blobject.h"

#	include "Factory/FactoryStore.h"

namespace Menge
{
	class DataflowAEK
		: public DataflowInterface
	{
	public:
		DataflowAEK();
		~DataflowAEK();

	public:
		void setServiceProvider( ServiceProviderInterface * _serviceProvider ) override;
		ServiceProviderInterface * getServiceProvider() const override;

	public:
		bool initialize() override;

	public:
		DataInterfacePtr create() override;
		bool load( const DataInterfacePtr & _data, const InputStreamInterfacePtr & _stream ) override;

	protected:
		ServiceProviderInterface * m_serviceProvider;

		ArchivatorInterfacePtr m_archivator;

		typedef FactoryPoolStore<MovieFramePack, 32> TFactoryPoolMovieFramePack;
		TFactoryPoolMovieFramePack m_poolMovieFramePack;
	};
}