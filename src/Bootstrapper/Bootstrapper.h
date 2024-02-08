#pragma once

#include "Interface/BootstrapperInterface.h"
#include "Interface/LoggerInterface.h"

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
        bool initialize() override;
        bool run() override;
        void stop() override;

    protected:
        bool createServices_();
        bool createStaticPlugins_();
        bool createDynamicPlugins_();
        bool createDynamicDevPlugins_();
        bool createDynamicSystemPlugins_();
        bool createDynamicSystemDevPlugins_();
        bool createDynamicPriorityPlugins_();
        bool createDynamicPriorityDevPlugins_();
        bool createDynamicOptionsPlugins_();
        bool createApplication_();
        bool createFrameworks_();

    protected:
        bool registerBaseTypes_();
        void unregisterBaseTypes_();

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

    protected:
        bool loadApplicationConfig_();
        bool mountUserFileGroup_();
        bool initializeFileLogger_();

    protected:
        VectorFilePath m_packagesPaths;
        VectorFilePath m_settingsPaths;

        LoggerInterfacePtr m_loggerFile;
    };
}