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
        DataInterfacePtr dataflow( const DataflowInterfacePtr & _dataflow, const InputStreamInterfacePtr & _stream, const Char * _doc ) override;
    };
}