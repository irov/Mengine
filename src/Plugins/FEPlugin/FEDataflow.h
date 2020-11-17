#pragma once

#include "Interface/DataflowInterface.h"
#include "Interface/ArchivatorInterface.h"

#include "Kernel/Factorable.h"
#include "Kernel/Factory.h"

namespace Mengine
{
    //////////////////////////////////////////////////////////////////////////
    class FEDataflow
        : public DataflowInterface
        , public Factorable
    {
    public:
        FEDataflow();
        ~FEDataflow() override;

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
        FactoryPtr m_factoryFEData;
    };
    //////////////////////////////////////////////////////////////////////////
    typedef IntrusivePtr<FEDataflow, DataflowInterface> FEDataflowPtr;
    //////////////////////////////////////////////////////////////////////////
}