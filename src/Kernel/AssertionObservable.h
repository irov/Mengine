#pragma once

#include "Interface/NotificationServiceInterface.h"

#include "Kernel/Assertion.h"

//////////////////////////////////////////////////////////////////////////
#if MENGINE_ASSERTION_DEBUG
#   define MENGINE_ASSERTION_OBSERVABLE(Ptr, ...) if( NOTIFICATION_SERVICE()->hasObserver(Ptr) == true) Mengine::Helper::AssertionOperator( Mengine::ASSERTION_LEVEL_FATAL, "observer", MENGINE_CODE_FUNCTION, MENGINE_CODE_LINE ) (__VA_ARGS__)
#else
#   define MENGINE_ASSERTION_OBSERVABLE(Ptr, ...)
#endif
//////////////////////////////////////////////////////////////////////////
