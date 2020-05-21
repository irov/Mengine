#pragma once

#include "Interface/RandomizerInterface.h"

#include <random>

namespace Mengine
{
    class MT19937Randomizer
        : public RandomizerInterface
    {
    public:
        MT19937Randomizer();
        ~MT19937Randomizer() override;

    protected:
        void setSeed( uint32_t _seed ) override;

    protected:
        uint32_t getRandom( uint32_t _max ) const override;
        uint32_t getRandomRange( uint32_t _min, uint32_t _max ) const override;

        int32_t getRandomi( int32_t _max ) const override;
        int32_t getRandomRangei( int32_t _min, int32_t _max ) const override;

        float getRandomf( float _max ) const override;
        float getRandomRangef( float _min, float _max ) const override;

    protected:
        mutable std::mt19937 m_engineRandomize;

        float m_epsilon;
    };
}