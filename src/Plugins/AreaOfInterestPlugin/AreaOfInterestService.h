#pragma once

#include "AreaOfInterestInterface.h"

#include "AreaOfInterestZone.h"

#include "Kernel/ServiceBase.h"
#include "Kernel/Factory.h"
#include "Kernel/Vector.h"

namespace Mengine
{
    class AreaOfInterestService
        : public ServiceBase<AreaOfInterestServiceInterface>
    {
    public:
        AreaOfInterestService();
        ~AreaOfInterestService() override;

    public:
        bool _initializeService() override;
        void _finalizeService() override;

    public:
        AreaOfInterestZoneInterfacePtr createZone( const DocumentPtr & _doc ) override;
        void removeZone( const AreaOfInterestZoneInterfacePtr & _zone ) override;

    public:
        void updateZones() override;

    protected:
        typedef Vector<AreaOfInterestZonePtr> VectorAreaOfInterestZones;
        VectorAreaOfInterestZones m_zones;

        FactoryPtr m_factoryAreaOfInterestZones;
    };
    //////////////////////////////////////////////////////////////////////////
    typedef IntrusivePtr<AreaOfInterestService> AreaOfInterestPtr;
    //////////////////////////////////////////////////////////////////////////
}