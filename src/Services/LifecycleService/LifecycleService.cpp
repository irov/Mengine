#include "LifecycleService.h"

#include "Kernel/AssertionMemoryPanic.h"
#include "Kernel/MixinDebug.h"
#include "Kernel/Logger.h"
#include "Kernel/NotificationHelper.h"
#include "Kernel/IntrusivePtrScope.h"

#include "Config/StdAlgorithm.h"

//////////////////////////////////////////////////////////////////////////
SERVICE_FACTORY( LifecycleService, Mengine::LifecycleService );
//////////////////////////////////////////////////////////////////////////
namespace Mengine
{
    //////////////////////////////////////////////////////////////////////////
    LifecycleService::LifecycleService()
    {
    }
    //////////////////////////////////////////////////////////////////////////
    LifecycleService::~LifecycleService()
    {
    }
    //////////////////////////////////////////////////////////////////////////
    bool LifecycleService::registerService( const ServiceInterfacePtr & _service )
    {
        ApplicationLifecycleInterface * application = _service->getApplicationLifecycleable();

        if( application != nullptr )
        {
            this->registerApplicationLifecycle_( application );
        }

        RenderDeviceLifecycleInterface * renderDevice = _service->getRenderDeviceLifecycleable();

        if( renderDevice != nullptr )
        {
            this->registerRenderDeviceLifecycle_( renderDevice );
        }

        LifecycleInterface * lifecycle = _service->getLifecycleable();

        if( lifecycle != nullptr )
        {
            this->registerLifecycle( lifecycle );
        }

        return true;
    }
    //////////////////////////////////////////////////////////////////////////
    void LifecycleService::unregisterService( const ServiceInterfacePtr & _service )
    {
        ApplicationLifecycleInterface * application = _service->getApplicationLifecycleable();

        if( application != nullptr )
        {
            this->unregisterApplicationLifecycle_( application );
        }

        RenderDeviceLifecycleInterface * renderDevice = _service->getRenderDeviceLifecycleable();

        if( renderDevice != nullptr )
        {
            this->unregisterRenderDeviceLifecycle_( renderDevice );
        }

        LifecycleInterface * lifecycle = _service->getLifecycleable();

        if( lifecycle != nullptr )
        {
            this->unregisterLifecycle( lifecycle );
        }
    }
    //////////////////////////////////////////////////////////////////////////
    void LifecycleService::registerPlugin( const PluginInterfacePtr & _plugin )
    {
        ApplicationLifecycleInterface * application = _plugin->getApplicationLifecycleable();

        if( application != nullptr )
        {
            this->registerApplicationLifecycle_( application );
        }

        RenderDeviceLifecycleInterface * renderDevice = _plugin->getRenderDeviceLifecycleable();

        if( renderDevice != nullptr )
        {
            this->registerRenderDeviceLifecycle_( renderDevice );
        }
    }
    //////////////////////////////////////////////////////////////////////////
    void LifecycleService::unregisterPlugin( const PluginInterfacePtr & _plugin )
    {
        ApplicationLifecycleInterface * application = _plugin->getApplicationLifecycleable();

        if( application != nullptr )
        {
            this->unregisterApplicationLifecycle_( application );
        }

        RenderDeviceLifecycleInterface * renderDevice = _plugin->getRenderDeviceLifecycleable();

        if( renderDevice != nullptr )
        {
            this->unregisterRenderDeviceLifecycle_( renderDevice );
        }
    }
    //////////////////////////////////////////////////////////////////////////
    bool LifecycleService::_initializeService()
    {
        //Empty

        return true;
    }
    //////////////////////////////////////////////////////////////////////////
    void LifecycleService::_finalizeService()
    {
        StdAlgorithm::erase_if( m_lifecycles, []( const LifecycleDesc & _desc )
        {
            return _desc.lifecycle == nullptr;
        } );

        StdAlgorithm::erase_if( m_lifecyclesAdd, []( const LifecycleDesc & _desc )
        {
            return _desc.lifecycle == nullptr;
        } );

#if defined(MENGINE_DOCUMENT_ENABLE)
        for( const LifecycleDesc & desc : m_lifecycles )
        {
            LOGGER_ASSERTION( "was forgotten remove lifecycle '%s'"
                , MENGINE_MIXIN_DEBUG_NAME( desc.lifecycle )
            );
        }

        for( const LifecycleDesc & desc : m_lifecyclesAdd )
        {
            LOGGER_ASSERTION( "was forgotten remove lifecycle '%s'"
                , MENGINE_MIXIN_DEBUG_NAME( desc.lifecycle )
            );
        }
#endif

        m_applicationLifecycles.clear();
        m_applicationLifecyclesAdd.clear();
        m_renderDeviceLifecycles.clear();
        m_renderDeviceLifecyclesAdd.clear();

        m_lifecycles.clear();
        m_lifecyclesAdd.clear();
    }
    //////////////////////////////////////////////////////////////////////////
    void LifecycleService::registerLifecycle( LifecycleInterface * _lifecycle )
    {
        MENGINE_ASSERTION_MEMORY_PANIC( _lifecycle, "lifecycle is nullptr" );

        MENGINE_ASSERTION_FATAL( StdAlgorithm::find_if( m_lifecycles.begin(), m_lifecycles.end(), [_lifecycle]( const LifecycleDesc & _desc )
        {
            return _desc.lifecycle == _lifecycle;
        } ) == m_lifecycles.end(), "lifecycle '%s' already added"
            , MENGINE_MIXIN_DEBUG_NAME( _lifecycle )
        );

        MENGINE_ASSERTION_FATAL( StdAlgorithm::find_if( m_lifecyclesAdd.begin(), m_lifecyclesAdd.end(), [_lifecycle]( const LifecycleDesc & _desc )
        {
            return _desc.lifecycle == _lifecycle;
        } ) == m_lifecyclesAdd.end(), "lifecycle '%s' already added"
            , MENGINE_MIXIN_DEBUG_NAME( _lifecycle )
        );

        LifecycleDesc desc;
        desc.lifecycle = _lifecycle;

        m_lifecyclesAdd.emplace_back( desc );
    }
    //////////////////////////////////////////////////////////////////////////
    void LifecycleService::unregisterLifecycle( LifecycleInterface * _lifecycle )
    {
        MENGINE_ASSERTION_MEMORY_PANIC( _lifecycle, "lifecycle is nullptr" );

        VectorLifecycle::iterator it_found = StdAlgorithm::find_if( m_lifecycles.begin(), m_lifecycles.end(), [_lifecycle]( const LifecycleDesc & _desc )
        {
            return _desc.lifecycle == _lifecycle;
        } );

        if( it_found != m_lifecycles.end() )
        {
            it_found->lifecycle = nullptr;

            return;
        }

        VectorLifecycle::iterator it_add_found = StdAlgorithm::find_if( m_lifecyclesAdd.begin(), m_lifecyclesAdd.end(), [_lifecycle]( const LifecycleDesc & _desc )
        {
            return _desc.lifecycle == _lifecycle;
        } );

        if( it_add_found != m_lifecyclesAdd.end() )
        {
            m_lifecyclesAdd.erase( it_add_found );

            return;
        }
    }
    //////////////////////////////////////////////////////////////////////////
    void LifecycleService::registerApplicationLifecycle_( ApplicationLifecycleInterface * _application )
    {
        MENGINE_ASSERTION_MEMORY_PANIC( _application, "application lifecycle is nullptr" );

        MENGINE_ASSERTION_FATAL( StdAlgorithm::find_if( m_applicationLifecycles.begin(), m_applicationLifecycles.end(), [_application]( const ApplicationLifecycleDesc & _desc )
        {
            return _desc.application == _application;
        } ) == m_applicationLifecycles.end(), "application lifecycle '%s' already added"
            , MENGINE_MIXIN_DEBUG_NAME( _application )
        );

        MENGINE_ASSERTION_FATAL( StdAlgorithm::find_if( m_applicationLifecyclesAdd.begin(), m_applicationLifecyclesAdd.end(), [_application]( const ApplicationLifecycleDesc & _desc )
        {
            return _desc.application == _application;
        } ) == m_applicationLifecyclesAdd.end(), "application lifecycle '%s' already added"
            , MENGINE_MIXIN_DEBUG_NAME( _application )
        );

        ApplicationLifecycleDesc desc;
        desc.application = _application;

        m_applicationLifecyclesAdd.emplace_back( desc );
    }
    //////////////////////////////////////////////////////////////////////////
    void LifecycleService::unregisterApplicationLifecycle_( ApplicationLifecycleInterface * _application )
    {
        MENGINE_ASSERTION_MEMORY_PANIC( _application, "application lifecycle is nullptr" );

        VectorApplicationLifecycles::iterator it_found = StdAlgorithm::find_if( m_applicationLifecycles.begin(), m_applicationLifecycles.end(), [_application]( const ApplicationLifecycleDesc & _desc )
        {
            return _desc.application == _application;
        } );

        if( it_found != m_applicationLifecycles.end() )
        {
            it_found->application = nullptr;

            return;
        }

        VectorApplicationLifecycles::iterator it_add_found = StdAlgorithm::find_if( m_applicationLifecyclesAdd.begin(), m_applicationLifecyclesAdd.end(), [_application]( const ApplicationLifecycleDesc & _desc )
        {
            return _desc.application == _application;
        } );

        if( it_add_found != m_applicationLifecyclesAdd.end() )
        {
            m_applicationLifecyclesAdd.erase( it_add_found );

            return;
        }
    }
    //////////////////////////////////////////////////////////////////////////
    void LifecycleService::dispatchApplicationLifecycles_( const LambdaApplicationLifecycle & _lambda )
    {
        m_applicationLifecycles.insert( m_applicationLifecycles.end(), m_applicationLifecyclesAdd.begin(), m_applicationLifecyclesAdd.end() );
        m_applicationLifecyclesAdd.clear();

        VectorApplicationLifecycles::size_type count = m_applicationLifecycles.size();

        for( VectorApplicationLifecycles::size_type index = 0; index != count; ++index )
        {
            ApplicationLifecycleInterface * application = m_applicationLifecycles[index].application;

            if( application == nullptr )
            {
                continue;
            }

            IntrusivePtrScope applicationScope( application );

            _lambda( application );
        }
    }
    //////////////////////////////////////////////////////////////////////////
    void LifecycleService::registerRenderDeviceLifecycle_( RenderDeviceLifecycleInterface * _renderDevice )
    {
        MENGINE_ASSERTION_MEMORY_PANIC( _renderDevice, "render device lifecycle is nullptr" );

        MENGINE_ASSERTION_FATAL( StdAlgorithm::find_if( m_renderDeviceLifecycles.begin(), m_renderDeviceLifecycles.end(), [_renderDevice]( const RenderDeviceLifecycleDesc & _desc )
        {
            return _desc.renderDevice == _renderDevice;
        } ) == m_renderDeviceLifecycles.end(), "render device lifecycle '%s' already added"
            , MENGINE_MIXIN_DEBUG_NAME( _renderDevice )
        );

        MENGINE_ASSERTION_FATAL( StdAlgorithm::find_if( m_renderDeviceLifecyclesAdd.begin(), m_renderDeviceLifecyclesAdd.end(), [_renderDevice]( const RenderDeviceLifecycleDesc & _desc )
        {
            return _desc.renderDevice == _renderDevice;
        } ) == m_renderDeviceLifecyclesAdd.end(), "render device lifecycle '%s' already added"
            , MENGINE_MIXIN_DEBUG_NAME( _renderDevice )
        );

        RenderDeviceLifecycleDesc desc;
        desc.renderDevice = _renderDevice;

        m_renderDeviceLifecyclesAdd.emplace_back( desc );
    }
    //////////////////////////////////////////////////////////////////////////
    void LifecycleService::unregisterRenderDeviceLifecycle_( RenderDeviceLifecycleInterface * _renderDevice )
    {
        MENGINE_ASSERTION_MEMORY_PANIC( _renderDevice, "render device lifecycle is nullptr" );

        VectorRenderDeviceLifecycles::iterator it_found = StdAlgorithm::find_if( m_renderDeviceLifecycles.begin(), m_renderDeviceLifecycles.end(), [_renderDevice]( const RenderDeviceLifecycleDesc & _desc )
        {
            return _desc.renderDevice == _renderDevice;
        } );

        if( it_found != m_renderDeviceLifecycles.end() )
        {
            it_found->renderDevice = nullptr;

            return;
        }

        VectorRenderDeviceLifecycles::iterator it_add_found = StdAlgorithm::find_if( m_renderDeviceLifecyclesAdd.begin(), m_renderDeviceLifecyclesAdd.end(), [_renderDevice]( const RenderDeviceLifecycleDesc & _desc )
        {
            return _desc.renderDevice == _renderDevice;
        } );

        if( it_add_found != m_renderDeviceLifecyclesAdd.end() )
        {
            m_renderDeviceLifecyclesAdd.erase( it_add_found );

            return;
        }
    }
    //////////////////////////////////////////////////////////////////////////
    void LifecycleService::dispatchRenderDeviceLifecycles_( const LambdaRenderDeviceLifecycle & _lambda )
    {
        m_renderDeviceLifecycles.insert( m_renderDeviceLifecycles.end(), m_renderDeviceLifecyclesAdd.begin(), m_renderDeviceLifecyclesAdd.end() );
        m_renderDeviceLifecyclesAdd.clear();

        VectorRenderDeviceLifecycles::size_type count = m_renderDeviceLifecycles.size();

        for( VectorRenderDeviceLifecycles::size_type index = 0; index != count; ++index )
        {
            RenderDeviceLifecycleInterface * renderDevice = m_renderDeviceLifecycles[index].renderDevice;

            if( renderDevice == nullptr )
            {
                continue;
            }

            IntrusivePtrScope renderDeviceScope( renderDevice );

            _lambda( renderDevice );
        }
    }
    //////////////////////////////////////////////////////////////////////////
    void LifecycleService::onApplicationDidBecomeActive()
    {
        this->dispatchApplicationLifecycles_( []( ApplicationLifecycleInterface * _application )
        {
            _application->onApplicationDidBecomeActive();
        } );

        NOTIFICATION_NOTIFY( NOTIFICATOR_APPLICATION_DID_BECOME_ACTIVE );
    }
    //////////////////////////////////////////////////////////////////////////
    void LifecycleService::onApplicationWillEnterForeground()
    {
        this->dispatchApplicationLifecycles_( []( ApplicationLifecycleInterface * _application )
        {
            _application->onApplicationWillEnterForeground();
        } );

        NOTIFICATION_NOTIFY( NOTIFICATOR_APPLICATION_WILL_ENTER_FOREGROUND );
    }
    //////////////////////////////////////////////////////////////////////////
    void LifecycleService::onApplicationDidEnterBackground()
    {
        this->dispatchApplicationLifecycles_( []( ApplicationLifecycleInterface * _application )
        {
            _application->onApplicationDidEnterBackground();
        } );

        NOTIFICATION_NOTIFY( NOTIFICATOR_APPLICATION_DID_ENTER_BACKGROUND );
    }
    //////////////////////////////////////////////////////////////////////////
    void LifecycleService::onApplicationWillResignActive()
    {
        this->dispatchApplicationLifecycles_( []( ApplicationLifecycleInterface * _application )
        {
            _application->onApplicationWillResignActive();
        } );

        NOTIFICATION_NOTIFY( NOTIFICATOR_APPLICATION_WILL_RESIGN_ACTIVE );
    }
    //////////////////////////////////////////////////////////////////////////
    void LifecycleService::onApplicationWillTerminate()
    {
        this->dispatchApplicationLifecycles_( []( ApplicationLifecycleInterface * _application )
        {
            _application->onApplicationWillTerminate();
        } );

        NOTIFICATION_NOTIFY( NOTIFICATOR_APPLICATION_WILL_TERMINATE );
    }
    //////////////////////////////////////////////////////////////////////////
    void LifecycleService::onApplicationDidReceiveMemoryWarning()
    {
        this->dispatchApplicationLifecycles_( []( ApplicationLifecycleInterface * _application )
        {
            _application->onApplicationDidReceiveMemoryWarning();
        } );

        NOTIFICATION_NOTIFY( NOTIFICATOR_APPLICATION_DID_RECEIVE_MEMORY_WARNING );
    }
    //////////////////////////////////////////////////////////////////////////
    void LifecycleService::onApplicationDidReceiveTrimMemory( int32_t _level )
    {
        this->dispatchApplicationLifecycles_( [_level]( ApplicationLifecycleInterface * _application )
        {
            _application->onApplicationDidReceiveTrimMemory( _level );
        } );

        NOTIFICATION_NOTIFY( NOTIFICATOR_APPLICATION_DID_RECEIVE_TRIM_MEMORY, _level );
    }
    //////////////////////////////////////////////////////////////////////////
    void LifecycleService::onRenderDeviceCreate()
    {
        this->dispatchRenderDeviceLifecycles_( []( RenderDeviceLifecycleInterface * _renderDevice )
        {
            _renderDevice->onRenderDeviceCreate();
        } );

        NOTIFICATION_NOTIFY( NOTIFICATOR_RENDER_DEVICE_CREATE );
    }
    //////////////////////////////////////////////////////////////////////////
    void LifecycleService::onRenderDeviceDestroy()
    {
        this->dispatchRenderDeviceLifecycles_( []( RenderDeviceLifecycleInterface * _renderDevice )
        {
            _renderDevice->onRenderDeviceDestroy();
        } );

        NOTIFICATION_NOTIFY( NOTIFICATOR_RENDER_DEVICE_DESTROY );
    }
    //////////////////////////////////////////////////////////////////////////
    void LifecycleService::onRenderDeviceLostPrepare()
    {
        this->dispatchRenderDeviceLifecycles_( []( RenderDeviceLifecycleInterface * _renderDevice )
        {
            _renderDevice->onRenderDeviceLostPrepare();
        } );

        NOTIFICATION_NOTIFY( NOTIFICATOR_RENDER_DEVICE_LOST_PREPARE );
    }
    //////////////////////////////////////////////////////////////////////////
    void LifecycleService::onRenderDeviceLostRestore()
    {
        this->dispatchRenderDeviceLifecycles_( []( RenderDeviceLifecycleInterface * _renderDevice )
        {
            _renderDevice->onRenderDeviceLostRestore();
        } );

        NOTIFICATION_NOTIFY( NOTIFICATOR_RENDER_DEVICE_LOST_RESTORE );
    }
    //////////////////////////////////////////////////////////////////////////
    void LifecycleService::preUpdate()
    {
        StdAlgorithm::erase_if( m_applicationLifecycles, []( const ApplicationLifecycleDesc & _desc )
        {
            return _desc.application == nullptr;
        } );

        StdAlgorithm::erase_if( m_renderDeviceLifecycles, []( const RenderDeviceLifecycleDesc & _desc )
        {
            return _desc.renderDevice == nullptr;
        } );

        StdAlgorithm::erase_if( m_lifecycles, []( const LifecycleDesc & _desc )
        {
            return _desc.lifecycle == nullptr;
        } );

        if( m_lifecyclesAdd.empty() == false )
        {
            m_lifecycles.insert( m_lifecycles.end(), m_lifecyclesAdd.begin(), m_lifecyclesAdd.end() );
            m_lifecyclesAdd.clear();
        }

        for( const LifecycleDesc & desc : m_lifecycles )
        {
            if( desc.lifecycle == nullptr )
            {
                continue;
            }

            desc.lifecycle->preUpdate();
        }
    }
    //////////////////////////////////////////////////////////////////////////
    void LifecycleService::update()
    {
        for( const LifecycleDesc & desc : m_lifecycles )
        {
            if( desc.lifecycle == nullptr )
            {
                continue;
            }

            desc.lifecycle->update();
        }
    }
    //////////////////////////////////////////////////////////////////////////
    void LifecycleService::postUpdate()
    {
        for( const LifecycleDesc & desc : m_lifecycles )
        {
            if( desc.lifecycle == nullptr )
            {
                continue;
            }

            desc.lifecycle->postUpdate();
        }
    }
    //////////////////////////////////////////////////////////////////////////
}
