#pragma once

#include "Environment/Android/AndroidIncluder.h"

#define MENGINE_JAVA_CONCAT2(prefix, class, function) Java_##prefix##_##class##_## function
#define MENGINE_JAVA_FUNCTION_INTERFACE(prefix, class, function) MENGINE_JAVA_CONCAT2(prefix, class, function)

#define MENGINE_JAVA_PREFIX org_Mengine_Base
#define MENGINE_JAVA_INTERFACE(function) MENGINE_JAVA_FUNCTION_INTERFACE(MENGINE_JAVA_PREFIX, MengineNative, function)
