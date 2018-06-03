#pragma once

#include "Config/Typedef.h"

#include "Interface/ServiceInterface.h"
#include "Interface/StreamInterface.h"
#include "Interface/MemoryInterface.h"

#include "Factory/Factorable.h"

#include "Core/ConstString.h"
#include "Core/FilePath.h"

#include <stdint.h>

namespace Mengine
{	
	class DataInterface
		: public Factorable
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

	typedef IntrusivePtr<DataInterface> DataInterfacePtr;

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

	typedef IntrusivePtr<DataflowInterface> DataflowInterfacePtr;

	class DataflowFactoryInterface
		: public ServantInterface
	{
    public:
        virtual bool initialize() = 0;

	public:
		virtual DataflowInterfacePtr createDataflow() = 0;
	};

	typedef IntrusivePtr<DataflowFactoryInterface> DataflowFactoryInterfacePtr;

	class DataServiceInterface
		: public ServiceInterface
	{
        SERVICE_DECLARE("DataService")

	public:
		virtual void registerDataflow( const ConstString& _type, const DataflowInterfacePtr & _dataflow ) = 0;
		virtual void unregisterDataflow( const ConstString& _type ) = 0;
		
	public:
		virtual const DataflowInterfacePtr & getDataflow( const ConstString & _type ) const = 0;

    public:
		virtual DataInterfacePtr dataflow( const DataflowInterfacePtr & _dataflow, const InputStreamInterfacePtr & _stream ) = 0;
			
	public:
        template<class T>
        T dataflowT( const DataflowInterfacePtr & _dataflow, const InputStreamInterfacePtr & _stream )
        {
            DataInterfacePtr data = this->dataflow( _dataflow, _stream );

#ifndef NDEBUG
			if (data == nullptr)
			{
				return nullptr;
			}
			
			if( stdex::intrusive_dynamic_cast<T>(data) == nullptr )
			{
                throw;
			}
#endif

            T t = stdex::intrusive_static_cast<T>(data);

            return t;
        }
	};

#   define DATA_SERVICE()\
    ((Mengine::DataServiceInterface*)SERVICE_GET(Mengine::DataServiceInterface))
}

