#pragma once

#   include "Interface/ServiceInterface.h"

namespace Mengine
{
	class TimelineServiceInterface
		: public ServiceInterface
	{
		SERVICE_DECLARE( "TimelineService" )

	public:
		virtual void beginOffset( float _offset ) = 0;
		virtual float getOffset() const = 0;
		virtual void endOffset() = 0;

	public:
		virtual float getTime() const = 0;		

	public:
		virtual void tick( float _timing ) = 0;
	};

#   define TIMELINE_SERVICE()\
    ((Mengine::TimelineServiceInterface*)SERVICE_GET(Mengine::TimelineServiceInterface))
}