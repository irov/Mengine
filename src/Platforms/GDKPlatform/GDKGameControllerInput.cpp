#include "GDKGameControllerInput.h"

#include "Interface/InputServiceInterface.h"

#include "Kernel/InputServiceHelper.h"
#include "Kernel/Logger.h"
#include "Kernel/TimestampHelper.h"

#include "Config/StdAlgorithm.h"
#include "Config/StdString.h"

namespace Mengine
{
    //////////////////////////////////////////////////////////////////////////
    GDKGameControllerInput::GDKGameControllerInput()
        : m_gameInput( nullptr )
        , m_callbackToken( 0 )
        , m_nextControllerId( 1 )
    {
    }
    //////////////////////////////////////////////////////////////////////////
    GDKGameControllerInput::~GDKGameControllerInput()
    {
    }
    //////////////////////////////////////////////////////////////////////////
    bool GDKGameControllerInput::initialize()
    {
        HRESULT hr = ::GameInputCreate( &m_gameInput );

        if( FAILED( hr ) )
        {
            LOGGER_ERROR( "failed GameInputCreate [0x%08lX]"
                , hr
            );

            m_gameInput = nullptr;

            return false;
        }

        hr = m_gameInput->RegisterDeviceCallback( nullptr
            , GameInputKindGamepad
            , GameInputDeviceConnected
            , GameInputBlockingEnumeration
            , this
            , &GDKGameControllerInput::deviceCallback_
            , &m_callbackToken );

        if( FAILED( hr ) )
        {
            LOGGER_ERROR( "failed GameInput RegisterDeviceCallback [0x%08lX]"
                , hr
            );

            m_gameInput->Release();
            m_gameInput = nullptr;
            m_callbackToken = 0;

            return false;
        }

        return true;
    }
    //////////////////////////////////////////////////////////////////////////
    void GDKGameControllerInput::finalize()
    {
        if( m_gameInput != nullptr && m_callbackToken != 0 )
        {
            m_gameInput->UnregisterCallback( m_callbackToken, 5000 );
            m_callbackToken = 0;
        }

        std::lock_guard<std::mutex> guard( m_mutex );

        for( DeviceDesc & desc : m_devices )
        {
            if( desc.announced == true && SERVICE_PROVIDER_EXIST() == true && SERVICE_IS_INITIALIZE( InputServiceInterface ) == true )
            {
                Helper::pushControllerConnectEvent( Helper::getSystemTimestamp(), desc.controllerId, false );
            }

            desc.device->Release();
        }

        m_devices.clear();

        if( m_gameInput != nullptr )
        {
            m_gameInput->Release();
            m_gameInput = nullptr;
        }
    }
    //////////////////////////////////////////////////////////////////////////
    void GDKGameControllerInput::update()
    {
        std::lock_guard<std::mutex> guard( m_mutex );

        for( Vector<DeviceDesc>::iterator it = m_devices.begin(); it != m_devices.end(); )
        {
            if( it->connected == false )
            {
                if( it->announced == true )
                {
                    Helper::pushControllerConnectEvent( Helper::getSystemTimestamp(), it->controllerId, false );
                }

                it->device->Release();
                it = m_devices.erase( it );
                continue;
            }

            if( it->announced == false )
            {
                Helper::pushControllerConnectEvent( Helper::getSystemTimestamp(), it->controllerId, true );
                it->announced = true;
            }

            this->updateDevice_( it->controllerId, it->device, &it->state );
            ++it;
        }
    }
    //////////////////////////////////////////////////////////////////////////
    void CALLBACK GDKGameControllerInput::deviceCallback_( GameInputCallbackToken _callbackToken, void * _context, IGameInputDevice * _device, uint64_t _timestamp, GameInputDeviceStatus _currentStatus, GameInputDeviceStatus _previousStatus )
    {
        MENGINE_UNUSED( _callbackToken );
        MENGINE_UNUSED( _timestamp );
        MENGINE_UNUSED( _previousStatus );

        GDKGameControllerInput * self = static_cast<GDKGameControllerInput *>(_context);
        bool connected = (_currentStatus & GameInputDeviceConnected) != 0;

        self->handleDevice_( _device, connected );
    }
    //////////////////////////////////////////////////////////////////////////
    void GDKGameControllerInput::handleDevice_( IGameInputDevice * _device, bool _connected )
    {
        std::lock_guard<std::mutex> guard( m_mutex );

        Vector<DeviceDesc>::iterator it_found = StdAlgorithm::find_if( m_devices.begin(), m_devices.end(), [_device]( const DeviceDesc & _desc )
        {
            return _desc.device == _device;
        } );

        if( it_found != m_devices.end() )
        {
            it_found->connected = _connected;
            return;
        }

        if( _connected == false )
        {
            return;
        }

        DeviceDesc desc;
        desc.device = _device;
        desc.device->AddRef();
        desc.controllerId = m_nextControllerId++;
        desc.connected = true;
        desc.announced = false;
        StdString::memset( &desc.state, 0, sizeof( desc.state ) );

        m_devices.emplace_back( desc );
    }
    //////////////////////////////////////////////////////////////////////////
    void GDKGameControllerInput::updateDevice_( ControllerId _controllerId, IGameInputDevice * _device, GameInputGamepadState * const _previousState )
    {
        IGameInputReading * reading = nullptr;
        HRESULT hr = m_gameInput->GetCurrentReading( GameInputKindGamepad, _device, &reading );

        if( FAILED( hr ) || reading == nullptr )
        {
            return;
        }

        GameInputGamepadState state;
        bool successful = reading->GetGamepadState( &state );
        reading->Release();

        if( successful == false )
        {
            return;
        }

        Timestamp timestamp = Helper::getSystemTimestamp();

        struct ButtonMapping
        {
            GameInputGamepadButtons mask;
            EControllerButton button;
        };

        static const ButtonMapping mappings[] = {
            {GameInputGamepadA, CB_A},
            {GameInputGamepadB, CB_B},
            {GameInputGamepadX, CB_X},
            {GameInputGamepadY, CB_Y},
            {GameInputGamepadLeftShoulder, CB_LEFT_SHOULDER},
            {GameInputGamepadRightShoulder, CB_RIGHT_SHOULDER},
            {GameInputGamepadMenu, CB_MENU},
            {GameInputGamepadView, CB_OPTIONS},
            {GameInputGamepadLeftThumbstick, CB_LEFT_THUMB},
            {GameInputGamepadRightThumbstick, CB_RIGHT_THUMB},
            {GameInputGamepadDPadUp, CB_DPAD_UP},
            {GameInputGamepadDPadDown, CB_DPAD_DOWN},
            {GameInputGamepadDPadLeft, CB_DPAD_LEFT},
            {GameInputGamepadDPadRight, CB_DPAD_RIGHT}
        };

        for( const ButtonMapping & mapping : mappings )
        {
            bool wasDown = (_previousState->buttons & mapping.mask) != 0;
            bool isDown = (state.buttons & mapping.mask) != 0;

            if( wasDown != isDown )
            {
                Helper::pushControllerButtonEvent( timestamp, _controllerId, mapping.button, isDown == true ? 1.f : 0.f, isDown );
            }
        }

        const float previousTriggers[] = {_previousState->leftTrigger, _previousState->rightTrigger};
        const float triggers[] = {state.leftTrigger, state.rightTrigger};
        const EControllerButton triggerButtons[] = {CB_LEFT_TRIGGER, CB_RIGHT_TRIGGER};
        const EControllerAxis triggerAxes[] = {CA_LEFT_TRIGGER, CA_RIGHT_TRIGGER};

        for( uint32_t index = 0; index != 2; ++index )
        {
            if( previousTriggers[index] == triggers[index] )
            {
                continue;
            }

            bool wasDown = previousTriggers[index] > 0.15f;
            bool isDown = triggers[index] > 0.15f;

            Helper::pushControllerAxisEvent( timestamp, _controllerId, triggerAxes[index], triggers[index] );

            if( wasDown != isDown )
            {
                Helper::pushControllerButtonEvent( timestamp, _controllerId, triggerButtons[index], triggers[index], isDown );
            }
        }

        const float previousAxes[] = {_previousState->leftThumbstickX, _previousState->leftThumbstickY, _previousState->rightThumbstickX, _previousState->rightThumbstickY};
        const float axes[] = {state.leftThumbstickX, state.leftThumbstickY, state.rightThumbstickX, state.rightThumbstickY};
        const EControllerAxis axisTypes[] = {CA_LEFT_X, CA_LEFT_Y, CA_RIGHT_X, CA_RIGHT_Y};

        for( uint32_t index = 0; index != 4; ++index )
        {
            if( previousAxes[index] != axes[index] )
            {
                Helper::pushControllerAxisEvent( timestamp, _controllerId, axisTypes[index], axes[index] );
            }
        }

        *_previousState = state;
    }
    //////////////////////////////////////////////////////////////////////////
}
