#pragma once

#include "Interface/RandomizerInterface.h"

#include <random>

namespace Mengine
{
    class MT19937Randomizer
        : public RandomizerInterface
    {
        DECLARE_FACTORABLE( MT19937Randomizer );

    public:
        MT19937Randomizer();
        ~MT19937Randomizer() override;

    protected:
        void setSeed( uint64_t _seed ) override;

    protected:
        uint32_t getRandom32( uint32_t _max ) const override;
        uint32_t getRandomRange32( uint32_t _min, uint32_t _max ) const override;

        int32_t getRandom32i( int32_t _max ) const override;
        int32_t getRandomRange32i( int32_t _min, int32_t _max ) const override;

        uint64_t getRandom64( uint64_t _max ) const override;
        uint64_t getRandomRange64( uint64_t _min, uint64_t _max ) const override;

        int64_t getRandom64i( int64_t _max ) const override;
        int64_t getRandomRange64i( int64_t _min, int64_t _max ) const override;

        float getRandomf( float _max ) const override;
        float getRandomRangef( float _min, float _max ) const override;

        double getRandomd( double _max ) const override;
        double getRandomRanged( double _min, double _max ) const override;

    protected:
        mutable std::mt19937_64 m_engineRandomize;

        float m_epsilonf;
        double m_epsilond;
    };
}