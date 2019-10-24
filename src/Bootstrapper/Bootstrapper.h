#pragma once

#include "Interface/BootstrapperInterface.h"

#include "Kernel/ServiceBase.h"

namespace Mengine
{
    class Bootstrapper
        : public ServiceBase<BootstrapperInterface>
    {
    public:
        Bootstrapper();
        ~Bootstrapper() override;

    public:
        bool _initializeService() override;
        void _finalizeService() override;

    public:
        bool run( const VectorFilePath & _packagesPaths ) override;
        void stop() override;

    protected:
        bool createServices_();
        bool createStaticPlugins_();
        bool createDynamicPlugins_();
        bool createDynamicDevPlugins_();
        bool createApplication_();
        bool createFrameworks_();

    protected:
        void finalizeFrameworks_();

    protected:
        bool runModules_();
        bool runDevModules_();        
        bool runFrameworks_();

    protected:
        void stopModules_();
        void stopDevModules_();
        void stopFrameworks_();
    };
}