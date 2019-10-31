#include "Kernel/DummyEntityEventReceiver.h"

namespace Mengine
{
    //////////////////////////////////////////////////////////////////////////
    DummyEntityEventReceiver::DummyEntityEventReceiver()
    {
    }
    //////////////////////////////////////////////////////////////////////////
    DummyEntityEventReceiver::~DummyEntityEventReceiver()
    {
    }
    //////////////////////////////////////////////////////////////////////////
    bool DummyEntityEventReceiver::onEntityCreate( const EntityBehaviorInterfacePtr & _behavior, class Entity * _entity )
    {
        MENGINE_UNUSED( _behavior );
        MENGINE_UNUSED( _entity );

        //Empty

        return true;
    }
    //////////////////////////////////////////////////////////////////////////
    void DummyEntityEventReceiver::onEntityDestroy( const EntityBehaviorInterfacePtr & _behavior )
    {
        MENGINE_UNUSED( _behavior );

        //Empty
    }
    //////////////////////////////////////////////////////////////////////////
    bool DummyEntityEventReceiver::onEntityPreparation( const EntityBehaviorInterfacePtr & _behavior )
    {
        MENGINE_UNUSED( _behavior );

        //Empty

        return true;
    }
    //////////////////////////////////////////////////////////////////////////
    bool DummyEntityEventReceiver::onEntityActivate( const EntityBehaviorInterfacePtr & _behavior )
    {
        MENGINE_UNUSED( _behavior );

        //Empty

        return true;
    }
    //////////////////////////////////////////////////////////////////////////
    void DummyEntityEventReceiver::onEntityPreparationDeactivate( const EntityBehaviorInterfacePtr & _behavior )
    {
        MENGINE_UNUSED( _behavior );

        //Empty
    }
    //////////////////////////////////////////////////////////////////////////
    void DummyEntityEventReceiver::onEntityDeactivate( const EntityBehaviorInterfacePtr & _behavior )
    {
        MENGINE_UNUSED( _behavior );

        //Empty
    }
    //////////////////////////////////////////////////////////////////////////
    void DummyEntityEventReceiver::onEntityCompile( const EntityBehaviorInterfacePtr & _behavior )
    {
        MENGINE_UNUSED( _behavior );

        //Empty
    }
    //////////////////////////////////////////////////////////////////////////
    void DummyEntityEventReceiver::onEntityRelease( const EntityBehaviorInterfacePtr & _behavior )
    {
        MENGINE_UNUSED( _behavior );

        //Empty
    }
}
