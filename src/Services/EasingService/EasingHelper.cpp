#include "EasingHelper.h"

#include "math/constant.h"

namespace Mengine
{
    //////////////////////////////////////////////////////////////////////////
    float EasingLinear::easing( float _t ) const
    {
        return _t;
    }
    //////////////////////////////////////////////////////////////////////////
    float EasingIn::easing( float _t ) const
    {
        return _t * _t;
    }
    //////////////////////////////////////////////////////////////////////////
    float EasingOut::easing( float _t ) const
    {
        return MT_sqrtf( _t );
    }
    //////////////////////////////////////////////////////////////////////////
    float EasingInOut::easing( float _t ) const
    {
        _t *= 2.f;

        if( _t < 1.f )
        {
            return 0.5f * _t * _t;
        }
        else
        {
            _t -= 2.f;

            return 1.f - 0.5f * _t * _t;
        }
    }
    //////////////////////////////////////////////////////////////////////////
    float EasingSineIn::easing( float _t ) const
    {
        return -1.f * MT_cosf( _t * mt::constant::half_pi ) + 1.f;
    }
    //////////////////////////////////////////////////////////////////////////
    float EasingSineOut::easing( float _t ) const
    {
        return MT_sinf( _t * mt::constant::half_pi );
    }
    //////////////////////////////////////////////////////////////////////////
    float EasingSineInOut::easing( float _t ) const
    {
        return -0.5f * (MT_cosf( mt::constant::pi * _t ) - 1.f);
    }
    //////////////////////////////////////////////////////////////////////////
    float EasingQuadIn::easing( float _t ) const
    {
        return _t * _t;
    }
    //////////////////////////////////////////////////////////////////////////
    float EasingQuadOut::easing( float _t ) const
    {
        return -1.f * _t * (_t - 2.f);
    }
    //////////////////////////////////////////////////////////////////////////
    float EasingQuadInOut::easing( float _t ) const
    {
        _t *= 2.f;

        if( _t < 1.f )
        {
            return 0.5f * _t * _t;
        }
        else
        {
            _t -= 1.f;

            return -0.5f * (_t * (_t - 2.f) - 1.f);
        }
    }
    //////////////////////////////////////////////////////////////////////////
    float EasingCubicIn::easing( float _t ) const
    {
        return _t * _t * _t;
    }
    //////////////////////////////////////////////////////////////////////////
    float EasingCubicOut::easing( float _t ) const
    {
        _t -= 1.f;

        return _t * _t * _t + 1.f;
    }
    //////////////////////////////////////////////////////////////////////////
    float EasingCubicInOut::easing( float _t ) const
    {
        _t *= 2.f;

        if( _t < 1.f )
        {
            return 0.5f * _t * _t * _t;
        }
        else
        {
            _t -= 2.f;

            return 0.5f * (_t * _t * _t + 2.f);
        }
    }
    //////////////////////////////////////////////////////////////////////////
    float EasingQuartIn::easing( float _t ) const
    {
        return _t * _t * _t * _t;
    }
    //////////////////////////////////////////////////////////////////////////
    float EasingQuartOut::easing( float _t ) const
    {
        _t -= 1.f;

        return -_t * _t * _t * _t + 1.f;
    }
    //////////////////////////////////////////////////////////////////////////
    float EasingQuartInOut::easing( float _t ) const
    {
        _t *= 2.f;

        if( _t < 1.f )
        {
            return 0.5f * _t * _t * _t * _t;
        }
        else
        {
            _t -= 2.f;

            return -0.5f * (_t * _t * _t * _t - 2.f);
        }
    }
    //////////////////////////////////////////////////////////////////////////
    float EasingQuintIn::easing( float _t ) const
    {
        return _t * _t * _t * _t * _t;
    }
    //////////////////////////////////////////////////////////////////////////
    float EasingQuintOut::easing( float _t ) const
    {
        _t -= 1.f;

        return -_t * _t * _t * _t * _t + 1.f;
    }
    //////////////////////////////////////////////////////////////////////////
    float EasingQuintInOut::easing( float _t ) const
    {
        _t *= 2.f;

        if( _t < 1.f )
        {
            return 0.5f * _t * _t * _t * _t * _t;
        }
        else
        {
            _t -= 2.f;

            return 0.5f * (_t * _t * _t * _t * _t + 2.f);
        }
    }
    //////////////////////////////////////////////////////////////////////////
    float EasingExpoIn::easing( float _t ) const
    {
        return _t == 0.f ? 0.f : MT_powf( 2.f, 10.f * (_t - 1.f) ) - 0.001f;
    }
    //////////////////////////////////////////////////////////////////////////
    float EasingExpoOut::easing( float _t ) const
    {
        return _t == 1.f ? 1.f : -MT_powf( 2.f, -10.f * _t ) + 1.f;
    }
    //////////////////////////////////////////////////////////////////////////
    float EasingExpoInOut::easing( float _t ) const
    {
        if( _t == 0.f || _t == 1.f )
        {
            return _t;
        }

        if( _t < 0.5f )
        {
            return 0.5f * MT_powf( 2.f, 10.f * (_t * 2.f - 1.f) );
        }
        else
        {
            return 0.5f * (-MT_powf( 2.f, -10.f * (_t * 2.f - 1.f) ) + 2.f);
        }
    }
    //////////////////////////////////////////////////////////////////////////
    float EasingCircIn::easing( float _t ) const
    {
        return -1.f * (MT_sqrtf( 1.f - _t * _t ) - 1.f);
    }
    //////////////////////////////////////////////////////////////////////////
    float EasingCircOut::easing( float _t ) const
    {
        _t -= 1.f;

        return MT_sqrtf( 1.f - _t * _t );
    }
    //////////////////////////////////////////////////////////////////////////
    float EasingCircInOut::easing( float _t ) const
    {
        _t *= 2.f;

        if( _t < 1.f )
        {
            return -0.5f * (MT_sqrtf( 1.f - _t * _t ) - 1.f);
        }
        else
        {
            _t -= 2.f;

            return 0.5f * (MT_sqrtf( 1.f - _t * _t ) + 1.f);
        }
    }
    //////////////////////////////////////////////////////////////////////////
    float EasingElasticIn::easing( float _t ) const
    {
        if( _t == 0.f || _t == 1.f )
        {
            return _t;
        }

        _t -= 1.f;

        return -MT_powf( 2.f, 10.f * _t ) * MT_sinf( (_t - 0.3f * 0.25f) * mt::constant::two_pi / 0.3f );
    }
    //////////////////////////////////////////////////////////////////////////
    float EasingElasticOut::easing( float _t ) const
    {
        if( _t == 0.f || _t == 1.f )
        {
            return _t;
        }

        return MT_powf( 2.f, -10.f * _t ) * MT_sinf( (_t - 0.3f * 0.25f) * mt::constant::two_pi / 0.3f ) + 1.f;
    }
    //////////////////////////////////////////////////////////////////////////
    float EasingElasticInOut::easing( float _t ) const
    {
        if( _t == 0.f || _t == 1.f )
        {
            return _t;
        }

        _t *= 2.f;
        _t -= 1.f;

        if( _t < 0.f )
        {
            return -0.5f * MT_powf( 2.f, 10.f * _t ) * MT_sinf( (_t - 0.3f * 0.25f) * mt::constant::two_pi / 0.3f );
        }
        else
        {
            return MT_powf( 2.f, -10.f * _t ) * MT_sinf( (_t - 0.3f * 0.25f) * mt::constant::two_pi / 0.3f ) * 0.5f + 1.f;
        }
    }
    //////////////////////////////////////////////////////////////////////////
    float EasingBackIn::easing( float _t ) const
    {
        return _t * _t * (2.70158f * _t - 1.70158f);
    }
    //////////////////////////////////////////////////////////////////////////
    float EasingBackOut::easing( float _t ) const
    {
        _t -= 1.f;

        return _t * _t * (2.70158f * _t + 1.70158f) + 1.f;
    }
    //////////////////////////////////////////////////////////////////////////
    float EasingBackInOut::easing( float _t ) const
    {
        _t *= 2.f;

        if( _t < 1.f )
        {
            return _t * _t * (3.5949095f * _t - 2.5949095f) * 0.5f;
        }
        else
        {
            _t -= 2.f;

            return _t * _t * (3.5949095f * _t + 2.5949095f) * 0.5f + 1.f;
        }
    }
    //////////////////////////////////////////////////////////////////////////
    namespace Detail
    {
        static float bounce( float  _t )
        {
            if( _t < 1.f / 2.75f )
            {
                return 7.5625f * _t * _t;
            }
            else if( _t < 2.f / 2.75f )
            {
                _t -= 1.5f / 2.75f;

                return 7.5625f * _t * _t + 0.75f;
            }
            else if( _t < 2.5f / 2.75f )
            {
                _t -= 2.25f / 2.75f;

                return 7.5625f * _t * _t + 0.9375f;
            }
            else
            {
                _t -= 2.625f / 2.75f;

                return 7.5625f * _t * _t + 0.984375f;
            }
        }
    }
    //////////////////////////////////////////////////////////////////////////
    float EasingBounceIn::easing( float _t ) const
    {
        return 1.f - Detail::bounce( 1.f - _t );
    }
    //////////////////////////////////////////////////////////////////////////
    float EasingBounceOut::easing( float _t ) const
    {
        return Detail::bounce( _t );
    }
    //////////////////////////////////////////////////////////////////////////
    float EasingBounceInOut::easing( float _t ) const
    {
        if( _t < 0.5f )
        {
            _t *= 2.f;
            return (1.f - Detail::bounce( 1.f - _t )) * 0.5f;
        }
        else
        {
            return Detail::bounce( _t * 2.f - 1.f ) * 0.5f + 0.5f;
        }
    }
    //////////////////////////////////////////////////////////////////////////
}