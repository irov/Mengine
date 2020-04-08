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
    class SimpleBox2DEventReceiver
        : public DummySceneEventReceiver
        , public Factorable
    {
    public:
        SimpleBox2DEventReceiver();
        ~SimpleBox2DEventReceiver() override;

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
    typedef IntrusivePtr<SimpleBox2DEventReceiver> SimpleBox2DEventReceiverPtr;
    //////////////////////////////////////////////////////////////////////////
}