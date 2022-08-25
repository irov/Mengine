#pragma once

#include "Interface/ServiceInterface.h"
#include "Interface/ProfilerInterface.h"
#include "Interface/ProfilerDescriptionInterface.h"

#include "Kernel/Unknowable.h"

namespace Mengine
{
    class ProfilerSystemInterface
        : public ServiceInterface
        , public Unknowable
    {
        SERVICE_DECLARE( "ProfilerSystem" )

    public:
        virtual void beginApplication() = 0;
        virtual void endApplication() = 0;

        virtual ProfilerInterfacePtr addThread( const Char * _name ) = 0;
        virtual ProfilerInterfacePtr addFrame( const ProfilerDescriptionInterface * _description ) = 0;
        virtual ProfilerInterfacePtr addCategory( const ProfilerDescriptionInterface * _description ) = 0;

    public:
        virtual const ProfilerDescriptionInterface * createDescription( const Char * _name, const Char * _file, uint32_t _line ) = 0;
    };
}
//////////////////////////////////////////////////////////////////////////
#define PROFILER_SYSTEM()\
    ((Mengine::ProfilerSystemInterface *)SERVICE_GET(Mengine::ProfilerSystemInterface))
//////////////////////////////////////////////////////////////////////////