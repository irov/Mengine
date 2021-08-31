#pragma once

#include "Interface/ProfilerInterface.h"
#include "Interface/ProfilerDescriptionInterface.h"

#include "optick.h"

namespace Mengine
{
    //////////////////////////////////////////////////////////////////////////
    class OptickFrameProfiler
        : public ProfilerInterface
    {
        DECLARE_FACTORABLE( OptickFrameProfiler );

    public:
        OptickFrameProfiler();
        ~OptickFrameProfiler() override;

    public:
        bool initialize( ProfilerDescriptionInterface * _description );

    protected:
        Optick::EventData * m_data;
    };
    //////////////////////////////////////////////////////////////////////////
    typedef IntrusivePtr<OptickFrameProfiler, ProfilerInterface> OptickFrameProfilerPtr;
    //////////////////////////////////////////////////////////////////////////
}