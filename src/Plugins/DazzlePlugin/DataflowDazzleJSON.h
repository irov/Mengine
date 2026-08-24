#pragma once

#include "Interface/DataflowInterface.h"
#include "Interface/FactoryInterface.h"

#include "DazzleIncluder.h"

#include "Kernel/Factorable.h"

namespace Mengine
{
    //////////////////////////////////////////////////////////////////////////
    class DataflowDazzleJSON
        : public DataflowInterface
        , public Factorable
    {
        DECLARE_FACTORABLE( DataflowDazzleJSON );

    public:
        DataflowDazzleJSON();
        ~DataflowDazzleJSON() override;

    public:
        void setDazzleService( const dz_service_t * _service );

    public:
        bool initialize() override;
        void finalize() override;

    public:
        bool isThreadFlow() const override;

    public:
        DataInterfacePtr create( const DocumentInterfacePtr & _doc ) override;

    public:
        MemoryInterfacePtr load( const InputStreamInterfacePtr & _stream, const DocumentInterfacePtr & _doc ) override;
        bool flow( const DataInterfacePtr & _data, const MemoryInterfacePtr & _memory, const DataflowContext * _context, const DocumentInterfacePtr & _doc ) override;

    protected:
        const dz_service_t * m_service;
        FactoryInterfacePtr m_factoryDazzleData;
    };
    //////////////////////////////////////////////////////////////////////////
    typedef IntrusivePtr<DataflowDazzleJSON, DataflowInterface> DataflowDazzleJSONPtr;
    //////////////////////////////////////////////////////////////////////////
}
