#pragma once

#include "Config/Typedef.h"

#include "Interface/ServiceInterface.h"
#include "Interface/MemoryInterface.h"
#include "Interface/DataInterface.h"
#include "Interface/DataflowInterface.h"

#include "Kernel/Mixin.h"
#include "Kernel/ConstString.h"

namespace Mengine
{
	//////////////////////////////////////////////////////////////////////////
	class DataServiceInterface
		: public ServiceInterface
	{
		SERVICE_DECLARE("DataService")

	public:
		virtual void registerDataflow(const ConstString& _type, const DataflowInterfacePtr & _dataflow) = 0;
		virtual void unregisterDataflow(const ConstString& _type) = 0;

	public:
		virtual const DataflowInterfacePtr & getDataflow(const ConstString & _type) const = 0;

	public:
		virtual DataInterfacePtr dataflow(const DataflowInterfacePtr & _dataflow, const InputStreamInterfacePtr & _stream) = 0;

	public:
		template<class T>
		T dataflowT(const DataflowInterfacePtr & _dataflow, const InputStreamInterfacePtr & _stream)
		{
			DataInterfacePtr data = this->dataflow(_dataflow, _stream);

#ifndef NDEBUG
			if (data == nullptr)
			{
				return nullptr;
			}

			if (stdex::intrusive_dynamic_cast<T>(data) == nullptr)
			{
				throw;
			}
#endif

			T t = stdex::intrusive_static_cast<T>(data);

			return t;
		}
	};
}
//////////////////////////////////////////////////////////////////////////
#define DATA_SERVICE()\
    ((Mengine::DataServiceInterface*)SERVICE_GET(Mengine::DataServiceInterface))
//////////////////////////////////////////////////////////////////////////


