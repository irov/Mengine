#pragma once

#include "Interface/ComponentInterface.h"
#include "Interface/ComponentServiceInterface.h"

#include "Kernel/Mixin.h"

namespace Mengine
{
    //////////////////////////////////////////////////////////////////////////
    class ComponentHubInterface
        : public Mixin
    {
    public:
        virtual void addComponent( const ComponentInterfacePtr & _component ) = 0;

    public:
        template<class T>
        void removeComponent()
        {
            uint32_t id = T::getComponentId();

            this->removeComponentById( id );
        }

        template<class T>
        T * getComponent() const
        {
            uint32_t id = T::getComponentId();

            const ComponentInterfacePtr & component = this->getComponentById( id );

            T * t = component.getT<T *>();

            return t;
        }

    protected:
        virtual void removeComponentById( uint32_t id ) = 0;
        virtual const ComponentInterfacePtr & getComponentById( uint32_t id ) const = 0;
    };
    //////////////////////////////////////////////////////////////////////////
    typedef IntrusivePtr<ComponentHubInterface> ComponentHubInterfacePtr;
    //////////////////////////////////////////////////////////////////////////
}