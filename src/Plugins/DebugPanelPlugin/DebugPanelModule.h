#pragma once

#include "Frameworks/MosaicFramework/MosaicInterface.h"

#include "Kernel/ModuleBase.h"
#include "Kernel/Histogram.h"
#include "Kernel/PathString.h"
#include "Kernel/ConstString.h"
#include "Kernel/Deque.h"
#include "Kernel/String.h"

#include "Config/Timestamp.h"
#include "Config/UniqueId.h"

#include "Mosaic/Mosaic.hpp"

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
        typedef Histogram<float, MENGINE_DEBUG_PANEL_HISTOGRAM_UPDATE_COUNT> HistogramUpdate;
        typedef Histogram<float, MENGINE_DEBUG_PANEL_HISTOGRAM_PERFRAME_COUNT> HistogramPerframe;

    protected:
        void updateHistogramUpdate( HistogramUpdate * const _histogram, uint32_t _statisticId, float _coeffTime, float _multiplier );

    protected:
        void drawHistogramUpdate( Mosaic::Context * _ui, const HistogramUpdate & _histogram, const Char * _label, const Char * _overlayFormat, float _maxValue, float _height );
        void drawHistogramPerFrame( Mosaic::Context * _ui, const HistogramPerframe & _histogram, const Char * _label, const Char * _overlayFormat, float _maxValue, float _height );

    protected:
        void renderPanel_( Mosaic::Context * _ui );
        void renderDebugPanel_( Mosaic::Context * _ui );
        void renderTextureMonitor_( Mosaic::Context * _ui );
        void renderResourceMonitor_( Mosaic::Context * _ui );

    protected:
        void textLine_( Mosaic::Context * _ui, const Char * _format, ... );
        Mosaic::StringView keepLine_( const Char * _format, ... );

    protected:
        Mosaic::Color getBorderColor_( Mosaic::Context * _ui, Timestamp _alive ) const;

    protected:
        MosaicProviderId m_providerId;

        HistogramUpdate m_histogramFPS;
        HistogramUpdate m_histogramAllocatorNew;
        HistogramUpdate m_histogramAllocatorFree;
        HistogramUpdate m_histogramImageNew;
        HistogramUpdate m_histogramImageFree;

        HistogramPerframe m_histogramPerFrameDrawIndexPrimitives;
        HistogramPerframe m_histogramPerFrameObjects;
        HistogramPerframe m_histogramPerFrameTriangles;
        HistogramPerframe m_histogramPerFrameBatches;

        HistogramPerframe m_histogramPerFrameFillrate;

        Timestamp m_warning;
        Timestamp m_critical;

        UniqueId m_selectedTextureId;
        PathString m_selectedPath;

        ConstString m_selectedResourceGroup;
        ConstString m_selectedResourceName;
        PathString m_selectedResourcePath;
        int32_t m_filterResourceCompileRef;

        typedef Deque<String> DequeReadout;
        DequeReadout m_readout;

        bool m_show;
        int32_t m_selectedTab;
    };
}
