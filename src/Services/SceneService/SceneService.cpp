#include "SceneService.h"

#include "Interface/ModuleServiceInterface.h"
#include "Interface/StringizeServiceInterface.h"
#include "Interface/NotificationServiceInterface.h"
#include "Interface/PrototypeServiceInterface.h"

#include "Kernel/Scene.h"
#include "Kernel/Document.h"

//////////////////////////////////////////////////////////////////////////
SERVICE_FACTORY( SceneService, Mengine::SceneService );
//////////////////////////////////////////////////////////////////////////
namespace Mengine
{
    //////////////////////////////////////////////////////////////////////////
    SceneService::SceneService()
    {
    }
    //////////////////////////////////////////////////////////////////////////
    SceneService::~SceneService()
    {
    }
    //////////////////////////////////////////////////////////////////////////
    bool SceneService::_initializeService()
    {
        //Empty

        return true;
    }
    //////////////////////////////////////////////////////////////////////////
    void SceneService::_finalizeService()
    {
        m_commands.clear();

        this->destroyCurrentScene();
        this->removeGlobalScene();
    }
    //////////////////////////////////////////////////////////////////////////
    bool SceneService::setCurrentScene( const ScenePtr & _scene, bool _immediately, bool _destroyOld, const SceneChangeCallbackInterfacePtr & _cb )
    {
        if( m_commands.empty() == false )
        {
            return false;
        }

        SceneCommandDesc desc;
        desc.type = ESCT_SET;
        desc.scene = _scene;
        desc.destroyOld = _destroyOld;
        desc.cb = _cb;

        m_commands.emplace_back( desc );

        if( _immediately == true )
        {
            this->update();
        }

        return true;
    }
    //////////////////////////////////////////////////////////////////////////
    bool SceneService::restartCurrentScene( bool _immediately, const SceneChangeCallbackInterfacePtr & _cb )
    {
        if( m_commands.empty() == false )
        {
            return false;
        }

        if( m_scene == nullptr )
        {
            return false;
        }

        SceneCommandDesc desc;
        desc.type = ESCT_RESTART;
        desc.cb = _cb;

        m_commands.emplace_back( desc );

        if( _immediately == true )
        {
            this->update();
        }

        return true;
    }
    //////////////////////////////////////////////////////////////////////////
    bool SceneService::removeCurrentScene( bool _immediately, const SceneChangeCallbackInterfacePtr & _cb )
    {
        if( m_commands.empty() == false )
        {
            return false;
        }

        if( m_scene == nullptr )
        {
            return false;
        }

        SceneCommandDesc desc;
        desc.type = ESCT_REMOVE;
        desc.cb = _cb;

        m_commands.emplace_back( desc );

        if( _immediately == true )
        {
            this->update();
        }

        return true;
    }
    //////////////////////////////////////////////////////////////////////////
    void SceneService::setCurrentScene_( const SceneCommandDesc & _desc )
    {
        ScenePtr oldScene = m_scene;

        NOTIFICATION_NOTIFY( NOTIFICATOR_CHANGE_SCENE_PREPARE_DESTROY, (m_scene, _desc.scene) );

        if( m_scene != nullptr )
        {
            m_scene->disable();
            m_scene->onDestroy();
            m_scene = nullptr;
        }

        NOTIFICATION_NOTIFY( NOTIFICATOR_CHANGE_SCENE_DESTROY, (oldScene) );

        if( _desc.cb != nullptr )
        {
            _desc.cb->onSceneChange( nullptr, false, false );
        }

        NOTIFICATION_NOTIFY( NOTIFICATOR_CHANGE_SCENE_PREPARE_INITIALIZE, (_desc.scene) );

        m_scene = _desc.scene;

        NOTIFICATION_NOTIFY( NOTIFICATOR_CHANGE_SCENE_INITIALIZE, (m_scene) );

        if( _desc.cb != nullptr )
        {
            _desc.cb->onSceneChange( m_scene, false, false );
        }

        NOTIFICATION_NOTIFY( NOTIFICATOR_CHANGE_SCENE_PREPARE_ENABLE, (m_scene) );

        m_scene->enableForce();

        NOTIFICATION_NOTIFY( NOTIFICATOR_CHANGE_SCENE_ENABLE, (m_scene) );

        NOTIFICATION_NOTIFY( NOTIFICATOR_CHANGE_SCENE_ENABLE_FINALLY, (m_scene) );

        NOTIFICATION_NOTIFY( NOTIFICATOR_CHANGE_SCENE_PREPARE_COMPLETE, (m_scene) );

        if( _desc.cb != nullptr )
        {
            _desc.cb->onSceneChange( m_scene, true, false );
        }

        NOTIFICATION_NOTIFY( NOTIFICATOR_CHANGE_SCENE_COMPLETE, (m_scene) );
    }
    //////////////////////////////////////////////////////////////////////////
    void SceneService::restartCurrentScene_( const SceneCommandDesc & _desc )
    {
        NOTIFICATION_NOTIFY( NOTIFICATOR_RESTART_SCENE_PREPARE_DISABLE, (m_scene) );

        m_scene->disable();

        NOTIFICATION_NOTIFY( NOTIFICATOR_RESTART_SCENE_DISABLE, (m_scene) );

        if( _desc.cb != nullptr )
        {
            _desc.cb->onSceneChange( nullptr, false, false );
        }

        NOTIFICATION_NOTIFY( NOTIFICATOR_RESTART_SCENE_INITIALIZE, (m_scene) );

        if( _desc.cb != nullptr )
        {
            _desc.cb->onSceneChange( m_scene, false, false );
        }

        NOTIFICATION_NOTIFY( NOTIFICATOR_RESTART_SCENE_PREPARE_ENABLE, (m_scene) );

        m_scene->enableForce();

        NOTIFICATION_NOTIFY( NOTIFICATOR_RESTART_SCENE_ENABLE, (m_scene) );

        NOTIFICATION_NOTIFY( NOTIFICATOR_RESTART_SCENE_ENABLE_FINALLY, (m_scene) );

        NOTIFICATION_NOTIFY( NOTIFICATOR_RESTART_SCENE_PREPARE_COMPLETE, (m_scene) );

        if( _desc.cb != nullptr )
        {
            _desc.cb->onSceneChange( m_scene, true, false );
        }

        NOTIFICATION_NOTIFY( NOTIFICATOR_RESTART_SCENE_COMPLETE, (m_scene) );
    }
    //////////////////////////////////////////////////////////////////////////
    void SceneService::removeCurrentScene_( const SceneCommandDesc & _desc )
    {
        NOTIFICATION_NOTIFY( NOTIFICATOR_REMOVE_SCENE_PREPARE_DESTROY, (m_scene) );

        if( m_scene != nullptr )
        {
            m_scene->disable();
            m_scene->onDestroy();
            m_scene = nullptr;
        }

        NOTIFICATION_NOTIFY( NOTIFICATOR_REMOVE_SCENE_DESTROY, () );

        NOTIFICATION_NOTIFY( NOTIFICATOR_REMOVE_SCENE_PREPARE_COMPLETE, () );

        if( _desc.cb != nullptr )
        {
            _desc.cb->onSceneChange( nullptr, false, true );
        }

        NOTIFICATION_NOTIFY( NOTIFICATOR_REMOVE_SCENE_COMPLETE, () );
    }
    //////////////////////////////////////////////////////////////////////////
    void SceneService::destroyCurrentScene()
    {
        ScenePtr destroyScene = m_scene;
        m_scene = nullptr;

        if( destroyScene != nullptr )
        {
            destroyScene->release();
            destroyScene->onDestroy();
            destroyScene = nullptr;
        }
    }
    //////////////////////////////////////////////////////////////////////////
    const ScenePtr & SceneService::getCurrentScene() const
    {
        return m_scene;
    }
    //////////////////////////////////////////////////////////////////////////
    bool SceneService::createGlobalScene()
    {
        if( m_globalScene != nullptr )
        {
            return true;
        }

        ScenePtr scene = PROTOTYPE_SERVICE()
            ->generatePrototype( STRINGIZE_STRING_LOCAL( "Node" ), STRINGIZE_STRING_LOCAL( "Scene" ), MENGINE_DOCUMENT_FUNCTION );

        if( scene == nullptr )
        {
            return false;
        }

        m_globalScene = scene;

        return true;
    }
    //////////////////////////////////////////////////////////////////////////
    void SceneService::removeGlobalScene()
    {
        if( m_globalScene != nullptr )
        {
            m_globalScene->disable();
            m_globalScene->onDestroy();
            m_globalScene = nullptr;
        }
    }
    //////////////////////////////////////////////////////////////////////////
    const ScenePtr & SceneService::getGlobalScene() const
    {
        return m_globalScene;
    }
    //////////////////////////////////////////////////////////////////////////
    void SceneService::update()
    {
        for( const SceneCommandDesc & desc : m_commands )
        {
            switch( desc.type )
            {
            case ESCT_SET:
                {
                    this->setCurrentScene_( desc );
                }break;
            case ESCT_RESTART:
                {
                    this->restartCurrentScene_( desc );
                }break;
            case ESCT_REMOVE:
                {
                    this->removeCurrentScene_( desc );
                }break;
            default:
                {
                }break;
            }
        }

        m_commands.clear();
    }
}
