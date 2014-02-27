#	pragma once

#	include "Interface/Model3DInterface.h"
#	include "Interface/DataInterface.h"

#	include "Model3DPack.h"

#	include "Config/Blobject.h"

#	include "Factory/FactoryStore.h"

namespace Menge
{
	class DataflowMDL
		: public DataflowInterface
	{
	public:
		DataflowMDL();
		~DataflowMDL();

	public:
		void setServiceProvider( ServiceProviderInterface * _serviceProvider ) override;
		ServiceProviderInterface * getServiceProvider() const override;

	public:
		DataInterfacePtr load( const InputStreamInterfacePtr & _stream ) override;

	protected:
		ServiceProviderInterface * m_serviceProvider;

		typedef FactoryPoolStore<Model3DPack, 32> TFactoryPoolModel3DPack;
		TFactoryPoolModel3DPack m_poolModel3DPack;
	};
}