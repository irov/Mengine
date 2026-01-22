#pragma once

#include "Interface/ComponentServiceInterface.h"

#include "Kernel/ServiceBase.h"
#include "Kernel/Hashtable.h"
#include "Kernel/ConstString.h"

namespace Mengine
{
    class ComponentService
        : public ServiceBase<ComponentServiceInterface>
    {
        DECLARE_FACTORABLE( ComponentService );

    public:
        ComponentService();
        ~ComponentService() override;

    public:
        bool _initializeService() override;
        void _finalizeService() override;

    public:
        void addComponentHub( const ConstString & _type, const ComponentHubInterfacePtr & _componentHub ) override;
        ComponentHubInterfacePtr removeComponentHub( const ConstString & _type ) override;

    public:
        const ComponentHubInterfacePtr & getComponentHub( const ConstString & _type ) const override;

    protected:
        uint32_t m_enumerator;

        typedef Hashtable<ConstString, ComponentHubInterfacePtr> HashtableComponentHubs;
        HashtableComponentHubs m_componentHubs;
    };
}
