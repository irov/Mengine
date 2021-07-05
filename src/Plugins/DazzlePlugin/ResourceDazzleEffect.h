#pragma once

#include "DazzleInterface.h"
#include "DazzleIncluder.h"
#include "DazzleDataInterface.h"

#include "Kernel/Resource.h"

namespace Mengine
{
    //////////////////////////////////////////////////////////////////////////
    class ResourceDazzleEffect
        : public Resource
        , public UnknownResourceDazzleEffectInterface
    {
        DECLARE_FACTORABLE( ResourceDazzleEffect );
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