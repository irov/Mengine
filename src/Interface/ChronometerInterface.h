#pragma once

#include "Interface/ServantInterface.h"

#include "Config/Lambda.h"
#include "Config/Timestamp.h"

namespace Mengine
{
    //////////////////////////////////////////////////////////////////////////
    typedef Lambda<void( Timestamp _time )> LambdaChronometer;
    //////////////////////////////////////////////////////////////////////////
    class ChronometerInterface
        : public ServantInterface
    {
    public:
        virtual bool pause() = 0;
        virtual bool resume() = 0;

        virtual bool isPause() const = 0;

    public:
        virtual Timestamp getTime() const = 0;
    };
    //////////////////////////////////////////////////////////////////////////
    typedef IntrusivePtr<ChronometerInterface> ChronometerInterfacePtr;
    //////////////////////////////////////////////////////////////////////////
}