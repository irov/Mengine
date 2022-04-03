#pragma once

#include "Interface/ExecutorInterface.h"

namespace Mengine
{
    class Win32PlatformDevToDebug
        : public ExecutorInterface
    {
    public:
        Win32PlatformDevToDebug();
        ~Win32PlatformDevToDebug() override;

    public:
        bool execute() override;
    };
}