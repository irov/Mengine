#	pragma once

#	include "Interface/MovieKeyFrameInterface.h"
#	include "Interface/DataInterface.h"

#	include "MovieFramePack.h"

#	include "Config/Blobject.h"

#	include "Factory/FactoryPool.h"

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
		DataInterfacePtr load( const InputStreamInterfacePtr & _stream ) override;

	protected:
		ServiceProviderInterface * m_serviceProvider;
		
		TBlobject m_cacheBinary;

		typedef FactoryPool<MovieFramePack, 32> TFactoryPoolMovieFramePack;
		TFactoryPoolMovieFramePack m_poolMovieFramePack;
	};
}