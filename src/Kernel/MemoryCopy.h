#pragma once

#include "Config/StdAlgorithm.h"
#include "Config/StdDef.h"

namespace Mengine
{
    namespace Helper
    {
        //////////////////////////////////////////////////////////////////////////
        void memoryCopy( void * _dst, size_t _dstOffset, const void * _src, size_t _srcOffset, size_t _size );
        bool memoryCopySafe( void * _dist, size_t _dstOffset, size_t _capacity, const void * _src, size_t _srcOffset, size_t _size );
        //////////////////////////////////////////////////////////////////////////
        template<class T>
        void memoryCopyPod( T * _dist, size_t _dstOffset, const T * _src, size_t _srcOffset, size_t _size )
        {
            const size_t sizeof_T = sizeof( T );

            size_t dstOffset_pod = _dstOffset * sizeof_T;
            size_t srcOffset_pod = _srcOffset * sizeof_T;
            size_t size_pod = _size * sizeof_T;

            Helper::memoryCopy( _dist, dstOffset_pod, _src, srcOffset_pod, size_pod );
        }
        //////////////////////////////////////////////////////////////////////////
        template<class T>
        bool memoryCopyPodSafe( T * _dist, size_t _dstOffset, size_t _capacity, const T * _src, size_t _srcOffset, size_t _size )
        {
            if( _dstOffset + _size > _capacity )
            {
                return false;
            }

            Helper::memoryCopyPod( _dist, _dstOffset, _src, _srcOffset, _size );

            return true;
        }
        //////////////////////////////////////////////////////////////////////////
    }
}