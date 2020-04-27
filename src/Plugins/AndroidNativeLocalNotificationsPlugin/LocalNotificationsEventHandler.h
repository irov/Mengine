#pragma once

#include "Kernel/Mixin.h"
#include "Kernel/IntrusivePtr.h"

#include "Kernel/String.h"

namespace Mengine
{
    //////////////////////////////////////////////////////////////////////////
    enum EnumLocalNotificationsEventHandler
    {
        LOCAL_NOTIFICATIONS_INITIALIZE,
        LOCAL_NOTIFICATIONS_PRESS
    };
    //////////////////////////////////////////////////////////////////////////
    class LocalNotificationsEventHandler
        : public Mixin
    {
    public:
        virtual void onLocalNotificationsInitialized() = 0;

    public:
        virtual void onLocalNotificationsPress( int _id ) = 0;
    };
    //////////////////////////////////////////////////////////////////////////
    typedef IntrusivePtr<LocalNotificationsEventHandler> LocalNotificationsEventHandlerPtr;
    //////////////////////////////////////////////////////////////////////////
}
