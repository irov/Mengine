#pragma once

#include "Environment/Windows/WindowsIncluder.h"

#include "Kernel/ControllerCode.h"

#include <Xinput.h>

namespace Mengine
{
    //////////////////////////////////////////////////////////////////////////
    class Win32GameControllerInput
    {
    public:
        Win32GameControllerInput();
        ~Win32GameControllerInput();

    public:
        bool initialize();
        void finalize();
        void update();

    protected:
        typedef DWORD( WINAPI * PFNXInputGetState )(DWORD, XINPUT_STATE *);

        void updateController_( DWORD _userIndex, const XINPUT_STATE & _state );
        void disconnectController_( DWORD _userIndex );

    protected:
        struct ControllerState
        {
            bool connected;
            XINPUT_STATE state;
        };

        HMODULE m_library;
        PFNXInputGetState m_getState;
        ControllerState m_states[XUSER_MAX_COUNT];
    };
    //////////////////////////////////////////////////////////////////////////
}
