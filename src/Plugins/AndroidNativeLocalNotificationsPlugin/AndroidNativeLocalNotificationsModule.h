#pragma once

#include "LocalNotificationsEventHandler.h"

#include "Environment/Android/AndroidEventation.h"

#include "Kernel/ModuleBase.h"

#include "Config/Lambda.h"

namespace Mengine
{
    //////////////////////////////////////////////////////////////////////////
    class AndroidNativeLocalNotificationsModule
        : public ModuleBase
    {
        DECLARE_FACTORABLE( AndroidNativeLocalNotificationsModule );

    public:
        typedef AndroidEventation<LocalNotificationsEventHandler> LocalNotificationsEventation;
        typedef typename LocalNotificationsEventation::LambdaEventHandler LambdaLocalNotificationsEventHandler;

    public:
        AndroidNativeLocalNotificationsModule();
        ~AndroidNativeLocalNotificationsModule() override;

    protected:
        bool _initializeModule() override;
        void _finalizeModule() override;

    protected:
        void _update( bool _focus ) override;

    public:
        void addCommand( const LambdaLocalNotificationsEventHandler & _command );

    public:
        void setEventHandler( const LocalNotificationsEventHandlerPtr & _handler );

    public:
        bool initializeLocalNotifications();

    public:
        bool scheduleLocalNotification( int _id, const String & _title, const String & _content, int _delay );
        bool instantlyPresentLocalNotification( int _id, const String & _title, const String & _content );
        bool cancelAllLocalNotification();

    protected:
        LocalNotificationsEventation m_eventation;
    };
}
