#pragma once

#include "Interface/ProfilerInterface.h"

#include "optick.h"

namespace Mengine
{
    //////////////////////////////////////////////////////////////////////////
    class OptickThreadProfiler
        : public ProfilerInterface
    {
        DECLARE_FACTORABLE( OptickThreadProfiler );

    public:
        OptickThreadProfiler();
        ~OptickThreadProfiler() override;

    public:
        bool initialize( const Char * _name );
    };
    //////////////////////////////////////////////////////////////////////////
    typedef IntrusivePtr<OptickThreadProfiler, ProfilerInterface> OptickThreadProfilerPtr;
    //////////////////////////////////////////////////////////////////////////
}