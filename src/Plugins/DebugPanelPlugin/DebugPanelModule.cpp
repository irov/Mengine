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

#include "Config/StdIntTypes.h"
#include "Config/StdIO.h"
#include "Config/StdAlgorithm.h"
#include "Config/StdString.h"
#include "Config/Timestamp.h"

namespace Mengine
{
    //////////////////////////////////////////////////////////////////////////
    DebugPanelModule::DebugPanelModule()
        : m_warning( 0 )
        , m_critical( 0 )
        , m_selectedTextureId( INVALID_UNIQUE_ID )
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

        const ImGUIRenderProviderInterfacePtr & imguiRenderProvider = IMGUI_SERVICE()
            ->getRenderProvider();

        m_imguiRenderProvider = imguiRenderProvider;

        return true;
    }
    //////////////////////////////////////////////////////////////////////////
    void DebugPanelModule::_finalizeModule()
    {
        m_imguiRenderProvider = nullptr;
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
    void DebugPanelModule::drawHistogramUpdate( const HistogramUpdate & _histogram, const Char * _label, const Char * _overlayFormat, float _maxValue, float _height ) const
    {
        float currentValue = _histogram.getLastValue();
        const float * values = _histogram.getValues();

        Char overlayText[32 + 1] = {'\0'};
        MENGINE_SNPRINTF( overlayText, 32, _overlayFormat, currentValue );

        Char imGuiLabel[32 + 1] = {'\0'};
        MENGINE_SNPRINTF( imGuiLabel, 32, "##Update%s", _label );

        ImGui::PlotHistogram( imGuiLabel, values, MENGINE_DEBUG_PANEL_HISTOGRAM_UPDATE_COUNT, 0, overlayText, 0.f, _maxValue, ImVec2( 0, _height ) );
    }
    //////////////////////////////////////////////////////////////////////////
    void DebugPanelModule::drawHistogramPerFrame( const HistogramPerframe & _histogram, const Char * _label, const Char * _overlayFormat, float _maxValue, float _height ) const
    {
        float currentValue = _histogram.getLastValue();
        const float * values = _histogram.getValues();

        Char overlayText[32 + 1] = {'\0'};
        MENGINE_SNPRINTF( overlayText, 32, _overlayFormat, currentValue );

        Char imGuiLabel[32 + 1] = {'\0'};
        MENGINE_SNPRINTF( imGuiLabel, 32, "##PerFrame%s", _label );

        ImGui::PlotHistogram( imGuiLabel, values, MENGINE_DEBUG_PANEL_HISTOGRAM_PERFRAME_COUNT, 0, overlayText, 0.f, _maxValue, ImVec2( 0, _height ) );
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

        const RenderMaterialInterfacePtr & renderMaterial = RENDERMATERIAL_SERVICE()
            ->getDebugTriangleMaterial();

        _renderPipeline->addRenderExternal( _context, renderMaterial, nullptr, RenderDrawPrimitiveInterfacePtr::from( this ), MENGINE_DOCUMENT_FACTORABLE );
    }
    //////////////////////////////////////////////////////////////////////////
    void DebugPanelModule::onRenderDrawPrimitives( const RenderPrimitive * _primitives, uint32_t _count ) const
    {
        MENGINE_UNUSED( _primitives );
        MENGINE_UNUSED( _count );

        if( m_imguiRenderProvider == nullptr )
        {
            return;
        }

        if( SERVICE_IS_INITIALIZE( ImGUIServiceInterface ) == false )
        {
            return;
        }

        m_imguiRenderProvider->newFrame();

        ImGui::SetNextWindowSize( ImVec2( 720.f, 520.f ), ImGuiCond_FirstUseEver );
        ImGui::SetNextWindowPos( ImVec2( 100.f, 100.f ), ImGuiCond_FirstUseEver );
        if( ImGui::Begin( "Debug Panel", nullptr, ImGuiWindowFlags_None ) )
        {
            if( ImGui::BeginTabBar( "DebugTabs" ) )
            {
                if( ImGui::BeginTabItem( "Statistics" ) )
                {
                    m_selectedTab = 0;
                    this->renderDebugPanel();
                    ImGui::EndTabItem();
                }

                if( ImGui::BeginTabItem( "Textures" ) )
                {
                    m_selectedTab = 1;
                    this->renderTextureMonitor();
                    ImGui::EndTabItem();
                }

                if( ImGui::BeginTabItem( "Resources" ) )
                {
                    m_selectedTab = 2;
                    this->renderResourceMonitor();
                    ImGui::EndTabItem();
                }

                ImGui::EndTabBar();
            }
        }
        ImGui::End();

        m_imguiRenderProvider->endFrame();
    }
    //////////////////////////////////////////////////////////////////////////
    void DebugPanelModule::renderDebugPanel() const
    {
        if( ImGui::Button( "Print to logs" ) )
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

        this->drawHistogramUpdate( m_histogramFPS, "fps", "FPS: %.2f", histogramFPSHeight, 80.f );

        int64_t Statistic_AllocatorSize = STATISTIC_GET_INTEGER( STATISTIC_ALLOCATOR_SIZE );
        ImGui::Text( "Allocator size: %" MENGINE_PRId64 "mb %" MENGINE_PRId64 "kb", Statistic_AllocatorSize / (1024 * 1024), (Statistic_AllocatorSize % (1024 * 1024)) / 1024 );

        this->drawHistogramUpdate( m_histogramAllocatorNew, "memalloc", "Memory new: %.2fkb", FLT_MAX, 60.f );
        this->drawHistogramUpdate( m_histogramAllocatorFree, "memfree", "Memory free: %.2fkb", FLT_MAX, 60.f );

        int64_t Statistic_Render_ImageSize = STATISTIC_GET_INTEGER( STATISTIC_RENDER_TEXTURE_ALLOC_SIZE );
        ImGui::Text( "Image size: %" MENGINE_PRId64 "mb %" MENGINE_PRId64 "kb", Statistic_Render_ImageSize / (1024 * 1024), (Statistic_Render_ImageSize % (1024 * 1024)) / 1024 );

        this->drawHistogramUpdate( m_histogramImageNew, "imagenew", "Image new: %.2fkb", FLT_MAX, 60.f );
        this->drawHistogramUpdate( m_histogramImageFree, "imagefree", "Image free: %.2fkb", FLT_MAX, 60.f );

        this->drawHistogramPerFrame( m_histogramPerFrameDrawIndexPrimitives, "dip", "DIP: %.0f", FLT_MAX, 50.f );

        float maxFillrate = m_histogramPerFrameFillrate.getMaxValue();
        float histogramFillrateHeight = maxFillrate > 10.f ? maxFillrate : 13.f;

        this->drawHistogramPerFrame( m_histogramPerFrameFillrate, "fillrate", "Fillrate: %.2f", histogramFillrateHeight, 50.f );
        this->drawHistogramPerFrame( m_histogramPerFrameObjects, "objects", "Objects: %.0f", FLT_MAX, 50.f );
        this->drawHistogramPerFrame( m_histogramPerFrameTriangles, "triangles", "Triangles: %.0f", FLT_MAX, 50.f );
        this->drawHistogramPerFrame( m_histogramPerFrameBatches, "batches", "Batches: %.0f", FLT_MAX, 50.f );
    }
    //////////////////////////////////////////////////////////////////////////
    void DebugPanelModule::renderTextureMonitor() const
    {
        if( SERVICE_IS_INITIALIZE( RenderTextureServiceInterface ) == false )
        {
            ImGui::Text( "RenderTextureService is not initialized" );
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

        ImGui::Text( "Total textures: %u", totalTextures );
        ImGui::SameLine();
        if( ImGui::Button( "Print to logs" ) )
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
                    Char memStr[32] = {'\0'};
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

        for( const GroupDesc & group : groups )
        {
            Char groupLabel[256 + 1] = {'\0'};
            MENGINE_SNPRINTF( groupLabel, 256, "%s (%u)", group.name.c_str(), (uint32_t)group.textures.size() );

            if( ImGui::CollapsingHeader( groupLabel, ImGuiTreeNodeFlags_DefaultOpen ) == false )
            {
                continue;
            }

            ImGui::PushID( group.name.c_str() );

            if( ImGui::BeginTable( "textures", 4, ImGuiTableFlags_Borders | ImGuiTableFlags_RowBg | ImGuiTableFlags_Resizable ) )
            {
                ImGui::TableSetupColumn( "Texture", ImGuiTableColumnFlags_WidthStretch );
                ImGui::TableSetupColumn( "Size", ImGuiTableColumnFlags_WidthFixed, 90.f );
                ImGui::TableSetupColumn( "Memory", ImGuiTableColumnFlags_WidthFixed, 110.f );
                ImGui::TableSetupColumn( "Life", ImGuiTableColumnFlags_WidthFixed, 90.f );
                ImGui::TableHeadersRow();

                for( const TextureDesc & desc : group.textures )
                {
                    const RenderTextureInterfacePtr & texture = desc.texture;
                    const bool selected = (m_selectedTextureId == texture->getTextureId());

                    const Timestamp alive = now > desc.createTimestamp ? (now - desc.createTimestamp) : 0;
                    const Timestamp aliveSeconds = alive / TIMESTAMP_MILLISECONDS_SECOND64;
                    const uint32_t aliveMinutes = (uint32_t)(aliveSeconds / 60);
                    const uint32_t aliveSecondsRemainder = (uint32_t)(aliveSeconds % 60);

                    const ImVec4 borderColor = this->getBorderColor_( alive );

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

                    ImGui::TableNextRow();
                    ImGui::TableNextColumn();

                    ImGui::PushID( (int)texture->getTextureId() );
                    ImGui::PushStyleVar( ImGuiStyleVar_FrameBorderSize, 1.f );
                    ImGui::PushStyleColor( ImGuiCol_Border, borderColor );

                    if( ImGui::Selectable( displayName, selected, ImGuiSelectableFlags_SpanAllColumns ) == true )
                    {
                        m_selectedTextureId = texture->getTextureId();
                        m_selectedPath = desc.fullPath;
                    }

                    ImGui::PopStyleColor();
                    ImGui::PopStyleVar();
                    ImGui::PopID();

                    ImGui::TableNextColumn();
                    ImGui::Text( "%ux%u", desc.width, desc.height );

                    ImGui::TableNextColumn();
                    ImGui::Text( "%s", memoryText );

                    ImGui::TableNextColumn();
                    ImGui::Text( "%um %us", aliveMinutes, aliveSecondsRemainder );
                }

                ImGui::EndTable();
            }

            ImGui::PopID();
        }

        ImGui::Separator();

        ImGui::Text( "Selected path:" );
        if( m_selectedTextureId != INVALID_UNIQUE_ID )
        {
            ImGui::TextWrapped( "%s", m_selectedPath.c_str() );
        }
        else
        {
            ImGui::Text( "<none>" );
        }
    }
    //////////////////////////////////////////////////////////////////////////
    void DebugPanelModule::renderResourceMonitor() const
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
            desc.type = _resource->getType();
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

        ImGui::Text( "Filter:" );
        ImGui::SameLine();
        ImGui::RadioButton( "0", &m_filterResourceCompileRef, 0 );
        ImGui::SameLine();
        ImGui::RadioButton( "1", &m_filterResourceCompileRef, 1 );
        ImGui::SameLine();
        ImGui::RadioButton( ">1", &m_filterResourceCompileRef, 2 );
        ImGui::SameLine();
        ImGui::Text( "Total: %u", totalResources );

        if( ImGui::Button( "Print to logs" ) )
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

        for( const GroupDesc & group : groups )
        {
            Char groupLabel[256 + 1] = {'\0'};
            MENGINE_SNPRINTF( groupLabel, 256, "%s (%u)", group.name.c_str(), (uint32_t)group.resources.size() );

            if( ImGui::CollapsingHeader( groupLabel, ImGuiTreeNodeFlags_DefaultOpen ) == false )
            {
                continue;
            }

            ImGui::PushID( group.name.c_str() );

            if( ImGui::BeginTable( "resources", 7, ImGuiTableFlags_Borders | ImGuiTableFlags_RowBg | ImGuiTableFlags_Resizable ) )
            {
                ImGui::TableSetupColumn( "Name", ImGuiTableColumnFlags_WidthStretch );
                ImGui::TableSetupColumn( "Type", ImGuiTableColumnFlags_WidthFixed, 120.f );
                ImGui::TableSetupColumn( "Compile", ImGuiTableColumnFlags_WidthFixed, 60.f );
                ImGui::TableSetupColumn( "Prefetch", ImGuiTableColumnFlags_WidthFixed, 65.f );
                ImGui::TableSetupColumn( "Cache", ImGuiTableColumnFlags_WidthFixed, 55.f );
                ImGui::TableSetupColumn( "Cached", ImGuiTableColumnFlags_WidthFixed, 55.f );
                ImGui::TableHeadersRow();

                for( const ResourceDesc & desc : group.resources )
                {
                    const bool selected = (m_selectedResourceGroup == group.name && m_selectedResourceName == desc.name);

                    const Char * displayName = desc.name.empty() == false ? desc.name.c_str() : "[unnamed]";

                    ImGui::TableNextRow();
                    ImGui::TableNextColumn();

                    ImGui::PushID( (void *)desc.resource.get() );
                    if( ImGui::Selectable( displayName, selected, ImGuiSelectableFlags_SpanAllColumns ) == true )
                    {
                        m_selectedResourceGroup = group.name;
                        m_selectedResourceName = desc.name;
                        m_selectedResourcePath = desc.fullPath;
                    }
                    ImGui::PopID();

                    ImGui::TableNextColumn();
                    ImGui::Text( "%s", desc.type.c_str() );

                    ImGui::TableNextColumn();
                    ImGui::Text( "%u", desc.compileRef );

                    ImGui::TableNextColumn();
                    ImGui::Text( "%u", desc.prefetchRef );

                    ImGui::TableNextColumn();
                    ImGui::Text( "%u", desc.cacheRef );

                    ImGui::TableNextColumn();
                    ImGui::Text( "%s", desc.isCached ? "yes" : "-" );
                }

                ImGui::EndTable();
            }

            ImGui::PopID();
        }

        ImGui::Separator();

        ImGui::Text( "Selected path:" );
        if( m_selectedResourceGroup.empty() == false )
        {
            ImGui::TextWrapped( "%s", m_selectedResourcePath.c_str() );
        }
        else
        {
            ImGui::Text( "<none>" );
        }
    }
    //////////////////////////////////////////////////////////////////////////
    ImVec4 DebugPanelModule::getBorderColor_( Timestamp _alive ) const
    {
        if( _alive >= m_critical )
        {
            return ImVec4( 1.f, 0.2f, 0.2f, 1.f );
        }

        if( _alive >= m_warning )
        {
            return ImVec4( 0.2f, 0.6f, 1.f, 1.f );
        }

        return ImGui::GetStyleColorVec4( ImGuiCol_Border );
    }
    //////////////////////////////////////////////////////////////////////////
}
