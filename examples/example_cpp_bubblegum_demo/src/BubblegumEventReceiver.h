#pragma once

#include "Kernel/DummySceneEventReceiver.h"

#include "Engine/HotSpotGlobal.h"
#include "Engine/TextField.h"

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
        void onEntityCreate( const EntityBehaviorInterfacePtr & _behavior, Entity * _entity ) override;
        void onEntityDestroy( const EntityBehaviorInterfacePtr & _behavior ) override;
        void onEntityPreparation( const EntityBehaviorInterfacePtr & _behavior ) override;
        void onEntityActivate( const EntityBehaviorInterfacePtr & _behavior ) override;

    protected:
        void spawnBubble_( const GOAP::SourcePtr & _source, uint32_t _iterator );
        void addScore_( const NodePtr & _shape, uint32_t _iterator );

    protected:
        Scene * m_scene;

        HotSpotGlobalPtr m_globalHotspot;
        GOAP::SemaphorePtr m_semaphoreBurst;

        TextFieldPtr m_textScore;
        TextFieldPtr m_textFinish;

        uint32_t m_score;
    };
    //////////////////////////////////////////////////////////////////////////
    typedef IntrusivePtr<BubblegumEventReceiver> MySceneEventReceiverPtr;
    //////////////////////////////////////////////////////////////////////////
}