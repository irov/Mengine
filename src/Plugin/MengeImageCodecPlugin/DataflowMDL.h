#	pragma once

#	include "Interface/Model3DInterface.h"
#	include "Interface/DataInterface.h"
#   include "Interface/ArchiveInterface.h"

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
		bool initialize() override;
		void finalize() override;

	public:
		DataInterfacePtr create() override;
		bool load( const DataInterfacePtr & _data, const InputStreamInterfacePtr & _stream ) override;

	protected:
		ServiceProviderInterface * m_serviceProvider;

		ArchivatorInterfacePtr m_archivator;

		typedef FactoryPoolStore<Model3DPack, 32> TFactoryPoolModel3DPack;
		TFactoryPoolModel3DPack m_poolModel3DPack;
	};
}