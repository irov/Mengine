#pragma once

#include "Interface/ServiceInterface.h"

#include "Kernel/UpdateContext.h"
#include "Kernel/DocumentHelper.h"

namespace Mengine
{
    class TimelineServiceInterface
        : public ServiceInterface
    {
        SERVICE_DECLARE( "TimelineService" )

    public:
        virtual void beginOffset( float _offset, const DocumentInterfacePtr & _doc ) = 0;
        virtual float getOffset() const = 0;
        virtual void endOffset() = 0;

    public:
        virtual uint32_t getCurrentRevision() const = 0;
        virtual float getCurrentTime() const = 0;
        virtual float getCurrentDelta() const = 0;

    public:
        virtual float getTotalTime() const = 0;

    public:
        virtual void begin( const UpdateContext * _context ) = 0;
        virtual void end() = 0;

    public:
        virtual float calcTimeFactor() const = 0;

    public:
        virtual void setTimeFactorBase( float _timeFactor ) = 0;
        virtual float getTimeFactorBase() const = 0;
        virtual void setTimeFactorCoefficient( float _timeFactorCoefficient ) = 0;
        virtual float getTimeFactorCoefficient() const = 0;
        virtual void setTimeFactorCount( int32_t _timeFactorCount ) = 0;
        virtual int32_t getTimeFactorCount() const = 0;
    };
}
//////////////////////////////////////////////////////////////////////////
#define TIMELINE_SERVICE()\
    ((Mengine::TimelineServiceInterface*)SERVICE_GET(Mengine::TimelineServiceInterface))
//////////////////////////////////////////////////////////////////////////