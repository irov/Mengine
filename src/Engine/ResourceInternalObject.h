#pragma once

#include "Kernel/Resource.h"

namespace Mengine
{
    class ResourceInternalObject
        : public Resource
    {
        DECLARE_VISITABLE( Resource );

    public:
        ResourceInternalObject();
        ~ResourceInternalObject() override;

    public:
        const ConstString & getInternalName() const;
        const ConstString & getInternalGroup() const;

    protected:
        bool _loader( const Metabuf::Metadata * _parser ) override;

    protected:
        bool _compile() override;
        void _release() override;

    protected:
        ConstString m_internalName;
        ConstString m_internalGroup;
    };
    //////////////////////////////////////////////////////////////////////////
    typedef IntrusiveResourcePtr<ResourceInternalObject> ResourceInternalObjectPtr;
}