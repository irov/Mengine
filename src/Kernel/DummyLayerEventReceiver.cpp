#include "Kernel/DummyLayerEventReceiver.h"

namespace Mengine
{
    //////////////////////////////////////////////////////////////////////////
    DummyLayerEventReceiver::DummyLayerEventReceiver()
    {
    }
    //////////////////////////////////////////////////////////////////////////
    DummyLayerEventReceiver::~DummyLayerEventReceiver()
    {
    }
    //////////////////////////////////////////////////////////////////////////
    bool DummyLayerEventReceiver::onEntityCreate( const EntityBehaviorInterfacePtr & _behavior, Entity * _entity )
    {
        MENGINE_UNUSED( _behavior );
        MENGINE_UNUSED( _entity );

        return true;
    }
    //////////////////////////////////////////////////////////////////////////
    void DummyLayerEventReceiver::onEntityDestroy( const EntityBehaviorInterfacePtr & _behavior )
    {
        MENGINE_UNUSED( _behavior );
    }
    //////////////////////////////////////////////////////////////////////////
    bool DummyLayerEventReceiver::onEntityPreparation( const EntityBehaviorInterfacePtr & _behavior )
    {
        MENGINE_UNUSED( _behavior );

        return true;
    }
    //////////////////////////////////////////////////////////////////////////
    bool DummyLayerEventReceiver::onEntityActivate( const EntityBehaviorInterfacePtr & _behavior )
    {
        MENGINE_UNUSED( _behavior );

        return true;
    }
    //////////////////////////////////////////////////////////////////////////
    void DummyLayerEventReceiver::onEntityPassivate( const EntityBehaviorInterfacePtr & _behavior )
    {
        MENGINE_UNUSED( _behavior );
    }
    //////////////////////////////////////////////////////////////////////////
    void DummyLayerEventReceiver::onEntityDeactivate( const EntityBehaviorInterfacePtr & _behavior )
    {
        MENGINE_UNUSED( _behavior );
    }
    //////////////////////////////////////////////////////////////////////////
    void DummyLayerEventReceiver::onEntityCompile( const EntityBehaviorInterfacePtr & _behavior )
    {
        MENGINE_UNUSED( _behavior );
    }
    //////////////////////////////////////////////////////////////////////////
    void DummyLayerEventReceiver::onEntityRelease( const EntityBehaviorInterfacePtr & _behavior )
    {
        MENGINE_UNUSED( _behavior );
    }
    //////////////////////////////////////////////////////////////////////////
}