#pragma once

#include "Interface/ServantInterface.h"
#include "Interface/ComponentInterface.h"

#include "Config/UniqueId.h"

namespace Mengine
{
    //////////////////////////////////////////////////////////////////////////
    class ComponentHubInterface
        : public ServantInterface
    {
    public:
        virtual void setMask( uint32_t _id ) = 0;
        virtual uint32_t getMask() const = 0;

    public:
        virtual ComponentInterfacePtr createComponent( UniqueId _id ) = 0;
        virtual void removeComponent( UniqueId _id ) = 0;

    public:
        virtual const ComponentInterfacePtr & getComponent( UniqueId _id ) const = 0;

    public:
        friend class ComponentServiceInterface;
    };
    //////////////////////////////////////////////////////////////////////////
    typedef IntrusivePtr<ComponentHubInterface> ComponentHubInterfacePtr;
    //////////////////////////////////////////////////////////////////////////
}