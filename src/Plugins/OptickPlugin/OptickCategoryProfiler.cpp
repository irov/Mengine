#include "OptickCategoryProfiler.h"

#include "OptickProfilerDescription.h"

namespace Mengine
{
    //////////////////////////////////////////////////////////////////////////
    OptickCategoryProfiler::OptickCategoryProfiler()
        : m_data( nullptr )
    {
    }
    //////////////////////////////////////////////////////////////////////////
    OptickCategoryProfiler::~OptickCategoryProfiler()
    {
        Optick::Event::Stop( *m_data );
    }
    //////////////////////////////////////////////////////////////////////////
    bool OptickCategoryProfiler::initialize( const ProfilerDescriptionInterface * _description )
    {
        //OPTICK_CATEGORY("UpdateAI", Optick::Category::AI)

        const OptickProfilerDescription * description = static_cast<const OptickProfilerDescription *>(_description);

        Optick::EventDescription * optickDesc = description->getOptickDescription();

        m_data = Optick::Event::Start( *optickDesc );

        return true;
    }
    //////////////////////////////////////////////////////////////////////////
}