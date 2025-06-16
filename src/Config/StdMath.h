#pragma once

#include "Config/Config.h"

#include <cmath>

namespace Mengine
{
    namespace StdMath
    {
        MENGINE_CONSTEXPR const float constant_pi = 3.1415926535897932384626433832795f;
        MENGINE_CONSTEXPR const float constant_inv_pi = 0.31830988618379067153776752674503f;
        MENGINE_CONSTEXPR const float constant_inv_two_pi = 0.15915494309189533576888376337251f;
        MENGINE_CONSTEXPR const float constant_negative_pi = -3.1415926535897932384626433832795f;
        MENGINE_CONSTEXPR const float constant_zero_pi = 0.f;
        MENGINE_CONSTEXPR const float constant_fourth_pi = 0.78539816339744830961566084581988f;
        MENGINE_CONSTEXPR const float constant_half_pi = 1.5707963267948966192313216916398f;
        MENGINE_CONSTEXPR const float constant_one_and_a_half_pi = 4.7123889803846898576939650749193f;
        MENGINE_CONSTEXPR const float constant_two_pi = 6.283185307179586476925286766559f;
        MENGINE_CONSTEXPR const float constant_e = 2.71828182845904523536f;
        MENGINE_CONSTEXPR const float constant_sqrt2 = 1.4142135623730950488016887242097f;
        MENGINE_CONSTEXPR const float constant_deg2rad = 0.01745329251994329576923690768489f;
        MENGINE_CONSTEXPR const float constant_rad2deg = 57.295779513082320876798154814105f;
        MENGINE_CONSTEXPR const float constant_eps = 0.00001f;
        MENGINE_CONSTEXPR const float constant_eps2 = constant_eps * 10.f;
        MENGINE_CONSTEXPR const float constant_neps = -constant_eps;
        MENGINE_CONSTEXPR const float constant_one_eps = 1.f + constant_eps;
        MENGINE_CONSTEXPR const float constant_one_neps = 1.f - constant_eps;
        MENGINE_CONSTEXPR const float constant_log2 = 0.69314718055994530941723212145818f;
        MENGINE_CONSTEXPR const float constant_inv_log2 = 1.4426950408889634073599246810019f;
        MENGINE_CONSTEXPR const float constant_decimal_base = 10.f;
        MENGINE_CONSTEXPR const float constant_binary_base = 2.f;
        MENGINE_CONSTEXPR const float constant_half_progression[32] = {
            0.5f, 0.25f, 0.125f, 0.0625f, 0.03125f, 0.015625f, 0.0078125f, 0.00390625f,
            0.001953125f, 0.0009765625f, 0.00048828125f, 0.000244140625f, 0.0001220703125f,
            0.00006103515625f, 0.000030517578125f, 0.0000152587890625f, 0.00000762939453125f,
            0.000003814697265625f, 0.0000019073486328125f, 0.00000095367431640625f,
            0.000000476837158203125f, 0.0000002384185791015625f, 0.00000011920928955078125f,
            0.000000059604644775390625f, 0.0000000298023223876953125f, 0.00000001490116119384765625f,
            0.000000007450580596923828125f, 0.0000000037252902984619140625f, 0.00000000186264514923095703125f,
            0.000000000931322574615478515625f, 0.0000000004656612873077392578125f, 0.00000000023283064365386962890625f
        };

        using std::floorf;
        using std::acosf;
        using std::asinf;
        using std::atan2f;
        using std::atanf;
        using std::sqrtf;
        using std::powf;
        using std::cosf;
        using std::sinf;
        using std::tanf;
        using std::fabsf;
        using std::logf;
        using std::log10f;
        using std::roundf;
        using std::ceilf;
        using std::modff;
        using std::abs;
        using std::floor;
        using std::acos;
        using std::asin;
        using std::atan2;
        using std::atan;
        using std::sqrt;
        using std::pow;
        using std::cos;
        using std::sin;
        using std::tan;
        using std::fabs;
        using std::log;
        using std::log10;
        using std::round;
        using std::ceil;
    }
}

#ifndef MENGINE_DEG2RAD
#define MENGINE_DEG2RAD( DEG ) ((DEG) * mt::constant::deg2rad)
#endif

#ifndef MENGINE_RAD2DEG
#define MENGINE_RAD2DEG( RAD ) ((RAD) * mt::constant::rad2deg)
#endif