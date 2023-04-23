#pragma once

#include "Interface/AnalyticsServiceInterface.h"

#define ANALYTICS_EVENT(Event) ANALYTICS_SERVICE()->buildEvent(STRINGIZE_STRING_LOCAL_I(Event))