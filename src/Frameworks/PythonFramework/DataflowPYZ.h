#pragma once

#include "Interface/DataflowInterface.h"
#include "Interface/MemoryInterface.h"
#include "Interface/ArchivatorInterface.h"

#include "Environment/Python/PythonIncluder.h"

#include "Kernel/Factorable.h"
#include "Kernel/Factory.h"

namespace Mengine
{
    //////////////////////////////////////////////////////////////////////////
    class DataflowPYZ
        : public DataflowInterface
        , public Factorable
    {
        DECLARE_FACTORABLE( DataflowPY );

    public:
        DataflowPYZ();
        ~DataflowPYZ() override;

    public:
        void setKernel( pybind::kernel_interface * _kernel );
        pybind::kernel_interface * getKernel() const;

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
        pybind::kernel_interface * m_kernel;

        ArchivatorInterfacePtr m_archivator;

        FactoryPtr m_factoryScriptCodeData;
    };
    //////////////////////////////////////////////////////////////////////////
    typedef IntrusivePtr<DataflowPYZ, DataflowInterface> DataflowPYZPtr;
    //////////////////////////////////////////////////////////////////////////
}