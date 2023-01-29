#pragma once

#include "Config/StdArg.h"

#include <cstdio>

//////////////////////////////////////////////////////////////////////////
#ifndef MENGINE_VSNPRINTF
//////////////////////////////////////////////////////////////////////////
#if defined(MENGINE_COMPILER_MSVC) || defined( MENGINE_COMPILER_CLANG ) || defined( MENGINE_COMPILER_GCC )
//////////////////////////////////////////////////////////////////////////
namespace Mengine
{
    namespace Detail
    {
        //////////////////////////////////////////////////////////////////////////
        MENGINE_INLINE int32_t __mengine_VSPRINTF( Char * const _buffer, size_t const _bufferCount, Char const * const _format, va_list _argList )
        {
            int32_t size_vsnprintf = std::vsnprintf( _buffer, _bufferCount, _format, _argList );

            if( size_vsnprintf < 0 )
            {
                return size_vsnprintf;
            }

            int32_t size_correct = MENGINE_MIN( size_vsnprintf, MENGINE_MAX( 0, (int32_t)_bufferCount - 1 ) );

            return size_correct;
        }
        //////////////////////////////////////////////////////////////////////////
    }
}
//////////////////////////////////////////////////////////////////////////
#   define MENGINE_VSNPRINTF(Buffer, Capacity, Format, Args) Mengine::Detail::__mengine_VSPRINTF(Buffer, Capacity, Format, Args)
//////////////////////////////////////////////////////////////////////////
#else
#   error "undefined MENGINE_VSNPRINTF"
#endif
#endif
//////////////////////////////////////////////////////////////////////////
#ifndef MENGINE_VSNCOUNTF
//////////////////////////////////////////////////////////////////////////
#if defined(MENGINE_COMPILER_MSVC) || defined( MENGINE_COMPILER_CLANG ) || defined( MENGINE_COMPILER_GCC )
//////////////////////////////////////////////////////////////////////////
namespace Mengine
{
    namespace Detail
    {
        //////////////////////////////////////////////////////////////////////////
        MENGINE_INLINE int32_t __mengine_VSNCOUNTF( Char const * const _format, va_list _argList )
        {
            int32_t size_vsnprintf = std::vsnprintf( nullptr, 0, _format, _argList );

            return size_vsnprintf;
        }
        //////////////////////////////////////////////////////////////////////////
    }
}
//////////////////////////////////////////////////////////////////////////
#   define MENGINE_VSNCOUNTF(Format, Args) Mengine::Detail::__mengine_VSNCOUNTF(Format, Args)
//////////////////////////////////////////////////////////////////////////
#else
#   error "undefined MENGINE_VSNPRINTF"
#endif
#endif
//////////////////////////////////////////////////////////////////////////
//////////////////////////////////////////////////////////////////////////
#ifndef MENGINE_SWSCANF
#define MENGINE_SWSCANF(Buffer, ...) std::swscanf( Buffer, __VA_ARGS__ )
#endif
//////////////////////////////////////////////////////////////////////////
#ifndef MENGINE_SSCANF
#define MENGINE_SSCANF(Buffer, ...) std::sscanf( Buffer, __VA_ARGS__ )
#endif
//////////////////////////////////////////////////////////////////////////
#ifndef MENGINE_WNSPRINTF
#define MENGINE_WNSPRINTF(Buffer, Capacity, ...) std::swprintf(Buffer, Capacity, __VA_ARGS__ )
#endif
//////////////////////////////////////////////////////////////////////////
#ifndef MENGINE_SPRINTF
#define MENGINE_SPRINTF(Buffer, ...) std::sprintf(Buffer, __VA_ARGS__ )
#endif
//////////////////////////////////////////////////////////////////////////
#ifndef MENGINE_SNPRINTF
//////////////////////////////////////////////////////////////////////////
namespace Mengine
{
    namespace Detail
    {
        //////////////////////////////////////////////////////////////////////////
        MENGINE_INLINE int32_t __mengine_SNPRINTF( Char * const _buffer, size_t const _bufferCount, Char const * const _format, ... )
        {
            MENGINE_VA_LIST_TYPE args;
            MENGINE_VA_LIST_START( args, _format );

            int32_t size_vsnprintf = MENGINE_VSNPRINTF( _buffer, _bufferCount, _format, args );

            MENGINE_VA_LIST_END( args );

            return size_vsnprintf;
        }
        //////////////////////////////////////////////////////////////////////////
    }
}
//////////////////////////////////////////////////////////////////////////
#define MENGINE_SNPRINTF(Buffer, Capacity, ...) Mengine::Detail::__mengine_SNPRINTF( Buffer, Capacity, __VA_ARGS__ )
//////////////////////////////////////////////////////////////////////////
#endif
//////////////////////////////////////////////////////////////////////////
#ifndef MENGINE_VSPRINTF
#define MENGINE_VSPRINTF(Buffer, Format, Args) std::vsprintf( Buffer, Format, Args )
#endif
//////////////////////////////////////////////////////////////////////////