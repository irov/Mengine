#pragma once

#include "Plugins/GOAPPlugin/GOAPInterface.h"

#include "Engine/HotSpotPolygon.h"

#include "Kernel/ShapeQuadFixed.h"
#include "Kernel/ResourceImageDefault.h"
#include "Kernel/DummySceneEventReceiver.h"
#include "Kernel/Scene.h"
#include "Kernel/Resource.h"
#include "Kernel/Vector.h"

namespace Mengine
{
    //////////////////////////////////////////////////////////////////////////
    typedef Vector<ShapeQuadFixedPtr> VectorShapeQuadFixedPtr;
    typedef Vector<Vector<HotSpotPolygonPtr>> Vector2DHotSpotPolygonPtr;
    typedef Vector<HotSpotPolygonPtr> VectorHotSpotPolygonPtr;
    typedef Vector<int32_t> VectorInt;
    typedef Vector<VectorInt> GridVector;
    //////////////////////////////////////////////////////////////////////////
    class FifteenSceneEventReceiver
        : public DummySceneEventReceiver
        , public Factorable
    {
    public:
        FifteenSceneEventReceiver();
        ~FifteenSceneEventReceiver() override;

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

    public:
        ShapeQuadFixedPtr createSprite( const ConstString & _name, const ResourcePtr & _resource );
        HotSpotPolygonPtr createHotSpot( const ConstString & _name, const mt::vec2f & _size );
        ResourceImageDefaultPtr createImageSubstractResource( const ConstString & _resourceName, const ConstString & _fileGroupName, const FilePath & _filePath, const mt::vec2f & _maxSize, const mt::vec4f & _splitSize );
        ResourceImageDefaultPtr createImageResource( const ConstString & _resourceName, const ConstString & _fileGroupName, const FilePath & _filePath, const mt::vec2f & _maxSize );
        void createSplitBackgroundSprites( const ConstString & _name, const ConstString & _fileGroup, const FilePath & _filePath );
        void shuffle( GridVector & _vector );

    protected:
        bool setupGame( const ConstString & _imagePath );
        bool setupBackground( const ConstString & _imagePath );
        void setupRandomMap();
        void setupHotspots();
        void setupSprites();

    protected:
        void makeTurn( const GOAP::SourceInterfacePtr & _scope, const HotSpotPolygonPtr & _hotspot );
        void runTaskChains();
        void swapPositions( const HotSpotPolygonPtr & _empty, const HotSpotPolygonPtr & _hotspot );
        void calculateAroundEmpty( const GOAP::SourceInterfacePtr & _scope );
        void calculateEmptyIndex();
        void checkWin();
        bool checkSolution();
        int32_t checkBiggerGroups( int32_t  _row, int32_t  _col );

    protected:
        void enableSprites( bool _value );
        void enableBackground( bool _value );

    protected:
        bool scopeGameOver( const GOAP::SourceInterfacePtr & _scope );
        void resetGame();

    protected:
        void clearGameNode();
        void clearBackground();
        void clearSprites();
        void clearHotspots();
        void clearMap();
        void clearTaskChain();

    protected:
        NodePtr m_gameNode;
        GOAP::ChainInterfacePtr m_chain;
        GOAP::SemaphoreInterfacePtr m_semaphoreGameOver;

        ShapeQuadFixedPtr m_background;
        VectorShapeQuadFixedPtr m_sprites;
        Vector2DHotSpotPolygonPtr m_hotspots;
        VectorHotSpotPolygonPtr m_hotspotsAroundEmpty;
        GridVector m_map;

        mt::vec2f m_backgroundSize;
        mt::vec2f m_sceneSize;

        float m_minimumDistanceX;
        float m_minimumDistanceY;

        int32_t m_emptyIndexX;
        int32_t m_emptyIndexY;

        uint32_t m_imageIndex;

    protected:
        Scene * m_scene;
    };
    //////////////////////////////////////////////////////////////////////////
    typedef IntrusivePtr<FifteenSceneEventReceiver> FifteenSceneEventReceiverPtr;
    //////////////////////////////////////////////////////////////////////////
}