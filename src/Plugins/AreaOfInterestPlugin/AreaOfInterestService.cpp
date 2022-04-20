#include "AreaOfInterestService.h"

#include "Interface/TimepipeServiceInterface.h"
#include "Interface/PrototypeServiceInterface.h"

#include "NodeAreaOfInterestTrigger.h"
#include "NodeAreaOfInterestActor.h"

#ifdef MENGINE_USE_SCRIPT_SERVICE
#   include "Interface/ScriptServiceInterface.h"

#   include "AreaOfInterestScriptEmbedding.h"
#endif

#include "Kernel/NotificationHelper.h"
#include "Kernel/NodePrototypeGenerator.h"
#include "Kernel/FactoryPool.h"
#include "Kernel/AssertionFactory.h"
#include "Kernel/DocumentHelper.h"

#include "Config/Algorithm.h"

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
        PROTOTYPE_SERVICE()
            ->addPrototype( STRINGIZE_STRING_LOCAL( "Node" ), STRINGIZE_STRING_LOCAL( "NodeAreaOfInterestTrigger" ), Helper::makeFactorableUnique<NodePrototypeGenerator<NodeAreaOfInterestTrigger, 32>>( MENGINE_DOCUMENT_FACTORABLE ) );

        PROTOTYPE_SERVICE()
            ->addPrototype( STRINGIZE_STRING_LOCAL( "Node" ), STRINGIZE_STRING_LOCAL( "NodeAreaOfInterestActor" ), Helper::makeFactorableUnique<NodePrototypeGenerator<NodeAreaOfInterestActor, 32>>( MENGINE_DOCUMENT_FACTORABLE ) );

#ifdef MENGINE_USE_SCRIPT_SERVICE
        NOTIFICATION_ADDOBSERVERLAMBDA_THIS( NOTIFICATOR_SCRIPT_EMBEDDING, [this]()
        {
            SCRIPT_SERVICE()
                ->addScriptEmbedding( STRINGIZE_STRING_LOCAL( "AreaOfInterestScriptEmbedding" ), Helper::makeFactorableUnique<AreaOfInterestScriptEmbedding>( MENGINE_DOCUMENT_FACTORABLE ) );
        }, MENGINE_DOCUMENT_FACTORABLE );

        NOTIFICATION_ADDOBSERVERLAMBDA_THIS( NOTIFICATOR_SCRIPT_EJECTING, []()
        {
            SCRIPT_SERVICE()
                ->removeScriptEmbedding( STRINGIZE_STRING_LOCAL( "Movie2ScriptEmbedding" ) );
        }, MENGINE_DOCUMENT_FACTORABLE );
#endif

        m_factoryAreaOfInterestZones = Helper::makeFactoryPool<AreaOfInterestZone, 8>( MENGINE_DOCUMENT_FACTORABLE );

        TIMEPIPE_SERVICE()
            ->addTimepipe( TimepipeInterfacePtr::from( this ), MENGINE_DOCUMENT_FACTORABLE );

        return true;
    }
    //////////////////////////////////////////////////////////////////////////
    void AreaOfInterestService::_finalizeService()
    {
#ifdef MENGINE_USE_SCRIPT_SERVICE
        NOTIFICATION_REMOVEOBSERVER_THIS( NOTIFICATOR_SCRIPT_EMBEDDING );
        NOTIFICATION_REMOVEOBSERVER_THIS( NOTIFICATOR_SCRIPT_EJECTING );
#endif

        PROTOTYPE_SERVICE()
            ->removePrototype( STRINGIZE_STRING_LOCAL( "Node" ), STRINGIZE_STRING_LOCAL( "NodeAreaOfInterestTrigger" ), nullptr );

        PROTOTYPE_SERVICE()
            ->removePrototype( STRINGIZE_STRING_LOCAL( "Node" ), STRINGIZE_STRING_LOCAL( "NodeAreaOfInterestTrigger" ), nullptr );

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

        VectorAreaOfInterestZones::iterator it_found = Algorithm::find( m_zones.begin(), m_zones.end(), zone );

        MENGINE_ASSERTION_FATAL( it_found != m_zones.end() );

        m_zones.erase( it_found );

        zone->finalize();
    }
    //////////////////////////////////////////////////////////////////////////
    void AreaOfInterestService::onTimepipe( const UpdateContext * _contet )
    {
        MENGINE_UNUSED( _contet );

        for( const AreaOfInterestZonePtr & zone : m_zones )
        {
            zone->update();
        }
    }
    //////////////////////////////////////////////////////////////////////////
}