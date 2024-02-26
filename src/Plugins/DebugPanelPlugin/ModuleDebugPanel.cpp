#include "ModuleDebugPanel.h"

#include "Interface/RenderMaterialServiceInterface.h"

#include "Kernel/StatisticHelper.h"
#include "Kernel/TimestampHelper.h"

#include "Config/StdIntTypes.h"
#include "Config/StdIO.h"

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
    void ModuleDebugPanel::updateHistogramUpdate( HistogramUpdate * const _histogram, uint32_t _statisticId, float _coeffTime, float _multiplier )
    {
        static int64_t old_value[MENGINE_STATISTIC_MAX_COUNT] = {0};

        int64_t Statistic_Value = STATISTIC_GET_INTEGER( _statisticId );

        int64_t delta_value = Statistic_Value - old_value[_statisticId];
        old_value[_statisticId] = Statistic_Value;

        float value = float( delta_value ) * _coeffTime * _multiplier;

        _histogram->add( value );
    }
    //////////////////////////////////////////////////////////////////////////
    void ModuleDebugPanel::drawHistogramUpdate( const HistogramUpdate & _histogram, const Char * _overlayFormat, float _maxValue, float _height ) const
    {
        float currentValue = _histogram.getLastValue();
        const float * values = _histogram.getValues();

        Char overlayText[32] = {'\0'};
        MENGINE_SNPRINTF( overlayText, 32, _overlayFormat, currentValue );

        ImGui::PlotHistogram( "", values, MENGINE_DEBUG_PANEL_HISTOGRAM_UPDATE_COUNT, 0, overlayText, 0.f, _maxValue, ImVec2( 0, _height ) );
    }
    //////////////////////////////////////////////////////////////////////////
    void ModuleDebugPanel::drawHistogramPerFrame( const HistogramPerframe & _histogram, const Char * _overlayFormat, float _maxValue, float _height ) const
    {
        float currentValue = _histogram.getLastValue();
        const float * values = _histogram.getValues();

        Char overlayText[32] = {'\0'};
        MENGINE_SNPRINTF( overlayText, 32, _overlayFormat, currentValue );

        ImGui::PlotHistogram( "", values, MENGINE_DEBUG_PANEL_HISTOGRAM_PERFRAME_COUNT, 0, overlayText, 0.f, _maxValue, ImVec2( 0, _height ) );
    }
    //////////////////////////////////////////////////////////////////////////
    void ModuleDebugPanel::_beginUpdate( bool _focus )
    {
        MENGINE_UNUSED( _focus );

        static Timestamp old_Time = Helper::getSystemTimestamp();

        Timestamp current_Time = Helper::getSystemTimestamp();

        Timestamp delta_Time = current_Time - old_Time;

        if( delta_Time > 1000 )
        {
            old_Time = current_Time;

            float coeffTime = 1000.f / (float)delta_Time;

            this->updateHistogramUpdate( &m_histogramFPS, STATISTIC_RENDER_FRAME_COUNT, coeffTime, 1.f );
            this->updateHistogramUpdate( &m_histogramAllocatorNew, STATISTIC_ALLOCATOR_NEW, coeffTime, 1.f / 1024.f );
            this->updateHistogramUpdate( &m_histogramAllocatorFree, STATISTIC_ALLOCATOR_FREE, coeffTime, 1.f / 1024.f );
            this->updateHistogramUpdate( &m_histogramImageNew, STATISTIC_RENDER_IMAGE_ALLOC_NEW, coeffTime, 1.f );
            this->updateHistogramUpdate( &m_histogramImageFree, STATISTIC_RENDER_IMAGE_ALLOC_FREE, coeffTime, 1.f );
        }
    }
    //////////////////////////////////////////////////////////////////////////
    void ModuleDebugPanel::_render( const RenderPipelineInterfacePtr & _renderPipeline, const RenderContext * _context )
    {
        if( m_showDebugPanel == false )
        {
            return;
        }

        int64_t Statistic_PerFrame_DrawIndexPrimitives = STATISTIC_GET_INTEGER( STATISTIC_RENDER_PERFRAME_DRAWINDEXPRIMITIVES );
        m_histogramPerFrameDrawIndexPrimitives.add( (float)Statistic_PerFrame_DrawIndexPrimitives );

        int64_t Statistic_PerFrame_Objects = STATISTIC_GET_INTEGER( STATISTIC_RENDER_PERFRAME_OBJECTS );
        m_histogramPerFrameObjects.add( (float)Statistic_PerFrame_Objects );

        int64_t Statistic_PerFrame_Triangles = STATISTIC_GET_INTEGER( STATISTIC_RENDER_PERFRAME_TRIANGLES );
        m_histogramPerFrameTriangles.add( (float)Statistic_PerFrame_Triangles );

        int64_t Statistic_PerFrame_Batches = STATISTIC_GET_INTEGER( STATISTIC_RENDER_PERFRAME_BATCHES );
        m_histogramPerFrameBatches.add( (float)Statistic_PerFrame_Batches );

        double Statistic_PerFrame_Fillrate = STATISTIC_GET_DOUBLE( STATISTIC_RENDER_PERFRAME_FILLRATE );

        const Viewport & viewport = _context->viewport->getViewport();

        float viewportWidth = viewport.getWidth();
        float viewportHeight = viewport.getHeight();

        Statistic_PerFrame_Fillrate /= viewportWidth;
        Statistic_PerFrame_Fillrate /= viewportHeight;

        m_histogramPerFrameFillrate.add( (float)Statistic_PerFrame_Fillrate );

        const RenderMaterialInterfacePtr & renderMaterial = RENDERMATERIAL_SERVICE()
            ->getDebugTriangleMaterial();

        _renderPipeline->addRenderExternal( _context, renderMaterial, nullptr, RenderDrawPrimitiveInterfacePtr::from( this ), MENGINE_DOCUMENT_FACTORABLE );
    }
    //////////////////////////////////////////////////////////////////////////
    void ModuleDebugPanel::onRenderDrawPrimitives( const RenderPrimitive * _primitives, uint32_t _count ) const
    {
        MENGINE_UNUSED( _primitives );
        MENGINE_UNUSED( _count );

        m_imguiRenderProvider->newFrame();

        ImGui::Begin( "debug panel" );

        float maxFPS = m_histogramFPS.getMaxValue();
        float histogramFPSHeight = maxFPS > 80.f ? maxFPS : 80.f;

        this->drawHistogramUpdate( m_histogramFPS, "FPS: %.2f", histogramFPSHeight, 80.f );
        this->drawHistogramUpdate( m_histogramAllocatorNew, "Memory alloc: %.2fkb", FLT_MAX, 60.f );
        this->drawHistogramUpdate( m_histogramAllocatorFree, "Memory free: %.2fkb", FLT_MAX, 60.f );

        int64_t Statistic_AllocatorSize = STATISTIC_GET_INTEGER( STATISTIC_ALLOCATOR_SIZE );
        ImGui::Text( "Allocator size: %lldmb %lldkb", Statistic_AllocatorSize / (1024 * 1024), (Statistic_AllocatorSize % (1024 * 1024)) / 1024 );

        this->drawHistogramUpdate( m_histogramImageNew, "Image new: %.0f", FLT_MAX, 60.f );
        this->drawHistogramUpdate( m_histogramImageFree, "Image free: %.0f", FLT_MAX, 60.f );

        int64_t Statistic_Render_ImageSize = STATISTIC_GET_INTEGER( STATISTIC_RENDER_IMAGE_ALLOC_SIZE );
        ImGui::Text( "Image size: %lld", Statistic_Render_ImageSize );

        this->drawHistogramPerFrame( m_histogramPerFrameDrawIndexPrimitives, "DIP: %.0f", FLT_MAX, 50.f );

        float maxFillrate = m_histogramPerFrameFillrate.getMaxValue();
        float histogramFillrateHeight = maxFillrate > 10.f ? maxFillrate : 13.f;

        this->drawHistogramPerFrame( m_histogramPerFrameFillrate, "Fillrate: %.2f", histogramFillrateHeight, 50.f );
        this->drawHistogramPerFrame( m_histogramPerFrameObjects, "Objects: %.0f", FLT_MAX, 50.f );
        this->drawHistogramPerFrame( m_histogramPerFrameTriangles, "Triangles: %.0f", FLT_MAX, 50.f );
        this->drawHistogramPerFrame( m_histogramPerFrameBatches, "Batches: %.0f", FLT_MAX, 50.f );

        ImGui::End();

        m_imguiRenderProvider->endFrame();
    }
    //////////////////////////////////////////////////////////////////////////
}
