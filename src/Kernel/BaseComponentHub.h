#pragma once

#include "Interface/ComponentHubInterface.h"

namespace Mengine
{
    //////////////////////////////////////////////////////////////////////////
    class BaseComponentHub
        : public ComponentHubInterface
    {
    public:
        BaseComponentHub();
        ~BaseComponentHub() override;

    public:
        void addComponent( const ComponentInterfacePtr & _component ) override;

    public:
        void removeComponentById( uint32_t id ) override;
        MENGINE_INLINE const ComponentInterfacePtr & getComponentById( uint32_t id ) const override;

    protected:
        uint32_t m_componentFamily;

        ComponentInterfacePtr m_components[32];
    };
    //////////////////////////////////////////////////////////////////////////
    MENGINE_INLINE const ComponentInterfacePtr & BaseComponentHub::getComponentById( uint32_t id ) const
    {
        return m_components[id];
    }
    //////////////////////////////////////////////////////////////////////////
}