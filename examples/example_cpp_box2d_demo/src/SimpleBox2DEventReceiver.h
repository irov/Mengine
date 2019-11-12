#pragma once

#include "Kernel/Factory.h"
#include "Kernel/DummySceneEventReceiver.h"

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

        GOAP::ChainPtr m_chain;
    };
    //////////////////////////////////////////////////////////////////////////
    typedef IntrusivePtr<SimpleBox2DEventReceiver> SimpleBox2DEventReceiverPtr;
    //////////////////////////////////////////////////////////////////////////
}