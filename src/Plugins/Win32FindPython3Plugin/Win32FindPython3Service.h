#pragma once

#include "Win32FindPython3Interface.h"

#include "Kernel/ServiceBase.h"

namespace Mengine
{
    class Win32FindPython3Service
        : public ServiceBase<Win32FindPython3ServiceInterface>
    {
    public:
        Win32FindPython3Service();
        ~Win32FindPython3Service() override;

    public:
        bool _initializeService() override;
        void _finalizeService() override;

    protected:
        void getPython3Path( Char * const _python3Path ) const override;

    protected:
        Char m_python3Path[MENGINE_MAX_PATH] = {'\0'};
    };
}