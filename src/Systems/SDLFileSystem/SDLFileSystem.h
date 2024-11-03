#	pragma once

#include "Interface/FileSystemInterface.h"

#include "Kernel/ServiceBase.h"

namespace Mengine
{
    class SDLFileSystem
        : public ServiceBase<FileSystemInterface>
    {
    public:
        SDLFileSystem();
        ~SDLFileSystem() override;

    protected:
        bool _initializeService() override;
        void _finalizeService() override;
    };
}
