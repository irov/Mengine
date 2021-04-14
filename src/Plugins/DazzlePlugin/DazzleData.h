#pragma once

#include "Interface/RenderMaterialInterface.h"

#include "DazzleDataInterface.h"

#include "Kernel/Pool.h"

namespace Mengine
{
    //////////////////////////////////////////////////////////////////////////
    class DazzleData
        : public DazzleDataInterface
    {
        DECLARE_FACTORABLE( DazzleData );

    public:
        DazzleData();
        ~DazzleData() override;

    public:
        bool acquire() override;
        void release() override;

    public:
        void setDazzleService( const dz_service_t * _service );
        const dz_service_t * getDazzleService() const;

    public:
        void setDazzleEffect( const dz_effect_t * _effect );
        const dz_effect_t * getDazzleEffect() const override;

    protected:
        const dz_service_t * m_service;
        const dz_effect_t * m_effect;
    };
    //////////////////////////////////////////////////////////////////////////
    typedef IntrusivePtr<DazzleData, DazzleDataInterface> DazzleDataPtr;
    //////////////////////////////////////////////////////////////////////////
}