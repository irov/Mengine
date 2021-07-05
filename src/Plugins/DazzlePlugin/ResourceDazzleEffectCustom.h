#pragma once

#include "DazzleInterface.h"
#include "DazzleIncluder.h"
#include "DazzleDataInterface.h"

#include "Kernel/Resource.h"

namespace Mengine
{
    //////////////////////////////////////////////////////////////////////////
    class ResourceDazzleEffectCustom
        : public Resource
        , public UnknownResourceDazzleEffectCustomInterface
    {
        DECLARE_FACTORABLE( ResourceDazzleEffectCustom );
        DECLARE_VISITABLE( Resource );
        DECLARE_UNKNOWABLE();

    public:
        ResourceDazzleEffectCustom();
        ~ResourceDazzleEffectCustom() override;

    public:
        void setDazzleEffect( const dz_effect_t * _effect ) override;
        const dz_effect_t * getDazzleEffect() const override;

    protected:
        const DataInterfacePtr & getData() const override;

    protected:
        bool _compile() override;
        void _release() override;

    protected:
        const dz_effect_t * m_effect;
    };
    //////////////////////////////////////////////////////////////////////////
    typedef IntrusiveResourcePtr<ResourceDazzleEffectCustom> ResourceDazzleEffectCustomPtr;
    //////////////////////////////////////////////////////////////////////////
}