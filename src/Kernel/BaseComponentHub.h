#pragma once

#include "Interface/ComponentHubInterface.h"

#include "Kernel/Hashtable.h"

namespace Mengine
{
    class BaseComponentHub
        : public ComponentHubInterface
    {
    public:
        BaseComponentHub();
        ~BaseComponentHub() override;

    public:
        void setMask( uint32_t _mask ) override;
        uint32_t getMask() const override;

    public:
        const ComponentInterfacePtr & getComponent( UniqueId _id ) const override;

    protected:
        uint32_t m_mask;

        typedef Hashtable<UniqueId, ComponentInterfacePtr> HashtableComponents;
        HashtableComponents m_components;
    };
}