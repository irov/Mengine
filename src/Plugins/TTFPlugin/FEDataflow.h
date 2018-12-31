#pragma once

#include "Interface/DataflowInterface.h"
#include "Interface/ArchivatorInterface.h"

#include "Kernel/Factorable.h"
#include "Kernel/Factory.h"

namespace Mengine
{
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
        DataInterfacePtr create() override;

    public:
        bool load( const DataInterfacePtr & _data, const InputStreamInterfacePtr & _stream, const Char * _doc ) override;

    protected:
        FactoryPtr m_factoryFEData;
    };
    //////////////////////////////////////////////////////////////////////////
    typedef IntrusivePtr<FEDataflow, DataflowInterface> FEDataflowPtr;
    //////////////////////////////////////////////////////////////////////////
}