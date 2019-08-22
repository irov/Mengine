#pragma once

#include "Kernel/Scene.h"

namespace Mengine
{
    //////////////////////////////////////////////////////////////////////////
    class MySceneEventReceiver
        : public SceneEventReceiverInterface
        , public Factorable
    {
    public:
        MySceneEventReceiver();
        ~MySceneEventReceiver() override;

    public:
        bool onSceneAppMouseLeave( const EntityBehaviorInterfacePtr & _behavior ) override;
        bool onSceneAppMouseEnter( const EntityBehaviorInterfacePtr & _behavior ) override;
        bool onSceneAppFocus( const EntityBehaviorInterfacePtr & _behavior, bool _focus ) override;

    public:
        void onEntityCreate( const EntityBehaviorInterfacePtr & _behavior, Entity * _entity ) override;
        void onEntityDestroy( const EntityBehaviorInterfacePtr & _behavior ) override;
        void onEntityPreparation( const EntityBehaviorInterfacePtr & _behavior ) override;
        void onEntityActivate( const EntityBehaviorInterfacePtr & _behavior ) override;
        void onEntityPreparationDeactivate( const EntityBehaviorInterfacePtr & _behavior ) override;
        void onEntityDeactivate( const EntityBehaviorInterfacePtr & _behavior ) override;
        void onEntityCompile( const EntityBehaviorInterfacePtr & _behavior ) override;
        void onEntityRelease( const EntityBehaviorInterfacePtr & _behavior ) override;

    protected:
        Scene * m_scene;
    };
    //////////////////////////////////////////////////////////////////////////
    typedef IntrusivePtr<MySceneEventReceiver> MySceneEventReceiverPtr;
    //////////////////////////////////////////////////////////////////////////
}