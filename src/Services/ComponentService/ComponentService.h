#pragma once

#include "Interface/ComponentServiceInterface.h"

#include "Kernel/ServiceBase.h"
#include "Kernel/Map.h"

namespace Mengine
{
    class ComponentService
        : public ServiceBase<ComponentServiceInterface>
    {
    public:
        ComponentService();
        ~ComponentService() override;

    public:
        bool _initializeService() override;
        void _finalizeService() override;

    protected:
        uint32_t makeComponentId( const std::type_index & _index ) override;

    protected:
        uint32_t m_enumerator;

        typedef Map<std::type_index, uint32_t> MapComponentIds;
        MapComponentIds m_componentIds;
    };
}
