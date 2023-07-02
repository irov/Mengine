#pragma once

#include "Config/Typedef.h"

#include "Interface/ServiceInterface.h"
#include "Interface/MemoryInterface.h"
#include "Interface/DataInterface.h"
#include "Interface/DataflowInterface.h"

#include "Kernel/ConstString.h"
#include "Kernel/DataInterfacePointer.h"

namespace Mengine
{    
    class DataServiceInterface
        : public ServiceInterface
    {
        SERVICE_DECLARE( "DataService" )

    public:
        virtual DataInterfacePointer dataflow( const DataflowInterfacePtr & _dataflow, const InputStreamInterfacePtr & _stream, const DataflowContext * _context, const DocumentInterfacePtr & _doc ) = 0;
    };
}
//////////////////////////////////////////////////////////////////////////
#define DATA_SERVICE()\
    ((Mengine::DataServiceInterface*)SERVICE_GET(Mengine::DataServiceInterface))
//////////////////////////////////////////////////////////////////////////


