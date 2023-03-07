#include "ModuleDebugPanel.h"

#include "Interface/RenderMaterialServiceInterface.h"

#include "Kernel/StatisticHelper.h"
#include "Kernel/TimeHelper.h"

#include "Config/StdIntTypes.h"
#include "Config/StdIO.h"

#include "implot.h"

namespace Mengine
{
    //////////////////////////////////////////////////////////////////////////
    ModuleDebugPanel::ModuleDebugPanel()
        : m_showDebugPanel( false )
    {
    }
    //////////////////////////////////////////////////////////////////////////
    ModuleDebugPanel::~ModuleDebugPanel()
    {
    }
    //////////////////////////////////////////////////////////////////////////
    bool ModuleDebugPanel::_initializeModule()
    {
        const ImGUIRenderProviderInterfacePtr & imguiRenderProvider = IMGUI_SERVICE()
            ->getRenderProvider();

        m_imguiRenderProvider = imguiRenderProvider;

        return true;
    }
    //////////////////////////////////////////////////////////////////////////
    void ModuleDebugPanel::_finalizeModule()
    {
        m_imguiRenderProvider = nullptr;
    }
    //////////////////////////////////////////////////////////////////////////
    bool ModuleDebugPanel::_handleKeyEvent( const InputKeyEvent & _event )
    {
        if( _event.code == KC_F9 && _event.isDown == true )
        {
            m_showDebugPanel = !m_showDebugPanel;            
        }

        return false;
    }
    //////////////////////////////////////////////////////////////////////////
    void ModuleDebugPanel::_beginUpdate( bool _focus )
    {
        MENGINE_UNUSED( _focus );

        static TimeMilliseconds old_Time = Helper::getTimeMilliseconds();

        TimeMilliseconds current_Time = Helper::getTimeMilliseconds();

        TimeMilliseconds deltha_Time = current_Time - old_Time;

        if( deltha_Time > 1000 )
        {
            old_Time = current_Time;

            static int64_t old_RenderFrameCount = 0;

            int64_t Statistic_RenderFrameCount = STATISTIC_GET_INTEGER( STATISTIC_RENDER_FRAME_COUNT );

            int64_t deltha_RenderFrameCount = Statistic_RenderFrameCount - old_RenderFrameCount;
            old_RenderFrameCount = Statistic_RenderFrameCount;

            float fps = float( deltha_RenderFrameCount ) * 1000.f / float( deltha_Time );

            m_histogramFPS.add( fps );
        }
    }
    //////////////////////////////////////////////////////////////////////////
    void ModuleDebugPanel::_render( const RenderPipelineInterfacePtr & _renderPipeline, const RenderContext * _context )
    {
        if( m_showDebugPanel == false )
        {
            return;
        }

        const RenderMaterialInterfacePtr & renderMaterial = RENDERMATERIAL_SERVICE()
            ->getDebugTriangleMaterial();

        _renderPipeline->addRenderExternal( _context, renderMaterial, nullptr, RenderExternalInterfacePtr::from( this ), MENGINE_DOCUMENT_FACTORABLE );
    }
    //////////////////////////////////////////////////////////////////////////
    void ModuleDebugPanel::onRenderExternal() const
    {
        m_imguiRenderProvider->newFrame();

        ImGui::Begin( "debug panel" );

        float currentFPS = m_histogramFPS.getLastValue();
        float maxFPS = m_histogramFPS.getMaxValue();
        float histogramFPSHeight = maxFPS > 80.f ? maxFPS : 80.f;

        const float * values = m_histogramFPS.getValues();

        Char histogramFPSTitle[32] = {'\0'};
        MENGINE_SNPRINTF( histogramFPSTitle, 32, "FPS: %.2f", currentFPS );

        if( ImPlot::BeginPlot( "##DebugPanel_FPS" ) )
        {
            ImPlot::SetupAxes( "time", "fps" );
            ImPlot::SetupAxesLimits( 0.0, (double)MENGINE_DEBUG_PANEL_HISTOGRAM_COUNT, 0.0, (double)histogramFPSHeight );

            ImPlot::PlotBars( histogramFPSTitle, values, MENGINE_DEBUG_PANEL_HISTOGRAM_COUNT );

            ImPlot::EndPlot();
        }

        ImGui::End();

        m_imguiRenderProvider->endFrame();
    }
    //////////////////////////////////////////////////////////////////////////
}
