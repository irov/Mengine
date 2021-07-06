#pragma once

#include "Kernel/Factory.h"
#include "Kernel/DummySceneEventReceiver.h"

#include "Plugins/Box2DPlugin/Box2DInterface.h"
#include "Plugins/GraphicsPlugin/GraphicsInterface.h"

#include "Interface/TimepipeServiceInterface.h"

#include "Kernel/Scene.h"

#include "GOAP/GOAP.h"

namespace Mengine
{
    //////////////////////////////////////////////////////////////////////////
    class Box2DEventReceiver
        : public DummySceneEventReceiver
        , public Factorable
    {
    public:
        Box2DEventReceiver();
        ~Box2DEventReceiver() override;

    public:
        bool onEntityCreate( const EntityBehaviorInterfacePtr & _behavior, Entity * _entity ) override;
        void onEntityDestroy( const EntityBehaviorInterfacePtr & _behavior ) override;
        bool onEntityPreparation( const EntityBehaviorInterfacePtr & _behavior ) override;
        bool onEntityActivate( const EntityBehaviorInterfacePtr & _behavior ) override;
        void onEntityDeactivate( const EntityBehaviorInterfacePtr & _behavior ) override;

    protected:
        Scene * m_scene;

        NodePtr m_boxNode;

        GOAP::ChainInterfacePtr m_chain;

        Box2DWorldInterfacePtr m_world;
    };
    //////////////////////////////////////////////////////////////////////////
    typedef IntrusivePtr<Box2DEventReceiver> Box2DEventReceiverPtr;
    //////////////////////////////////////////////////////////////////////////
}