#pragma once

#include "kinefix/kinefix.h"

#include <cstdint>
#include <string_view>

namespace Arena3D
{
    using FixedAngle = kf_angle16_t;
    using FixedSignedAngle = kf_sangle16_t;

    enum class FaultCode : uint8_t
    {
        None,
        Overflow,
        DivisionByZero,
        InvalidSquareRoot,
        InvalidConfiguration,
        InvalidSnapshot
    };

    struct FaultState
    {
        FaultCode code = FaultCode::None;
        const char * operation = nullptr;

        void clear();
        void raise( FaultCode fault, const char * source );
        bool failed() const;
    };

    class FixedFaultScope
    {
    public:
        explicit FixedFaultScope( FaultState * state );
        ~FixedFaultScope();

        FixedFaultScope( const FixedFaultScope & ) = delete;
        FixedFaultScope & operator=( const FixedFaultScope & ) = delete;

    private:
        kf_fault_t * m_previous;
    };

    class Fixed
    {
    public:
        static constexpr int FractionBits = 32;
        static constexpr int64_t Scale = int64_t{1} << FractionBits;

        constexpr Fixed() = default;

        static constexpr Fixed fromRaw( int64_t raw )
        {
            Fixed value;
            value.m_raw = raw;
            return value;
        }

        static Fixed fromInt( int64_t value );
        static Fixed fromDecimal( std::string_view value );

        constexpr int64_t raw() const
        {
            return m_raw;
        }

        double toDouble() const;

        Fixed operator+() const;
        Fixed operator-() const;

        friend Fixed operator+( Fixed left, Fixed right );
        friend Fixed operator-( Fixed left, Fixed right );
        friend Fixed operator*( Fixed left, Fixed right );
        friend Fixed operator/( Fixed left, Fixed right );

        Fixed & operator+=( Fixed value );
        Fixed & operator-=( Fixed value );
        Fixed & operator*=( Fixed value );
        Fixed & operator/=( Fixed value );

        friend constexpr bool operator==( Fixed left, Fixed right ) { return left.m_raw == right.m_raw; }
        friend constexpr bool operator!=( Fixed left, Fixed right ) { return left.m_raw != right.m_raw; }
        friend constexpr bool operator<( Fixed left, Fixed right ) { return left.m_raw < right.m_raw; }
        friend constexpr bool operator<=( Fixed left, Fixed right ) { return left.m_raw <= right.m_raw; }
        friend constexpr bool operator>( Fixed left, Fixed right ) { return left.m_raw > right.m_raw; }
        friend constexpr bool operator>=( Fixed left, Fixed right ) { return left.m_raw >= right.m_raw; }

        static constexpr Fixed zero() { return fromRaw( 0 ); }
        static constexpr Fixed one() { return fromRaw( Scale ); }
        static constexpr Fixed pi() { return fromRaw( INT64_C(13493037704) ); }
        static constexpr Fixed halfPi() { return fromRaw( INT64_C(6746518852) ); }
        static constexpr Fixed twoPi() { return fromRaw( INT64_C(26986075409) ); }

    private:
        explicit constexpr Fixed( int64_t raw )
            : m_raw( raw )
        {
        }

    private:
        int64_t m_raw = 0;
    };

    struct FixedVec3
    {
        Fixed x;
        Fixed y;
        Fixed z;

        FixedVec3() = default;
        constexpr FixedVec3( Fixed x_, Fixed y_, Fixed z_ )
            : x( x_ )
            , y( y_ )
            , z( z_ )
        {
        }

        FixedVec3 & operator+=( const FixedVec3 & value );
        FixedVec3 & operator-=( const FixedVec3 & value );

        friend FixedVec3 operator+( FixedVec3 left, const FixedVec3 & right );
        friend FixedVec3 operator-( FixedVec3 left, const FixedVec3 & right );
        friend FixedVec3 operator*( const FixedVec3 & value, Fixed scalar );
        friend FixedVec3 operator/( const FixedVec3 & value, Fixed scalar );
        friend bool operator==( const FixedVec3 & left, const FixedVec3 & right );
    };

    Fixed fixedAbs( Fixed value );
    Fixed fixedMin( Fixed left, Fixed right );
    Fixed fixedMax( Fixed left, Fixed right );
    Fixed fixedClamp( Fixed value, Fixed minimum, Fixed maximum );
    Fixed fixedSqrt( Fixed value );
    Fixed fixedDot( const FixedVec3 & left, const FixedVec3 & right );
    FixedVec3 fixedCross( const FixedVec3 & left, const FixedVec3 & right );
    Fixed fixedLengthSquared( const FixedVec3 & value );
    Fixed fixedLength( const FixedVec3 & value );
    FixedVec3 fixedNormalize( const FixedVec3 & value );
    Fixed fixedWrapAngle( Fixed angle );
    void fixedSinCos( Fixed angle, Fixed * sine, Fixed * cosine );
    FixedAngle fixedAngleFromRadians( Fixed radians );
    FixedSignedAngle fixedSignedAngleFromRadians( Fixed radians );
    Fixed fixedAngleToRadians( FixedAngle angle );
    Fixed fixedSignedAngleToRadians( FixedSignedAngle angle );
    FixedAngle fixedAngleAdd( FixedAngle angle, FixedSignedAngle delta );
    FixedSignedAngle fixedSignedAngleAddClamped( FixedSignedAngle angle, FixedSignedAngle delta,
        FixedSignedAngle minimum, FixedSignedAngle maximum );
    void fixedAngleSinCos( FixedAngle angle, Fixed * sine, Fixed * cosine );
    void fixedSignedAngleSinCos( FixedSignedAngle angle, Fixed * sine, Fixed * cosine );

    inline kf_vec3_t toKinefix( const FixedVec3 & value )
    {
        return {value.x.raw(), value.y.raw(), value.z.raw()};
    }

    inline FixedVec3 fromKinefix( const kf_vec3_t & value )
    {
        return {Fixed::fromRaw( value.x ), Fixed::fromRaw( value.y ), Fixed::fromRaw( value.z )};
    }
}
