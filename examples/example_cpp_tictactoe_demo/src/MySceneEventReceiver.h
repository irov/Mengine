#pragma once

#include "Kernel/Scene.h"
#include "Kernel/Resource.h"

#include "Engine/HotSpotPolygon.h"

#include "Config/Vector.h"

#include "GOAP/Chain.h"

#include "GOAP/Semaphore.h"

namespace Mengine
{
    //////////////////////////////////////////////////////////////////////////
    typedef IntrusivePtr<class Resource> ResourcePtr;
    typedef IntrusivePtr<class ShapeQuadFixed> ShapeQuadFixedPtr;
    typedef IntrusiveResourcePtr<class ResourceImageDefault> ResourceImageDefaultPtr;
    //////////////////////////////////////////////////////////////////////////
    typedef Vector<ShapeQuadFixedPtr> VectorShapeQuadFixedPtr;
    typedef Vector<HotSpotPolygonPtr> VectorHotSpotPolygonPtr;
    typedef Vector<GOAP::ChainPtr> VectorChainPtr;
    //////////////////////////////////////////////////////////////////////////
    class FilePath;
    //////////////////////////////////////////////////////////////////////////
    enum EPlayerType
    {
        EPlayerType_NONE,
        EPlayerType_X,
        EPlayerType_O
    };
    //////////////////////////////////////////////////////////////////////////
    typedef Vector<EPlayerType> VectorPlayers;
    //////////////////////////////////////////////////////////////////////////
    class MySceneEventReceiver
        : public SceneEventReceiverInterface
        , public Factorable
    {
    public:
        MySceneEventReceiver();
        ~MySceneEventReceiver() override;

    public:
        bool onSceneAppMouseLeave( const EntityBehaviorInterfacePtr & _behavior ) override;
        bool onSceneAppMouseEnter( const EntityBehaviorInterfacePtr & _behavior ) override;
        bool onSceneAppFocus( const EntityBehaviorInterfacePtr & _behavior, bool _focus ) override;

    public:
        void onEntityCreate( const EntityBehaviorInterfacePtr & _behavior, Entity * _entity ) override;
        void onEntityDestroy( const EntityBehaviorInterfacePtr & _behavior ) override;
        void onEntityPreparation( const EntityBehaviorInterfacePtr & _behavior ) override;
        void onEntityActivate( const EntityBehaviorInterfacePtr & _behavior ) override;
        void onEntityPreparationDeactivate( const EntityBehaviorInterfacePtr & _behavior ) override;
        void onEntityDeactivate( const EntityBehaviorInterfacePtr & _behavior ) override;
        void onEntityCompile( const EntityBehaviorInterfacePtr & _behavior ) override;
        void onEntityRelease( const EntityBehaviorInterfacePtr & _behavior ) override;

    // utils
    public:
        ShapeQuadFixedPtr createSprite( const ConstString & _name, const ResourcePtr & _resource );
        HotSpotPolygonPtr createHotSpot( const ConstString & _name, const mt::vec2f & _size );

        ResourcePtr createResource( const ConstString & _type );
        ResourceImageDefaultPtr createImageResource( const ConstString & _resourceName, const ConstString & _fileGroupName, const FilePath & _filePath, const mt::vec2f & _maxSize );

    // game
    protected:
        bool scopeLoadAssets( const GOAP::SourcePtr & _scope );

        bool setupGame();
        bool setupBackground();

        bool createPlayerSprite( const EPlayerType & _player, int32_t _position );

    protected:
        void makeTurn( int32_t _position );
        void changePlayer();
        bool isGameOver();
        bool checkWin( EPlayerType _cell1, EPlayerType _cell2, EPlayerType _cell3 );

        void enableSprites( bool _value );

        bool scopeGameOver( const GOAP::SourcePtr _scope );

        void resetGame();

    protected:
        void clearGameNode();

        void clearBackground();
        void clearSprites();
        void clearHotspots();

        void clearTaskChain();

    protected:
        NodePtr m_gameNode;

        VectorChainPtr m_taskChains;

        ShapeQuadFixedPtr m_background;

        VectorShapeQuadFixedPtr m_sprites;
        VectorHotSpotPolygonPtr m_hotspots;

        VectorPlayers m_grid;

        mt::vec2f m_gridSize;
        mt::vec2f m_backgroundSize;

        GOAP::SemaphorePtr m_semaphoreGameOver;

        GOAP::SemaphorePtr m_semaphoreGameLoadAssets;  // 1 - success, 2 - fail

        EPlayerType m_currentPlayer;

    protected:
        Scene * m_scene;
    };
    //////////////////////////////////////////////////////////////////////////
    typedef IntrusivePtr<MySceneEventReceiver> MySceneEventReceiverPtr;
    //////////////////////////////////////////////////////////////////////////
}