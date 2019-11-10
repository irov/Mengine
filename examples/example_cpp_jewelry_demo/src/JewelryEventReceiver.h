#pragma once

#include "Interface/RandomizerInterface.h"

#include "Kernel/Factory.h"
#include "Kernel/DummySceneEventReceiver.h"

#include "Jewelry.h"
#include "JewelryMatrix.h"

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
        void spawnJewelry_( const GOAP::SourcePtr & _source, EJewelrySuper _super, uint32_t _iterator );
        void explosiveJewelry_( const GOAP::SourcePtr & _source, const JewelryPtr & _jewelry );

    protected:
        NodePtr spawnExplosive_();

    protected:
        JewelryPtr makeJewelry_( EJewelrySuper _super, uint32_t _type, uint32_t _column, uint32_t _row, const Char * _doc );

    protected:
        Scene * m_scene;

        NodePtr m_base;

        uint32_t m_jewelry_type_count;
        float m_jewelry_size;
        float m_jewelry_stride;

        float m_jewelry_cell_fall_time_ms;
        float m_jewelry_cell_explosive_time_ms;
        uint32_t m_jewelry_cell_explosive_count;
        float m_jewelry_spawn_time_ms;

        FactoryPtr m_factoryJewelry;

        typedef Vector<JewelryPtr> VectorJewelryHand;
        VectorJewelryHand m_jewelryHand;

        GOAP::EventPtr m_eventFall;

        JewelryMatrixPtr m_jewelryMatrix;

        RandomizerInterfacePtr m_randomizer;
    };
    //////////////////////////////////////////////////////////////////////////
    typedef IntrusivePtr<JewelryEventReceiver> JewelryEventReceiverPtr;
    //////////////////////////////////////////////////////////////////////////
}