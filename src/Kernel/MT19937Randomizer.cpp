#include "MT19937Randomizer.h"

#include "Kernel/RandomDevice.h"
#include "Kernel/Assertion.h"
#include "Kernel/Logger.h"

namespace Mengine
{
    //////////////////////////////////////////////////////////////////////////
    MT19937Randomizer::MT19937Randomizer()
    {
        uint32_t seed = Helper::generateRandomLocaleSeed();

        m_engineRandomize = std::mt19937{seed};

        std::mt19937::result_type min_value = (m_engineRandomize.min)();
        std::mt19937::result_type max_value = (m_engineRandomize.max)();

        m_epsilon = 1.f / float( max_value - min_value );
    }
    //////////////////////////////////////////////////////////////////////////
    MT19937Randomizer::~MT19937Randomizer()
    {
    }
    //////////////////////////////////////////////////////////////////////////
    void MT19937Randomizer::setSeed( uint32_t _seed )
    {
        m_engineRandomize.seed( _seed );
    }
    //////////////////////////////////////////////////////////////////////////
    uint32_t MT19937Randomizer::getRandom( uint32_t _max ) const
    {
        if( _max < 2 )
        {
            return 0U;
        }

        std::uniform_int_distribution<uint32_t> uid( 0, _max - 1 );

        uint32_t rand_value = uid( m_engineRandomize );

        return rand_value;
    }
    //////////////////////////////////////////////////////////////////////////
    uint32_t MT19937Randomizer::getRandomRange( uint32_t _min, uint32_t _max ) const
    {
        if( _max < 2 )
        {
            return 0U;
        }

        if( _min >= _max )
        {
            return _max - 1;
        }

        std::uniform_int_distribution<uint32_t> uid( _min, _max - 1 );

        uint32_t rand_value = uid( m_engineRandomize );

        return rand_value;
    }
    //////////////////////////////////////////////////////////////////////////
    int32_t MT19937Randomizer::getRandomi( int32_t _max ) const
    {
        std::uniform_int_distribution<int32_t> uid( _max > 0 ? 0 : _max, _max > 0 ? _max : 0 );

        int32_t rand_value = uid( m_engineRandomize );

        return rand_value;
    }
    //////////////////////////////////////////////////////////////////////////
    int32_t MT19937Randomizer::getRandomRangei( int32_t _min, int32_t _max ) const
    {
        if( _min >= _max )
        {
            return _max - 1;
        }

        std::uniform_int_distribution<int32_t> uid( _min, _max - 1 );

        int32_t rand_value = uid( m_engineRandomize );

        return rand_value;
    }
    //////////////////////////////////////////////////////////////////////////
    float MT19937Randomizer::getRandomf( float _max ) const
    {
        MENGINE_ASSERTION_FATAL( _max >= 0.f );

        if( _max < m_epsilon )
        {
            return 0.f;
        }

        std::uniform_real_distribution<float> uid( 0.f, _max );

        float rand_value = uid( m_engineRandomize );

        return rand_value;
    }
    //////////////////////////////////////////////////////////////////////////
    float MT19937Randomizer::getRandomRangef( float _min, float _max ) const
    {
        MENGINE_ASSERTION_FATAL( _min >= 0.f );
        MENGINE_ASSERTION_FATAL( _max >= 0.f );

        if( _max < m_epsilon )
        {
            return 0.f;
        }

        if( _min >= _max )
        {
            return _max;
        }

        std::uniform_real_distribution<float> uid( _min, _max );

        float rand_value = uid( m_engineRandomize );

        return rand_value;
    }
    //////////////////////////////////////////////////////////////////////////
}