#pragma once

#include "Interface/FindPython3ServiceInterface.h"

#include "Kernel/ServiceBase.h"

#include "Config/Path.h"

namespace Mengine
{
    class Win32FindPython3Service
        : public ServiceBase<FindPython3ServiceInterface>
    {
        DECLARE_FACTORABLE( Win32FindPython3Service );

    public:
        Win32FindPython3Service();
        ~Win32FindPython3Service() override;

    public:
        bool _initializeService() override;
        void _finalizeService() override;

    protected:
        void getPython3ExecutablePath( Char * const _path ) const override;

    protected:
        Path m_python3ExecutablePath = {'\0'};
    };
}