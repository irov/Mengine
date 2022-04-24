#pragma once

#include "Config/Config.h"

#include <math.h>

namespace Mengine
{
    namespace Constant
    {
        MENGINE_CONSTEXPR const float pi = 3.1415926535897932384626433832795f;
        MENGINE_CONSTEXPR const float inv_pi = 0.31830988618379067153776752674503f;
        MENGINE_CONSTEXPR const float inv_two_pi = 0.15915494309189533576888376337251f;
        MENGINE_CONSTEXPR const float negative_pi = -3.1415926535897932384626433832795f;
        MENGINE_CONSTEXPR const float zero_pi = 0.f;
        MENGINE_CONSTEXPR const float fourth_pi = 0.78539816339744830961566084581988f;
        MENGINE_CONSTEXPR const float half_pi = 1.5707963267948966192313216916398f;
        MENGINE_CONSTEXPR const float one_and_a_half_pi = 4.7123889803846898576939650749193f;
        MENGINE_CONSTEXPR const float two_pi = 6.283185307179586476925286766559f;
        MENGINE_CONSTEXPR const float e = 2.71828182845904523536f;
        MENGINE_CONSTEXPR const float sqrt2 = 1.4142135623730950488016887242097f;
        MENGINE_CONSTEXPR const float deg2rad = 0.01745329251994329576923690768489f;
        MENGINE_CONSTEXPR const float rad2deg = 57.295779513082320876798154814105f;
        MENGINE_CONSTEXPR const float eps = 0.00001f;
        MENGINE_CONSTEXPR const float eps2 = eps * 10.f;
        MENGINE_CONSTEXPR const float neps = -eps;
        MENGINE_CONSTEXPR const float one_eps = 1.f + eps;
        MENGINE_CONSTEXPR const float one_neps = 1.f - eps;
        MENGINE_CONSTEXPR const float log2 = 0.69314718055994530941723212145818f;
        MENGINE_CONSTEXPR const float inv_log2 = 1.4426950408889634073599246810019f;
        MENGINE_CONSTEXPR const float decimal_base = 10.f;
        MENGINE_CONSTEXPR const float binary_base = 2.f;
    }
}

#ifndef MENGINE_FLOORF
#define MENGINE_FLOORF (::floorf)
#endif

#ifndef MENGINE_ACOSF
#define MENGINE_ACOSF (::acosf)
#endif

#ifndef MENGINE_ASINF
#define MENGINE_ASINF (::asinf)
#endif

#ifndef MENGINE_ATAN2F
#define MENGINE_ATAN2F (::atan2f)
#endif

#ifndef MENGINE_ATANF
#define MENGINE_ATANF (::atanf)
#endif

#ifndef MENGINE_SQRTF
#define MENGINE_SQRTF (::sqrtf)
#endif

#ifndef MENGINE_POWF
#define MENGINE_POWF (::powf)
#endif

#ifndef MENGINE_COSF
#define MENGINE_COSF (::cosf)
#endif

#ifndef MENGINE_SINF
#define MENGINE_SINF (::sinf)
#endif

#ifndef MENGINE_TANF
#define MENGINE_TANF (::tanf)
#endif

#ifndef MENGINE_FABSF
#define MENGINE_FABSF (::fabsf)
#endif

#ifndef MENGINE_LOGF
#define MENGINE_LOGF (::logf)
#endif

#ifndef MENGINE_LOG10F
#define MENGINE_LOG10F (::log10f)
#endif

#ifndef MENGINE_ROUNDF
#define MENGINE_ROUNDF (::roundf)
#endif

#ifndef MENGINE_CEILF
#define MENGINE_CEILF (::ceilf)
#endif

#ifndef MENGINE_ABS
#define MENGINE_ABS (::abs)
#endif

#ifndef MENGINE_FLOOR
#define MENGINE_FLOOR (::floor)
#endif

#ifndef MENGINE_ACOS
#define MENGINE_ACOS (::acos)
#endif

#ifndef MENGINE_ASIN
#define MENGINE_ASIN (::asin)
#endif

#ifndef MENGINE_ATAN2
#define MENGINE_ATAN2 (::atan2)
#endif

#ifndef MENGINE_ATAN
#define MENGINE_ATAN (::atan)
#endif

#ifndef MENGINE_SQRT
#define MENGINE_SQRT (::sqrt)
#endif

#ifndef MENGINE_POW
#define MENGINE_POW (::pow)
#endif

#ifndef MENGINE_COS
#define MENGINE_COS (::cos)
#endif

#ifndef MENGINE_SIN
#define MENGINE_SIN (::sin)
#endif

#ifndef MENGINE_TAN
#define MENGINE_TAN (::tan)
#endif

#ifndef MENGINE_FABS
#define MENGINE_FABS (::fabs)
#endif

#ifndef MENGINE_LOG
#define MENGINE_LOG (::log)
#endif

#ifndef MENGINE_LOG10
#define MENGINE_LOG10 (::log10)
#endif

#ifndef MENGINE_ROUND
#define MENGINE_ROUND (::round)
#endif

#ifndef MENGINE_CEIL
#define MENGINE_CEIL (::ceil)
#endif

#ifndef MENGINE_DEG2RAD
#define MENGINE_DEG2RAD( DEG ) ((DEG) * mt::constant::deg2rad)
#endif

#ifndef MENGINE_RAD2DEG
#define MENGINE_RAD2DEG( RAD ) ((RAD) * mt::constant::rad2deg)
#endif