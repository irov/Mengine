#include "Arena3DFixed.h"

#include <cstddef>
#include <type_traits>

namespace Arena3D
{
    namespace
    {
        kf_fault_t * nativeFault( FaultState * state )
        {
            static_assert( std::is_standard_layout_v<FaultState> );
            static_assert( sizeof(FaultState) == sizeof(kf_fault_t) );
            static_assert( alignof(FaultState) == alignof(kf_fault_t) );
            static_assert( offsetof(FaultState, code) == offsetof(kf_fault_t, code) );
            static_assert( offsetof(FaultState, operation) == offsetof(kf_fault_t, operation) );
            static_assert( static_cast<uint8_t>(FaultCode::None) == KF_FAULT_NONE );
            static_assert( static_cast<uint8_t>(FaultCode::Overflow) == KF_FAULT_OVERFLOW );
            static_assert( static_cast<uint8_t>(FaultCode::DivisionByZero) == KF_FAULT_DIVISION_BY_ZERO );
            static_assert( static_cast<uint8_t>(FaultCode::InvalidSquareRoot) == KF_FAULT_INVALID_SQUARE_ROOT );
            static_assert( static_cast<uint8_t>(FaultCode::InvalidConfiguration) == KF_FAULT_INVALID_CONFIGURATION );
            static_assert( static_cast<uint8_t>(FaultCode::InvalidSnapshot) == KF_FAULT_INVALID_SNAPSHOT );
            return reinterpret_cast<kf_fault_t *>(state);
        }
    }

    void FaultState::clear()
    {
        kf_fault_clear( nativeFault( this ) );
    }

    void FaultState::raise( FaultCode fault, const char * source )
    {
        kf_fault_raise( nativeFault( this ), static_cast<kf_fault_code_t>(fault), source );
    }

    bool FaultState::failed() const
    {
        return kf_fault_failed( reinterpret_cast<const kf_fault_t *>(this) ) != 0;
    }

    FixedFaultScope::FixedFaultScope( FaultState * state )
        : m_previous( kf_fault_bind( nativeFault( state ) ) )
    {
    }

    FixedFaultScope::~FixedFaultScope()
    {
        kf_fault_restore( m_previous );
    }

    Fixed Fixed::fromInt( int64_t value ) { return fromRaw( kf_fixed_from_int( value ) ); }
    Fixed Fixed::fromDecimal( std::string_view value ) { return fromRaw( kf_fixed_from_decimal_n( value.data(), value.size() ) ); }
    double Fixed::toDouble() const { return kf_fixed_to_double( m_raw ); }
    Fixed Fixed::operator+() const { return *this; }
    Fixed Fixed::operator-() const { return fromRaw( kf_fixed_neg( m_raw ) ); }
    Fixed operator+( Fixed left, Fixed right ) { return Fixed::fromRaw( kf_fixed_add( left.m_raw, right.m_raw ) ); }
    Fixed operator-( Fixed left, Fixed right ) { return Fixed::fromRaw( kf_fixed_sub( left.m_raw, right.m_raw ) ); }
    Fixed operator*( Fixed left, Fixed right ) { return Fixed::fromRaw( kf_fixed_mul( left.m_raw, right.m_raw ) ); }
    Fixed operator/( Fixed left, Fixed right ) { return Fixed::fromRaw( kf_fixed_div( left.m_raw, right.m_raw ) ); }
    Fixed & Fixed::operator+=( Fixed value ) { *this = *this + value; return *this; }
    Fixed & Fixed::operator-=( Fixed value ) { *this = *this - value; return *this; }
    Fixed & Fixed::operator*=( Fixed value ) { *this = *this * value; return *this; }
    Fixed & Fixed::operator/=( Fixed value ) { *this = *this / value; return *this; }

    FixedVec3 & FixedVec3::operator+=( const FixedVec3 & value ) { *this = fromKinefix( kf_vec3_add( toKinefix( *this ), toKinefix( value ) ) ); return *this; }
    FixedVec3 & FixedVec3::operator-=( const FixedVec3 & value ) { *this = fromKinefix( kf_vec3_sub( toKinefix( *this ), toKinefix( value ) ) ); return *this; }
    FixedVec3 operator+( FixedVec3 left, const FixedVec3 & right ) { return fromKinefix( kf_vec3_add( toKinefix( left ), toKinefix( right ) ) ); }
    FixedVec3 operator-( FixedVec3 left, const FixedVec3 & right ) { return fromKinefix( kf_vec3_sub( toKinefix( left ), toKinefix( right ) ) ); }
    FixedVec3 operator*( const FixedVec3 & value, Fixed scalar ) { return fromKinefix( kf_vec3_mul( toKinefix( value ), scalar.raw() ) ); }
    FixedVec3 operator/( const FixedVec3 & value, Fixed scalar ) { return fromKinefix( kf_vec3_div( toKinefix( value ), scalar.raw() ) ); }
    bool operator==( const FixedVec3 & left, const FixedVec3 & right ) { return left.x == right.x && left.y == right.y && left.z == right.z; }

    Fixed fixedAbs( Fixed value ) { return Fixed::fromRaw( kf_fixed_abs( value.raw() ) ); }
    Fixed fixedMin( Fixed left, Fixed right ) { return Fixed::fromRaw( kf_fixed_min( left.raw(), right.raw() ) ); }
    Fixed fixedMax( Fixed left, Fixed right ) { return Fixed::fromRaw( kf_fixed_max( left.raw(), right.raw() ) ); }
    Fixed fixedClamp( Fixed value, Fixed minimum, Fixed maximum ) { return Fixed::fromRaw( kf_fixed_clamp( value.raw(), minimum.raw(), maximum.raw() ) ); }
    Fixed fixedSqrt( Fixed value ) { return Fixed::fromRaw( kf_fixed_sqrt( value.raw() ) ); }
    Fixed fixedDot( const FixedVec3 & left, const FixedVec3 & right ) { return Fixed::fromRaw( kf_vec3_dot( toKinefix( left ), toKinefix( right ) ) ); }
    FixedVec3 fixedCross( const FixedVec3 & left, const FixedVec3 & right ) { return fromKinefix( kf_vec3_cross( toKinefix( left ), toKinefix( right ) ) ); }
    Fixed fixedLengthSquared( const FixedVec3 & value ) { return Fixed::fromRaw( kf_vec3_length_squared( toKinefix( value ) ) ); }
    Fixed fixedLength( const FixedVec3 & value ) { return Fixed::fromRaw( kf_vec3_length( toKinefix( value ) ) ); }
    FixedVec3 fixedNormalize( const FixedVec3 & value ) { return fromKinefix( kf_vec3_normalize( toKinefix( value ) ) ); }
    Fixed fixedWrapAngle( Fixed angle ) { return Fixed::fromRaw( kf_fixed_wrap_angle( angle.raw() ) ); }

    void fixedSinCos( Fixed angle, Fixed * sine, Fixed * cosine )
    {
        kf_fixed_t nativeSine = 0;
        kf_fixed_t nativeCosine = 0;
        kf_fixed_sin_cos( angle.raw(), &nativeSine, &nativeCosine );
        *sine = Fixed::fromRaw( nativeSine );
        *cosine = Fixed::fromRaw( nativeCosine );
    }

    FixedAngle fixedAngleFromRadians( Fixed radians ) { return kf_angle16_from_fixed_radians( radians.raw() ); }
    FixedSignedAngle fixedSignedAngleFromRadians( Fixed radians ) { return kf_sangle16_from_fixed_radians( radians.raw() ); }
    Fixed fixedAngleToRadians( FixedAngle angle ) { return Fixed::fromRaw( kf_angle16_to_fixed_radians( angle ) ); }
    Fixed fixedSignedAngleToRadians( FixedSignedAngle angle ) { return Fixed::fromRaw( kf_sangle16_to_fixed_radians( angle ) ); }
    FixedAngle fixedAngleAdd( FixedAngle angle, FixedSignedAngle delta ) { return kf_angle16_add( angle, delta ); }
    FixedSignedAngle fixedSignedAngleAddClamped( FixedSignedAngle angle, FixedSignedAngle delta,
        FixedSignedAngle minimum, FixedSignedAngle maximum )
    {
        return kf_sangle16_add_clamped( angle, delta, minimum, maximum );
    }

    void fixedAngleSinCos( FixedAngle angle, Fixed * sine, Fixed * cosine )
    {
        kf_fixed_t nativeSine = 0;
        kf_fixed_t nativeCosine = 0;
        kf_angle16_sin_cos( angle, &nativeSine, &nativeCosine );
        *sine = Fixed::fromRaw( nativeSine );
        *cosine = Fixed::fromRaw( nativeCosine );
    }

    void fixedSignedAngleSinCos( FixedSignedAngle angle, Fixed * sine, Fixed * cosine )
    {
        kf_fixed_t nativeSine = 0;
        kf_fixed_t nativeCosine = 0;
        kf_sangle16_sin_cos( angle, &nativeSine, &nativeCosine );
        *sine = Fixed::fromRaw( nativeSine );
        *cosine = Fixed::fromRaw( nativeCosine );
    }
}
