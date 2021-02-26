#pragma once

#include "DazzleInterface.h"
#include "DazzleDataInterface.h"

#include "Kernel/Resource.h"

#include "dazzle/dazzle.hpp"

namespace Mengine
{
    //////////////////////////////////////////////////////////////////////////
    class ResourceDazzleEffect
        : public Resource
        , public UnknownResourceDazzleEffectInterface
    {
        DECLARE_VISITABLE( Resource );
        DECLARE_UNKNOWABLE();

    public:
        ResourceDazzleEffect();
        ~ResourceDazzleEffect() override;

    public:
        const DataInterfacePtr & getData() const override;

    protected:
        bool _compile() override;
        void _release() override;

    protected:
        DazzleDataInterfacePtr m_data;
    };
    //////////////////////////////////////////////////////////////////////////
    typedef IntrusiveResourcePtr<ResourceDazzleEffect> ResourceDazzleEffectPtr;
    //////////////////////////////////////////////////////////////////////////
}