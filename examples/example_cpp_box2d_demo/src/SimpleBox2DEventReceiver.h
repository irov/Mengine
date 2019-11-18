#pragma once

#include "Kernel/Factory.h"
#include "Kernel/DummySceneEventReceiver.h"

#include "Engine/Vectorizator.h"

#include "Plugins/Box2DPlugin/Box2DInterface.h"

#include "Interface/TimepipeServiceInterface.h"

#include "GOAP/GOAP.h"

#include "Box.h"

namespace Mengine
{
    //////////////////////////////////////////////////////////////////////////
    class SimpleBox2DEventReceiver
        : public DummySceneEventReceiver
        , public TimepipeInterface
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

    public:
        void onTimepipe( const UpdateContext * _context ) override;

    
    protected:
        NodePtr m_boxNode;

        GOAP::ChainPtr m_chain;

        Box2DWorldInterfacePtr m_world;

        uint32_t m_timepipeId;

        VectorBoxPtr m_boxes;

    protected:
        Scene * m_scene;
    };
    //////////////////////////////////////////////////////////////////////////
    typedef IntrusivePtr<SimpleBox2DEventReceiver> SimpleBox2DEventReceiverPtr;
    //////////////////////////////////////////////////////////////////////////
}