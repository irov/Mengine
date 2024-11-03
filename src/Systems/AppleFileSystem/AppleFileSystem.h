#	pragma once

#include "Interface/FileSystemInterface.h"

#include "Kernel/ServiceBase.h"

namespace Mengine
{
    class AppleFileSystem
        : public ServiceBase<FileSystemInterface>
    {
    public:
        AppleFileSystem();
        ~AppleFileSystem() override;

    protected:
        bool _initializeService() override;
        void _finalizeService() override;
    };
}
