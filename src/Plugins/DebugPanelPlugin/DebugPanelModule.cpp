#include "DebugPanelModule.h"

#include "Interface/RenderMaterialServiceInterface.h"

#include "Kernel/Logger.h"
#include "Interface/RenderTextureServiceInterface.h"
#include "Interface/ResourceServiceInterface.h"
#include "Interface/FileGroupInterface.h"

#include "Kernel/StatisticHelper.h"
#include "Kernel/TimestampHelper.h"
#include "Kernel/Assertion.h"
#include "Kernel/ConfigHelper.h"
#include "Kernel/ContentHelper.h"
#include "Kernel/ConstStringHelper.h"
#include "Kernel/PixelFormatHelper.h"
#include "Kernel/UnorderedConstStringMap.h"
#include "Kernel/Vector.h"

#include "Config/Limits.h"
#include "Config/StdIntTypes.h"
#include "Config/StdIO.h"
#include "Config/StdAlgorithm.h"
#include "Config/StdArg.h"
#include "Config/StdString.h"
#include "Config/Timestamp.h"

namespace Mengine
{
    //////////////////////////////////////////////////////////////////////////
    DebugPanelModule::DebugPanelModule()
        : m_providerId( MOSAIC_INVALID_PROVIDER_ID )
        , m_warning( 0 )
        , m_critical( 0 )
        , m_selectedTextureId( INVALID_UNIQUE_ID )
        , m_filterResourceCompileRef( 0 )
        , m_show( false )
        , m_selectedTab( 0 )
    {
    }
    //////////////////////////////////////////////////////////////////////////
    DebugPanelModule::~DebugPanelModule()
    {
    }
    //////////////////////////////////////////////////////////////////////////
    bool DebugPanelModule::_initializeModule()
    {
        uint32_t TextureMonitor_WarningSeconds = CONFIG_VALUE_INTEGER( "TextureMonitorPlugin", "WarningSeconds", MENGINE_UINT32_C( 60 ) );
        uint32_t TextureMonitor_CriticalSeconds = CONFIG_VALUE_INTEGER( "TextureMonitorPlugin", "CriticalSeconds", MENGINE_UINT32_C( 300 ) );

        MENGINE_ASSERTION_FATAL( TextureMonitor_WarningSeconds != 0, "invalid warning seconds" );
        MENGINE_ASSERTION_FATAL( TextureMonitor_CriticalSeconds != 0, "invalid critical seconds" );

        if( TextureMonitor_CriticalSeconds < TextureMonitor_WarningSeconds )
        {
            TextureMonitor_CriticalSeconds = TextureMonitor_WarningSeconds;
        }

        m_warning = TextureMonitor_WarningSeconds * TIMESTAMP_MILLISECONDS_SECOND64;
        m_critical = TextureMonitor_CriticalSeconds * TIMESTAMP_MILLISECONDS_SECOND64;

        m_providerId = MOSAIC_SERVICE()
            ->addProvider( [this]( Mosaic::Context * _ui )
        {
            this->renderPanel_( _ui );
        } );

        return true;
    }
    //////////////////////////////////////////////////////////////////////////
    void DebugPanelModule::_finalizeModule()
    {
        MOSAIC_SERVICE()
            ->removeProvider( m_providerId );

        m_providerId = MOSAIC_INVALID_PROVIDER_ID;

        m_readout.clear();
    }
    //////////////////////////////////////////////////////////////////////////
    bool DebugPanelModule::_handleKeyEvent( const InputKeyEvent & _event )
    {
        if( _event.code == KC_F9 && _event.isDown == true )
        {
            m_show = !m_show;
        }

        return false;
    }
    //////////////////////////////////////////////////////////////////////////
    Mosaic::StringView DebugPanelModule::keepLine_( const Char * _format, ... )
    {
        Char buffer[1024] = {'\0'};

        MENGINE_VA_LIST_TYPE args;
        MENGINE_VA_LIST_START( args, _format );
        MENGINE_VSNPRINTF( buffer, sizeof( buffer ) - 1, _format, args );
        MENGINE_VA_LIST_END( args );

        m_readout.emplace_back( buffer );

        const String & line = m_readout.back();

        return Mosaic::StringView( line.c_str(), line.size() );
    }
    //////////////////////////////////////////////////////////////////////////
    void DebugPanelModule::textLine_( Mosaic::Context * _ui, const Char * _format, ... )
    {
        Char buffer[1024] = {'\0'};

        MENGINE_VA_LIST_TYPE args;
        MENGINE_VA_LIST_START( args, _format );
        MENGINE_VSNPRINTF( buffer, sizeof( buffer ) - 1, _format, args );
        MENGINE_VA_LIST_END( args );

        m_readout.emplace_back( buffer );

        const String & line = m_readout.back();

        Mosaic::Scope lineScope = Mosaic::scope( _ui, Mosaic::Key( m_readout.size() ) );

        Mosaic::text( _ui, Mosaic::StringView( line.c_str(), line.size() ) );
    }
    //////////////////////////////////////////////////////////////////////////
    void DebugPanelModule::updateHistogramUpdate( HistogramUpdate * const _histogram, uint32_t _statisticId, float _coeffTime, float _multiplier )
    {
        static int64_t old_value[MENGINE_STATISTIC_MAX_COUNT] = {MENGINE_INT64_C( 0 )};

        int64_t Statistic_Value = STATISTIC_GET_INTEGER( _statisticId );

        int64_t delta_value = Statistic_Value - old_value[_statisticId];
        old_value[_statisticId] = Statistic_Value;

        float value = float( delta_value ) * _coeffTime * _multiplier;

        _histogram->add( value );
    }
    //////////////////////////////////////////////////////////////////////////
    void DebugPanelModule::drawHistogramUpdate( Mosaic::Context * _ui, const HistogramUpdate & _histogram, const Char * _label, const Char * _overlayFormat, float _maxValue, float _height )
    {
        float currentValue = _histogram.getLastValue();
        const float * values = _histogram.getValues();

        Mosaic::PlotOptions options;
        options.height = _height;
        options.minimum = 0.f;
        options.maximum = _maxValue == MENGINE_FLT_MAX ? _histogram.getMaxValue() : _maxValue;
        options.overlay = this->keepLine_( _overlayFormat, currentValue );

        Mosaic::Scope histogramScope = Mosaic::scope( _ui, Mosaic::Key( Mosaic::StringView( _label ) ) );

        Mosaic::plotHistogram( _ui, _label, Mosaic::ConstFloatSpan( values, MENGINE_DEBUG_PANEL_HISTOGRAM_UPDATE_COUNT ), options );
    }
    //////////////////////////////////////////////////////////////////////////
    void DebugPanelModule::drawHistogramPerFrame( Mosaic::Context * _ui, const HistogramPerframe & _histogram, const Char * _label, const Char * _overlayFormat, float _maxValue, float _height )
    {
        float currentValue = _histogram.getLastValue();
        const float * values = _histogram.getValues();

        Mosaic::PlotOptions options;
        options.height = _height;
        options.minimum = 0.f;
        options.maximum = _maxValue == MENGINE_FLT_MAX ? _histogram.getMaxValue() : _maxValue;
        options.overlay = this->keepLine_( _overlayFormat, currentValue );

        Mosaic::Scope histogramScope = Mosaic::scope( _ui, Mosaic::Key( Mosaic::StringView( _label ) ) );

        Mosaic::plotHistogram( _ui, _label, Mosaic::ConstFloatSpan( values, MENGINE_DEBUG_PANEL_HISTOGRAM_PERFRAME_COUNT ), options );
    }
    //////////////////////////////////////////////////////////////////////////
    void DebugPanelModule::_preUpdate()
    {
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
            this->updateHistogramUpdate( &m_histogramImageNew, STATISTIC_RENDER_TEXTURE_ALLOC_NEW, coeffTime, 1.f );
            this->updateHistogramUpdate( &m_histogramImageFree, STATISTIC_RENDER_TEXTURE_ALLOC_FREE, coeffTime, 1.f );
        }
    }
    //////////////////////////////////////////////////////////////////////////
    void DebugPanelModule::_render( const RenderPipelineInterfacePtr & _renderPipeline, const RenderContext * _context )
    {
        MENGINE_UNUSED( _renderPipeline );

        if( m_show == false )
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

        const Viewport & viewport = _context->viewport->getViewportWM();

        float viewportWidth = viewport.getWidth();
        float viewportHeight = viewport.getHeight();

        Statistic_PerFrame_Fillrate /= viewportWidth;
        Statistic_PerFrame_Fillrate /= viewportHeight;

        m_histogramPerFrameFillrate.add( (float)Statistic_PerFrame_Fillrate );
    }
    //////////////////////////////////////////////////////////////////////////
    void DebugPanelModule::renderPanel_( Mosaic::Context * _ui )
    {
        if( m_show == false )
        {
            return;
        }

        m_readout.clear();

        Mosaic::setNextWindowSize( _ui, {720.f, 520.f}, Mosaic::Condition::FirstUseEver );
        Mosaic::setNextWindowPosition( _ui, {100.f, 100.f}, Mosaic::Condition::FirstUseEver );

        Mosaic::WindowScope window = Mosaic::window( _ui, "Debug Panel" );

        if( window.visible() == false )
        {
            return;
        }

        Mosaic::TabBarScope tabBar = Mosaic::beginTabBar( _ui, "DebugTabs" );

        if( tabBar.visible() == false )
        {
            return;
        }

        if( Mosaic::TreeScope tab = Mosaic::beginTabItem( _ui, "Statistics" ); tab.expanded() == true )
        {
            m_selectedTab = 0;

            this->renderDebugPanel_( _ui );
        }

        if( Mosaic::TreeScope tab = Mosaic::beginTabItem( _ui, "Textures" ); tab.expanded() == true )
        {
            m_selectedTab = 1;

            this->renderTextureMonitor_( _ui );
        }

        if( Mosaic::TreeScope tab = Mosaic::beginTabItem( _ui, "Resources" ); tab.expanded() == true )
        {
            m_selectedTab = 2;

            this->renderResourceMonitor_( _ui );
        }
    }
    //////////////////////////////////////////////////////////////////////////
    void DebugPanelModule::renderDebugPanel_( Mosaic::Context * _ui )
    {
        if( Mosaic::button( _ui, "Print to logs" ).clicked() == true )
        {
            LOGGER_SCOPE_MESSAGES();

            float currentFPS = m_histogramFPS.getLastValue();
            int64_t Statistic_AllocatorSize = STATISTIC_GET_INTEGER( STATISTIC_ALLOCATOR_SIZE );
            int64_t Statistic_Render_ImageSize = STATISTIC_GET_INTEGER( STATISTIC_RENDER_TEXTURE_ALLOC_SIZE );
            float currentDIP = m_histogramPerFrameDrawIndexPrimitives.getLastValue();
            float currentFillrate = m_histogramPerFrameFillrate.getLastValue();
            float currentObjects = m_histogramPerFrameObjects.getLastValue();
            float currentTriangles = m_histogramPerFrameTriangles.getLastValue();
            float currentBatches = m_histogramPerFrameBatches.getLastValue();

            LOGGER_MESSAGE( "DebugPanel [Statistics] FPS: %.2f | Allocator: %" MENGINE_PRId64 " mb %" MENGINE_PRId64 " kb | Image: %" MENGINE_PRId64 " mb %" MENGINE_PRId64 " kb | DIP: %.0f | Fillrate: %.2f | Objects: %.0f | Triangles: %.0f | Batches: %.0f"
                , currentFPS
                , Statistic_AllocatorSize / MENGINE_INT64_C( 1024 * 1024 ), (Statistic_AllocatorSize % MENGINE_INT64_C( 1024 * 1024 )) / 1024
                , Statistic_Render_ImageSize / MENGINE_INT64_C( 1024 * 1024 ), (Statistic_Render_ImageSize % MENGINE_INT64_C( 1024 * 1024 )) / 1024
                , currentDIP, currentFillrate, currentObjects, currentTriangles, currentBatches
            );
        }

        float maxFPS = m_histogramFPS.getMaxValue();
        float histogramFPSHeight = maxFPS > 80.f ? maxFPS : 80.f;

        this->drawHistogramUpdate( _ui, m_histogramFPS, "fps", "FPS: %.2f", histogramFPSHeight, 80.f );

        int64_t Statistic_AllocatorSize = STATISTIC_GET_INTEGER( STATISTIC_ALLOCATOR_SIZE );
        this->textLine_( _ui, "Allocator size: %" MENGINE_PRId64 "mb %" MENGINE_PRId64 "kb", Statistic_AllocatorSize / (1024 * 1024), (Statistic_AllocatorSize % (1024 * 1024)) / 1024 );

        this->drawHistogramUpdate( _ui, m_histogramAllocatorNew, "memalloc", "Memory new: %.2fkb", MENGINE_FLT_MAX, 60.f );
        this->drawHistogramUpdate( _ui, m_histogramAllocatorFree, "memfree", "Memory free: %.2fkb", MENGINE_FLT_MAX, 60.f );

        int64_t Statistic_Render_ImageSize = STATISTIC_GET_INTEGER( STATISTIC_RENDER_TEXTURE_ALLOC_SIZE );
        this->textLine_( _ui, "Image size: %" MENGINE_PRId64 "mb %" MENGINE_PRId64 "kb", Statistic_Render_ImageSize / (1024 * 1024), (Statistic_Render_ImageSize % (1024 * 1024)) / 1024 );

        this->drawHistogramUpdate( _ui, m_histogramImageNew, "imagenew", "Image new: %.2fkb", MENGINE_FLT_MAX, 60.f );
        this->drawHistogramUpdate( _ui, m_histogramImageFree, "imagefree", "Image free: %.2fkb", MENGINE_FLT_MAX, 60.f );

        this->drawHistogramPerFrame( _ui, m_histogramPerFrameDrawIndexPrimitives, "dip", "DIP: %.0f", MENGINE_FLT_MAX, 50.f );

        float maxFillrate = m_histogramPerFrameFillrate.getMaxValue();
        float histogramFillrateHeight = maxFillrate > 10.f ? maxFillrate : 13.f;

        this->drawHistogramPerFrame( _ui, m_histogramPerFrameFillrate, "fillrate", "Fillrate: %.2f", histogramFillrateHeight, 50.f );
        this->drawHistogramPerFrame( _ui, m_histogramPerFrameObjects, "objects", "Objects: %.0f", MENGINE_FLT_MAX, 50.f );
        this->drawHistogramPerFrame( _ui, m_histogramPerFrameTriangles, "triangles", "Triangles: %.0f", MENGINE_FLT_MAX, 50.f );
        this->drawHistogramPerFrame( _ui, m_histogramPerFrameBatches, "batches", "Batches: %.0f", MENGINE_FLT_MAX, 50.f );
    }
    //////////////////////////////////////////////////////////////////////////
    void DebugPanelModule::renderTextureMonitor_( Mosaic::Context * _ui )
    {
        if( SERVICE_IS_INITIALIZE( RenderTextureServiceInterface ) == false )
        {
            this->textLine_( _ui, "RenderTextureService is not initialized" );

            return;
        }

        struct TextureDesc
        {
            RenderTextureInterfacePtr texture;
            FilePath filePath;
            PathString fullPath;
            Timestamp createTimestamp;
            uint32_t width;
            uint32_t height;
            uint32_t memorySize;
        };

        struct GroupDesc
        {
            ConstString name;
            Vector<TextureDesc> textures;
        };

        Vector<GroupDesc> groups;
        UnorderedConstStringMap<size_t> groupIndices;

        const Timestamp now = Helper::getSystemTimestamp();

        RENDERTEXTURE_SERVICE()
            ->foreachTexture( [&groups, &groupIndices]( const RenderTextureInterfacePtr & _texture )
        {
            const ContentInterfacePtr & content = _texture->getContent();

            ConstString groupName = STRINGIZE_STRING_LOCAL( "runtime" );
            FilePath filePath = FilePath::none();
            PathString fullPath( "runtime" );

            if( content != nullptr )
            {
                const ConstString & contentGroupName = content->getGroupName();

                if( contentGroupName.empty() == false )
                {
                    groupName = contentGroupName;
                }

                filePath = content->getFilePath();
                fullPath = Helper::getContentFullPath( content );
            }

            uint32_t memorySize = 0;
            const RenderImageInterfacePtr & image = _texture->getImage();
            if( image != nullptr )
            {
                memorySize = Helper::getTextureMemorySize( image->getHWWidth(), image->getHWHeight(), image->getHWPixelFormat() );
            }

            TextureDesc desc;
            desc.texture = _texture;
            desc.filePath = filePath;
            desc.fullPath = fullPath;
            desc.createTimestamp = _texture->getCreateTimestamp();
            desc.width = _texture->getWidth();
            desc.height = _texture->getHeight();
            desc.memorySize = memorySize;

            size_t groupIndex = 0;
            UnorderedConstStringMap<size_t>::const_iterator it_found = groupIndices.find( groupName );
            if( it_found == groupIndices.end() )
            {
                GroupDesc group;
                group.name = groupName;

                groups.emplace_back( group );
                groupIndex = groups.size() - 1;
                groupIndices.emplace( groupName, groupIndex );
            }
            else
            {
                groupIndex = it_found->second;
            }

            groups[groupIndex].textures.emplace_back( desc );
        } );

        StdAlgorithm::sort( groups.begin(), groups.end(), []( const GroupDesc & _l, const GroupDesc & _r )
        {
            return StdString::strcmp( _l.name.c_str(), _r.name.c_str() ) < 0;
        } );

        uint32_t totalTextures = 0;
        for( GroupDesc & group : groups )
        {
            StdAlgorithm::sort( group.textures.begin(), group.textures.end(), []( const TextureDesc & _l, const TextureDesc & _r )
            {
                return _l.createTimestamp < _r.createTimestamp;
            } );

            totalTextures += (uint32_t)group.textures.size();
        }

        this->textLine_( _ui, "Total textures: %u", totalTextures );

        Mosaic::sameLine( _ui );

        if( Mosaic::button( _ui, "Print to logs" ).clicked() == true )
        {
            LOGGER_SCOPE_MESSAGES();

            LOGGER_MESSAGE( "DebugPanel [Textures] Total: %u", totalTextures );
            for( const GroupDesc & group : groups )
            {
                LOGGER_MESSAGE( "DebugPanel [Textures] Group: %s (%u)", group.name.c_str(), (uint32_t)group.textures.size() );
                for( const TextureDesc & desc : group.textures )
                {
                    const Timestamp alive = now > desc.createTimestamp ? (now - desc.createTimestamp) : 0;
                    const Timestamp aliveSeconds = alive / TIMESTAMP_MILLISECONDS_SECOND64;
                    const uint32_t aliveMinutes = (uint32_t)(aliveSeconds / 60);
                    const uint32_t aliveSecondsRemainder = (uint32_t)(aliveSeconds % 60);
                    const Char * displayName = desc.filePath.empty() == false ? desc.filePath.c_str() : "runtime";
                    Char memStr[32 + 1] = {'\0'};
                    if( desc.memorySize < 1024 )
                        MENGINE_SNPRINTF( memStr, 32, "%u b", desc.memorySize );
                    else if( desc.memorySize < 1024 * 1024 )
                        MENGINE_SNPRINTF( memStr, 32, "%u kb", desc.memorySize / 1024 );
                    else
                        MENGINE_SNPRINTF( memStr, 32, "%u mb %u kb", desc.memorySize / (1024 * 1024), (desc.memorySize / 1024) % 1024 );
                    LOGGER_MESSAGE( "DebugPanel [Textures]   %s | %ux%u | %s | %um %us"
                        , displayName, desc.width, desc.height, memStr, aliveMinutes, aliveSecondsRemainder
                    );
                }
            }
        }

        Mosaic::TableOptions tableOptions;
        tableOptions.rowBackground = true;
        tableOptions.bordersInnerHorizontal = true;
        tableOptions.bordersInnerVertical = true;
        tableOptions.bordersOuterHorizontal = true;
        tableOptions.bordersOuterVertical = true;

        for( const GroupDesc & group : groups )
        {
            Mosaic::StringView groupLabel = this->keepLine_( "%s (%u)", group.name.c_str(), (uint32_t)group.textures.size() );

            Mosaic::Scope groupScope = Mosaic::scope( _ui, Mosaic::Key( Mosaic::StringView( group.name.c_str(), group.name.size() ) ) );

            Mosaic::TreeScope groupHeader = Mosaic::collapsingHeader( _ui, groupLabel, true );

            if( groupHeader.expanded() == false )
            {
                continue;
            }

            Mosaic::Scope table = Mosaic::table( _ui, "textures", 4, tableOptions );

            if( table.visible() == false )
            {
                continue;
            }

            Mosaic::TableColumnOptions textureColumn;
            textureColumn.sizing = Mosaic::TableSizing::Stretch;

            Mosaic::TableColumnOptions sizeColumn;
            sizeColumn.sizing = Mosaic::TableSizing::Fixed;
            sizeColumn.widthOrWeight = 90.f;

            Mosaic::TableColumnOptions memoryColumn;
            memoryColumn.sizing = Mosaic::TableSizing::Fixed;
            memoryColumn.widthOrWeight = 110.f;

            Mosaic::TableColumnOptions lifeColumn;
            lifeColumn.sizing = Mosaic::TableSizing::Fixed;
            lifeColumn.widthOrWeight = 90.f;

            Mosaic::tableSetupColumn( _ui, 0, "Texture", textureColumn );
            Mosaic::tableSetupColumn( _ui, 1, "Size", sizeColumn );
            Mosaic::tableSetupColumn( _ui, 2, "Memory", memoryColumn );
            Mosaic::tableSetupColumn( _ui, 3, "Life", lifeColumn );
            Mosaic::tableHeadersRow( _ui );

            for( const TextureDesc & desc : group.textures )
            {
                const RenderTextureInterfacePtr & texture = desc.texture;
                const bool selected = (m_selectedTextureId == texture->getTextureId());

                const Timestamp alive = now > desc.createTimestamp ? (now - desc.createTimestamp) : 0;
                const Timestamp aliveSeconds = alive / TIMESTAMP_MILLISECONDS_SECOND64;
                const uint32_t aliveMinutes = (uint32_t)(aliveSeconds / 60);
                const uint32_t aliveSecondsRemainder = (uint32_t)(aliveSeconds % 60);

                Char memoryText[64 + 1] = {'\0'};
                if( desc.memorySize < 1024 )
                {
                    MENGINE_SNPRINTF( memoryText, 64, "%u b", desc.memorySize );
                }
                else
                {
                    uint32_t kb = desc.memorySize / 1024;
                    uint32_t mb = kb / 1024;
                    uint32_t kbRem = kb % 1024;

                    if( mb > 0 )
                    {
                        MENGINE_SNPRINTF( memoryText, 64, "%u mb %u kb", mb, kbRem );
                    }
                    else
                    {
                        MENGINE_SNPRINTF( memoryText, 64, "%u kb", kb );
                    }
                }

                const Char * displayName = desc.filePath.empty() == false ? desc.filePath.c_str() : "runtime";

                Mosaic::tableNextRow( _ui, Mosaic::Key( (int32_t)texture->getTextureId() ) );
                if( Mosaic::tableSetColumn( _ui, 0 ) == true )
                {
                    Mosaic::Theme rowTheme = Mosaic::getTheme( _ui );
                    rowTheme.colors.border = this->getBorderColor_( _ui, alive );
                    rowTheme.metrics.borderWidth = 1.f;

                    Mosaic::SelectableOptions selectableOptions;
                    selectableOptions.spanAllColumns = true;

                    Mosaic::Scope rowStyle = Mosaic::styleScope( _ui, rowTheme );

                    if( Mosaic::selectable( _ui, Mosaic::Key( (int32_t)texture->getTextureId() ), this->keepLine_( "%s", displayName ), selected, selectableOptions ).clicked() == true )
                    {
                        m_selectedTextureId = texture->getTextureId();
                        m_selectedPath = desc.fullPath;
                    }
                }

                if( Mosaic::tableSetColumn( _ui, 1 ) == true )
                {
                    Mosaic::text( _ui, this->keepLine_( "%ux%u", desc.width, desc.height ) );
                }

                if( Mosaic::tableSetColumn( _ui, 2 ) == true )
                {
                    Mosaic::text( _ui, this->keepLine_( "%s", memoryText ) );
                }

                if( Mosaic::tableSetColumn( _ui, 3 ) == true )
                {
                    Mosaic::text( _ui, this->keepLine_( "%um %us", aliveMinutes, aliveSecondsRemainder ) );
                }
            }
        }

        Mosaic::separator( _ui );

        this->textLine_( _ui, "Selected path:" );

        if( m_selectedTextureId != INVALID_UNIQUE_ID )
        {
            Mosaic::StringView selectedPath = this->keepLine_( "%s", m_selectedPath.c_str() );

            Mosaic::TextOptions pathOptions;
            pathOptions.wordWrap = true;

            Mosaic::text( _ui, selectedPath, pathOptions );
        }
        else
        {
            this->textLine_( _ui, "<none>" );
        }
    }
    //////////////////////////////////////////////////////////////////////////
    void DebugPanelModule::renderResourceMonitor_( Mosaic::Context * _ui )
    {
        struct ResourceDesc
        {
            ResourcePtr resource;
            ConstString name;
            FilePath filePath;
            PathString fullPath;
            uint32_t compileRef;
            uint32_t prefetchRef;
            uint32_t cacheRef;
            ConstString type;
            bool isCached;
        };

        struct GroupDesc
        {
            ConstString name;
            Vector<ResourceDesc> resources;
        };

        Vector<GroupDesc> groups;
        UnorderedConstStringMap<size_t> groupIndices;

        RESOURCE_SERVICE()
            ->foreachResources( [&groups, &groupIndices, this]( const ResourcePtr & _resource )
        {
            uint32_t compileRef = _resource->getCompileReferenceCount();
            if( m_filterResourceCompileRef == 0 && compileRef != 0 )
                return;
            if( m_filterResourceCompileRef == 1 && compileRef != 1 )
                return;
            if( m_filterResourceCompileRef == 2 && compileRef <= 1 )
                return;

            ConstString groupName = STRINGIZE_STRING_LOCAL( "runtime" );
            FilePath filePath = FilePath::none();
            PathString fullPath( "runtime" );

            const ConstString & resourceGroupName = _resource->getGroupName();
            if( resourceGroupName.empty() == false )
            {
                groupName = resourceGroupName;
            }

            const ContentInterfacePtr & content = _resource->getContent();
            if( content != nullptr )
            {
                filePath = content->getFilePath();
                fullPath = Helper::getContentFullPath( content );
            }

            ResourceDesc desc;
            desc.resource = _resource;
            desc.name = _resource->getName();
            desc.filePath = filePath;
            desc.fullPath = fullPath;
            desc.compileRef = _resource->getCompileReferenceCount();
            desc.prefetchRef = _resource->getPrefetchReferenceCount();
            desc.cacheRef = _resource->getCacheReferenceCount();
            desc.type = Helper::getFactorableType( _resource );
            desc.isCached = _resource->isCache();

            size_t groupIndex = 0;
            UnorderedConstStringMap<size_t>::const_iterator it_found = groupIndices.find( groupName );
            if( it_found == groupIndices.end() )
            {
                GroupDesc group;
                group.name = groupName;

                groups.emplace_back( group );
                groupIndex = groups.size() - 1;
                groupIndices.emplace( groupName, groupIndex );
            }
            else
            {
                groupIndex = it_found->second;
            }

            groups[groupIndex].resources.emplace_back( desc );
        } );

        StdAlgorithm::sort( groups.begin(), groups.end(), []( const GroupDesc & _l, const GroupDesc & _r )
        {
            return StdString::strcmp( _l.name.c_str(), _r.name.c_str() ) < 0;
        } );

        uint32_t totalResources = 0;
        for( GroupDesc & group : groups )
        {
            StdAlgorithm::sort( group.resources.begin(), group.resources.end(), []( const ResourceDesc & _l, const ResourceDesc & _r )
            {
                return StdString::strcmp( _l.name.c_str(), _r.name.c_str() ) < 0;
            } );

            totalResources += (uint32_t)group.resources.size();
        }

        this->textLine_( _ui, "Filter:" );

        Mosaic::sameLine( _ui );

        if( Mosaic::radioButton( _ui, "0", m_filterResourceCompileRef == 0 ).clicked() == true )
        {
            m_filterResourceCompileRef = 0;
        }

        Mosaic::sameLine( _ui );

        if( Mosaic::radioButton( _ui, "1", m_filterResourceCompileRef == 1 ).clicked() == true )
        {
            m_filterResourceCompileRef = 1;
        }

        Mosaic::sameLine( _ui );

        if( Mosaic::radioButton( _ui, ">1", m_filterResourceCompileRef == 2 ).clicked() == true )
        {
            m_filterResourceCompileRef = 2;
        }

        Mosaic::sameLine( _ui );

        this->textLine_( _ui, "Total: %u", totalResources );

        if( Mosaic::button( _ui, "Print to logs" ).clicked() == true )
        {
            LOGGER_SCOPE_MESSAGES();

            LOGGER_MESSAGE( "DebugPanel [Resources] Filter: %d | Total: %u", m_filterResourceCompileRef, totalResources );
            for( const GroupDesc & group : groups )
            {
                LOGGER_MESSAGE( "DebugPanel [Resources] Group: %s (%u)", group.name.c_str(), (uint32_t)group.resources.size() );
                for( const ResourceDesc & desc : group.resources )
                {
                    const Char * displayName = desc.name.empty() == false ? desc.name.c_str() : "[unnamed]";
                    LOGGER_MESSAGE( "DebugPanel [Resources]   %s | %s | compile: %u | prefetch: %u | cache: %u | cached: %s"
                        , displayName, desc.type.c_str(), desc.compileRef, desc.prefetchRef, desc.cacheRef, desc.isCached ? "yes" : "-"
                    );
                }
            }
        }

        Mosaic::TableOptions tableOptions;
        tableOptions.rowBackground = true;
        tableOptions.bordersInnerHorizontal = true;
        tableOptions.bordersInnerVertical = true;
        tableOptions.bordersOuterHorizontal = true;
        tableOptions.bordersOuterVertical = true;

        for( const GroupDesc & group : groups )
        {
            Mosaic::StringView groupLabel = this->keepLine_( "%s (%u)", group.name.c_str(), (uint32_t)group.resources.size() );

            Mosaic::Scope groupScope = Mosaic::scope( _ui, Mosaic::Key( Mosaic::StringView( group.name.c_str(), group.name.size() ) ) );

            Mosaic::TreeScope groupHeader = Mosaic::collapsingHeader( _ui, groupLabel, true );

            if( groupHeader.expanded() == false )
            {
                continue;
            }

            Mosaic::Scope table = Mosaic::table( _ui, "resources", 6, tableOptions );

            if( table.visible() == false )
            {
                continue;
            }

            Mosaic::TableColumnOptions nameColumn;
            nameColumn.sizing = Mosaic::TableSizing::Stretch;

            Mosaic::TableColumnOptions typeColumn;
            typeColumn.sizing = Mosaic::TableSizing::Fixed;
            typeColumn.widthOrWeight = 120.f;

            Mosaic::TableColumnOptions compileColumn;
            compileColumn.sizing = Mosaic::TableSizing::Fixed;
            compileColumn.widthOrWeight = 60.f;

            Mosaic::TableColumnOptions prefetchColumn;
            prefetchColumn.sizing = Mosaic::TableSizing::Fixed;
            prefetchColumn.widthOrWeight = 65.f;

            Mosaic::TableColumnOptions cacheColumn;
            cacheColumn.sizing = Mosaic::TableSizing::Fixed;
            cacheColumn.widthOrWeight = 55.f;

            Mosaic::TableColumnOptions cachedColumn;
            cachedColumn.sizing = Mosaic::TableSizing::Fixed;
            cachedColumn.widthOrWeight = 55.f;

            Mosaic::tableSetupColumn( _ui, 0, "Name", nameColumn );
            Mosaic::tableSetupColumn( _ui, 1, "Type", typeColumn );
            Mosaic::tableSetupColumn( _ui, 2, "Compile", compileColumn );
            Mosaic::tableSetupColumn( _ui, 3, "Prefetch", prefetchColumn );
            Mosaic::tableSetupColumn( _ui, 4, "Cache", cacheColumn );
            Mosaic::tableSetupColumn( _ui, 5, "Cached", cachedColumn );
            Mosaic::tableHeadersRow( _ui );

            for( const ResourceDesc & desc : group.resources )
            {
                const bool selected = (m_selectedResourceGroup == group.name && m_selectedResourceName == desc.name);

                const Char * displayName = desc.name.empty() == false ? desc.name.c_str() : "[unnamed]";

                Mosaic::tableNextRow( _ui, Mosaic::Key( Mosaic::StringView( desc.name.c_str(), desc.name.size() ) ) );
                if( Mosaic::tableSetColumn( _ui, 0 ) == true )
                {
                    Mosaic::SelectableOptions selectableOptions;
                    selectableOptions.spanAllColumns = true;

                    if( Mosaic::selectable( _ui, Mosaic::Key( Mosaic::StringView( desc.name.c_str(), desc.name.size() ) ), this->keepLine_( "%s", displayName ), selected, selectableOptions ).clicked() == true )
                    {
                        m_selectedResourceGroup = group.name;
                        m_selectedResourceName = desc.name;
                        m_selectedResourcePath = desc.fullPath;
                    }
                }

                if( Mosaic::tableSetColumn( _ui, 1 ) == true )
                {
                    Mosaic::text( _ui, this->keepLine_( "%s", desc.type.c_str() ) );
                }

                if( Mosaic::tableSetColumn( _ui, 2 ) == true )
                {
                    Mosaic::text( _ui, this->keepLine_( "%u", desc.compileRef ) );
                }

                if( Mosaic::tableSetColumn( _ui, 3 ) == true )
                {
                    Mosaic::text( _ui, this->keepLine_( "%u", desc.prefetchRef ) );
                }

                if( Mosaic::tableSetColumn( _ui, 4 ) == true )
                {
                    Mosaic::text( _ui, this->keepLine_( "%u", desc.cacheRef ) );
                }

                if( Mosaic::tableSetColumn( _ui, 5 ) == true )
                {
                    Mosaic::text( _ui, this->keepLine_( "%s", desc.isCached ? "yes" : "-" ) );
                }
            }
        }

        Mosaic::separator( _ui );

        this->textLine_( _ui, "Selected path:" );

        if( m_selectedResourceGroup.empty() == false )
        {
            Mosaic::StringView selectedPath = this->keepLine_( "%s", m_selectedResourcePath.c_str() );

            Mosaic::TextOptions pathOptions;
            pathOptions.wordWrap = true;

            Mosaic::text( _ui, selectedPath, pathOptions );
        }
        else
        {
            this->textLine_( _ui, "<none>" );
        }
    }
    //////////////////////////////////////////////////////////////////////////
    Mosaic::Color DebugPanelModule::getBorderColor_( Mosaic::Context * _ui, Timestamp _alive ) const
    {
        if( _alive >= m_critical )
        {
            return Mosaic::Color{1.f, 0.2f, 0.2f, 1.f};
        }

        if( _alive >= m_warning )
        {
            return Mosaic::Color{0.2f, 0.6f, 1.f, 1.f};
        }

        const Mosaic::Theme & theme = Mosaic::getTheme( _ui );

        return theme.colors.border;
    }
    //////////////////////////////////////////////////////////////////////////
}
