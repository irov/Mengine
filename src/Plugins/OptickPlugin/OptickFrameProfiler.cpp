#include "OptickFrameProfiler.h"

#include "OptickProfilerDescription.h"

namespace Mengine
{
    //////////////////////////////////////////////////////////////////////////
    OptickFrameProfiler::OptickFrameProfiler()
        : m_data( nullptr )
    {
    }
    //////////////////////////////////////////////////////////////////////////
    OptickFrameProfiler::~OptickFrameProfiler()
    {
        Optick::Event::Stop( *m_data );
    }
    //////////////////////////////////////////////////////////////////////////
    bool OptickFrameProfiler::initialize( ProfilerDescriptionInterface * _description )
    {
        //OPTICK_FRAME( "MainThread" );

        OptickProfilerDescription * description = static_cast<OptickProfilerDescription *>(_description);

        Optick::EndFrame();
        Optick::Update();

        uint32_t frameNumber = Optick::BeginFrame();

        m_data = Optick::Event::Start( *::Optick::GetFrameDescription( Optick::FrameType::CPU ) );

        Optick::EventDescription * optickDescription = description->getOptickDescription();

        Optick::Tag::Attach( *optickDescription, frameNumber );

        return true;
    }
    //////////////////////////////////////////////////////////////////////////
}