#include "PathfinderService.h"

#include "PathfinderMap.h"

#include "Kernel/NotificationHelper.h"
#include "Kernel/FactoryDefault.h"
#include "Kernel/AssertionFactory.h"

namespace Mengine
{
    //////////////////////////////////////////////////////////////////////////
    PathfinderService::PathfinderService()
    {
    }
    //////////////////////////////////////////////////////////////////////////
    PathfinderService::~PathfinderService()
    {
    }
    //////////////////////////////////////////////////////////////////////////
    bool PathfinderService::_initializeService()
    {
        m_factoryPathfinderMap = Helper::makeFactoryDefault<PathfinderMap>( MENGINE_DOCUMENT_FACTORABLE );

        return true;
    }
    //////////////////////////////////////////////////////////////////////////
    void PathfinderService::_finalizeService()
    {
        MENGINE_ASSERTION_FACTORY_EMPTY( m_factoryPathfinderMap );

        m_factoryPathfinderMap = nullptr;
    }
    //////////////////////////////////////////////////////////////////////////
    PathfinderMapInterfacePtr PathfinderService::createPathfinderMap( uint16_t _width, uint16_t _height )
    {
        PathfinderMapPtr map = m_factoryPathfinderMap->createObject( MENGINE_DOCUMENT_FACTORABLE );

        MENGINE_ASSERTION_MEMORY_PANIC( map, "invalid create map" );

        map->initialize( _width, _height );

        return map;
    }
    //////////////////////////////////////////////////////////////////////////
}