#include "MT19937Randomizer.h"

#include "Kernel/RandomDevice.h"
#include "Kernel/Assertion.h"
#include "Kernel/Logger.h"

namespace Mengine
{
    //////////////////////////////////////////////////////////////////////////
    MT19937Randomizer::MT19937Randomizer()
        : m_engineRandomize( Helper::generateRandomDeviceMT19937() )
    {
        StdRandom::mt19937_64::result_type min_value = (m_engineRandomize.min)();
        StdRandom::mt19937_64::result_type max_value = (m_engineRandomize.max)();

        m_epsilonf = 1.f / float( max_value - min_value );
        m_epsilond = 1.0 / double( max_value - min_value );
    }
    //////////////////////////////////////////////////////////////////////////
    MT19937Randomizer::MT19937Randomizer( uint64_t _seed )
        : m_engineRandomize{_seed}
    {
        StdRandom::mt19937_64::result_type min_value = (m_engineRandomize.min)();
        StdRandom::mt19937_64::result_type max_value = (m_engineRandomize.max)();

        m_epsilonf = 1.f / float( max_value - min_value );
        m_epsilond = 1.0 / double( max_value - min_value );
    }
    //////////////////////////////////////////////////////////////////////////
    MT19937Randomizer::~MT19937Randomizer()
    {
    }
    //////////////////////////////////////////////////////////////////////////
    void MT19937Randomizer::setSeed( uint64_t _seed )
    {
        m_engineRandomize.seed( _seed );
    }
    //////////////////////////////////////////////////////////////////////////
    uint32_t MT19937Randomizer::getRandom32( uint32_t _max ) const
    {
        if( _max < 2 )
        {
            return 0U;
        }

        StdRandom::uniform_int_distribution<uint32_t> uid( 0, _max - 1 );

        uint32_t rand_value = uid( m_engineRandomize );

        return rand_value;
    }
    //////////////////////////////////////////////////////////////////////////
    uint32_t MT19937Randomizer::getRandomRange32( uint32_t _min, uint32_t _max ) const
    {
        if( _max < 2 )
        {
            return 0U;
        }

        if( _min >= _max )
        {
            return _max - 1;
        }

        StdRandom::uniform_int_distribution<uint32_t> uid( _min, _max - 1 );

        uint32_t rand_value = uid( m_engineRandomize );

        return rand_value;
    }
    //////////////////////////////////////////////////////////////////////////
    int32_t MT19937Randomizer::getRandom32i( int32_t _max ) const
    {
        StdRandom::uniform_int_distribution<int32_t> uid( _max > 0 ? 0 : _max, _max > 0 ? _max : 0 );

        int32_t rand_value = uid( m_engineRandomize );

        return rand_value;
    }
    //////////////////////////////////////////////////////////////////////////
    int32_t MT19937Randomizer::getRandomRange32i( int32_t _min, int32_t _max ) const
    {
        if( _min >= _max )
        {
            return _max - 1;
        }

        StdRandom::uniform_int_distribution<int32_t> uid( _min, _max - 1 );

        int32_t rand_value = uid( m_engineRandomize );

        return rand_value;
    }
    //////////////////////////////////////////////////////////////////////////
    uint64_t MT19937Randomizer::getRandom64( uint64_t _max ) const
    {
        if( _max < 2 )
        {
            return 0U;
        }

        StdRandom::uniform_int_distribution<uint64_t> uid( 0, _max - 1 );

        uint64_t rand_value = uid( m_engineRandomize );

        return rand_value;
    }
    //////////////////////////////////////////////////////////////////////////
    uint64_t MT19937Randomizer::getRandomRange64( uint64_t _min, uint64_t _max ) const
    {
        if( _max < 2 )
        {
            return 0U;
        }

        if( _min >= _max )
        {
            return _max - 1;
        }

        StdRandom::uniform_int_distribution<uint64_t> uid( _min, _max - 1 );

        uint64_t rand_value = uid( m_engineRandomize );

        return rand_value;
    }
    //////////////////////////////////////////////////////////////////////////
    int64_t MT19937Randomizer::getRandom64i( int64_t _max ) const
    {
        StdRandom::uniform_int_distribution<int64_t> uid( _max > 0 ? 0 : _max, _max > 0 ? _max : 0 );

        int64_t rand_value = uid( m_engineRandomize );

        return rand_value;
    }
    //////////////////////////////////////////////////////////////////////////
    int64_t MT19937Randomizer::getRandomRange64i( int64_t _min, int64_t _max ) const
    {
        if( _min >= _max )
        {
            return _max - 1;
        }

        StdRandom::uniform_int_distribution<int64_t> uid( _min, _max - 1 );

        int64_t rand_value = uid( m_engineRandomize );

        return rand_value;
    }
    //////////////////////////////////////////////////////////////////////////
    float MT19937Randomizer::getRandomf( float _max ) const
    {
        MENGINE_ASSERTION_FATAL( _max >= 0.f, "invalid max value %f"
            , _max
        );

        if( _max < m_epsilonf )
        {
            return 0.f;
        }

        StdRandom::uniform_real_distribution<float> uid( 0.f, _max );

        float rand_value = uid( m_engineRandomize );

        return rand_value;
    }
    //////////////////////////////////////////////////////////////////////////
    float MT19937Randomizer::getRandomRangef( float _min, float _max ) const
    {
        MENGINE_ASSERTION_FATAL( _min >= 0.f, "invalid min value %f"
            , _min
        );

        MENGINE_ASSERTION_FATAL( _max >= 0.f, "invalid max value %f"
            , _max
        );

        if( _max < m_epsilonf )
        {
            return 0.f;
        }

        if( _min >= _max )
        {
            return _max;
        }

        StdRandom::uniform_real_distribution<float> uid( _min, _max );

        float rand_value = uid( m_engineRandomize );

        return rand_value;
    }
    //////////////////////////////////////////////////////////////////////////
    double MT19937Randomizer::getRandomd( double _max ) const
    {
        MENGINE_ASSERTION_FATAL( _max >= 0.0, "invalid max value %lf"
            , _max
        );

        if( _max < m_epsilond )
        {
            return 0.0;
        }

        StdRandom::uniform_real_distribution<double> uid( 0.0, _max );

        double rand_value = uid( m_engineRandomize );

        return rand_value;
    }
    //////////////////////////////////////////////////////////////////////////
    double MT19937Randomizer::getRandomRanged( double _min, double _max ) const
    {
        MENGINE_ASSERTION_FATAL( _min >= 0.0, "invalid min value %lf"
            , _min
        );

        MENGINE_ASSERTION_FATAL( _max >= 0.0, "invalid max value %lf"
            , _max
        );

        if( _max < m_epsilond )
        {
            return 0.0;
        }

        if( _min >= _max )
        {
            return _max;
        }

        StdRandom::uniform_real_distribution<double> uid( _min, _max );

        double rand_value = uid( m_engineRandomize );

        return rand_value;
    }
    //////////////////////////////////////////////////////////////////////////
}