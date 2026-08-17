#include "Win32GameControllerInput.h"

#include "Interface/InputServiceInterface.h"

#include "Kernel/InputServiceHelper.h"
#include "Kernel/TimestampHelper.h"

#include "Config/StdString.h"

#ifndef MENGINE_WINDOWS_XINPUT_LIBRARIES
#   define MENGINE_WINDOWS_XINPUT_LIBRARIES L"xinput1_4.dll", L"xinput1_3.dll", L"xinput9_1_0.dll"
#endif
//////////////////////////////////////////////////////////////////////////

namespace Mengine
{
    namespace Detail
    {
        //////////////////////////////////////////////////////////////////////////
        static float normalizeThumb_( SHORT _value )
        {
            if( _value < 0 )
            {
                return (float)_value / 32768.f;
            }

            return (float)_value / 32767.f;
        }
        //////////////////////////////////////////////////////////////////////////
    }
    //////////////////////////////////////////////////////////////////////////
    Win32GameControllerInput::Win32GameControllerInput()
        : m_library( nullptr )
        , m_getState( nullptr )
    {
        for( ControllerState & state : m_states )
        {
            state.connected = false;
            StdString::memset( &state.state, 0, sizeof( state.state ) );
        }
    }
    //////////////////////////////////////////////////////////////////////////
    Win32GameControllerInput::~Win32GameControllerInput()
    {
    }
    //////////////////////////////////////////////////////////////////////////
    bool Win32GameControllerInput::initialize()
    {
        static const WChar * const libraries[] = {MENGINE_WINDOWS_XINPUT_LIBRARIES};

        for( const WChar * library : libraries )
        {
            m_library = ::LoadLibraryW( library );

            if( m_library != nullptr )
            {
                break;
            }
        }

        if( m_library == nullptr )
        {
            return true;
        }

        m_getState = (PFNXInputGetState)::GetProcAddress( m_library, "XInputGetState" );

        if( m_getState == nullptr )
        {
            ::FreeLibrary( m_library );
            m_library = nullptr;
        }

        return true;
    }
    //////////////////////////////////////////////////////////////////////////
    void Win32GameControllerInput::finalize()
    {
        for( DWORD userIndex = 0; userIndex != XUSER_MAX_COUNT; ++userIndex )
        {
            this->disconnectController_( userIndex );
        }

        m_getState = nullptr;

        if( m_library != nullptr )
        {
            ::FreeLibrary( m_library );
            m_library = nullptr;
        }
    }
    //////////////////////////////////////////////////////////////////////////
    void Win32GameControllerInput::update()
    {
        if( m_getState == nullptr || SERVICE_PROVIDER_EXIST() == false || SERVICE_IS_INITIALIZE( InputServiceInterface ) == false )
        {
            return;
        }

        for( DWORD userIndex = 0; userIndex != XUSER_MAX_COUNT; ++userIndex )
        {
            XINPUT_STATE state;
            StdString::memset( &state, 0, sizeof( state ) );

            DWORD result = m_getState( userIndex, &state );

            if( result != ERROR_SUCCESS )
            {
                this->disconnectController_( userIndex );
                continue;
            }

            this->updateController_( userIndex, state );
        }
    }
    //////////////////////////////////////////////////////////////////////////
    void Win32GameControllerInput::updateController_( DWORD _userIndex, const XINPUT_STATE & _state )
    {
        ControllerState & previous = m_states[_userIndex];
        ControllerId controllerId = _userIndex + 1;
        Timestamp timestamp = Helper::getSystemTimestamp();

        if( previous.connected == false )
        {
            previous.connected = true;
            Helper::pushControllerConnectEvent( timestamp, controllerId, true );
        }

        if( previous.state.dwPacketNumber == _state.dwPacketNumber )
        {
            return;
        }

        struct ButtonMapping
        {
            WORD mask;
            EControllerButton button;
        };

        static const ButtonMapping mappings[] = {
            {XINPUT_GAMEPAD_A, CB_A},
            {XINPUT_GAMEPAD_B, CB_B},
            {XINPUT_GAMEPAD_X, CB_X},
            {XINPUT_GAMEPAD_Y, CB_Y},
            {XINPUT_GAMEPAD_LEFT_SHOULDER, CB_LEFT_SHOULDER},
            {XINPUT_GAMEPAD_RIGHT_SHOULDER, CB_RIGHT_SHOULDER},
            {XINPUT_GAMEPAD_START, CB_MENU},
            {XINPUT_GAMEPAD_BACK, CB_OPTIONS},
            {XINPUT_GAMEPAD_LEFT_THUMB, CB_LEFT_THUMB},
            {XINPUT_GAMEPAD_RIGHT_THUMB, CB_RIGHT_THUMB},
            {XINPUT_GAMEPAD_DPAD_UP, CB_DPAD_UP},
            {XINPUT_GAMEPAD_DPAD_DOWN, CB_DPAD_DOWN},
            {XINPUT_GAMEPAD_DPAD_LEFT, CB_DPAD_LEFT},
            {XINPUT_GAMEPAD_DPAD_RIGHT, CB_DPAD_RIGHT}
        };

        for( const ButtonMapping & mapping : mappings )
        {
            bool wasDown = (previous.state.Gamepad.wButtons & mapping.mask) != 0;
            bool isDown = (_state.Gamepad.wButtons & mapping.mask) != 0;

            if( wasDown != isDown )
            {
                Helper::pushControllerButtonEvent( timestamp, controllerId, mapping.button, isDown == true ? 1.f : 0.f, isDown );
            }
        }

        const BYTE previousTriggers[] = {previous.state.Gamepad.bLeftTrigger, previous.state.Gamepad.bRightTrigger};
        const BYTE triggers[] = {_state.Gamepad.bLeftTrigger, _state.Gamepad.bRightTrigger};
        const EControllerButton triggerButtons[] = {CB_LEFT_TRIGGER, CB_RIGHT_TRIGGER};
        const EControllerAxis triggerAxes[] = {CA_LEFT_TRIGGER, CA_RIGHT_TRIGGER};

        for( uint32_t index = 0; index != 2; ++index )
        {
            if( previousTriggers[index] == triggers[index] )
            {
                continue;
            }

            float value = (float)triggers[index] / 255.f;
            bool wasDown = previousTriggers[index] > XINPUT_GAMEPAD_TRIGGER_THRESHOLD;
            bool isDown = triggers[index] > XINPUT_GAMEPAD_TRIGGER_THRESHOLD;

            Helper::pushControllerAxisEvent( timestamp, controllerId, triggerAxes[index], value );

            if( wasDown != isDown )
            {
                Helper::pushControllerButtonEvent( timestamp, controllerId, triggerButtons[index], value, isDown );
            }
        }

        const SHORT previousThumbs[] = {previous.state.Gamepad.sThumbLX, previous.state.Gamepad.sThumbLY, previous.state.Gamepad.sThumbRX, previous.state.Gamepad.sThumbRY};
        const SHORT thumbs[] = {_state.Gamepad.sThumbLX, _state.Gamepad.sThumbLY, _state.Gamepad.sThumbRX, _state.Gamepad.sThumbRY};
        const EControllerAxis thumbAxes[] = {CA_LEFT_X, CA_LEFT_Y, CA_RIGHT_X, CA_RIGHT_Y};

        for( uint32_t index = 0; index != 4; ++index )
        {
            if( previousThumbs[index] != thumbs[index] )
            {
                Helper::pushControllerAxisEvent( timestamp, controllerId, thumbAxes[index], Detail::normalizeThumb_( thumbs[index] ) );
            }
        }

        previous.state = _state;
    }
    //////////////////////////////////////////////////////////////////////////
    void Win32GameControllerInput::disconnectController_( DWORD _userIndex )
    {
        ControllerState & state = m_states[_userIndex];

        if( state.connected == false )
        {
            return;
        }

        state.connected = false;
        StdString::memset( &state.state, 0, sizeof( state.state ) );

        if( SERVICE_PROVIDER_EXIST() == true && SERVICE_IS_INITIALIZE( InputServiceInterface ) == true )
        {
            Helper::pushControllerConnectEvent( Helper::getSystemTimestamp(), _userIndex + 1, false );
        }
    }
    //////////////////////////////////////////////////////////////////////////
}
