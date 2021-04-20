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
        SERVICE_DECLARE( "DataService" )

    public:
        virtual DataInterfacePtr dataflow( const DataflowInterfacePtr & _dataflow, const InputStreamInterfacePtr & _stream, const DataflowContext * _context, const DocumentPtr & _doc ) = 0;

    public:
        template<class T>
        T dataflowT( const DataflowInterfacePtr & _dataflow, const InputStreamInterfacePtr & _stream, const DataflowContext * _context, const DocumentPtr & _doc )
        {
            DataInterfacePtr data = this->dataflow( _dataflow, _stream, _context, _doc );

#ifdef MENGINE_DEBUG
            if( data == nullptr )
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
}
//////////////////////////////////////////////////////////////////////////
#define DATA_SERVICE()\
    ((Mengine::DataServiceInterface*)SERVICE_GET(Mengine::DataServiceInterface))
//////////////////////////////////////////////////////////////////////////


