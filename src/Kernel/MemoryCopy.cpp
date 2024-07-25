#include "Kernel/MemoryCopy.h"

#include "Config/StdInt.h"

namespace Mengine
{
    namespace Helper
    {
        //////////////////////////////////////////////////////////////////////////
        void memoryCopy( void * _dst, size_t _dstOffset, const void * _src, size_t _srcOffset, size_t _size )
        {
            const uint8_t * src_base_u8 = reinterpret_cast<const uint8_t *>(_src);
            const uint8_t * src_begin_u8 = src_base_u8 + _srcOffset;
            const uint8_t * src_end_u8 = src_begin_u8 + _size;

            uint8_t * dst_base_u8 = reinterpret_cast<uint8_t *>(_dst);
            uint8_t * dst_begin_u8 = dst_base_u8 + _dstOffset;

            Mengine::Algorithm::copy( src_begin_u8, src_end_u8, dst_begin_u8 );
        }
        //////////////////////////////////////////////////////////////////////////
        bool memoryCopySafe( void * _dist, size_t _dstOffset, size_t _capacity, const void * _src, size_t _srcOffset, size_t _size )
        {
            if( _dstOffset + _size > _capacity )
            {
                return false;
            }

            Helper::memoryCopy( _dist, _dstOffset, _src, _srcOffset, _size );

            return true;
        }
        //////////////////////////////////////////////////////////////////////////
    }
}