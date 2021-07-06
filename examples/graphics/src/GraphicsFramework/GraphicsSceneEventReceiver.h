#pragma once

#include "Kernel/DummySceneEventReceiver.h"
#include "Kernel/Scene.h"

namespace Mengine
{
    //////////////////////////////////////////////////////////////////////////
    class GraphicsSceneEventReceiver
        : public DummySceneEventReceiver
        , public Factorable
    {
    public:
        GraphicsSceneEventReceiver();
        ~GraphicsSceneEventReceiver() override;

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
    typedef IntrusivePtr<GraphicsSceneEventReceiver> GraphicsSceneEventReceiverPtr;
    //////////////////////////////////////////////////////////////////////////
}