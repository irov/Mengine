#include "PityDice.h"

#include "Config/StdMath.h"

namespace Mengine
{
    //////////////////////////////////////////////////////////////////////////
    PityDice::PityDice()
        : m_chance( 0.f )
        , m_delta( 0.f )
        , m_fails( 0.f )
    {
    }
    //////////////////////////////////////////////////////////////////////////
    PityDice::~PityDice()
    {
    }
    //////////////////////////////////////////////////////////////////////////
    void PityDice::setRandomizer( const RandomizerInterfacePtr & _randomizer )
    {
        m_randomizer = _randomizer;
    }
    //////////////////////////////////////////////////////////////////////////
    const RandomizerInterfacePtr & PityDice::getRandomizer() const
    {
        return m_randomizer;
    }
    //////////////////////////////////////////////////////////////////////////
    void PityDice::setChance( float _chance )
    {
        m_chance = _chance;

        const float M = StdMath::ceil( 2.f / _chance );

        m_delta = (1.f - _chance) / (M - 1.f);
        m_fails = 0.f;
    }
    //////////////////////////////////////////////////////////////////////////
    float PityDice::getChance() const
    {
        return m_chance;
    }
    //////////////////////////////////////////////////////////////////////////
    bool PityDice::roll() const
    {
        float roll = m_randomizer->getRandomf( 1.f );

        const float test = m_chance + m_fails * m_delta;

        if( roll >= test )
        {
            m_fails = 0.f;

            return true;
        }

        m_fails += 1.f;

        return false;
    }
    //////////////////////////////////////////////////////////////////////////
}