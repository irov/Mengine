#pragma once

#include "Environment/Python/PythonEventReceiver.h"

#include "Kernel/SceneEventReceiverInterface.h"
#include "Kernel/Factorable.h"

namespace Mengine
{
    class PythonSceneEventReceiver
        : public SceneEventReceiverInterface
        , public PythonEventReceiver
        , public Factorable
    {
    public:
        PythonSceneEventReceiver();
        ~PythonSceneEventReceiver() override;

    protected:
        bool onSceneAppMouseLeave( const EntityBehaviorInterfacePtr & _behavior ) override;
        bool onSceneAppMouseEnter( const EntityBehaviorInterfacePtr & _behavior ) override;
        bool onSceneAppFocus( const EntityBehaviorInterfacePtr & _behavior, bool _focus ) override;

    protected:
        void onSceneRestartBegin( const EntityBehaviorInterfacePtr & _behavior ) override;
        void onSceneRestartEnd( const EntityBehaviorInterfacePtr & _behavior ) override;

    protected:
        bool onEntityPreparation( const EntityBehaviorInterfacePtr & _behavior ) override;
        bool onEntityActivate( const EntityBehaviorInterfacePtr & _behavior ) override;
        void onEntityPreparationDeactivate( const EntityBehaviorInterfacePtr & _behavior ) override;
        void onEntityDeactivate( const EntityBehaviorInterfacePtr & _behavior ) override;
        void onEntityCompile( const EntityBehaviorInterfacePtr & _behavior ) override;
        void onEntityRelease( const EntityBehaviorInterfacePtr & _behavior ) override;
        bool onEntityCreate( const EntityBehaviorInterfacePtr & _behavior, Entity * _entity ) override;
        void onEntityDestroy( const EntityBehaviorInterfacePtr & _behavior ) override;
    };
}