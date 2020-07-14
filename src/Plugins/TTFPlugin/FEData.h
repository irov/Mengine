#pragma once

#include "FEDataInterface.h"

namespace Mengine
{
    //////////////////////////////////////////////////////////////////////////
    class FEData
        : public FEDataInterface
    {
    public:
        FEData();
        ~FEData() override;

    public:
        bool acquire() override;
        void release() override;

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