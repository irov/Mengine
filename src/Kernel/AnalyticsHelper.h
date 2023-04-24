#pragma once

#include "Interface/AnalyticsServiceInterface.h"

#define ANALYTICS_EVENT(Event) ANALYTICS_SERVICE()->buildEvent(STRINGIZE_STRING_LOCAL_I(Event))
#define ANALYTICS_PARAMETER_BOOLEAN(Name, Value) ->addParameterBoolean(STRINGIZE_STRING_LOCAL_I(Name), (Value))
#define ANALYTICS_PARAMETER_INTEGER(Name, Value) ->addParameterInteger(STRINGIZE_STRING_LOCAL_I(Name), (Value))
#define ANALYTICS_PARAMETER_DOUBLE(Name, Value) ->addParameterDouble(STRINGIZE_STRING_LOCAL_I(Name), (Value))
#define ANALYTICS_PARAMETER_STRING(Name, Value) ->addParameterString(STRINGIZE_STRING_LOCAL_I(Name), (Value))
#define ANALYTICS_PARAMETER_CONSTSTRING(Name, Value) ->addParameterConstString(STRINGIZE_STRING_LOCAL_I(Name), (Value))
#define ANALYTICS_LOG() ->log()
