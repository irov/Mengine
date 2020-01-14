#include "VectorizatorSceneEventReceiver.h"

#include "Engine/Vectorizator.h"
#include "Engine/NodeGenerators.h"

#include "Kernel/Logger.h"

namespace Mengine
{
    //////////////////////////////////////////////////////////////////////////
    VectorizatorSceneEventReceiver::VectorizatorSceneEventReceiver()
    {
    }
    //////////////////////////////////////////////////////////////////////////
    VectorizatorSceneEventReceiver::~VectorizatorSceneEventReceiver()
    {
    }
    //////////////////////////////////////////////////////////////////////////
    bool VectorizatorSceneEventReceiver::onEntityCreate( const EntityBehaviorInterfacePtr & _behavior, Entity * _entity )
    {
        MENGINE_UNUSED( _behavior );

        m_scene = Helper::staticNodeCast<Scene *>( _entity );

        LOGGER_MESSAGE( "Scene onEntityCreate [%s]"
            , m_scene->getName().c_str()
        );

        VectorizatorPtr vectorizator = Helper::generateVectorizator( MENGINE_DOCUMENT_FACTORABLE );

        vectorizator->setLineWidth( 10.f );
        vectorizator->setLineSoft( 5.f );
        vectorizator->beginFill( Color::identity() );

        vectorizator->drawCircle( {200.f, 300.f}, 50.f );

        vectorizator->drawRect( {400.f, 300.f}, 100.f, 100.f );
        vectorizator->drawRoundedRect( {600.f, 300.f}, 100.f, 100.f, 25.f );

        m_scene->addChild( vectorizator );

        return true;
    }
    //////////////////////////////////////////////////////////////////////////
    void VectorizatorSceneEventReceiver::onEntityDestroy( const EntityBehaviorInterfacePtr & _behavior )
    {
        MENGINE_UNUSED( _behavior );

        LOGGER_MESSAGE( "Scene onEntityDestroy [%s]"
            , m_scene->getName().c_str()
        );

        m_scene = nullptr;
    }
    //////////////////////////////////////////////////////////////////////////
    bool VectorizatorSceneEventReceiver::onEntityPreparation( const EntityBehaviorInterfacePtr & _behavior )
    {
        MENGINE_UNUSED( _behavior );

        LOGGER_MESSAGE( "Scene onEntityActivate [%s]"
            , m_scene->getName().c_str()
        );

        return true;
    }
    //////////////////////////////////////////////////////////////////////////
    bool VectorizatorSceneEventReceiver::onEntityActivate( const EntityBehaviorInterfacePtr & _behavior )
    {
        MENGINE_UNUSED( _behavior );

        LOGGER_MESSAGE( "Scene onEntityActivate [%s]"
            , m_scene->getName().c_str()
        );

        return true;
    }
    //////////////////////////////////////////////////////////////////////////
    void VectorizatorSceneEventReceiver::onEntityDeactivate( const EntityBehaviorInterfacePtr & _behavior )
    {
        MENGINE_UNUSED( _behavior );
        
        LOGGER_MESSAGE( "Scene onEntityDeactivate [%s]"
            , m_scene->getName().c_str()
        );

    }
};