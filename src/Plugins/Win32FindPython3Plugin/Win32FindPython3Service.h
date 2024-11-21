#pragma once

#include "Win32FindPython3Interface.h"

#include "Kernel/ServiceBase.h"

#include "Config/Path.h"

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
        void getPython3ExecutablePathW( WChar * const _path ) const override;
        void getPython3LibraryPathW( WChar * const _path ) const override;
        void getPython3DllPathW( WChar * const _path ) const override;

        void getPython3ExecutablePathA( Char * const _path ) const override;
        void getPython3LibraryPathA( Char * const _path ) const override;
        void getPython3DllPathA( Char * const _path ) const override;

    protected:
        Path m_python3ExecutablePathA = {'\0'};
        Path m_python3LibraryPathA = {'\0'};
        Path m_python3DllPathA = {'\0'};

        WPath m_python3ExecutablePathW = {L'\0'};
        WPath m_python3LibraryPathW = {L'\0'};
        WPath m_python3DllPathW = {L'\0'};
    };
}