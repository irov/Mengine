#pragma once

#include "Interface/ChronometerInterface.h"

namespace Mengine
{
    //////////////////////////////////////////////////////////////////////////
    class Chronometer
        : public ChronometerInterface
    {
        DECLARE_FACTORABLE("Chronometer")

    public:
        Chronometer();
        ~Chronometer() override;

    public:
        void initialize( const LambdaChronometer & _lambda );
        
    public:
        void update( Timestamp _currentTime );

    public:
        bool pause() override;
        bool resume() override;

        bool isPause() const override;

        Timestamp getTime() const override;

    protected:
        LambdaChronometer m_lambda;

        Timestamp m_timeBegin;
        Timestamp m_timePauseBegin;
        Timestamp m_timePauseCollect;

        Timestamp m_oldTime;
    };
    //////////////////////////////////////////////////////////////////////////
    typedef IntrusivePtr<Chronometer, ChronometerInterface> ChronometerPtr;
    //////////////////////////////////////////////////////////////////////////
};
