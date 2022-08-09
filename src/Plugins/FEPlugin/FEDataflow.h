#pragma once

#include "Interface/DataflowInterface.h"
#include "Interface/ArchivatorInterface.h"
#include "Interface/FactoryInterface.h"

#include "Kernel/Factorable.h"

namespace Mengine
{
    //////////////////////////////////////////////////////////////////////////
    class FEDataflow
        : public DataflowInterface
        , public Factorable
    {
        DECLARE_FACTORABLE( FEDataflow );

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
        FactoryInterfacePtr m_factoryFEData;
    };
    //////////////////////////////////////////////////////////////////////////
    typedef IntrusivePtr<FEDataflow, DataflowInterface> FEDataflowPtr;
    //////////////////////////////////////////////////////////////////////////
}