#include "SceneService.h"

#include "Interface/ModuleServiceInterface.h"
#include "Interface/PrototypeServiceInterface.h"

#include "Kernel/Scene.h"
#include "Kernel/DocumentHelper.h"
#include "Kernel/Logger.h"
#include "Kernel/AssertionMemoryPanic.h"
#include "Kernel/ConstStringHelper.h"
#include "Kernel/NotificationHelper.h"

//////////////////////////////////////////////////////////////////////////
SERVICE_FACTORY( SceneService, Mengine::SceneService );
//////////////////////////////////////////////////////////////////////////
namespace Mengine
{
    //////////////////////////////////////////////////////////////////////////
    SceneService::SceneService()
        : m_process( 0 )
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
        for( SceneCommandDesc & desc : m_commands )
        {
            switch( desc.type )
            {
            case ESCT_SET:
                {
                    desc.scene->dispose();
                    desc.scene = nullptr;
                }break;
            case ESCT_RESTART:
                {
                    //Empty
                }break;
            case ESCT_REMOVE:
                {
                    //Empty
                }break;
            }
        }

        m_commands.clear();

        this->destroyCurrentScene_();
        this->removeGlobalScene();
    }
    //////////////////////////////////////////////////////////////////////////
    bool SceneService::setCurrentScene( const ScenePtr & _scene, bool _immediately, bool _destroyOld, const SceneChangeCallbackInterfacePtr & _cb )
    {
        if( this->isInitializeService() == false )
        {
            return false;
        }

        LOGGER_INFO( "scene", "set current scene '%s' immediately [%s] destroy old [%s]"
            , _scene->getName().c_str()
            , _immediately == true ? "True" : "False"
            , _destroyOld == true ? "True" : "False"
        );

        if( m_commands.empty() == false )
        {
            return false;
        }

        MENGINE_ASSERTION_MEMORY_PANIC( _scene, "scene == nullptr" );

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
        if( this->isInitializeService() == false )
        {
            return false;
        }

        if( m_commands.empty() == false )
        {
            return false;
        }

        if( m_scene == nullptr )
        {
            return false;
        }

        LOGGER_INFO( "scene", "restart current scene '%s' immediately [%s]"
            , m_scene->getName().c_str()
            , _immediately == true ? "True" : "False"
        );

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
        if( this->isInitializeService() == false )
        {
            return false;
        }

        if( m_commands.empty() == false )
        {
            return false;
        }

        if( m_scene == nullptr )
        {
            return false;
        }

        LOGGER_INFO( "scene", "remove current scene '%s' immediately [%s]"
            , m_scene->getName().c_str()
            , _immediately == true ? "True" : "False"
        );

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

        NOTIFICATION_NOTIFY( NOTIFICATOR_CHANGE_SCENE_PREPARE_DESTROY, m_scene, _desc.scene );

        if( m_scene != nullptr )
        {
            m_scene->dispose();
            m_scene = nullptr;
        }

        NOTIFICATION_NOTIFY( NOTIFICATOR_CHANGE_SCENE_DESTROY, oldScene );

        if( _desc.cb != nullptr )
        {
            _desc.cb->onSceneChange( nullptr, false, false, false );
        }

        NOTIFICATION_NOTIFY( NOTIFICATOR_CHANGE_SCENE_PREPARE_INITIALIZE, _desc.scene );

        m_scene = _desc.scene;

        NOTIFICATION_NOTIFY( NOTIFICATOR_CHANGE_SCENE_INITIALIZE, m_scene );

        if( _desc.cb != nullptr )
        {
            _desc.cb->onSceneChange( m_scene, false, false, false );
        }

        NOTIFICATION_NOTIFY( NOTIFICATOR_CHANGE_SCENE_PREPARE_ENABLE, m_scene );

        if( m_scene->enableForce() == false )
        {
            NOTIFICATION_NOTIFY( NOTIFICATOR_CHANGE_SCENE_ERROR, m_scene );
        
            if( _desc.cb != nullptr )
            {
                _desc.cb->onSceneChange( m_scene, false, false, true );
            }

            return;
        }

        NOTIFICATION_NOTIFY( NOTIFICATOR_CHANGE_SCENE_ENABLE, m_scene );
        NOTIFICATION_NOTIFY( NOTIFICATOR_CHANGE_SCENE_ENABLE_FINALLY, m_scene );
        NOTIFICATION_NOTIFY( NOTIFICATOR_CHANGE_SCENE_PREPARE_COMPLETE, m_scene );

        if( _desc.cb != nullptr )
        {
            _desc.cb->onSceneChange( m_scene, true, false, false );
        }

        NOTIFICATION_NOTIFY( NOTIFICATOR_CHANGE_SCENE_COMPLETE, m_scene );
    }
    //////////////////////////////////////////////////////////////////////////
    void SceneService::restartCurrentScene_( const SceneCommandDesc & _desc )
    {
        NOTIFICATION_NOTIFY( NOTIFICATOR_RESTART_SCENE_PREPARE_DISABLE, m_scene );

        m_scene->disable();

        NOTIFICATION_NOTIFY( NOTIFICATOR_RESTART_SCENE_DISABLE, m_scene );

        if( _desc.cb != nullptr )
        {
            _desc.cb->onSceneChange( nullptr, false, false, false );
        }

        NOTIFICATION_NOTIFY( NOTIFICATOR_RESTART_SCENE_INITIALIZE, m_scene );

        if( _desc.cb != nullptr )
        {
            _desc.cb->onSceneChange( m_scene, false, false, false );
        }

        NOTIFICATION_NOTIFY( NOTIFICATOR_RESTART_SCENE_PREPARE_ENABLE, m_scene );

        if( m_scene->enableForce() == false )
        {
            NOTIFICATION_NOTIFY( NOTIFICATOR_RESTART_SCENE_ERROR, m_scene );

            if( _desc.cb != nullptr )
            {
                _desc.cb->onSceneChange( m_scene, false, false, true );
            }

            return;
        }

        NOTIFICATION_NOTIFY( NOTIFICATOR_RESTART_SCENE_ENABLE, m_scene );
        NOTIFICATION_NOTIFY( NOTIFICATOR_RESTART_SCENE_ENABLE_FINALLY, m_scene );
        NOTIFICATION_NOTIFY( NOTIFICATOR_RESTART_SCENE_PREPARE_COMPLETE, m_scene );

        if( _desc.cb != nullptr )
        {
            _desc.cb->onSceneChange( m_scene, true, false, false );
        }

        NOTIFICATION_NOTIFY( NOTIFICATOR_RESTART_SCENE_COMPLETE, m_scene );
    }
    //////////////////////////////////////////////////////////////////////////
    void SceneService::removeCurrentScene_( const SceneCommandDesc & _desc )
    {
        this->destroyCurrentScene_();

        NOTIFICATION_NOTIFY( NOTIFICATOR_REMOVE_SCENE_PREPARE_COMPLETE );

        if( _desc.cb != nullptr )
        {
            _desc.cb->onSceneChange( nullptr, false, true, false );
        }

        NOTIFICATION_NOTIFY( NOTIFICATOR_REMOVE_SCENE_COMPLETE );
    }
    //////////////////////////////////////////////////////////////////////////
    void SceneService::destroyCurrentScene_()
    {
        if( m_scene == m_globalScene )
        {
            LOGGER_ERROR( "block delete global scene" );

            return;
        }

        ScenePtr destroyScene = m_scene;
        m_scene = nullptr;

        if( destroyScene != nullptr )
        {
            NOTIFICATION_NOTIFY( NOTIFICATOR_REMOVE_SCENE_PREPARE_DESTROY, (destroyScene) );

            destroyScene->dispose();
            destroyScene = nullptr;

            NOTIFICATION_NOTIFY( NOTIFICATOR_REMOVE_SCENE_DESTROY );
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
            ->generatePrototype( STRINGIZE_STRING_LOCAL( "Node" ), STRINGIZE_STRING_LOCAL( "Scene" ), MENGINE_DOCUMENT_FACTORABLE );

        MENGINE_ASSERTION_MEMORY_PANIC( scene );

        m_globalScene = scene;

        return true;
    }
    //////////////////////////////////////////////////////////////////////////
    void SceneService::removeGlobalScene()
    {
        ScenePtr globalScene = m_globalScene;
        m_globalScene = nullptr;

        if( globalScene != nullptr )
        {
            globalScene->dispose();
            globalScene = nullptr;
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
        if( m_process != 0 )
        {
            return;
        }

        ++m_process;

        VectorSceneCommandDesc commands = std::move( m_commands );

        for( const SceneCommandDesc & desc : commands )
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

        --m_process;
    }
    //////////////////////////////////////////////////////////////////////////
    bool SceneService::isProcess() const
    {
        return m_process > 0;
    }
    //////////////////////////////////////////////////////////////////////////
}
