#pragma once

#include "FEDataInterface.h"

#include "Kernel/Factorable.h"

namespace Mengine
{
    //////////////////////////////////////////////////////////////////////////
    class FEData
        : public FEDataInterface
        , public Factorable
    {
    public:
        FEData();
        ~FEData() override;

    public:
        bool acquire() override;
        void release() override;

    protected:
        Pointer allocateMemory( size_t _size ) const override;

    public:
        void setFEBundle( fe_bundle * _bundle );
        fe_bundle * getFEBundle() const override;

    protected:
        fe_bundle * m_bundle;
    };
    //////////////////////////////////////////////////////////////////////////
    typedef IntrusivePtr<FEData, DataInterface> FEDataPtr;
    //////////////////////////////////////////////////////////////////////////
}