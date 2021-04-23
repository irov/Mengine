#pragma once

#include "Config/Typedef.h"

#include "Interface/ServiceInterface.h"
#include "Interface/MemoryInterface.h"
#include "Interface/DataInterface.h"
#include "Interface/DataflowInterface.h"

#include "Kernel/Mixin.h"
#include "Kernel/ConstString.h"

#ifdef MENGINE_DEBUG
#   include <type_traits>
#   include <stdexcept>
#endif


namespace Mengine
{
    //////////////////////////////////////////////////////////////////////////
    class DataServiceInterface
        : public ServiceInterface
    {
        SERVICE_DECLARE( "DataService" )

    public:
        virtual DataInterfacePtr dataflow( const DataflowInterfacePtr & _dataflow, const InputStreamInterfacePtr & _stream, const DataflowContext * _context, const DocumentPtr & _doc ) = 0;

    public:
        template<class T>
        T dataflowT( const DataflowInterfacePtr & _dataflow, const InputStreamInterfacePtr & _stream, const DataflowContext * _context, const DocumentPtr & _doc )
        {
            DataInterfacePtr data = this->dataflow( _dataflow, _stream, _context, _doc );

#ifdef MENGINE_DEBUG
            static_assert(std::is_base_of_v<DataInterface, std::remove_pointer_t<typename T::value_type>>, "static data cast use on non 'DataInterface' type");

            if( data == nullptr )
            {
                return nullptr;
            }

            if( stdex::intrusive_dynamic_cast<T>(data) == nullptr )
            {
                throw std::runtime_error( "static data cast" );
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


