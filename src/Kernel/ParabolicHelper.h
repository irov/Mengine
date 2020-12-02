#pragma once

namespace Mengine
{
    namespace Helper
    {
        //////////////////////////////////////////////////////////////////////////
        template<class T>
        void calculateParabolicPosition( T * _out, const T & _begin, const T & _end, const T & _height, float _time )
        {
            T a = 2.f * _end - 4.f * _height + 2.f * _begin;
            T b = 4.f * _height - _end - 3.f * _begin;
            T c = _begin;

            *_out = a * _time * _time + b * _time + c;
        }
        //////////////////////////////////////////////////////////////////////////
        template<class T, class LENGTH>
        float calculateParabolicLength( const T & _begin, const T & _end, const T & _height, LENGTH _length )
        {
            const uint32_t max_iterator = 10;

            float total_length = 0.f;

            T prevPoint = _begin;

            const float dt = 1.f / float( max_iterator );

            float t = 0.f;

            for( uint32_t i = 1; i != max_iterator; ++i )
            {
                t += dt;

                T nextPoint;
                Helper::calculateParabolicPosition( nextPoint, _begin, _end, _height, t );

                float length = _length( nextPoint, prevPoint );

                total_length += length;

                prevPoint = nextPoint;
            }

            float length = _length( _end, prevPoint );

            total_length += length;

            return total_length;
        }
        //////////////////////////////////////////////////////////////////////////
    }
}