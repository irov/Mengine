#pragma once

#include "Interface/ServiceInterface.h"

#include "Kernel/UpdateContext.h"

namespace Mengine
{
    class TimelineServiceInterface
        : public ServiceInterface
    {
        SERVICE_DECLARE( "TimelineService" )

    public:
        virtual void beginOffset( float _offset, const DocumentPtr & _doc ) = 0;
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
        virtual void setTimeFactor( float _timeFactor ) = 0;
        virtual float getTimeFactor() const = 0;
    };
}
//////////////////////////////////////////////////////////////////////////
#define TIMELINE_SERVICE()\
    ((Mengine::TimelineServiceInterface*)SERVICE_GET(Mengine::TimelineServiceInterface))
//////////////////////////////////////////////////////////////////////////