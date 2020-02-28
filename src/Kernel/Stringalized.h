#pragma once

#include "Kernel/ConstString.h"
#include "Kernel/FilePath.h"
#include "Kernel/Tags.h"
#include "Kernel/Resolution.h"
#include "Kernel/Color.h"
#include "Kernel/AspectRatioViewport.h"

#include "Config/String.h"
#include "Config/Char.h"
#include "Config/StdInt.h"

namespace Mengine
{
    namespace Helper
    {
        bool stringalized( const Char * _string, bool * _value );
        bool stringalized( const Char * _string, int8_t * _value );
        bool stringalized( const Char * _string, uint8_t * _value );
        bool stringalized( const Char * _string, int32_t * _value );
        bool stringalized( const Char * _string, uint32_t * _value );
        bool stringalized( const Char * _string, int64_t * _value );
        bool stringalized( const Char * _string, uint64_t * _value );
        bool stringalized( const Char * _string, float * _value );
        bool stringalized( const Char * _string, double * _value );
        bool stringalized( const Char * _string, const Char ** _value );
        bool stringalized( const Char * _string, String * _value );
        bool stringalized( const Char * _string, ConstString * _value );
        bool stringalized( const Char * _string, FilePath * _value );
        bool stringalized( const Char * _string, Tags * _value );
        bool stringalized( const Char * _string, Resolution * _value );
        bool stringalized( const Char * _string, Color * _value );
        bool stringalized( const Char * _string, AspectRatioViewport * _value );

        bool stringalized( bool _value, Char * _string, size_t _capacity );
        bool stringalized( int8_t _value, Char * _string, size_t _capacity );
        bool stringalized( uint8_t _value, Char * _string, size_t _capacity );
        bool stringalized( int32_t _value, Char * _string, size_t _capacity );
        bool stringalized( uint32_t _value, Char * _string, size_t _capacity );
        bool stringalized( int64_t _value, Char * _string, size_t _capacity );
        bool stringalized( uint64_t _value, Char * _string, size_t _capacity );
        bool stringalized( float _value, Char * _string, size_t _capacity );
        bool stringalized( double _value, Char * _string, size_t _capacity );
        bool stringalized( const Char * _value, Char * _string, size_t _capacity );
        bool stringalized( const String & _value, Char * _string, size_t _capacity );
        bool stringalized( const ConstString & _value, Char * _string, size_t _capacity );
        bool stringalized( const FilePath & _value, Char * _string, size_t _capacity );
        bool stringalized( const Tags & _value, Char * _string, size_t _capacity );
        bool stringalized( const Resolution & _value, Char * _string, size_t _capacity );
        bool stringalized( const Color & _value, Char * _string, size_t _capacity );
        bool stringalized( const AspectRatioViewport & _value, Char * _string, size_t _capacity );
    }
}