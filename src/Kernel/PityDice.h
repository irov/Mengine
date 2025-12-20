#pragma once

#include "Interface/DiceInterface.h"

namespace Mengine
{
    //////////////////////////////////////////////////////////////////////////
    class PityDice
        : public DiceInterface
    {
        DECLARE_FACTORABLE( PityDice );

    public:
        PityDice();
        ~PityDice() override;

    public:
        void setRandomizer( const RandomizerInterfacePtr & _randomizer ) override;
        const RandomizerInterfacePtr & getRandomizer() const override;

    public:
        void setChance( float _chance ) override;
        float getChance() const override;

    public:
        bool roll() const override;

    protected:
        RandomizerInterfacePtr m_randomizer;

        float m_chance;
        float m_delta;

        mutable float m_fails;
    };
    //////////////////////////////////////////////////////////////////////////
    typedef IntrusivePtr<PityDice, DiceInterface> PityDicePtr;
    //////////////////////////////////////////////////////////////////////////
}