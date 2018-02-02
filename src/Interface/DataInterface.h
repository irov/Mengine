#	pragma once

#	include "Config/Typedef.h"

#	include "Interface/ServiceInterface.h"
#	include "Interface/StreamInterface.h"
#	include "Interface/MemoryInterface.h"

#   include "Factory/FactorablePtr.h"

#	include "Core/ConstString.h"
#	include "Core/FilePath.h"

#	include <stdint.h>

namespace Menge
{	
	class DataInterface
		: public FactorablePtr
	{
	public:
		virtual Pointer allocateMemory( size_t _size ) const = 0;

	public:
		template<class T>
		inline Pointer allocateMemoryT( size_t _count ) const
		{ 
			size_t total_size = sizeof( T ) * _count;

			return this->allocateMemory( total_size );
		}

	};

	typedef stdex::intrusive_ptr<DataInterface> DataInterfacePtr;

	class DataflowInterface
		: public ServantInterface
	{
	public:
		virtual bool initialize() = 0;
		virtual void finalize() = 0;

	public:
		virtual DataInterfacePtr create() = 0;
		virtual bool load( const DataInterfacePtr & _data, const InputStreamInterfacePtr & _stream ) = 0;
	};

	typedef stdex::intrusive_ptr<DataflowInterface> DataflowInterfacePtr;

	class DataflowFactoryInterface
		: public ServantInterface
	{
    public:
        virtual bool initialize() = 0;

	public:
		virtual DataflowInterfacePtr createDataflow() = 0;
	};

	typedef stdex::intrusive_ptr<DataflowFactoryInterface> DataflowFactoryInterfacePtr;

	class DataServiceInterface
		: public ServiceInterface
	{
        SERVICE_DECLARE("DataService")

	public:
		virtual void registerDataflow( const ConstString& _type, const DataflowInterfacePtr & _dataflow ) = 0;
		virtual void unregisterDataflow( const ConstString& _type ) = 0;
		
	public:
		virtual DataflowInterfacePtr getDataflow( const ConstString & _type ) const = 0;

    public:
		virtual DataInterfacePtr dataflow( const ConstString & _type, const InputStreamInterfacePtr & _stream ) = 0;
			
	public:
        template<class T>
        T dataflowT( const ConstString & _type, const InputStreamInterfacePtr & _stream )
        {
            DataInterfacePtr data = this->dataflow( _type, _stream );

#   ifdef _DEBUG
			if (data == nullptr)
			{
				return nullptr;
			}
			
			if( stdex::intrusive_dynamic_cast<T>(data) == nullptr )
			{
                throw;
			}
#   endif

            T t = stdex::intrusive_static_cast<T>(data);

            return t;
        }
	};

#   define DATA_SERVICE()\
    SERVICE_GET(Menge::DataServiceInterface)
}

