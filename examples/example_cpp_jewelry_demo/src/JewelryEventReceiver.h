#pragma once

#include "Interface/RandomizerInterface.h"

#include "Kernel/Factory.h"
#include "Kernel/DummySceneEventReceiver.h"

#include "Jewelry.h"

#include "GOAP/GOAP.h"

namespace Mengine
{
    //////////////////////////////////////////////////////////////////////////
    class JewelryEventReceiver
        : public DummySceneEventReceiver
        , public Factorable
    {
    public:
        JewelryEventReceiver();
        ~JewelryEventReceiver() override;

    public:
        bool onEntityCreate( const EntityBehaviorInterfacePtr & _behavior, Entity * _entity ) override;
        void onEntityDestroy( const EntityBehaviorInterfacePtr & _behavior ) override;
        bool onEntityPreparation( const EntityBehaviorInterfacePtr & _behavior ) override;
        bool onEntityActivate( const EntityBehaviorInterfacePtr & _behavior ) override;

    protected:
        void spawnJewelry_( const GOAP::SourcePtr & _source, uint32_t _iterator );

    protected:
        NodePtr spawnExplosive_();

    protected:
        Scene * m_scene;

        uint32_t m_column;
        uint32_t m_row;
        uint32_t m_count;

        mt::vec3f m_offset;

        float m_cell_fall_time_ms;
        float m_cell_explosive_time_ms;

        FactoryPtr m_factoryJewelry;

        typedef Vector<JewelryPtr> VectorJewelryHand;
        VectorJewelryHand m_jewelryHand;

        GOAP::EventPtr m_eventFall;

        struct JewelrySlot
        {
            JewelryPtr jewelry;
        };

        typedef Vector<JewelrySlot> VectorJewelrySlots;
        VectorJewelrySlots m_jewelrySlots;

        RandomizerInterfacePtr m_randomizer;
    };
    //////////////////////////////////////////////////////////////////////////
    typedef IntrusivePtr<JewelryEventReceiver> JewelryEventReceiverPtr;
    //////////////////////////////////////////////////////////////////////////
}