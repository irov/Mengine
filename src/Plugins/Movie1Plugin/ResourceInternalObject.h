#pragma once

#include "Kernel/Resource.h"

namespace Mengine
{
    //////////////////////////////////////////////////////////////////////////
    class ResourceInternalObject
        : public Resource
    {
        DECLARE_FACTORABLE( ResourceInternalObject );
        DECLARE_VISITABLE( Resource );

    public:
        ResourceInternalObject();
        ~ResourceInternalObject() override;

    public:
        void setInternalName( const ConstString & _internalName );
        const ConstString & getInternalName() const;

        void setInternalGroup( const ConstString & _internalGroup );
        const ConstString & getInternalGroup() const;

    protected:
        bool _compile() override;
        void _release() override;

    protected:
        ConstString m_internalName;
        ConstString m_internalGroup;
    };
    //////////////////////////////////////////////////////////////////////////
    typedef IntrusiveResourcePtr<ResourceInternalObject> ResourceInternalObjectPtr;
    //////////////////////////////////////////////////////////////////////////
}