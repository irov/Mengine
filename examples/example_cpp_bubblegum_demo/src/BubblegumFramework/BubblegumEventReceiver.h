#pragma once

#include "Kernel/DummySceneEventReceiver.h"

#include "Engine/HotSpotGlobal.h"
#include "Engine/TextField.h"

#include "Kernel/Scene.h"

#include "GOAP/GOAP.h"

namespace Mengine
{
    //////////////////////////////////////////////////////////////////////////
    class BubblegumEventReceiver
        : public DummySceneEventReceiver
        , public Factorable
    {
    public:
        BubblegumEventReceiver();
        ~BubblegumEventReceiver() override;

    public:
        bool onEntityCreate( const EntityBehaviorInterfacePtr & _behavior, Entity * _entity ) override;
        void onEntityDestroy( const EntityBehaviorInterfacePtr & _behavior ) override;
        bool onEntityPreparation( const EntityBehaviorInterfacePtr & _behavior ) override;
        bool onEntityActivate( const EntityBehaviorInterfacePtr & _behavior ) override;
        void onEntityDeactivate( const EntityBehaviorInterfacePtr & _behavior ) override;

    protected:
        void spawnBubble_( const GOAP::SourceInterfacePtr & _source, uint32_t _iterator );
        void addScore_( const NodePtr & _shape, uint32_t _iterator );

    protected:
        Scene * m_scene;

        UniqueId m_timepipeId;

        HotSpotGlobalPtr m_globalHotspot;
        GOAP::SemaphoreInterfacePtr m_semaphoreBurst;

        GOAP::ChainInterfacePtr m_chain;

        TextFieldPtr m_textScore;
        TextFieldPtr m_textFinish;

        uint32_t m_score;
    };
    //////////////////////////////////////////////////////////////////////////
    typedef IntrusivePtr<BubblegumEventReceiver> BubblegumEventReceiverPtr;
    //////////////////////////////////////////////////////////////////////////
}