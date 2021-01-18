#pragma once

#include "JSONInterface.h"

#include "Kernel/Resource.h"
#include "Kernel/BaseContent.h"

namespace Mengine
{
    class ResourceJSON
        : public Resource
        , public UnknownResourceJSONInterface
        , protected BaseContent        
    {
        DECLARE_VISITABLE( Resource );
        DECLARE_UNKNOWABLE();
        DECLARE_CONTENTABLE();

    public:
        ResourceJSON();
        ~ResourceJSON() override;

    public:
        const JSONStorageInterfacePtr & getJSONStorage() const override;

    protected:
        bool _compile() override;
        void _release() override;

    protected:
        JSONStorageInterfacePtr m_storage;
    };
    //////////////////////////////////////////////////////////////////////////
    typedef IntrusiveResourcePtr<ResourceJSON> ResourceJSONPtr;
    //////////////////////////////////////////////////////////////////////////
}