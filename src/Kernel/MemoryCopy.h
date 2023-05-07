#pragma once

#include "Config/Algorithm.h"

namespace Mengine
{
    namespace Helper
    {
        //////////////////////////////////////////////////////////////////////////
        void memoryCopy( void * _dst, size_t _offset, const void * _src, size_t _size );
        bool memoryCopySafe( void * _dist, size_t _offset, size_t _capacity, const void * _src, size_t _size );
        //////////////////////////////////////////////////////////////////////////
        template<class T>
        MENGINE_INLINE void memoryCopyPod( T * _dist, size_t _offset, const T * _src, size_t _size )
        {
            const size_t sizeof_T = sizeof( T );

            size_t offset_pod = _offset * sizeof_T;
            size_t size_pod = _size * sizeof_T;

            Helper::memoryCopy( _dist, offset_pod, _src, size_pod );
        }
        //////////////////////////////////////////////////////////////////////////
        template<class T>
        MENGINE_INLINE bool memoryCopyPodSafe( T * _dist, size_t _offset, size_t _capacity, const T * _src, size_t _size )
        {
            if( _offset + _size > _capacity )
            {
                return false;
            }

            Helper::memoryCopyPod( _dist, _offset, _src, _size );

            return true;
        }
        //////////////////////////////////////////////////////////////////////////
    }
}