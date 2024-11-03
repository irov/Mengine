#	pragma once

#include "Interface/FileSystemInterface.h"

#include "Kernel/ServiceBase.h"

namespace Mengine
{
    class AndroidFileSystem
        : public ServiceBase<FileSystemInterface>
    {
    public:
        AndroidFileSystem();
        ~AndroidFileSystem() override;

    protected:
        bool _initializeService() override;
        void _finalizeService() override;
    };
}
