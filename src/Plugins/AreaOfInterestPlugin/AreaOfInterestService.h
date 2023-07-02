#pragma once

#include "Interface/TimepipeInterface.h"
#include "Interface/FactoryInterface.h"

#include "AreaOfInterestInterface.h"
#include "AreaOfInterestZone.h"

#include "Kernel/ServiceBase.h"
#include "Kernel/Vector.h"

namespace Mengine
{
    class AreaOfInterestService
        : public ServiceBase<AreaOfInterestServiceInterface>
        , public TimepipeInterface
    {
    public:
        AreaOfInterestService();
        ~AreaOfInterestService() override;

    public:
        bool _initializeService() override;
        void _finalizeService() override;

    public:
        AreaOfInterestZoneInterfacePtr createZone( const DocumentInterfacePtr & _doc ) override;
        void removeZone( const AreaOfInterestZoneInterfacePtr & _zone ) override;

    public:
        void onTimepipe( const UpdateContext * _contet ) override;

    protected:
        typedef Vector<AreaOfInterestZonePtr> VectorAreaOfInterestZones;
        VectorAreaOfInterestZones m_zones;

        FactoryInterfacePtr m_factoryAreaOfInterestZones;
    };
}