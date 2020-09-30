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
        NOTIFICATION_ADDOBSERVERMETHOD_THIS( NOTIFICATOR_CHANGE_SCENE_PREPARE_ENABLE, &ResourceUselessCompileChecker::notifyChangeScenePrepareEnable, MENGINE_DOCUMENT_FACTORABLE );
        NOTIFICATION_ADDOBSERVERMETHOD_THIS( NOTIFICATOR_CHANGE_SCENE_ENABLE_FINALLY, &ResourceUselessCompileChecker::notifyChangeSceneEnableFinally, MENGINE_DOCUMENT_FACTORABLE );

        NOTIFICATION_ADDOBSERVERMETHOD_THIS( NOTIFICATOR_RESTART_SCENE_PREPARE_ENABLE, &ResourceUselessCompileChecker::notifyRestartScenePrepareEnable, MENGINE_DOCUMENT_FACTORABLE );
        NOTIFICATION_ADDOBSERVERMETHOD_THIS( NOTIFICATOR_RESTART_SCENE_ENABLE_FINALLY, &ResourceUselessCompileChecker::notifyRestartSceneEnableFinally, MENGINE_DOCUMENT_FACTORABLE );
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

        NOTIFICATION_ADDOBSERVERMETHOD_THIS( NOTIFICATOR_DEVELOPMENT_RESOURCE_COMPILE, &ResourceUselessCompileChecker::notifyResourceCompile, MENGINE_DOCUMENT_FACTORABLE );
        NOTIFICATION_ADDOBSERVERMETHOD_THIS( NOTIFICATOR_DEVELOPMENT_RESOURCE_RELEASE, &ResourceUselessCompileChecker::notifyResourceRelease, MENGINE_DOCUMENT_FACTORABLE );
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

        NOTIFICATION_ADDOBSERVERMETHOD_THIS( NOTIFICATOR_DEVELOPMENT_RESOURCE_COMPILE, &ResourceUselessCompileChecker::notifyResourceCompile, MENGINE_DOCUMENT_FACTORABLE );
        NOTIFICATION_ADDOBSERVERMETHOD_THIS( NOTIFICATOR_DEVELOPMENT_RESOURCE_RELEASE, &ResourceUselessCompileChecker::notifyResourceRelease, MENGINE_DOCUMENT_FACTORABLE );
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