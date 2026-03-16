#include "Scene.h"

#include "SceneEventReceiverInterface.h"

#include "Kernel/EventableHelper.h"
#include "Kernel/NodeCast.h"
#include "Kernel/Logger.h"

namespace Mengine
{
    //////////////////////////////////////////////////////////////////////////
    Scene::Scene()
    {
    }
    //////////////////////////////////////////////////////////////////////////
    Scene::~Scene()
    {
    }
    //////////////////////////////////////////////////////////////////////////
    void Scene::onAppMouseLeave()
    {
        bool handle = EVENTABLE_METHODR( EVENT_SCENE_APP_MOUSE_LEAVE, false )
            ->onSceneAppMouseLeave( m_behavior );

        if( handle == false )
        {
            this->foreachChildrenSlug( []( const NodePtr & _hierarchy )
            {
                ScenePtr subScene = Helper::dynamicNodeCast<ScenePtr>( _hierarchy );

                if( subScene != nullptr )
                {
                    subScene->onAppMouseLeave();
                }
            } );
        }
    }
    //////////////////////////////////////////////////////////////////////////
    void Scene::onAppMouseEnter()
    {
        bool handle = EVENTABLE_METHODR( EVENT_SCENE_APP_MOUSE_ENTER, false )
            ->onSceneAppMouseEnter( m_behavior );

        if( handle == false )
        {
            this->foreachChildrenSlug( []( const NodePtr & _hierarchy )
            {
                ScenePtr subScene = Helper::dynamicNodeCast<ScenePtr>( _hierarchy );

                if( subScene != nullptr )
                {
                    subScene->onAppMouseEnter();
                }
            } );
        }
    }
    //////////////////////////////////////////////////////////////////////////
    void Scene::onFocus( bool _focus )
    {
        bool handle = EVENTABLE_METHODR( EVENT_SCENE_FOCUS, false )
            ->onSceneAppFocus( m_behavior, _focus );

        if( handle == false )
        {
            this->foreachChildrenSlug( [_focus]( const NodePtr & _hierarchy )
            {
                Scene * subScene = stdex::intrusive_dynamic_get<Scene *>( _hierarchy );

                if( subScene != nullptr )
                {
                    subScene->onFocus( _focus );
                }
            } );
        }
    }
    //////////////////////////////////////////////////////////////////////////
    void Scene::onRestartBegin()
    {
        EVENTABLE_METHOD( EVENT_SCENE_RESTART_BEGIN )
            ->onSceneRestartBegin( m_behavior );
    }
    //////////////////////////////////////////////////////////////////////////
    void Scene::onRestartEnd()
    {
        EVENTABLE_METHOD( EVENT_SCENE_RESTART_END )
            ->onSceneRestartEnd( m_behavior );
    }
    //////////////////////////////////////////////////////////////////////////
}
