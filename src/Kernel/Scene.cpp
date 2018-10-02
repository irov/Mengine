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
        EventationInterface * event = this->getScriptEventation();

        if( event != nullptr )
        {
            bool handle = EVENTABLE_METHODRT( event, EVENT_APP_MOUSE_LEAVE, false, SceneEventReceiver )
                ->onSceneAppMouseLeave( m_object );

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
    }
    //////////////////////////////////////////////////////////////////////////
    void Scene::onAppMouseEnter()
    {
        EventationInterface * event = this->getScriptEventation();

        if( event != nullptr )
        {
            bool handle = EVENTABLE_METHODRT( event, EVENT_APP_MOUSE_ENTER, false, SceneEventReceiver )
                ->onSceneAppMouseEnter( m_object );

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
    }
    //////////////////////////////////////////////////////////////////////////
    void Scene::onFocus( bool _focus )
    {
        EventationInterface * event = this->getScriptEventation();

        if( event != nullptr )
        {
            bool handle = EVENTABLE_METHODRT( event, EVENT_FOCUS, false, SceneEventReceiver )
                ->onSceneAppFocus( m_object, _focus );

            if( handle == false )
            {
                for( IntrusiveSlugChild it( m_children ); it.eof() == false; )
                {
                    const NodePtr & children = *it;

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
}
