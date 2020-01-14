#include "Scene.h"

#include "SceneEventReceiverInterface.h"

#include "Kernel/BaseEventation.h"
#include "Kernel/EventableHelper.h"
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
            for( IntrusiveSlugChild it( m_children ); it.eof() == false; )
            {
                Scene * subScene = dynamic_cast<Scene *>(*it);

                it.next_shuffle();

                if( subScene != nullptr )
                {
                    subScene->onAppMouseLeave();
                }
            }
        }
    }
    //////////////////////////////////////////////////////////////////////////
    void Scene::onAppMouseEnter()
    {
        bool handle = EVENTABLE_METHODR( EVENT_SCENE_APP_MOUSE_ENTER, false )
            ->onSceneAppMouseEnter( m_behavior );

        if( handle == false )
        {
            for( IntrusiveSlugChild it( m_children ); it.eof() == false; )
            {
                Scene * subScene = dynamic_cast<Scene *>(*it);

                it.next_shuffle();

                if( subScene != nullptr )
                {
                    subScene->onAppMouseEnter();
                }
            }
        }
    }
    //////////////////////////////////////////////////////////////////////////
    void Scene::onFocus( bool _focus )
    {
        bool handle = EVENTABLE_METHODR( EVENT_SCENE_FOCUS, false )
            ->onSceneAppFocus( m_behavior, _focus );

        if( handle == false )
        {
            for( IntrusiveSlugChild it( m_children ); it.eof() == false; )
            {
                NodePtr children( *it );

                it.next_shuffle();

                Scene * subScene = dynamic_cast<Scene *>(children.get());

                if( subScene != nullptr )
                {
                    subScene->onFocus( _focus );
                }
            }
        }
    }
}
