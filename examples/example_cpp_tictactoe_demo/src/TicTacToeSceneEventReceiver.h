#pragma once

#include "Kernel/Scene.h"
#include "Kernel/Resource.h"

#include "Engine/HotSpotPolygon.h"
#include "Engine/ShapeQuadFixed.h"
#include "Engine/ResourceImageDefault.h"

#include "Config/Vector.h"

#include "GOAP/Chain.h"

#include "GOAP/Semaphore.h"


namespace Mengine
{
    //////////////////////////////////////////////////////////////////////////
    typedef Vector<ShapeQuadFixedPtr> VectorShapeQuadFixedPtr;
    typedef Vector<HotSpotPolygonPtr> VectorHotSpotPolygonPtr;
    //////////////////////////////////////////////////////////////////////////
    //////////////////////////////////////////////////////////////////////////
    enum EPlayerType
    {
        EPlayerType_NONE,
        EPlayerType_X,
        EPlayerType_O
    };
    //////////////////////////////////////////////////////////////////////////
    class TicTacToeSceneEventReceiver
        : public SceneEventReceiverInterface
        , public Factorable
    {
    public:
        TicTacToeSceneEventReceiver();
        ~TicTacToeSceneEventReceiver() override;

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
        bool setupGame();
        bool setupBackground();

        bool createPlayerSprite( EPlayerType _player, int32_t _position );

        void runTaskChains();
    
    protected:
        void makeTurn( int32_t _position );
        void changePlayer();
        bool isGameOver();
        bool checkWin( uint32_t _cell1, uint32_t _cell2, uint32_t _cell3 );

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
        GOAP::ChainPtr m_tc;

        ShapeQuadFixedPtr m_background;

        VectorShapeQuadFixedPtr m_sprites;
        VectorHotSpotPolygonPtr m_hotspots;

        typedef Vector<EPlayerType> VectorPlayers;
        VectorPlayers m_grid;

        mt::vec2f m_gridSize;
        mt::vec2f m_backgroundSize;

        GOAP::SemaphorePtr m_semaphoreGameOver;

        EPlayerType m_currentPlayer;

    protected:
        Scene * m_scene;
    };
    //////////////////////////////////////////////////////////////////////////
    typedef IntrusivePtr<TicTacToeSceneEventReceiver> TicTacToeSceneEventReceiverPtr;
    //////////////////////////////////////////////////////////////////////////
}