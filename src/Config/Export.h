#pragma once

#include "Config/Config.h"

//////////////////////////////////////////////////////////////////////////
#ifndef MENGINE_EXPORT_API
#   if defined(MENGINE_TOOLCHAIN_MSVC)
#       define MENGINE_EXPORT_API __declspec(dllexport)
#       define MENGINE_IMPORT_API __declspec(dllimport)
#   else
#       define MENGINE_EXPORT_API
#       define MENGINE_IMPORT_API
#   endif
#endif
//////////////////////////////////////////////////////////////////////////