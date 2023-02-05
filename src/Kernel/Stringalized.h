#pragma once

#include "Kernel/ConstString.h"
#include "Kernel/FilePath.h"
#include "Kernel/Tags.h"
#include "Kernel/Resolution.h"
#include "Kernel/Color.h"
#include "Kernel/AspectRatioViewport.h"
#include "Kernel/String.h"

#include "Config/Char.h"
#include "Config/StdInt.h"

#include "math/vec2.h"
#include "math/vec3.h"
#include "math/vec4.h"

namespace Mengine
{
    namespace Helper
    {
        //////////////////////////////////////////////////////////////////////////
        bool stringalized( const Char * _string, bool * const _value );
        bool stringalized( const Char * _string, int8_t * const _value );
        bool stringalized( const Char * _string, uint8_t * const _value );
        bool stringalized( const Char * _string, int16_t * const _value );
        bool stringalized( const Char * _string, uint16_t * const _value );
        bool stringalized( const Char * _string, int32_t * const _value );
        bool stringalized( const Char * _string, uint32_t * const _value );
        bool stringalized( const Char * _string, int64_t * const _value );
        bool stringalized( const Char * _string, uint64_t * const _value );
        bool stringalized( const Char * _string, float * const _value );
        bool stringalized( const Char * _string, double * const _value );
        bool stringalized( const Char * _string, mt::vec2f * const _value );
        bool stringalized( const Char * _string, mt::vec3f * const _value );
        bool stringalized( const Char * _string, mt::vec4f * const _value );
        bool stringalized( const Char * _string, const Char ** const _value );
        bool stringalized( const Char * _string, String * const _value );
        bool stringalized( const Char * _string, ConstString * const _value );
        bool stringalized( const Char * _string, FilePath * const _value );
        bool stringalized( const Char * _string, Tags * const _value );
        bool stringalized( const Char * _string, Resolution * const _value );
        bool stringalized( const Char * _string, Color * const _value );
        bool stringalized( const Char * _string, AspectRatioViewport * const _value );
        //////////////////////////////////////////////////////////////////////////
        bool stringalized( bool _value, Char * const _string, size_t _capacity );
        bool stringalized( int8_t _value, Char * const _string, size_t _capacity );
        bool stringalized( uint8_t _value, Char * const _string, size_t _capacity );
        bool stringalized( int16_t _value, Char * const _string, size_t _capacity );
        bool stringalized( uint16_t _value, Char * const _string, size_t _capacity );
        bool stringalized( int32_t _value, Char * const _string, size_t _capacity );
        bool stringalized( uint32_t _value, Char * const _string, size_t _capacity );
        bool stringalized( int64_t _value, Char * const _string, size_t _capacity );
        bool stringalized( uint64_t _value, Char * const _string, size_t _capacity );
        bool stringalized( float _value, Char * const _string, size_t _capacity );
        bool stringalized( double _value, Char * const _string, size_t _capacity );
        bool stringalized( const mt::vec2f & _value, Char * const _string, size_t _capacity );
        bool stringalized( const mt::vec3f & _value, Char * const _string, size_t _capacity );
        bool stringalized( const mt::vec4f & _value, Char * const _string, size_t _capacity );
        bool stringalized( const Char * _value, Char * const _string, size_t _capacity );
        bool stringalized( const String & _value, Char * const _string, size_t _capacity );
        bool stringalized( const ConstString & _value, Char * const _string, size_t _capacity );
        bool stringalized( const FilePath & _value, Char * const _string, size_t _capacity );
        bool stringalized( const Tags & _value, Char * const _string, size_t _capacity );
        bool stringalized( const Resolution & _value, Char * const _string, size_t _capacity );
        bool stringalized( const Color & _value, Char * const _string, size_t _capacity );
        bool stringalized( const AspectRatioViewport & _value, Char * const _string, size_t _capacity );
        //////////////////////////////////////////////////////////////////////////
    }
}