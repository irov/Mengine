#	pragma once

#	include "Interface/DataInterface.h"

#	include "stdex/binary_vector.h"

namespace Menge
{
	class DataManager
		: public DataServiceInterface
	{
	public:
		DataManager();
		~DataManager();

	public:
		bool initialize() override;
		void finalize() override;

	public:
		void setServiceProvider( ServiceProviderInterface * _serviceProvider ) override;
		ServiceProviderInterface * getServiceProvider() const override;

	public:
		void registerDataflow( const ConstString& _type, const DataflowInterfacePtr & _dataflow ) override;
		void unregisterDataflow( const ConstString& _type ) override;

	public:
		DataflowInterfacePtr getDataflow( const ConstString & _type ) const override;

	public:
		DataInterfacePtr dataflow( const ConstString & _type, const InputStreamInterfacePtr & _stream ) override;

	protected:
		ServiceProviderInterface * m_serviceProvider;

		typedef stdex::binary_vector<ConstString, DataflowInterfacePtr> TMapDataflow;
		TMapDataflow m_dataflows;
	};
}