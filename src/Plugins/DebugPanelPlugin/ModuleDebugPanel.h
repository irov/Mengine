#pragma once

#include "Plugins/ImGUIPlugin/ImGUIInterface.h"

#include "Kernel/ModuleBase.h"
#include "Kernel/Histogram.h"

#ifndef MENGINE_DEBUG_PANEL_HISTOGRAM_UPDATE_COUNT
#define MENGINE_DEBUG_PANEL_HISTOGRAM_UPDATE_COUNT 32
#endif

#ifndef MENGINE_DEBUG_PANEL_HISTOGRAM_PERFRAME_COUNT
#define MENGINE_DEBUG_PANEL_HISTOGRAM_PERFRAME_COUNT 120
#endif

namespace Mengine
{
    class ModuleDebugPanel
        : public ModuleBase
        , public RenderExternalInterface
    {
        DECLARE_FACTORABLE( ModuleDebugPanel );

    public:
        ModuleDebugPanel();
        ~ModuleDebugPanel() override;

    public:
        bool _initializeModule() override;
        void _finalizeModule() override;

    protected:
        bool _handleKeyEvent( const InputKeyEvent & _event ) override;

    protected:
        void _beginUpdate( bool _focus ) override;

    protected:
        void _render( const RenderPipelineInterfacePtr & _renderPipeline, const RenderContext * _context ) override;

    protected:
        void onRenderExternal() const override;

    protected:
        typedef Histogram<float, MENGINE_DEBUG_PANEL_HISTOGRAM_UPDATE_COUNT> HistogramUpdate;
        typedef Histogram<float, MENGINE_DEBUG_PANEL_HISTOGRAM_PERFRAME_COUNT> HistogramPerframe;

    protected:
        void updateHistogramUpdate( HistogramUpdate * const _histogram, uint32_t _statisticId, float _coeffTime, float _multiplier );

    protected:
        void drawHistogramUpdate( const HistogramUpdate & _histogram, const Char * _overlayFormat, float _maxValue, float _height ) const;
        void drawHistogramPerFrame( const HistogramPerframe & _histogram, const Char * _overlayFormat, float _maxValue, float _height ) const;

    protected:
        ImGUIRenderProviderInterfacePtr m_imguiRenderProvider;

        mutable HistogramUpdate m_histogramFPS;
        mutable HistogramUpdate m_histogramAllocatorNew;
        mutable HistogramUpdate m_histogramAllocatorFree;

        mutable HistogramPerframe m_histogramPerFrameDrawIndexPrimitives;
        mutable HistogramPerframe m_histogramPerFrameObjects;
        mutable HistogramPerframe m_histogramPerFrameTriangles;
        mutable HistogramPerframe m_histogramPerFrameBatches;

        mutable HistogramPerframe m_histogramPerFrameFillrate;

        bool m_showDebugPanel;
    };
}