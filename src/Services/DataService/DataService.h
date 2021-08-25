#pragma once

#include "Interface/DataServiceInterface.h"

#include "Kernel/ServiceBase.h"

namespace Mengine
{
    class DataService
        : public ServiceBase<DataServiceInterface>
    {
    public:
        DataService();
        ~DataService() override;

    public:
        bool _initializeService() override;
        void _finalizeService() override;

    public:
        DataInterfacePointer dataflow( const DataflowInterfacePtr & _dataflow, const InputStreamInterfacePtr & _stream, const DataflowContext * _context, const DocumentPtr & _doc ) override;
    };
}