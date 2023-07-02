#include "SceneService.h"

#include "Interface/ModuleServiceInterface.h"
#include "Interface/PrototypeServiceInterface.h"
#include "Interface/ResourceServiceInterface.h"

#include "Kernel/Scene.h"
#include "Kernel/DocumentHelper.h"
#include "Kernel/Logger.h"
#include "Kernel/AssertionMemoryPanic.h"
#include "Kernel/ConstStringHelper.h"
#include "Kernel/NotificationHelper.h"
#include "Kernel/ThreadMutexScope.h"
#include "Kernel/StatisticHelper.h"
#include "Kernel/VectorResources.h"

#include "Config/Algorithm.h"

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
            case ESCT_SET_CURRENT_SCENE:
                {
                    desc.scene->dispose();
                    desc.scene = nullptr;
                }break;
            case ESCT_RESTART_CURRENT_SCENE:
                {
                    //Empty
                }break;
            case ESCT_REMOVE_CURRENT_SCENE:
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
    void SceneService::addCurrentSceneProvider( const CurrentSceneProviderInterfacePtr & _currentSceneProvider )
    {
        m_currentSceneProviders.emplace_back( _currentSceneProvider );
    }
    //////////////////////////////////////////////////////////////////////////
    void SceneService::removeCurrentSceneProvider( const CurrentSceneProviderInterfacePtr & _currentSceneProvider )
    {
        VectorCurrentSceneProviders::iterator it_found = Algorithm::find( m_currentSceneProviders.begin(), m_currentSceneProviders.end(), _currentSceneProvider );

        MENGINE_ASSERTION_FATAL( it_found != m_currentSceneProviders.end(), "not found current scene providers" );

        m_currentSceneProviders.erase( it_found );
    }
    //////////////////////////////////////////////////////////////////////////
    bool SceneService::setCurrentScene( const ScenePtr & _scene, bool _immediately, bool _destroyOld, const SceneChangeCallbackInterfacePtr & _cb )
    {
        if( this->isInitializeService() == false )
        {
            return false;
        }

        MENGINE_ASSERTION_MEMORY_PANIC( _scene, "scene == nullptr" );

        LOGGER_INFO( "scene", "set current scene '%s' immediately [%s] destroy old [%s]"
            , _scene->getName().c_str()
            , _immediately == true ? "True" : "False"
            , _destroyOld == true ? "True" : "False"
        );

        if( m_commands.empty() == false )
        {
            return false;
        }        

        SceneCommandDesc desc;
        desc.type = ESCT_SET_CURRENT_SCENE;
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

        if( m_currentScene == nullptr )
        {
            return false;
        }

        LOGGER_INFO( "scene", "restart current scene '%s' immediately [%s]"
            , m_currentScene->getName().c_str()
            , _immediately == true ? "True" : "False"
        );

        SceneCommandDesc desc;
        desc.type = ESCT_RESTART_CURRENT_SCENE;
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

        if( m_currentScene == nullptr )
        {
            return false;
        }

        LOGGER_INFO( "scene", "remove current scene '%s' immediately [%s]"
            , m_currentScene->getName().c_str()
            , _immediately == true ? "True" : "False"
        );

        SceneCommandDesc desc;
        desc.type = ESCT_REMOVE_CURRENT_SCENE;
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
        ScenePtr oldScene = m_currentScene;

        NOTIFICATION_NOTIFY( NOTIFICATOR_CHANGE_SCENE_PREPARE_DESTROY, m_currentScene, _desc.scene );

        if( m_currentScene != nullptr )
        {
            m_currentScene->dispose();
            m_currentScene = nullptr;
        }

        NOTIFICATION_NOTIFY( NOTIFICATOR_CHANGE_SCENE_DESTROY, oldScene );

        STATISTIC_SET_CONSTSTRING( STATISTIC_CURRENT_SCENE_NAME, ConstString::none() );

        if( _desc.cb != nullptr )
        {
            _desc.cb->onSceneChange( nullptr, false, false, false );
        }

        NOTIFICATION_NOTIFY( NOTIFICATOR_CHANGE_SCENE_PREPARE_INITIALIZE, _desc.scene );

        m_currentScene = _desc.scene;

        STATISTIC_SET_CONSTSTRING( STATISTIC_CURRENT_SCENE_NAME, m_currentScene->getName() );

        NOTIFICATION_NOTIFY( NOTIFICATOR_CHANGE_SCENE_INITIALIZE, m_currentScene );

        if( _desc.cb != nullptr )
        {
            _desc.cb->onSceneChange( m_currentScene, false, false, false );
        }

        NOTIFICATION_NOTIFY( NOTIFICATOR_CHANGE_SCENE_PREPARE_ENABLE, m_currentScene );

        if( m_currentScene->enableForce() == false )
        {
            NOTIFICATION_NOTIFY( NOTIFICATOR_CHANGE_SCENE_ERROR, m_currentScene );
        
            if( _desc.cb != nullptr )
            {
                _desc.cb->onSceneChange( m_currentScene, false, false, true );
            }

            return;
        }

        NOTIFICATION_NOTIFY( NOTIFICATOR_CHANGE_SCENE_ENABLE, m_currentScene );
        NOTIFICATION_NOTIFY( NOTIFICATOR_CHANGE_SCENE_ENABLE_FINALLY, m_currentScene );
        NOTIFICATION_NOTIFY( NOTIFICATOR_CHANGE_SCENE_PREPARE_COMPLETE, m_currentScene );

        if( _desc.cb != nullptr )
        {
            _desc.cb->onSceneChange( m_currentScene, true, false, false );
        }

        for( const CurrentSceneProviderInterfacePtr & provider : m_currentSceneProviders )
        {
            provider->onCurrentSceneChange( m_currentScene );
        }

        NOTIFICATION_NOTIFY( NOTIFICATOR_CHANGE_SCENE_COMPLETE, m_currentScene );
    }
    //////////////////////////////////////////////////////////////////////////
    void SceneService::restartCurrentScene_( const SceneCommandDesc & _desc )
    {
        VectorResources cacheResources;

        RESOURCE_SERVICE()
            ->foreachResources( [this, &cacheResources]( const ResourcePtr & _resource )
        {
            if( _resource->isCompile() == false )
            {
                return;
            }

            cacheResources.emplace_back( _resource );
        } );

        for( const ResourcePtr & resource : cacheResources )
        {
            resource->compile();
        }

        NOTIFICATION_NOTIFY( NOTIFICATOR_RESTART_SCENE_PREPARE_DISABLE, m_currentScene );

        if( m_currentScene != nullptr )
        {
            m_currentScene->disable();
        }

        NOTIFICATION_NOTIFY( NOTIFICATOR_RESTART_SCENE_DISABLE, m_currentScene );

        if( _desc.cb != nullptr )
        {
            _desc.cb->onSceneChange( nullptr, false, false, false );
        }

        NOTIFICATION_NOTIFY( NOTIFICATOR_RESTART_SCENE_INITIALIZE, m_currentScene );

        if( _desc.cb != nullptr )
        {
            _desc.cb->onSceneChange( m_currentScene, false, false, false );
        }

        NOTIFICATION_NOTIFY( NOTIFICATOR_RESTART_SCENE_PREPARE_ENABLE, m_currentScene );

        if( m_currentScene != nullptr )
        {
            if( m_currentScene->enableForce() == false )
            {
                NOTIFICATION_NOTIFY( NOTIFICATOR_RESTART_SCENE_ERROR, m_currentScene );

                if( _desc.cb != nullptr )
                {
                    _desc.cb->onSceneChange( m_currentScene, false, false, true );
                }

                return;
            }
        }

        NOTIFICATION_NOTIFY( NOTIFICATOR_RESTART_SCENE_ENABLE, m_currentScene );
        NOTIFICATION_NOTIFY( NOTIFICATOR_RESTART_SCENE_ENABLE_FINALLY, m_currentScene );
        NOTIFICATION_NOTIFY( NOTIFICATOR_RESTART_SCENE_PREPARE_COMPLETE, m_currentScene );

        if( _desc.cb != nullptr )
        {
            _desc.cb->onSceneChange( m_currentScene, true, false, false );
        }

        for( const ResourcePtr & resource : cacheResources )
        {
            resource->release();
        }

        NOTIFICATION_NOTIFY( NOTIFICATOR_RESTART_SCENE_COMPLETE, m_currentScene );
    }
    //////////////////////////////////////////////////////////////////////////
    void SceneService::removeCurrentScene_( const SceneCommandDesc & _desc, bool _remove )
    {
        if( m_globalScene != nullptr )
        {
            if( m_currentScene == m_globalScene )
            {
                LOGGER_ERROR( "block delete global scene" );

                return;
            }
        }

        this->destroyCurrentScene_();

        NOTIFICATION_NOTIFY( NOTIFICATOR_REMOVE_SCENE_PREPARE_COMPLETE );

        if( _desc.cb != nullptr )
        {
            _desc.cb->onSceneChange( nullptr, false, _remove, false );
        }

        for( const CurrentSceneProviderInterfacePtr & provider : m_currentSceneProviders )
        {
            provider->onCurrentSceneChange( nullptr );
        }

        NOTIFICATION_NOTIFY( NOTIFICATOR_REMOVE_SCENE_COMPLETE );
    }
    //////////////////////////////////////////////////////////////////////////
    void SceneService::destroyCurrentScene_()
    {
        if( m_currentScene == nullptr )
        {
            return;
        }

        ScenePtr oldScene = m_currentScene;

        ScenePtr destroyScene = m_currentScene;
        m_currentScene = nullptr;

        NOTIFICATION_NOTIFY( NOTIFICATOR_REMOVE_SCENE_PREPARE_DESTROY, oldScene );

        if( destroyScene != nullptr )
        {
            destroyScene->dispose();
            destroyScene = nullptr;
        }

        NOTIFICATION_NOTIFY( NOTIFICATOR_REMOVE_SCENE_DESTROY );

        STATISTIC_SET_CONSTSTRING( STATISTIC_CURRENT_SCENE_NAME, ConstString::none() );
    }
    //////////////////////////////////////////////////////////////////////////
    const ScenePtr & SceneService::getCurrentScene() const
    {
        return m_currentScene;
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
            case ESCT_SET_CURRENT_SCENE:
                {
                    this->setCurrentScene_( desc );
                }break;
            case ESCT_RESTART_CURRENT_SCENE:
                {
                    this->restartCurrentScene_( desc );
                }break;
            case ESCT_REMOVE_CURRENT_SCENE:
                {
                    this->removeCurrentScene_( desc, true );
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
