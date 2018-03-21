#pragma once

#include "Interface/DataInterface.h"

#include "Core/ServiceBase.h"

#include "stdex/stl_map.h"

namespace Mengine
{
	class DataManager
		: public ServiceBase<DataServiceInterface>
	{
	public:
		DataManager();
		~DataManager();

	public:
		void registerDataflow( const ConstString& _type, const DataflowInterfacePtr & _dataflow ) override;
		void unregisterDataflow( const ConstString& _type ) override;

	public:
		DataflowInterfacePtr getDataflow( const ConstString & _type ) const override;

	public:
		DataInterfacePtr dataflow( const ConstString & _type, const InputStreamInterfacePtr & _stream ) override;

	protected:
		typedef stdex::map<ConstString, DataflowInterfacePtr> TMapDataflow;
		TMapDataflow m_dataflows;
	};
}