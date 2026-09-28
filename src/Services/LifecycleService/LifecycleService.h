#pragma once

#include "Interface/LifecycleServiceInterface.h"
#include "Interface/ApplicationLifecycleInterface.h"
#include "Interface/RenderDeviceLifecycleInterface.h"

#include "Kernel/ServiceBase.h"
#include "Kernel/Vector.h"

#include "Config/Lambda.h"

namespace Mengine
{
    //////////////////////////////////////////////////////////////////////////
    class LifecycleService
        : public ServiceBase<LifecycleServiceInterface>
    {
        DECLARE_FACTORABLE( LifecycleService );

    public:
        LifecycleService();
        ~LifecycleService() override;

    public:
        bool _initializeService() override;
        void _finalizeService() override;

    public:
        bool registerService( const ServiceInterfacePtr & _service ) override;
        void unregisterService( const ServiceInterfacePtr & _service ) override;

    public:
        void registerLifecycle( LifecycleInterface * _dispatchable ) override;
        void unregisterLifecycle( LifecycleInterface * _dispatchable ) override;

    public:
        void registerPlugin( const PluginInterfacePtr & _plugin ) override;
        void unregisterPlugin( const PluginInterfacePtr & _plugin ) override;

    public:
        void onApplicationDidBecomeActive() override;
        void onApplicationWillEnterForeground() override;
        void onApplicationDidEnterBackground() override;
        void onApplicationWillResignActive() override;
        void onApplicationWillTerminate() override;
        void onApplicationDidReceiveMemoryWarning() override;
        void onApplicationDidReceiveTrimMemory( int32_t _level ) override;
        void onRenderDeviceCreate() override;
        void onRenderDeviceDestroy() override;
        void onRenderDeviceLostPrepare() override;
        void onRenderDeviceLostRestore() override;

    public:
        void preUpdate() override;
        void update() override;
        void postUpdate() override;

    protected:
        typedef Lambda<void( ApplicationLifecycleInterface * )> LambdaApplicationLifecycle;
        typedef Lambda<void( RenderDeviceLifecycleInterface * )> LambdaRenderDeviceLifecycle;

        void registerApplicationLifecycle_( ApplicationLifecycleInterface * _application );
        void unregisterApplicationLifecycle_( ApplicationLifecycleInterface * _application );
        void dispatchApplicationLifecycles_( const LambdaApplicationLifecycle & _lambda );

        void registerRenderDeviceLifecycle_( RenderDeviceLifecycleInterface * _renderDevice );
        void unregisterRenderDeviceLifecycle_( RenderDeviceLifecycleInterface * _renderDevice );
        void dispatchRenderDeviceLifecycles_( const LambdaRenderDeviceLifecycle & _lambda );

    protected:
        struct ApplicationLifecycleDesc
        {
            ApplicationLifecycleInterface * application;
        };

        typedef Vector<ApplicationLifecycleDesc> VectorApplicationLifecycles;

        VectorApplicationLifecycles m_applicationLifecycles;
        VectorApplicationLifecycles m_applicationLifecyclesAdd;

    protected:
        struct RenderDeviceLifecycleDesc
        {
            RenderDeviceLifecycleInterface * renderDevice;
        };

        typedef Vector<RenderDeviceLifecycleDesc> VectorRenderDeviceLifecycles;

        VectorRenderDeviceLifecycles m_renderDeviceLifecycles;
        VectorRenderDeviceLifecycles m_renderDeviceLifecyclesAdd;

    protected:
        struct LifecycleDesc
        {
            LifecycleInterface * lifecycle;
        };

        typedef Vector<LifecycleDesc> VectorLifecycle;

        VectorLifecycle m_lifecycles;
        VectorLifecycle m_lifecyclesAdd;
    };
    //////////////////////////////////////////////////////////////////////////
}
