#pragma once

#include "Interface/DataflowInterface.h"
#include "Interface/ArchivatorInterface.h"

#include "Kernel/Factorable.h"
#include "Kernel/Factory.h"

#include "dazzle/dazzle.hpp"

namespace Mengine
{
    class DataflowDDZ
        : public DataflowInterface
        , public Factorable
    {
    public:
        DataflowDDZ();
        ~DataflowDDZ() override;

    public:
        void setDazzleService( const dz_service_t * _service );
        const dz_service_t * getDazzleService() const;

    public:
        void setArchivator( const ArchivatorInterfacePtr & _archivator );
        const ArchivatorInterfacePtr & getArchivator() const;

    public:
        bool initialize() override;
        void finalize() override;

    public:
        bool isThreadFlow() const override;

    public:
        DataInterfacePtr create( const DocumentPtr & _doc ) override;

    public:
        MemoryInterfacePtr load( const InputStreamInterfacePtr & _stream, const DocumentPtr & _doc ) override;
        bool flow( const DataInterfacePtr & _data, const MemoryInterfacePtr & _memory, const DataflowContext * _context, const DocumentPtr & _doc ) override;

    protected:
        const dz_service_t * m_service;

        ArchivatorInterfacePtr m_archivator;

        FactoryPtr m_factoryDazzleData;
    };
    //////////////////////////////////////////////////////////////////////////
    typedef IntrusivePtr<DataflowDDZ> DataflowDDZPtr;
    //////////////////////////////////////////////////////////////////////////
}