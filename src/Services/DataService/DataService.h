#pragma once

#include "Interface/DataServiceInterface.h"

#include "Kernel/ServiceBase.h"

namespace Mengine
{
    class DataService
        : public ServiceBase<DataServiceInterface>
    {
        DECLARE_FACTORABLE( DataService );

    public:
        DataService();
        ~DataService() override;

    public:
        bool _initializeService() override;
        void _finalizeService() override;

    public:
        DataInterfacePointer dataflow( const DataflowInterfacePtr & _dataflow, const InputStreamInterfacePtr & _stream, const DataflowContext * _context, const DocumentInterfacePtr & _doc ) override;
    };
}