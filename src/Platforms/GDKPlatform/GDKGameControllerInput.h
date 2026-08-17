#pragma once

#include "Kernel/ControllerCode.h"
#include "Kernel/Vector.h"

#pragma warning(push, 0)
#include <GameInput.h>
#pragma warning(pop)

#include <mutex>

namespace Mengine
{
    //////////////////////////////////////////////////////////////////////////
    class GDKGameControllerInput
    {
    public:
        GDKGameControllerInput();
        ~GDKGameControllerInput();

    public:
        bool initialize();
        void finalize();
        void update();

    protected:
        static void CALLBACK deviceCallback_( GameInputCallbackToken _callbackToken, void * _context, IGameInputDevice * _device, uint64_t _timestamp, GameInputDeviceStatus _currentStatus, GameInputDeviceStatus _previousStatus );
        void handleDevice_( IGameInputDevice * _device, bool _connected );
        void updateDevice_( ControllerId _controllerId, IGameInputDevice * _device, GameInputGamepadState * const _previousState );

    protected:
        struct DeviceDesc
        {
            IGameInputDevice * device;
            ControllerId controllerId;
            bool connected;
            bool announced;
            GameInputGamepadState state;
        };

        IGameInput * m_gameInput;
        GameInputCallbackToken m_callbackToken;
        Vector<DeviceDesc> m_devices;
        ControllerId m_nextControllerId;
        std::mutex m_mutex;
    };
    //////////////////////////////////////////////////////////////////////////
}
