#include "MT19937Randomizer.h"

#include "Kernel/Assertion.h"

namespace Mengine
{
    //////////////////////////////////////////////////////////////////////////
    MT19937Randomizer::MT19937Randomizer()
        : m_engineRandomize( std::random_device()() )
    {
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

        std::uniform_int_distribution<int32_t> uid( 0, _max - 1 );

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

        std::uniform_int_distribution<int32_t> uid( _min, _max - 1 );

        uint32_t rand_value = uid( m_engineRandomize );

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
}