#pragma once

#include "TrafficMap.h"

#include "Plugins/GOAPPlugin/GOAPInterface.h"

#include "Engine/HotSpotPolygon.h"
#include "Engine/ShapeQuadFixed.h"
#include "Engine/ResourceImageDefault.h"
#include "Engine/SurfaceSolidColor.h"

#include "Kernel/DummySceneEventReceiver.h"
#include "Kernel/Scene.h"
#include "Kernel/Resource.h"
#include "Kernel/Vector.h"

namespace Mengine
{
    //////////////////////////////////////////////////////////////////////////
    typedef Vector<ShapeQuadFixedPtr> VectorShapeQuadFixedPtr;
    typedef Vector<HotSpotPolygonPtr> VectorHotSpotPolygonPtr;
    //////////////////////////////////////////////////////////////////////////
    class TrafficJamSceneEventReceiver
        : public DummySceneEventReceiver
        , public Factorable
    {
    public:
        TrafficJamSceneEventReceiver();
        ~TrafficJamSceneEventReceiver() override;

    public:
        bool onSceneAppMouseLeave( const EntityBehaviorInterfacePtr & _behavior ) override;
        bool onSceneAppMouseEnter( const EntityBehaviorInterfacePtr & _behavior ) override;
        bool onSceneAppFocus( const EntityBehaviorInterfacePtr & _behavior, bool _focus ) override;

    public:
        bool onEntityCreate( const EntityBehaviorInterfacePtr & _behavior, Entity * _entity ) override;
        void onEntityDestroy( const EntityBehaviorInterfacePtr & _behavior ) override;
        bool onEntityPreparation( const EntityBehaviorInterfacePtr & _behavior ) override;
        bool onEntityActivate( const EntityBehaviorInterfacePtr & _behavior ) override;
        void onEntityPreparationDeactivate( const EntityBehaviorInterfacePtr & _behavior ) override;
        void onEntityDeactivate( const EntityBehaviorInterfacePtr & _behavior ) override;
        void onEntityCompile( const EntityBehaviorInterfacePtr & _behavior ) override;
        void onEntityRelease( const EntityBehaviorInterfacePtr & _behavior ) override;

        // utils
    public:
        ShapeQuadFixedPtr createColor( const ConstString & _name, const SurfaceSolidColorPtr & _resource );
        HotSpotPolygonPtr createHotSpot( const ConstString & _name, const mt::vec2f & _size );

        // game
    protected:
        bool setupGame();
        bool setupBackground();
        bool setupCars();
        bool setupCar( uint32_t _xPos, uint32_t _yPos, uint32_t recourceImageId );
        HotSpotPolygonPtr addHotspot( uint32_t _row, uint32_t _col, uint32_t _id );

        void runTaskChains();

    protected:
        void moveLeft( const ShapeQuadFixedPtr & _sprite );
        void moveRight( const ShapeQuadFixedPtr & _sprite );
        void moveUp( const ShapeQuadFixedPtr & _sprite );
        void moveDown( const ShapeQuadFixedPtr & _sprite );
        void movePlayerUp( const ShapeQuadFixedPtr & _sprite );
        void movePlayerDown( const ShapeQuadFixedPtr & _sprite );

        void resetGame();

    protected:
        void clearGameNode();

        void clearBackground();
        void clearCars();

        void clearTaskChain();

    protected:
        NodePtr m_gameNode;

        GOAP::ChainInterfacePtr m_chain;

        ShapeQuadFixedPtr m_background;

        mt::vec2f m_backgroundSize;

        GOAP::SemaphoreInterfacePtr m_semaphoreGameOver;

        TrafficMap m_trafficMap;

        struct Car
        {
            ShapeQuadFixedPtr sprite;
            HotSpotPolygonPtr hotspotFirst;
            HotSpotPolygonPtr hotspotSecond;
            int32_t carType;
        };

        typedef Vector<Car> VectorCar;
        VectorCar m_cars;

    protected:
        Scene * m_scene;
    };
    //////////////////////////////////////////////////////////////////////////
    typedef IntrusivePtr<TrafficJamSceneEventReceiver> TrafficJamSceneEventReceiverPtr;
    //////////////////////////////////////////////////////////////////////////
}