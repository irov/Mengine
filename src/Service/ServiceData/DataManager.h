#pragma once

#include "Interface/DataInterface.h"

#include "Kernel/ServiceBase.h"

#include "Config/Map.h"

namespace Mengine
{
    class DataManager
        : public ServiceBase<DataServiceInterface>
    {
    public:
        DataManager();
        ~DataManager() override;

    public:
        void registerDataflow( const ConstString& _type, const DataflowInterfacePtr & _dataflow ) override;
        void unregisterDataflow( const ConstString& _type ) override;

    public:
        const DataflowInterfacePtr & getDataflow( const ConstString & _type ) const override;

    public:
        DataInterfacePtr dataflow( const DataflowInterfacePtr & _dataflow, const InputStreamInterfacePtr & _stream ) override;

    protected:
        typedef Map<ConstString, DataflowInterfacePtr> MapDataflow;
        MapDataflow m_dataflows;
    };
}