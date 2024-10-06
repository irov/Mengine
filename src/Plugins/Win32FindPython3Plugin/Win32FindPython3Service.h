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
        void getPython3ExecutablePathW( WChar * const _path ) const override;
        void getPython3LibraryPathW( WChar * const _path ) const override;
        void getPython3DllPathW( WChar * const _path ) const override;

        void getPython3ExecutablePathA( Char * const _path ) const override;
        void getPython3LibraryPathA( Char * const _path ) const override;
        void getPython3DllPathA( Char * const _path ) const override;

    protected:
        Char m_python3ExecutablePathA[MENGINE_MAX_PATH + 1] = {'\0'};
        Char m_python3LibraryPathA[MENGINE_MAX_PATH + 1] = {'\0'};
        Char m_python3DllPathA[MENGINE_MAX_PATH + 1] = {'\0'};

        WChar m_python3ExecutablePathW[MENGINE_MAX_PATH + 1] = {L'\0'};
        WChar m_python3LibraryPathW[MENGINE_MAX_PATH + 1] = {L'\0'};
        WChar m_python3DllPathW[MENGINE_MAX_PATH + 1] = {L'\0'};
    };
}