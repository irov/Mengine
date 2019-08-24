#include "ResourceUselessCompileChecker.h"

#include "Interface/NotificationServiceInterface.h"

#include "Kernel/Logger.h"

namespace Mengine
{
    //////////////////////////////////////////////////////////////////////////
    ResourceUselessCompileChecker::ResourceUselessCompileChecker()
    {
    }
    //////////////////////////////////////////////////////////////////////////
    ResourceUselessCompileChecker::~ResourceUselessCompileChecker()
    {
    }
    //////////////////////////////////////////////////////////////////////////
    bool ResourceUselessCompileChecker::initialize()
    {
        NOTIFICATION_ADDOBSERVERMETHOD( NOTIFICATOR_CHANGE_SCENE_PREPARE_ENABLE, this, &ResourceUselessCompileChecker::notifyChangeScenePrepareEnable );
        NOTIFICATION_ADDOBSERVERMETHOD( NOTIFICATOR_CHANGE_SCENE_ENABLE_FINALLY, this, &ResourceUselessCompileChecker::notifyChangeSceneEnableFinally );

        NOTIFICATION_ADDOBSERVERMETHOD( NOTIFICATOR_RESTART_SCENE_PREPARE_ENABLE, this, &ResourceUselessCompileChecker::notifyRestartScenePrepareEnable );
        NOTIFICATION_ADDOBSERVERMETHOD( NOTIFICATOR_RESTART_SCENE_ENABLE_FINALLY, this, &ResourceUselessCompileChecker::notifyRestartSceneEnableFinally );
        return true;
    }
    //////////////////////////////////////////////////////////////////////////
    void ResourceUselessCompileChecker::finalize()
    {
        NOTIFICATION_REMOVEOBSERVER_THIS( NOTIFICATOR_CHANGE_SCENE_PREPARE_ENABLE );
        NOTIFICATION_REMOVEOBSERVER_THIS( NOTIFICATOR_CHANGE_SCENE_ENABLE_FINALLY );

        NOTIFICATION_REMOVEOBSERVER_THIS( NOTIFICATOR_RESTART_SCENE_PREPARE_ENABLE );
        NOTIFICATION_REMOVEOBSERVER_THIS( NOTIFICATOR_RESTART_SCENE_ENABLE_FINALLY );
    }
    //////////////////////////////////////////////////////////////////////////
    void ResourceUselessCompileChecker::notifyChangeScenePrepareEnable( const ScenePtr & _scene )
    {
        MENGINE_UNUSED( _scene );

        NOTIFICATION_ADDOBSERVERMETHOD( NOTIFICATOR_DEVELOPMENT_RESOURCE_COMPILE, this, &ResourceUselessCompileChecker::notifyResourceCompile );
        NOTIFICATION_ADDOBSERVERMETHOD( NOTIFICATOR_DEVELOPMENT_RESOURCE_RELEASE, this, &ResourceUselessCompileChecker::notifyResourceRelease );
    }
    //////////////////////////////////////////////////////////////////////////
    void ResourceUselessCompileChecker::notifyChangeSceneEnableFinally( const ScenePtr & _scene )
    {
        MENGINE_UNUSED( _scene );

        NOTIFICATION_REMOVEOBSERVER_THIS( NOTIFICATOR_DEVELOPMENT_RESOURCE_COMPILE );
        NOTIFICATION_REMOVEOBSERVER_THIS( NOTIFICATOR_DEVELOPMENT_RESOURCE_RELEASE );
    }
    //////////////////////////////////////////////////////////////////////////
    void ResourceUselessCompileChecker::notifyRestartScenePrepareEnable( const ScenePtr & _scene )
    {
        MENGINE_UNUSED( _scene );

        NOTIFICATION_ADDOBSERVERMETHOD( NOTIFICATOR_DEVELOPMENT_RESOURCE_COMPILE, this, &ResourceUselessCompileChecker::notifyResourceCompile );
        NOTIFICATION_ADDOBSERVERMETHOD( NOTIFICATOR_DEVELOPMENT_RESOURCE_RELEASE, this, &ResourceUselessCompileChecker::notifyResourceRelease );
    }
    //////////////////////////////////////////////////////////////////////////
    void ResourceUselessCompileChecker::notifyRestartSceneEnableFinally( const ScenePtr & _scene )
    {
        MENGINE_UNUSED( _scene );

        NOTIFICATION_REMOVEOBSERVER_THIS( NOTIFICATOR_DEVELOPMENT_RESOURCE_COMPILE );
        NOTIFICATION_REMOVEOBSERVER_THIS( NOTIFICATOR_DEVELOPMENT_RESOURCE_RELEASE );
    }
    //////////////////////////////////////////////////////////////////////////
    void ResourceUselessCompileChecker::notifyResourceCompile( Resource * _resource )
    {
        m_resources.emplace_back( _resource );
    }
    //////////////////////////////////////////////////////////////////////////
    void ResourceUselessCompileChecker::notifyResourceRelease( Resource * _resource )
    {
        VectorResourceDesc::iterator it_remove =
            std::find( m_resources.begin(), m_resources.end(), _resource );

        if( it_remove == m_resources.end() )
        {
            return;
        }

        LOGGER_PERFORMANCE( "Useless Compile '%s' '%s' '%s'"
            , _resource->getType().c_str()
            , _resource->getGroupName().c_str()
            , _resource->getName().c_str()
        );

        m_resources.erase( it_remove );
    }
}