#pragma once

#include "Config/Config.h"

#ifndef MENGINE_UINT32_TO_POINTER
#if defined(MENGINE_ENVIRONMENT_ARCHITECTURE_X86)
#   define MENGINE_UINT32_TO_POINTER(T, O) ((T)(const uint32_t *)(uint32_t)(O))
#endif

#if defined(MENGINE_ENVIRONMENT_ARCHITECTURE_X64)
#   define MENGINE_UINT32_TO_POINTER(T, O) ((T)(const uint64_t *)(uint64_t)(O))
#endif
#endif
