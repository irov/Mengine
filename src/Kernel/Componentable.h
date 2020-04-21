#pragma once

#include "Kernel/Mixin.h"
#include "Kernel/Component.h"

namespace Mengine
{
    class Componentable
        : public Mixin
    {
    public:
        Componentable();
        ~Componentable() override;

    public:
        void addComponent( const ComponentPtr & _component );

        template<class T>
        void removeComponent()
        {
            uint32_t id = T::getComponentId();

            this->removeComponentId( id );
        }

        template<class T>
        T * getComponent() const
        {
            uint32_t id = T::getComponentId();

            const ComponentPtr & component = this->getComponentId( id );

            T * t = component.getT<T *>();

            return t;
        }

    protected:
        void removeComponentId( uint32_t id );
        MENGINE_INLINE const ComponentPtr & getComponentId( uint32_t id ) const;

    protected:
        uint32_t m_componentFamily;

        ComponentPtr m_components[32];
    };
    //////////////////////////////////////////////////////////////////////////
    MENGINE_INLINE const ComponentPtr & Componentable::getComponentId( uint32_t id ) const
    {
        return m_components[id];
    }
}