#include "Scene.h"

#include "Interface/ResourceInterface.h"
#include "Interface/NodeInterface.h"

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
        bool handle = EVENTABLE_METHODRT( m_scriptEventable, EVENT_APP_MOUSE_LEAVE, false, SceneEventReceiver )
            ->onSceneAppMouseLeave( m_object );

        if( handle == false )
        {
            for( TSlugChild it( m_children ); it.eof() == false; )
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
        bool handle = EVENTABLE_METHODRT( m_scriptEventable, EVENT_APP_MOUSE_ENTER, false, SceneEventReceiver )
            ->onSceneAppMouseEnter( m_object );

        if( handle == false )
        {
            for( TSlugChild it( m_children ); it.eof() == false; )
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
        bool handle = EVENTABLE_METHODRT( m_scriptEventable, EVENT_FOCUS, false, SceneEventReceiver )
            ->onSceneAppFocus( m_object, _focus );

        if( handle == false )
        {
            for( TSlugChild it( m_children ); it.eof() == false; )
            {
                Node * children = *it;

                it.next_shuffle();

                Scene * subScene = dynamic_cast<Scene *>(children);

                if( subScene != nullptr )
                {
                    subScene->onFocus( _focus );
                }
            }
        }
    }
}
