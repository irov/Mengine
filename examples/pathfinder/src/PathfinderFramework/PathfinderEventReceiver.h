#pragma once

#include "Kernel/Factory.h"
#include "Kernel/DummySceneEventReceiver.h"

#include "Plugins/OzzAnimationPlugin/OzzAnimationInterface.h"

#include "Interface/TimepipeServiceInterface.h"

#include "Kernel/Scene.h"

#include "GOAP/GOAP.h"

namespace Mengine
{
    //////////////////////////////////////////////////////////////////////////
    class PathfinderEventReceiver
        : public DummySceneEventReceiver
        , public Factorable
    {
    public:
        PathfinderEventReceiver();
        ~PathfinderEventReceiver() override;

    public:
        bool onEntityCreate( const EntityBehaviorInterfacePtr & _behavior, Entity * _entity ) override;
        void onEntityDestroy( const EntityBehaviorInterfacePtr & _behavior ) override;
        bool onEntityPreparation( const EntityBehaviorInterfacePtr & _behavior ) override;
        bool onEntityActivate( const EntityBehaviorInterfacePtr & _behavior ) override;
        void onEntityDeactivate( const EntityBehaviorInterfacePtr & _behavior ) override;

    protected:
        Scene * m_scene;
    };
    //////////////////////////////////////////////////////////////////////////
    typedef IntrusivePtr<PathfinderEventReceiver> PathfinderEventReceiverPtr;
    //////////////////////////////////////////////////////////////////////////
}