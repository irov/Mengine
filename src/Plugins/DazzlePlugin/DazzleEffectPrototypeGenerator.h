#pragma once

#include "Kernel/NodePrototypeGenerator.h"

#include "DazzleEffect.h"

namespace Mengine
{
    //////////////////////////////////////////////////////////////////////////
    class DazzleEffectPrototypeGenerator
        : public NodePrototypeGenerator<DazzleEffect, 32>
    {
    public:
        DazzleEffectPrototypeGenerator();
        ~DazzleEffectPrototypeGenerator() override;

    public:
        void setDazzleService( const dz_service_t * _service );
        const dz_service_t * getDazzleService() const;

    protected:
        FactorablePointer generate( const DocumentInterfacePtr & _doc ) override;

    protected:
        const dz_service_t * m_service;
    };
    //////////////////////////////////////////////////////////////////////////
    typedef IntrusivePtr<DazzleEffectPrototypeGenerator> DazzleEffectPrototypeGeneratorPtr;
    //////////////////////////////////////////////////////////////////////////
}
