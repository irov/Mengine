#pragma once

#include "Interface/NotificationServiceInterface.h"
#include "Interface/NotificatorInterface.h"

//////////////////////////////////////////////////////////////////////////
#define NOTIFICATION_ADDOBSERVERMETHOD( ID, Observer, Method, Doc )\
    NOTIFICATION_SERVICE()->addObserverMethod<ID>( Observer, Method, Doc )
//////////////////////////////////////////////////////////////////////////
#define NOTIFICATION_ADDOBSERVERMETHOD_THIS( ID, Method, Doc )\
    NOTIFICATION_SERVICE()->addObserverMethod<ID>( this, Method, Doc )
//////////////////////////////////////////////////////////////////////////
#define NOTIFICATION_ADDOBSERVERLAMBDA( ID, Observer, L, Doc )\
    NOTIFICATION_SERVICE()->addObserverLambda<ID>( Observer, L, Doc )
//////////////////////////////////////////////////////////////////////////
#define NOTIFICATION_ADDOBSERVERLAMBDA_THIS( ID, L, Doc )\
    NOTIFICATION_SERVICE()->addObserverLambda<ID>( this, L, Doc )
//////////////////////////////////////////////////////////////////////////
#define NOTIFICATION_REMOVEOBSERVER( ID, Observer )\
    NOTIFICATION_SERVICE()->removeObserver( ID, Observer )
//////////////////////////////////////////////////////////////////////////
#define NOTIFICATION_REMOVEOBSERVER_THIS( ID )\
    NOTIFICATION_SERVICE()->removeObserver( ID, this )
//////////////////////////////////////////////////////////////////////////
#define NOTIFICATION_NOTIFY( ID, ... )\
    NOTIFICATION_SERVICE()->notify<ID>( __VA_ARGS__ )
//////////////////////////////////////////////////////////////////////////