#pragma once

#include "DazzleInterface.h"
#include "DazzleIncluder.h"
#include "DazzleDataInterface.h"

#include "Kernel/Data.h"
#include "Kernel/Resource.h"
#include "Kernel/ResourceImage.h"

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
        void setResourceEmitterImage( const ResourceImagePtr & _resource ) override;
        const ResourceImagePtr & getResourceEmitterImage() const override;

        const DataInterfacePtr & getData() const override;

    protected:
        bool _compile() override;
        void _release() override;
        void _finalize() override;

    protected:
        DazzleDataInterfacePtr m_data;
        ResourceImagePtr m_resourceEmitterImage;
        Data m_emitterTextureBuffer;
    };
    //////////////////////////////////////////////////////////////////////////
    typedef IntrusiveResourcePtr<ResourceDazzleEffect> ResourceDazzleEffectPtr;
    //////////////////////////////////////////////////////////////////////////
}
