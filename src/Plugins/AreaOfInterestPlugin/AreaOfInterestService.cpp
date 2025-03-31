#include "AreaOfInterestService.h"

#include "Interface/TimepipeServiceInterface.h"
#include "Interface/PrototypeServiceInterface.h"

#include "NodeAreaOfInterestTrigger.h"
#include "NodeAreaOfInterestActor.h"

#if defined(MENGINE_BUILD_MENGINE_SCRIPT_EMBEDDED)
#   include "Interface/ScriptServiceInterface.h"

#   include "AreaOfInterestScriptEmbedding.h"
#endif

#include "Kernel/NotificationHelper.h"
#include "Kernel/NodePrototypeGenerator.h"
#include "Kernel/FactoryPool.h"
#include "Kernel/AssertionFactory.h"
#include "Kernel/PrototypeHelper.h"

#include "Config/StdAlgorithm.h"

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
        Helper::addNodePrototype<NodeAreaOfInterestTrigger, 32>( MENGINE_DOCUMENT_FACTORABLE );
        Helper::addNodePrototype<NodeAreaOfInterestActor, 32>( MENGINE_DOCUMENT_FACTORABLE );

#if defined(MENGINE_BUILD_MENGINE_SCRIPT_EMBEDDED)
        NOTIFICATION_ADDOBSERVERLAMBDA_THIS( NOTIFICATOR_SCRIPT_EMBEDDING, [this]()
        {
            SCRIPT_SERVICE()
                ->addScriptEmbedding( AreaOfInterestScriptEmbedding::getFactorableType(), Helper::makeFactorableUnique<AreaOfInterestScriptEmbedding>( MENGINE_DOCUMENT_FACTORABLE ) );
        }, MENGINE_DOCUMENT_FACTORABLE );

        NOTIFICATION_ADDOBSERVERLAMBDA_THIS( NOTIFICATOR_SCRIPT_EJECTING, []()
        {
            SCRIPT_SERVICE()
                ->removeScriptEmbedding( AreaOfInterestScriptEmbedding::getFactorableType() );
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
#if defined(MENGINE_BUILD_MENGINE_SCRIPT_EMBEDDED)
        NOTIFICATION_REMOVEOBSERVER_THIS( NOTIFICATOR_SCRIPT_EMBEDDING );
        NOTIFICATION_REMOVEOBSERVER_THIS( NOTIFICATOR_SCRIPT_EJECTING );
#endif

        Helper::removeNodePrototype<NodeAreaOfInterestTrigger>();
        Helper::removeNodePrototype<NodeAreaOfInterestTrigger>();

        m_zones.clear();

        MENGINE_ASSERTION_FACTORY_EMPTY( m_factoryAreaOfInterestZones );

        m_factoryAreaOfInterestZones = nullptr;
    }
    //////////////////////////////////////////////////////////////////////////
    AreaOfInterestZoneInterfacePtr AreaOfInterestService::createZone( const DocumentInterfacePtr & _doc )
    {
        AreaOfInterestZonePtr zone = m_factoryAreaOfInterestZones->createObject( _doc );

        m_zones.emplace_back( zone );

        return zone;
    }
    //////////////////////////////////////////////////////////////////////////
    void AreaOfInterestService::removeZone( const AreaOfInterestZoneInterfacePtr & _zone )
    {
        AreaOfInterestZonePtr zone = _zone;

        VectorAreaOfInterestZones::iterator it_found = StdAlgorithm::find( m_zones.begin(), m_zones.end(), zone );

        MENGINE_ASSERTION_FATAL( it_found != m_zones.end(), "zone not found" );

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