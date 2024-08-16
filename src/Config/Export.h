#pragma once

#include "Config/Config.h"

//////////////////////////////////////////////////////////////////////////
#ifndef MENGINE_EXPORT_API
#   if defined(MENGINE_TOOLCHAIN_MSVC)
#       define MENGINE_EXPORT_API __declspec(dllexport)
#   else
#       define MENGINE_EXPORT_API
#   endif
#endif
//////////////////////////////////////////////////////////////////////////