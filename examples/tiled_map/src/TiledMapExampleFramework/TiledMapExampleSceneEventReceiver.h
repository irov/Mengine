#pragma once

#include "Engine/VirtualArea.h"

#include "Frameworks/ImGUIFramework/ImGUIRender.h"

#include "Plugins/TiledMapPlugin/ResourceTiledMap.h"
#include "Plugins/TiledMapPlugin/TiledMap2D.h"

#include "Kernel/DummySceneEventReceiver.h"
#include "Kernel/ResourceImageDefault.h"
#include "Kernel/Scene.h"
#include "Kernel/String.h"
#include "Kernel/Vector.h"
#include "Kernel/VectorResources.h"

namespace Mengine
{
    //////////////////////////////////////////////////////////////////////////
    struct TiledMapExampleImageDesc
    {
        ConstString name;
        FilePath path;
    };
    //////////////////////////////////////////////////////////////////////////
    typedef Vector<TiledMapExampleImageDesc> VectorTiledMapExampleImageDescs;
    //////////////////////////////////////////////////////////////////////////
    struct TiledMapExampleDesc
    {
        String title;
        FilePath mapPath;
    };
    //////////////////////////////////////////////////////////////////////////
    typedef Vector<TiledMapExampleDesc> VectorTiledMapExampleDescs;
    //////////////////////////////////////////////////////////////////////////
    class TiledMapExampleSceneEventReceiver
        : public DummySceneEventReceiver
        , public Factorable
    {
    public:
        TiledMapExampleSceneEventReceiver();
        ~TiledMapExampleSceneEventReceiver() override;

    public:
        bool onEntityCreate( const EntityBehaviorInterfacePtr & _behavior, Entity * _entity ) override;
        void onEntityDestroy( const EntityBehaviorInterfacePtr & _behavior ) override;
        bool onEntityPreparation( const EntityBehaviorInterfacePtr & _behavior ) override;
        bool onEntityActivate( const EntityBehaviorInterfacePtr & _behavior ) override;
        void onEntityDeactivate( const EntityBehaviorInterfacePtr & _behavior ) override;

    protected:
        void setupVirtualArea_();
        bool loadExamples_();
        bool loadMap_( uint32_t _index );
        ResourceImageDefaultPtr createImageResource_( const ConstString & _name, const FilePath & _path );
        ResourceTiledMapPtr createMapResource_( const FilePath & _path );
        void updateContentBounds_();
        void clearMap_();
        void renderControls_( const ImGUIRenderProviderInterfacePtr & _provider );

    protected:
        Scene * m_scene;

        VirtualAreaPtr m_virtualArea;
        TiledMap2DPtr m_tiledMap;
        ImGUIRenderPtr m_imguiRender;
        VectorResources m_resources;
        VectorTiledMapExampleDescs m_examples;

        String m_error;
        uint32_t m_mapIndex;
        uint32_t m_excludedMapCount;
    };
    //////////////////////////////////////////////////////////////////////////
    typedef IntrusivePtr<TiledMapExampleSceneEventReceiver> TiledMapExampleSceneEventReceiverPtr;
    //////////////////////////////////////////////////////////////////////////
}
