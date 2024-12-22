#pragma once

#include "Kernel/SceneEventReceiverInterface.h"

namespace Mengine
{
    class DummySceneEventReceiver
        : public SceneEventReceiverInterface
    {
    public:
        DummySceneEventReceiver();
        ~DummySceneEventReceiver() override;

    public:
        bool onSceneAppMouseLeave( const EntityBehaviorInterfacePtr & _behavior ) override;
        bool onSceneAppMouseEnter( const EntityBehaviorInterfacePtr & _behavior ) override;
        bool onSceneAppFocus( const EntityBehaviorInterfacePtr & _behavior, bool _focus ) override;

    public:
        bool onEntityCreate( const EntityBehaviorInterfacePtr & _behavior, Entity * _entity ) override;
        void onEntityDestroy( const EntityBehaviorInterfacePtr & _behavior ) override;
        bool onEntityPreparation( const EntityBehaviorInterfacePtr & _behavior ) override;
        bool onEntityActivate( const EntityBehaviorInterfacePtr & _behavior ) override;
        void onEntityPreparationDeactivate( const EntityBehaviorInterfacePtr & _behavior ) override;
        void onEntityDeactivate( const EntityBehaviorInterfacePtr & _behavior ) override;
        void onEntityCompile( const EntityBehaviorInterfacePtr & _behavior ) override;
        void onEntityRelease( const EntityBehaviorInterfacePtr & _behavior ) override;
    };
}