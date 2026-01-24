#pragma once

#include "Frameworks/ImGUIFramework/ImGUIInterface.h"

#include "Kernel/ModuleBase.h"
#include "Kernel/Histogram.h"
#include "Kernel/PathString.h"
#include "Kernel/ConstString.h"

#include "Config/Timestamp.h"
#include "Config/UniqueId.h"

#include "imgui.h"

#ifndef MENGINE_DEBUG_PANEL_HISTOGRAM_UPDATE_COUNT
#define MENGINE_DEBUG_PANEL_HISTOGRAM_UPDATE_COUNT 32
#endif

#ifndef MENGINE_DEBUG_PANEL_HISTOGRAM_PERFRAME_COUNT
#define MENGINE_DEBUG_PANEL_HISTOGRAM_PERFRAME_COUNT 120
#endif

namespace Mengine
{
    class DebugPanelModule
        : public ModuleBase
        , public RenderDrawPrimitiveInterface
    {
        DECLARE_FACTORABLE( DebugPanelModule );

    public:
        DebugPanelModule();
        ~DebugPanelModule() override;

    public:
        bool _initializeModule() override;
        void _finalizeModule() override;

    protected:
        bool _handleKeyEvent( const InputKeyEvent & _event ) override;

    protected:
        void _preUpdate() override;

    protected:
        void _render( const RenderPipelineInterfacePtr & _renderPipeline, const RenderContext * _context ) override;

    protected:
        void onRenderDrawPrimitives( const RenderPrimitive * _primitives, uint32_t _count ) const override;

    protected:
        typedef Histogram<float, MENGINE_DEBUG_PANEL_HISTOGRAM_UPDATE_COUNT> HistogramUpdate;
        typedef Histogram<float, MENGINE_DEBUG_PANEL_HISTOGRAM_PERFRAME_COUNT> HistogramPerframe;

    protected:
        void updateHistogramUpdate( HistogramUpdate * const _histogram, uint32_t _statisticId, float _coeffTime, float _multiplier );

    protected:
        void drawHistogramUpdate( const HistogramUpdate & _histogram, const Char * _label, const Char * _overlayFormat, float _maxValue, float _height ) const;
        void drawHistogramPerFrame( const HistogramPerframe & _histogram, const Char * _label, const Char * _overlayFormat, float _maxValue, float _height ) const;

    protected:
        void renderDebugPanel() const;
        void renderTextureMonitor() const;
        void renderResourceMonitor() const;

    protected:
        ImVec4 getBorderColor_( Timestamp _alive ) const;

    protected:
        ImGUIRenderProviderInterfacePtr m_imguiRenderProvider;

        mutable HistogramUpdate m_histogramFPS;
        mutable HistogramUpdate m_histogramAllocatorNew;
        mutable HistogramUpdate m_histogramAllocatorFree;
        mutable HistogramUpdate m_histogramImageNew;
        mutable HistogramUpdate m_histogramImageFree;

        mutable HistogramPerframe m_histogramPerFrameDrawIndexPrimitives;
        mutable HistogramPerframe m_histogramPerFrameObjects;
        mutable HistogramPerframe m_histogramPerFrameTriangles;
        mutable HistogramPerframe m_histogramPerFrameBatches;

        mutable HistogramPerframe m_histogramPerFrameFillrate;

        Timestamp m_warning;
        Timestamp m_critical;

        mutable UniqueId m_selectedTextureId;
        mutable PathString m_selectedPath;

        mutable ConstString m_selectedResourceGroup;
        mutable ConstString m_selectedResourceName;
        mutable PathString m_selectedResourcePath;
        mutable bool m_filterResourceCompileRefCountGT1{ false };

        bool m_show;
        mutable int32_t m_selectedTab;
    };
}