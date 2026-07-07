#include "ArchivatorZstd.h"

#include "Kernel/AllocatorHelper.h"
#include "Kernel/Logger.h"
#include "Kernel/ProfilerHelper.h"

#define ZSTD_STATIC_LINKING_ONLY
#include "zstd.h"

//////////////////////////////////////////////////////////////////////////
namespace Mengine
{
    namespace Detail
    {
        //////////////////////////////////////////////////////////////////////////
        static void * zstdAlloc( void * const _opaque, size_t _size )
        {
            MENGINE_UNUSED( _opaque );

            void * p = Helper::allocateMemory( _size, "zstd" );

            return p;
        }
        //////////////////////////////////////////////////////////////////////////
        static void zstdFree( void * const _opaque, void * const _address )
        {
            MENGINE_UNUSED( _opaque );

            Helper::deallocateMemory( _address, "zstd" );
        }
        //////////////////////////////////////////////////////////////////////////
        static ZSTD_customMem zstdCustomMem()
        {
            ZSTD_customMem customMem = {&zstdAlloc, &zstdFree, nullptr};

            return customMem;
        }
        //////////////////////////////////////////////////////////////////////////
    }
    //////////////////////////////////////////////////////////////////////////
    //////////////////////////////////////////////////////////////////////////
    ArchivatorZstd::ArchivatorZstd()
    {
    }
    //////////////////////////////////////////////////////////////////////////
    ArchivatorZstd::~ArchivatorZstd()
    {
    }
    //////////////////////////////////////////////////////////////////////////
    size_t ArchivatorZstd::compressBound( size_t _size ) const
    {
        size_t total_size = ::ZSTD_compressBound( _size );

        return total_size;
    }
    //////////////////////////////////////////////////////////////////////////
    bool ArchivatorZstd::compress( void * const _distance, size_t _bufferCapacity, const void * _source, size_t _sourceSize, size_t * const _compressSize, EArchivatorCompress _compress )
    {
        int32_t compressLevel = ZSTD_CLEVEL_DEFAULT;

        switch( _compress )
        {
        case EAC_FAST:
            {
                compressLevel = ::ZSTD_minCLevel();
            }break;
        case EAC_NORMAL:
            {
                compressLevel = ZSTD_CLEVEL_DEFAULT;
            }break;
        case EAC_BEST:
            {
                compressLevel = ::ZSTD_maxCLevel();
            }break;
        };

        ZSTD_CCtx * cctx = ::ZSTD_createCCtx_advanced( Detail::zstdCustomMem() );

        if( cctx == nullptr )
        {
            LOGGER_ERROR( "invalid create zstd compress context" );

            return false;
        }

        size_t compressSize = ::ZSTD_compressCCtx( cctx, _distance, _bufferCapacity, _source, _sourceSize, compressLevel );

        size_t freeResult = ::ZSTD_freeCCtx( cctx );

        if( ::ZSTD_isError( freeResult ) )
        {
            LOGGER_ERROR( "invalid free zstd compress context error [%s]"
                , ::ZSTD_getErrorName( freeResult )
            );

            return false;
        }

        if( ::ZSTD_isError( compressSize ) )
        {
            LOGGER_ERROR( "invalid zstd compress error [%s]"
                , ::ZSTD_getErrorName( compressSize )
            );

            return false;
        }

        *_compressSize = compressSize;

        return true;
    }
    //////////////////////////////////////////////////////////////////////////
    bool ArchivatorZstd::decompress( void * const _distance, size_t _bufferCapacity, const void * _source, size_t _sourceSize, size_t * const _decompressSize )
    {
        MENGINE_PROFILER_CATEGORY();

        ZSTD_DCtx * dctx = ::ZSTD_createDCtx_advanced( Detail::zstdCustomMem() );

        if( dctx == nullptr )
        {
            LOGGER_ERROR( "invalid create zstd decompress context" );

            return false;
        }

        size_t readBytes = ::ZSTD_decompressDCtx( dctx, _distance, _bufferCapacity, _source, _sourceSize );

        size_t freeResult = ::ZSTD_freeDCtx( dctx );

        if( ::ZSTD_isError( freeResult ) )
        {
            LOGGER_ERROR( "invalid free zstd decompress context error [%s]"
                , ::ZSTD_getErrorName( freeResult )
            );

            return false;
        }

        if( ::ZSTD_isError( readBytes ) )
        {
            LOGGER_ERROR( "invalid zstd decompress error [%s]"
                , ::ZSTD_getErrorName( readBytes )
            );

            return false;
        }

        *_decompressSize = readBytes;

        return true;
    }
    //////////////////////////////////////////////////////////////////////////
}
