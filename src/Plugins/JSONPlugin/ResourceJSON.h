#pragma once

#include "JSONInterface.h"

#include "Kernel/Resource.h"
#include "Kernel/Content.h"

namespace Mengine
{
    //////////////////////////////////////////////////////////////////////////
    class ResourceJSON
        : public Resource
        , public Content
        , public UnknownResourceJSONInterface
    {
        DECLARE_VISITABLE( Resource );
        DECLARE_UNKNOWABLE();
        DECLARE_CONTENTABLE();

    public:
        ResourceJSON();
        ~ResourceJSON() override;

    public:
        const jpp::object& getJSON() const override;

    protected:
        bool _compile() override;
        void _release() override;

    protected:
        jpp::object m_json;
    };
    //////////////////////////////////////////////////////////////////////////
    typedef IntrusiveResourcePtr<ResourceJSON> ResourceJSONPtr;
    //////////////////////////////////////////////////////////////////////////
}