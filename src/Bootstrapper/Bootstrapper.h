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
        bool run() override;
        void stop() override;

    protected:
        bool createServices_();
        bool createStaticPlugins_();
        bool createDynamicPlugins_();
        bool createDynamicDevPlugins_();
        bool createApplication_();
        bool runModules_();
        bool runDevModules_();
        bool loadRenderMaterials_();
    };
}