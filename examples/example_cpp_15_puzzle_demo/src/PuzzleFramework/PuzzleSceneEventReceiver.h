#pragma once

#include "Plugins/GOAPPlugin/GOAPInterface.h"

#include "Engine/TextField.h"
#include "Engine/HotSpotPolygon.h"

#include "Kernel/DummySceneEventReceiver.h"
#include "Kernel/ResourceImageDefault.h"
#include "Kernel/ShapeQuadFixed.h"
#include "Kernel/Scene.h"
#include "Kernel/Resource.h"
#include "Kernel/Vector.h"
#include "Kernel/Node.h"

namespace Mengine
{
    //////////////////////////////////////////////////////////////////////////
    typedef Vector<ShapeQuadFixedPtr> VectorShapeQuadFixedPtr;
    typedef Vector<HotSpotPolygonPtr> VectorHotSpotPolygonPtr;
    typedef Vector<NodePtr> VectorNodePtr;
    typedef Vector<TextFieldPtr> VectorTextFieldPtr;
    typedef Vector<mt::vec3f> VectorPositions;
    typedef Vector<int32_t> VectorIndexes;
    //////////////////////////////////////////////////////////////////////////
    class PuzzleSceneEventReceiver
        : public DummySceneEventReceiver
        , public Factorable
    {
    public:
        PuzzleSceneEventReceiver();
        ~PuzzleSceneEventReceiver() override;

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
        ShapeQuadFixedPtr createSprite( const ConstString & _name, const ResourcePtr & _resource );
        HotSpotPolygonPtr createHotSpot( const ConstString & _name, const mt::vec2f & _size );
        NodePtr createGraphicsRect( const ConstString & _name, const Color & _color, const mt::vec2f & _size );
        NodePtr createGraphicsRectFill( const ConstString & _name, const Color & _color, const mt::vec2f & _size );
        TextFieldPtr createTextField( const ConstString & _name, const ConstString & _id, const VectorString & _args );

        bool getImageSize( const FileGroupInterfacePtr & _fileGroup, const FilePath & _filePath, mt::vec2f * _out ) const;
        ResourceImageDefaultPtr createImageResource( const ConstString & _resourceName, const ConstString & _fileGroupName, const FilePath & _filePath, const mt::vec2f & _maxSize, const mt::uv4f & _uvImage, const mt::uv4f & _uvAlpha );

    // game
    protected:
        bool setupGame();

        void enableBorders( bool _value );
        void enableTexts( bool _value );

        void runTaskChains();
    
    protected:
        void scopeMakeTurn( const GOAP::SourceInterfacePtr & _scope, int32_t _position );
        void scopeMakeTurnToDirection( const GOAP::SourceInterfacePtr & _scope, int32_t _direction );
        bool scopeGameOver( const GOAP::SourceInterfacePtr & _scope );

        bool isGameOver() const;
        void resetGame();

    protected:
        void clearGame();
        void clearGameNode();

        void clearSprites();
        void clearHotspots();
        void clearTexts();
        void clearNodes();

        void clearTaskChain();

    protected:
        NodePtr m_gameNode;
        GOAP::ChainInterfacePtr m_chain;

        VectorShapeQuadFixedPtr m_sprites;
        VectorHotSpotPolygonPtr m_hotspots;
        VectorNodePtr m_nodes;

        VectorPositions m_positions;
        VectorIndexes m_indexes;
        VectorShapeQuadFixedPtr m_parts;
        VectorTextFieldPtr m_texts;

        GOAP::SemaphoreInterfacePtr m_semaphoreRandomOver;
        GOAP::SemaphoreInterfacePtr m_semaphoreGameOver;

    protected:
        Scene * m_scene;
    };
    //////////////////////////////////////////////////////////////////////////
    typedef IntrusivePtr<PuzzleSceneEventReceiver> PuzzleSceneEventReceiverPtr;
    //////////////////////////////////////////////////////////////////////////
}