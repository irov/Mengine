#pragma once

#include "Interface/DataflowInterface.h"
#include "Interface/FactoryInterface.h"

#include "Kernel/Factorable.h"

namespace Mengine
{
    //////////////////////////////////////////////////////////////////////////
    class FontEffectDataflow
        : public DataflowInterface
        , public Factorable
    {
        DECLARE_FACTORABLE( FontEffectDataflow );

    public:
        FontEffectDataflow();
        ~FontEffectDataflow() override;

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
        FactoryInterfacePtr m_factoryFontEffectData;
    };
    //////////////////////////////////////////////////////////////////////////
    typedef IntrusivePtr<FontEffectDataflow, DataflowInterface> FontEffectDataflowPtr;
    //////////////////////////////////////////////////////////////////////////
}
