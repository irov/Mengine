#include "Kernel/DummySceneEventReceiver.h"

namespace Mengine
{
    //////////////////////////////////////////////////////////////////////////
    DummySceneEventReceiver::DummySceneEventReceiver()
    {
    }
    //////////////////////////////////////////////////////////////////////////
    DummySceneEventReceiver::~DummySceneEventReceiver()
    {
    }
    //////////////////////////////////////////////////////////////////////////
    bool DummySceneEventReceiver::onSceneAppMouseLeave( const EntityBehaviorInterfacePtr & _behavior )
    {
        MENGINE_UNUSED( _behavior );

        return false;
    }
    //////////////////////////////////////////////////////////////////////////
    bool DummySceneEventReceiver::onSceneAppMouseEnter( const EntityBehaviorInterfacePtr & _behavior )
    {
        MENGINE_UNUSED( _behavior );

        return false;
    }
    //////////////////////////////////////////////////////////////////////////
    bool DummySceneEventReceiver::onSceneAppFocus( const EntityBehaviorInterfacePtr & _behavior, bool _focus )
    {
        MENGINE_UNUSED( _behavior );
        MENGINE_UNUSED( _focus );

        return false;
    }
    //////////////////////////////////////////////////////////////////////////
    bool DummySceneEventReceiver::onEntityCreate( const EntityBehaviorInterfacePtr & _behavior, Entity * _entity )
    {
        MENGINE_UNUSED( _behavior );
        MENGINE_UNUSED( _entity );

        //Empty

        return true;
    }
    //////////////////////////////////////////////////////////////////////////
    void DummySceneEventReceiver::onEntityDestroy( const EntityBehaviorInterfacePtr & _behavior )
    {
        MENGINE_UNUSED( _behavior );

        //Empty
    }
    //////////////////////////////////////////////////////////////////////////
    bool DummySceneEventReceiver::onEntityPreparation( const EntityBehaviorInterfacePtr & _behavior )
    {
        MENGINE_UNUSED( _behavior );

        //Empty

        return true;
    }
    //////////////////////////////////////////////////////////////////////////
    bool DummySceneEventReceiver::onEntityActivate( const EntityBehaviorInterfacePtr & _behavior )
    {
        MENGINE_UNUSED( _behavior );

        //Empty

        return true;
    }
    //////////////////////////////////////////////////////////////////////////
    void DummySceneEventReceiver::onEntityPreparationDeactivate( const EntityBehaviorInterfacePtr & _behavior )
    {
        MENGINE_UNUSED( _behavior );

        //Empty
    }
    //////////////////////////////////////////////////////////////////////////
    void DummySceneEventReceiver::onEntityDeactivate( const EntityBehaviorInterfacePtr & _behavior )
    {
        MENGINE_UNUSED( _behavior );

        //Empty
    }
    //////////////////////////////////////////////////////////////////////////
    void DummySceneEventReceiver::onEntityCompile( const EntityBehaviorInterfacePtr & _behavior )
    {
        MENGINE_UNUSED( _behavior );

        //Empty
    }
    //////////////////////////////////////////////////////////////////////////
    void DummySceneEventReceiver::onEntityRelease( const EntityBehaviorInterfacePtr & _behavior )
    {
        MENGINE_UNUSED( _behavior );

        //Empty
    }
    //////////////////////////////////////////////////////////////////////////
}
