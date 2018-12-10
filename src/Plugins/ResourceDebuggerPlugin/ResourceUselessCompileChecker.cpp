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

        return true;
    }
    //////////////////////////////////////////////////////////////////////////
    void ResourceUselessCompileChecker::finalize()
    {
        NOTIFICATION_ADDOBSERVERMETHOD( NOTIFICATOR_CHANGE_SCENE_PREPARE_ENABLE, this, &ResourceUselessCompileChecker::notifyChangeScenePrepareEnable );
        NOTIFICATION_ADDOBSERVERMETHOD( NOTIFICATOR_CHANGE_SCENE_ENABLE_FINALLY, this, &ResourceUselessCompileChecker::notifyChangeSceneEnableFinally );
    }
    //////////////////////////////////////////////////////////////////////////
    void ResourceUselessCompileChecker::notifyChangeScenePrepareEnable( const ScenePtr & _scene )
    {
        MENGINE_UNUSED( _scene );

        NOTIFICATION_ADDOBSERVERMETHOD( NOTIFICATOR_RESOURCE_COMPILE, this, &ResourceUselessCompileChecker::notifyResourceCompile );
        NOTIFICATION_ADDOBSERVERMETHOD( NOTIFICATOR_RESOURCE_RELEASE, this, &ResourceUselessCompileChecker::notifyResourceRelease );
    }
    //////////////////////////////////////////////////////////////////////////
    void ResourceUselessCompileChecker::notifyChangeSceneEnableFinally( const ScenePtr & _scene )
    {
        MENGINE_UNUSED( _scene );

        NOTIFICATION_REMOVEOBSERVER( NOTIFICATOR_RESOURCE_COMPILE, this );
        NOTIFICATION_REMOVEOBSERVER( NOTIFICATOR_RESOURCE_RELEASE, this );
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