#include "AreaOfInterestService.h"

#include "Kernel/FactoryPool.h"
#include "Kernel/AssertionFactory.h"
#include "Kernel/DocumentHelper.h"

#include <algorithm>

namespace Mengine
{
    //////////////////////////////////////////////////////////////////////////
    AreaOfInterestService::AreaOfInterestService()
    {
    }
    //////////////////////////////////////////////////////////////////////////
    AreaOfInterestService::~AreaOfInterestService()
    {
    }
    //////////////////////////////////////////////////////////////////////////
    bool AreaOfInterestService::_initializeService()
    {
        m_factoryAreaOfInterestZones = Helper::makeFactoryPool<AreaOfInterestZone, 8>( MENGINE_DOCUMENT_FACTORABLE );

        return true;
    }
    //////////////////////////////////////////////////////////////////////////
    void AreaOfInterestService::_finalizeService()
    {
        m_zones.clear();

        MENGINE_ASSERTION_FACTORY_EMPTY( m_factoryAreaOfInterestZones );

        m_factoryAreaOfInterestZones = nullptr;
    }
    //////////////////////////////////////////////////////////////////////////
    AreaOfInterestZoneInterfacePtr AreaOfInterestService::createZone( const DocumentPtr & _doc )
    {
        AreaOfInterestZonePtr zone = m_factoryAreaOfInterestZones->createObject( _doc );

        m_zones.emplace_back( zone );

        return zone;
    }
    //////////////////////////////////////////////////////////////////////////
    void AreaOfInterestService::removeZone( const AreaOfInterestZoneInterfacePtr & _zone )
    {
        AreaOfInterestZonePtr zone = _zone;

        VectorAreaOfInterestZones::iterator it_found = std::find( m_zones.begin(), m_zones.end(), zone );

        MENGINE_ASSERTION_FATAL( it_found != m_zones.end() );

        m_zones.erase( it_found );

        zone->finalize();
    }
    //////////////////////////////////////////////////////////////////////////
    void AreaOfInterestService::updateZones()
    {
        for( const AreaOfInterestZonePtr & zone : m_zones )
        {
            zone->update();
        }
    }
    //////////////////////////////////////////////////////////////////////////
}