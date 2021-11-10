#pragma once

#include "Interface/ServiceInterface.h"

#include "Kernel/Unknowable.h"

#include "Config/Char.h"

namespace Mengine
{
    class AnalyticsSystemInterface
        : public ServiceInterface
        , public Unknowable
    {
        SERVICE_DECLARE( "AnalyticsSystem" )

    public:
        virtual void startProgressionEvent( const Char * _name ) = 0;
        virtual void completeProgressionEvent( const Char * _name ) = 0;
        virtual void failProgressionEvent( const Char * _name ) = 0;

    public:
        virtual void addDesignEvent( const Char * _event ) = 0;
    };
}
//////////////////////////////////////////////////////////////////////////
#define ANALYTICS_SYSTEM()\
    ((Mengine::AnalyticsSystemInterface *)SERVICE_GET(Mengine::AnalyticsSystemInterface))
//////////////////////////////////////////////////////////////////////////