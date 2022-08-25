#pragma once

#include "Interface/ProfilerInterface.h"
#include "Interface/ProfilerDescriptionInterface.h"

#include "optick.h"

namespace Mengine
{
    //////////////////////////////////////////////////////////////////////////
    class OptickCategoryProfiler
        : public ProfilerInterface
    {
        DECLARE_FACTORABLE( OptickCategoryProfiler );

    public:
        OptickCategoryProfiler();
        ~OptickCategoryProfiler() override;

    public:
        bool initialize( const ProfilerDescriptionInterface * _description );

    protected:
        Optick::EventData * m_data;
    };
    //////////////////////////////////////////////////////////////////////////
    typedef IntrusivePtr<OptickCategoryProfiler, ProfilerInterface> OptickCategoryProfilerPtr;
    //////////////////////////////////////////////////////////////////////////
}