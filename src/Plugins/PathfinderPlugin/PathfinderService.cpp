#include "PathfinderService.h"

#include "PathfinderMap.h"

#if defined(MENGINE_USE_SCRIPT_SERVICE)
#   include "Interface/ScriptServiceInterface.h"

#   include "PathfinderScriptEmbedding.h"
#endif

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
#if defined(MENGINE_USE_SCRIPT_SERVICE)
        NOTIFICATION_ADDOBSERVERLAMBDA_THIS( NOTIFICATOR_SCRIPT_EMBEDDING, [this]()
        {
            SCRIPT_SERVICE()
                ->addScriptEmbedding( PathfinderScriptEmbedding::getFactorableType(), Helper::makeFactorableUnique<PathfinderScriptEmbedding>( MENGINE_DOCUMENT_FACTORABLE ) );
        }, MENGINE_DOCUMENT_FACTORABLE );

        NOTIFICATION_ADDOBSERVERLAMBDA_THIS( NOTIFICATOR_SCRIPT_EJECTING, []()
        {
            SCRIPT_SERVICE()
                ->removeScriptEmbedding( PathfinderScriptEmbedding::getFactorableType() );
        }, MENGINE_DOCUMENT_FACTORABLE );
#endif

        m_factoryPathfinderMap = Helper::makeFactoryDefault<PathfinderMap>( MENGINE_DOCUMENT_FACTORABLE );

        return true;
    }
    //////////////////////////////////////////////////////////////////////////
    void PathfinderService::_finalizeService()
    {
#if defined(MENGINE_USE_SCRIPT_SERVICE)
        NOTIFICATION_REMOVEOBSERVER_THIS( NOTIFICATOR_SCRIPT_EMBEDDING );
        NOTIFICATION_REMOVEOBSERVER_THIS( NOTIFICATOR_SCRIPT_EJECTING );
#endif

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