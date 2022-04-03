#pragma once

#include "JSONInterface.h"

#include "Kernel/Resource.h"

namespace Mengine
{
    //////////////////////////////////////////////////////////////////////////
    class ResourceJSON
        : public Resource
        , public UnknownResourceJSONInterface
    {
        DECLARE_FACTORABLE( ResourceJSON );
        DECLARE_VISITABLE( Resource );
        DECLARE_UNKNOWABLE();

    public:
        ResourceJSON();
        ~ResourceJSON() override;

    public:
        const jpp::object & getJSON() const override;

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