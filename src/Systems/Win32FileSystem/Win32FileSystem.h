#pragma once

#include "Interface/FileSystemInterface.h"

#include "Kernel/ServiceBase.h"

namespace Mengine
{
    class Win32FileSystem
        : public ServiceBase<FileSystemInterface>
    {
    public:
        Win32FileSystem();
        ~Win32FileSystem() override;

    protected:
        bool _initializeService() override;
        void _finalizeService() override;
    };
}