#include "FigmaViewerExampleSceneEventReceiver.h"

#include "FigmaViewerExampleFileDialog.h"

#include "Interface/DataflowInterface.h"
#include "Interface/ApplicationInterface.h"
#include "Interface/FileGroupInterface.h"
#include "Interface/FileServiceInterface.h"
#include "Interface/FileSystemInterface.h"
#include "Interface/PrototypeServiceInterface.h"
#include "Interface/TransformationInterface.h"

#include "Kernel/AssertionMemoryPanic.h"
#include "Kernel/ContentHelper.h"
#include "Kernel/ConstStringHelper.h"
#include "Kernel/DocumentHelper.h"
#include "Kernel/Entity.h"
#include "Kernel/FilePathHelper.h"
#include "Kernel/FileStreamHelper.h"
#include "Kernel/MemoryStreamHelper.h"
#include "Kernel/NodeCast.h"
#include "Kernel/VocabularyHelper.h"

#include "Figma/Types.h"

#include "imgui.h"

#include <algorithm>
#include <cstdlib>

namespace Mengine
{
    namespace
    {
        //////////////////////////////////////////////////////////////////////////
        struct FigmaViewerEntryDesc
        {
            const Char * label;
            const Char * kind;
            const Char * frameId;
            float width;
            float height;
            float scale;
        };
        //////////////////////////////////////////////////////////////////////////
        struct FigmaViewerFileDesc
        {
            const Char * label;
            uint32_t entryOffset;
            uint32_t entryCount;
        };
        //////////////////////////////////////////////////////////////////////////
        struct FigmaViewerPlaybackRateDesc
        {
            const Char * label;
            float rate;
        };
        //////////////////////////////////////////////////////////////////////////
        static const FigmaViewerEntryDesc s_figmaViewerEntries[] = {
            {"Prototype start", "screen", "", 1024.f, 768.f, 1.f},
            {"Screen", "screen", "", 1024.f, 768.f, 1.f},
            {"Composition", "composition", "", 1024.f, 180.f, 1.f},
            {"Component", "component", "", 360.f, 96.f, 1.f},

            {"Prototype start", "screen", "", 1024.f, 768.f, 1.f},
            {"Composition", "composition", "", 520.f, 360.f, 1.f},
            {"Component", "component", "", 220.f, 72.f, 1.f},
        };
        //////////////////////////////////////////////////////////////////////////
        static const FigmaViewerFileDesc s_figmaViewerFiles[] = {
            {"Game UI prototype", 0, 4},
            {"Meta UI kit", 4, 3},
        };
        //////////////////////////////////////////////////////////////////////////
        static const FigmaViewerPlaybackRateDesc s_figmaViewerPlaybackRates[] = {
            {"x1", 1.f},
            {"x2", 1.f / 2.f},
            {"x4", 1.f / 4.f},
            {"x8", 1.f / 8.f},
            {"x16", 1.f / 16.f},
            {"x32", 1.f / 32.f},
            {"x64", 1.f / 64.f},
        };
        //////////////////////////////////////////////////////////////////////////
        static uint32_t s_figmaViewerFileCount()
        {
            return sizeof( s_figmaViewerFiles ) / sizeof( s_figmaViewerFiles[0] );
        }
        //////////////////////////////////////////////////////////////////////////
        static const FigmaViewerFileDesc & s_figmaViewerFile( int32_t _index )
        {
            return s_figmaViewerFiles[_index];
        }
        //////////////////////////////////////////////////////////////////////////
        static const FigmaViewerEntryDesc & s_figmaViewerEntry( const FigmaViewerFileDesc & _file, int32_t _entryIndex )
        {
            return s_figmaViewerEntries[_file.entryOffset + _entryIndex];
        }
        //////////////////////////////////////////////////////////////////////////
        static uint32_t s_figmaViewerPlaybackRateCount()
        {
            return sizeof( s_figmaViewerPlaybackRates ) / sizeof( s_figmaViewerPlaybackRates[0] );
        }
        //////////////////////////////////////////////////////////////////////////
        static const FigmaViewerPlaybackRateDesc & s_figmaViewerPlaybackRate( int32_t _index )
        {
            return s_figmaViewerPlaybackRates[_index];
        }
        //////////////////////////////////////////////////////////////////////////
        static String s_normalizePathString( const Char * _path )
        {
            String path = _path == nullptr ? "" : _path;

            for( String::value_type & c : path )
            {
                if( c == '\\' )
                {
                    c = '/';
                }
            }

            return path;
        }
        //////////////////////////////////////////////////////////////////////////
        static bool s_isAlpha( Char _c )
        {
            return (_c >= 'A' && _c <= 'Z') || (_c >= 'a' && _c <= 'z');
        }
        //////////////////////////////////////////////////////////////////////////
        static bool s_isAbsolutePath( const String & _path )
        {
            if( _path.empty() == true )
            {
                return false;
            }

            if( _path[0] == '/' )
            {
                return true;
            }

            if( _path.size() > 2 && s_isAlpha( _path[0] ) == true && _path[1] == ':' )
            {
                return true;
            }

            return false;
        }
        //////////////////////////////////////////////////////////////////////////
        static bool s_splitFullPath( const String & _fullPath, String * const _folder, String * const _file )
        {
            String::size_type slash = _fullPath.find_last_of( '/' );

            if( slash == String::npos )
            {
                return false;
            }

            *_folder = _fullPath.substr( 0, slash + 1 );
            *_file = _fullPath.substr( slash + 1 );

            if( _file->empty() == true )
            {
                return false;
            }

            return true;
        }
        //////////////////////////////////////////////////////////////////////////
        static FilePath s_makeFilePath( const String & _path )
        {
            if( _path.empty() == true )
            {
                return FilePath::none();
            }

            return Helper::stringizeFilePathSize( _path.c_str(), (FilePath::size_type)_path.size() );
        }
        //////////////////////////////////////////////////////////////////////////
        static String s_makeDirectoryPath( String _path )
        {
            if( _path.empty() == false && _path[_path.size() - 1] != '/' )
            {
                _path += '/';
            }

            return _path;
        }
        //////////////////////////////////////////////////////////////////////////
        static void s_copyPathInput( Char * const _buffer, size_t _capacity, const Char * _path )
        {
            MENGINE_SNPRINTF( _buffer, _capacity, "%s", _path == nullptr ? "" : _path );
        }
        //////////////////////////////////////////////////////////////////////////
        static bool s_renderPathInput( const Char * _label, Char * const _buffer, size_t _capacity, bool _folder, const Char * _dialogTitle, const Char * _dialogStartPath )
        {
            ImGui::PushID( _label );

            const ImGuiStyle & style = ImGui::GetStyle();
            const float buttonWidth = 32.f;
            const float labelWidth = 112.f;
            float inputWidth = ImGui::GetContentRegionAvail().x - buttonWidth - labelWidth - style.ItemSpacing.x * 2.f;

            if( inputWidth < 160.f )
            {
                inputWidth = 160.f;
            }

            ImGui::SetNextItemWidth( inputWidth );
            bool changed = ImGui::InputText( "##path", _buffer, _capacity );

            ImGui::SameLine();

            if( ImGui::Button( "..." ) == true )
            {
                if( _folder == true )
                {
                    changed |= FigmaViewerExampleSelectFolderPath( _dialogTitle, _dialogStartPath, _buffer, _capacity );
                }
                else
                {
                    changed |= FigmaViewerExampleSelectFilePath( _dialogTitle, _dialogStartPath, _buffer, _capacity );
                }
            }

            ImGui::SameLine();
            ImGui::TextUnformatted( _label );

            ImGui::PopID();

            return changed;
        }
        //////////////////////////////////////////////////////////////////////////
        static bool s_findSettingValue( const String & _settings, const Char * _key, String * const _value )
        {
            String prefix = _key;
            prefix += '=';

            String::size_type lineBegin = 0;

            while( lineBegin < _settings.size() )
            {
                String::size_type lineEnd = _settings.find( '\n', lineBegin );

                if( lineEnd == String::npos )
                {
                    lineEnd = _settings.size();
                }

                String::size_type valueBegin = lineBegin + prefix.size();

                if( lineEnd >= valueBegin && _settings.compare( lineBegin, prefix.size(), prefix ) == 0 )
                {
                    *_value = _settings.substr( valueBegin, lineEnd - valueBegin );

                    if( _value->empty() == false && (*_value)[_value->size() - 1] == '\r' )
                    {
                        _value->pop_back();
                    }

                    return true;
                }

                lineBegin = lineEnd + 1;
            }

            return false;
        }
        //////////////////////////////////////////////////////////////////////////
        static bool s_parseSettingInt32( const String & _value, int32_t * const _result )
        {
            Char * end = nullptr;
            long value = std::strtol( _value.c_str(), &end, 10 );

            if( end == _value.c_str() )
            {
                return false;
            }

            *_result = (int32_t)value;

            return true;
        }
        //////////////////////////////////////////////////////////////////////////
        static bool s_parseSettingFloat( const String & _value, float * const _result )
        {
            Char * end = nullptr;
            float value = std::strtof( _value.c_str(), &end );

            if( end == _value.c_str() )
            {
                return false;
            }

            *_result = value;

            return true;
        }
        //////////////////////////////////////////////////////////////////////////
        static void s_appendSettingValue( String * const _settings, const Char * _key, const Char * _value )
        {
            *_settings += _key;
            *_settings += '=';
            *_settings += _value == nullptr ? "" : _value;
            *_settings += '\n';
        }
        //////////////////////////////////////////////////////////////////////////
        static void s_appendSettingInt32( String * const _settings, const Char * _key, int32_t _value )
        {
            Char buffer[64] = {'\0'};
            MENGINE_SNPRINTF( buffer, 64, "%d", _value );

            s_appendSettingValue( _settings, _key, buffer );
        }
        //////////////////////////////////////////////////////////////////////////
        static void s_appendSettingFloat( String * const _settings, const Char * _key, float _value )
        {
            Char buffer[64] = {'\0'};
            MENGINE_SNPRINTF( buffer, 64, "%.9g", _value );

            s_appendSettingValue( _settings, _key, buffer );
        }
        //////////////////////////////////////////////////////////////////////////
        static float s_calcAutoOutputScale( float _contentWidth, float _contentHeight, float _viewportWidth, float _viewportHeight )
        {
            constexpr float PreviewFactor = 0.9f;

            if( _contentWidth <= 0.f || _contentHeight <= 0.f || _viewportWidth <= 0.f || _viewportHeight <= 0.f )
            {
                return 1.f;
            }

            const float availableWidth = _contentWidth * PreviewFactor;
            const float availableHeight = _contentHeight * PreviewFactor;

            return std::min( availableWidth / _viewportWidth, availableHeight / _viewportHeight );
        }
        //////////////////////////////////////////////////////////////////////////
    }
    //////////////////////////////////////////////////////////////////////////
    FigmaViewerExampleSceneEventReceiver::FigmaViewerExampleSceneEventReceiver()
        : m_scene( nullptr )
        , m_figmaUnknown( nullptr )
        , m_scaleInput( 1.f )
        , m_autoViewportInput( true )
        , m_fileIndex( 0 )
        , m_entryIndex( 0 )
        , m_playbackRateIndex( 0 )
    {
        this->syncPathInputs_();

        m_fontsPathInput[0] = '\0';
        m_startFrameIdInput[0] = '\0';

        m_viewportInput[0] = 1024.f;
        m_viewportInput[1] = 768.f;

        m_positionInput[0] = 0.f;
        m_positionInput[1] = 0.f;

        m_status = "Ready";

        this->loadSettings_();
    }
    //////////////////////////////////////////////////////////////////////////
    FigmaViewerExampleSceneEventReceiver::~FigmaViewerExampleSceneEventReceiver()
    {
    }
    //////////////////////////////////////////////////////////////////////////
    bool FigmaViewerExampleSceneEventReceiver::onEntityCreate( const EntityBehaviorInterfacePtr & _behavior, Entity * _entity )
    {
        MENGINE_UNUSED( _behavior );

        m_scene = Helper::staticNodeCast<Scene *>( _entity );

        return true;
    }
    //////////////////////////////////////////////////////////////////////////
    void FigmaViewerExampleSceneEventReceiver::onEntityDestroy( const EntityBehaviorInterfacePtr & _behavior )
    {
        MENGINE_UNUSED( _behavior );

        m_scene = nullptr;
    }
    //////////////////////////////////////////////////////////////////////////
    bool FigmaViewerExampleSceneEventReceiver::onEntityPreparation( const EntityBehaviorInterfacePtr & _behavior )
    {
        MENGINE_UNUSED( _behavior );

        APPLICATION_SERVICE()->setFixedContentResolution( false );
        APPLICATION_SERVICE()->setFixedDisplayResolution( false );
        APPLICATION_SERVICE()->setFixedViewportResolution( false );

        ImGUIRenderPtr imguiRender = PROTOTYPE_SERVICE()
            ->generatePrototype( STRINGIZE_STRING_LOCAL( "Node" ), STRINGIZE_STRING_LOCAL( "ImGUIRender" ), MENGINE_DOCUMENT_FACTORABLE );

        MENGINE_ASSERTION_MEMORY_PANIC( imguiRender, "invalid create ImGUIRender" );

        imguiRender->setName( STRINGIZE_STRING_LOCAL( "FigmaViewerExampleImGUI" ) );
        imguiRender->setProvider( [this]( const ImGUIRenderProviderInterfacePtr & _provider )
        {
            this->renderControls_( _provider );
        } );

        m_scene->addChild( imguiRender );
        m_imguiRender = imguiRender;

        return true;
    }
    //////////////////////////////////////////////////////////////////////////
    void FigmaViewerExampleSceneEventReceiver::onEntityDeactivate( const EntityBehaviorInterfacePtr & _behavior )
    {
        MENGINE_UNUSED( _behavior );

        this->clearFigma_();

        if( m_imguiRender != nullptr )
        {
            m_imguiRender->dispose();
            m_imguiRender = nullptr;
        }
    }
    //////////////////////////////////////////////////////////////////////////
    void FigmaViewerExampleSceneEventReceiver::renderControls_( const ImGUIRenderProviderInterfacePtr & _provider )
    {
        MENGINE_UNUSED( _provider );

        this->updateFigmaViewport_();

        ImGui::SetNextWindowSize( ImVec2( 620.f, 440.f ), ImGuiCond_FirstUseEver );

        if( ImGui::Begin( "Figma Viewer" ) == false )
        {
            ImGui::End();
            return;
        }

        const uint32_t fileCount = s_figmaViewerFileCount();
        bool settingsChanged = false;

        if( m_fileIndex < 0 || (uint32_t)m_fileIndex >= fileCount )
        {
            m_fileIndex = 0;
            settingsChanged = true;
        }

        const FigmaViewerFileDesc & file = s_figmaViewerFile( m_fileIndex );

        if( m_entryIndex < 0 || (uint32_t)m_entryIndex >= file.entryCount )
        {
            m_entryIndex = 0;
            settingsChanged = true;
        }

        if( ImGui::BeginCombo( "Preset", file.label ) == true )
        {
            for( uint32_t index = 0; index != fileCount; ++index )
            {
                const bool selected = m_fileIndex == (int32_t)index;

                if( ImGui::Selectable( s_figmaViewerFiles[index].label, selected ) == true )
                {
                    m_fileIndex = (int32_t)index;
                    m_entryIndex = 0;
                    this->resetEntryParams_();
                    settingsChanged = true;
                }

                if( selected == true )
                {
                    ImGui::SetItemDefaultFocus();
                }
            }

            ImGui::EndCombo();
        }

        const FigmaViewerFileDesc & selectedFile = s_figmaViewerFile( m_fileIndex );
        const FigmaViewerEntryDesc & selectedEntry = s_figmaViewerEntry( selectedFile, m_entryIndex );

        settingsChanged |= s_renderPathInput( "Figma file", m_figPathInput, sizeof( m_figPathInput ), false, "Open Figma File", m_figPathInput );
        settingsChanged |= s_renderPathInput( "Fonts folder", m_fontsPathInput, sizeof( m_fontsPathInput ), true, "Choose Fonts Folder", m_fontsPathInput );

        if( ImGui::BeginCombo( "Start point", selectedEntry.label ) == true )
        {
            for( uint32_t index = 0; index != selectedFile.entryCount; ++index )
            {
                const FigmaViewerEntryDesc & entry = s_figmaViewerEntry( selectedFile, (int32_t)index );
                const bool selected = m_entryIndex == (int32_t)index;

                Char label[256] = {'\0'};
                MENGINE_SNPRINTF( label, 256, "%s [%s]", entry.label, entry.kind );

                if( ImGui::Selectable( label, selected ) == true )
                {
                    m_entryIndex = (int32_t)index;
                    this->resetEntryParams_();
                    settingsChanged = true;
                }

                if( selected == true )
                {
                    ImGui::SetItemDefaultFocus();
                }
            }

            ImGui::EndCombo();
        }

        settingsChanged |= ImGui::InputText( "Start frame id", m_startFrameIdInput, sizeof( m_startFrameIdInput ) );
        settingsChanged |= ImGui::Checkbox( "Auto viewport", &m_autoViewportInput );
        settingsChanged |= ImGui::DragFloat2( "Viewport", m_viewportInput, 1.f, 1.f, 4096.f, "%.0f" );
        settingsChanged |= ImGui::DragFloat2( "Offset", m_positionInput, 1.f, -4096.f, 4096.f, "%.0f" );
        ImGui::BeginDisabled( m_autoViewportInput );
        settingsChanged |= ImGui::SliderFloat( "Scale", &m_scaleInput, 0.1f, 4.f, "%.2f", ImGuiSliderFlags_Logarithmic | ImGuiSliderFlags_NoRoundToFormat );
        ImGui::EndDisabled();

        const uint32_t playbackRateCount = s_figmaViewerPlaybackRateCount();
        if( m_playbackRateIndex < 0 || (uint32_t)m_playbackRateIndex >= playbackRateCount )
        {
            m_playbackRateIndex = 0;
            settingsChanged = true;
        }

        const FigmaViewerPlaybackRateDesc & playbackRate = s_figmaViewerPlaybackRate( m_playbackRateIndex );
        if( ImGui::BeginCombo( "Slowdown", playbackRate.label ) == true )
        {
            for( uint32_t index = 0; index != playbackRateCount; ++index )
            {
                const bool selected = m_playbackRateIndex == (int32_t)index;

                if( ImGui::Selectable( s_figmaViewerPlaybackRates[index].label, selected ) == true )
                {
                    m_playbackRateIndex = (int32_t)index;
                    settingsChanged = true;

                    if( m_figmaUnknown != nullptr )
                    {
                        m_figmaUnknown->setPlaybackRate( s_figmaViewerPlaybackRates[index].rate );
                    }
                }

                if( selected == true )
                {
                    ImGui::SetItemDefaultFocus();
                }
            }

            ImGui::EndCombo();
        }

        const bool openSelection = ImGui::Button( "Open" );

        if( settingsChanged == true || openSelection == true )
        {
            this->saveSettings_();
        }

        if( openSelection == true )
        {
            this->applySelection_();
        }

        ImGui::SameLine();

        if( ImGui::Button( "Close" ) == true )
        {
            this->clearFigma_();
            m_status = "Closed";
        }

        ImGui::SameLine();

        if( ImGui::Button( "Replay" ) == true )
        {
            if( m_figmaUnknown == nullptr )
            {
                m_status = "Nothing to replay";
            }
            else if( m_figmaUnknown->replay() == false )
            {
                m_status = "Replay failed";
            }
            else
            {
                m_status = "Replaying";
            }
        }

        ImGui::Separator();

        ImGui::Text( "Figma SDK version: %u", ::Figma::FIGMA_SDK_VERSION );
        ImGui::Text( "Status: %s", m_status.c_str() );
        ImGui::Text( "File: %s", m_figPathInput );
        ImGui::Text( "Fonts: %s", m_fontsPathInput[0] != '\0' ? m_fontsPathInput : "default" );
        ImGui::Text( "Frame id: %s", m_startFrameIdInput[0] != '\0' ? m_startFrameIdInput : "prototype start" );
        ImGui::Text( "Kind: %s", selectedEntry.kind );
        ImGui::Text( "Node: %s", m_figmaNode != nullptr ? "active" : "none" );

        ImGui::End();
    }
    //////////////////////////////////////////////////////////////////////////
    void FigmaViewerExampleSceneEventReceiver::syncPathInputs_()
    {
        s_copyPathInput( m_figPathInput, sizeof( m_figPathInput ), "" );
    }
    //////////////////////////////////////////////////////////////////////////
    void FigmaViewerExampleSceneEventReceiver::loadSettings_()
    {
        const FileGroupInterfacePtr & userFileGroup = FILE_SERVICE()
            ->getFileGroup( STRINGIZE_STRING_LOCAL( "user" ) );

        if( userFileGroup == nullptr )
        {
            return;
        }

        const FilePath settingsPath = STRINGIZE_FILEPATH_LOCAL( "figma_viewer.ini" );

        if( userFileGroup->existFile( settingsPath, false ) == false )
        {
            return;
        }

        MemoryInterfacePtr memory = Helper::createMemoryFileString( userFileGroup, settingsPath, false, false, MENGINE_DOCUMENT_FACTORABLE );

        if( memory == nullptr || memory->empty() == true )
        {
            return;
        }

        const Char * buffer = memory->getBuffer();
        size_t size = memory->getSize();

        if( size != 0 && buffer[size - 1] == '\0' )
        {
            --size;
        }

        String settings( buffer, size );
        String value;

        if( s_findSettingValue( settings, "figma_file", &value ) == true )
        {
            s_copyPathInput( m_figPathInput, sizeof( m_figPathInput ), value.c_str() );
        }

        if( s_findSettingValue( settings, "fonts_folder", &value ) == true )
        {
            s_copyPathInput( m_fontsPathInput, sizeof( m_fontsPathInput ), value.c_str() );
        }

        if( s_findSettingValue( settings, "start_frame_id", &value ) == true )
        {
            s_copyPathInput( m_startFrameIdInput, sizeof( m_startFrameIdInput ), value.c_str() );
        }

        if( s_findSettingValue( settings, "file_index", &value ) == true )
        {
            s_parseSettingInt32( value, &m_fileIndex );
        }

        if( s_findSettingValue( settings, "entry_index", &value ) == true )
        {
            s_parseSettingInt32( value, &m_entryIndex );
        }

        if( s_findSettingValue( settings, "auto_viewport", &value ) == true )
        {
            int32_t autoViewport = 0;

            if( s_parseSettingInt32( value, &autoViewport ) == true )
            {
                m_autoViewportInput = autoViewport != 0;
            }
        }

        if( s_findSettingValue( settings, "viewport_x", &value ) == true )
        {
            s_parseSettingFloat( value, &m_viewportInput[0] );
        }

        if( s_findSettingValue( settings, "viewport_y", &value ) == true )
        {
            s_parseSettingFloat( value, &m_viewportInput[1] );
        }

        if( s_findSettingValue( settings, "position_x", &value ) == true )
        {
            s_parseSettingFloat( value, &m_positionInput[0] );
        }

        if( s_findSettingValue( settings, "position_y", &value ) == true )
        {
            s_parseSettingFloat( value, &m_positionInput[1] );
        }

        if( s_findSettingValue( settings, "scale", &value ) == true )
        {
            s_parseSettingFloat( value, &m_scaleInput );
        }

        if( s_findSettingValue( settings, "playback_rate_index", &value ) == true )
        {
            s_parseSettingInt32( value, &m_playbackRateIndex );
        }
    }
    //////////////////////////////////////////////////////////////////////////
    void FigmaViewerExampleSceneEventReceiver::saveSettings_() const
    {
        const FileGroupInterfacePtr & userFileGroup = FILE_SERVICE()
            ->getFileGroup( STRINGIZE_STRING_LOCAL( "user" ) );

        if( userFileGroup == nullptr )
        {
            return;
        }

        String settings;
        s_appendSettingValue( &settings, "figma_file", m_figPathInput );
        s_appendSettingValue( &settings, "fonts_folder", m_fontsPathInput );
        s_appendSettingValue( &settings, "start_frame_id", m_startFrameIdInput );
        s_appendSettingInt32( &settings, "file_index", m_fileIndex );
        s_appendSettingInt32( &settings, "entry_index", m_entryIndex );
        s_appendSettingInt32( &settings, "auto_viewport", m_autoViewportInput == true ? 1 : 0 );
        s_appendSettingFloat( &settings, "viewport_x", m_viewportInput[0] );
        s_appendSettingFloat( &settings, "viewport_y", m_viewportInput[1] );
        s_appendSettingFloat( &settings, "position_x", m_positionInput[0] );
        s_appendSettingFloat( &settings, "position_y", m_positionInput[1] );
        s_appendSettingFloat( &settings, "scale", m_scaleInput );
        s_appendSettingInt32( &settings, "playback_rate_index", m_playbackRateIndex );

        OutputStreamInterfacePtr stream = Helper::openOutputStreamFile( userFileGroup, STRINGIZE_FILEPATH_LOCAL( "figma_viewer.ini" ), true, MENGINE_DOCUMENT_FACTORABLE );

        if( stream == nullptr )
        {
            return;
        }

        stream->write( settings.data(), settings.size() );
        stream->flush();

        Helper::closeOutputStreamFile( userFileGroup, stream );
    }
    //////////////////////////////////////////////////////////////////////////
    void FigmaViewerExampleSceneEventReceiver::applySelection_()
    {
        if( m_scene == nullptr )
        {
            return;
        }

        this->clearFigma_();

        FileGroupInterfacePtr fileGroup;
        FileGroupInterfacePtr mountedFileGroup;
        FilePath figPath;

        if( this->resolveInputPaths_( &fileGroup, &mountedFileGroup, &figPath ) == false )
        {
            return;
        }

        m_mountedFileGroup = mountedFileGroup;

        ResourceFigmaPtr resource = this->createFigmaResource_( fileGroup, figPath );

        if( resource == nullptr )
        {
            this->clearFigma_();
            return;
        }

        if( m_autoViewportInput == true )
        {
            mt::vec2f frameViewportSize;

            if( resource->getFrameViewportSize( m_startFrameIdInput, &frameViewportSize ) == true )
            {
                m_viewportInput[0] = frameViewportSize.x;
                m_viewportInput[1] = frameViewportSize.y;

                this->saveSettings_();
            }
        }

        NodePtr figmaNode = PROTOTYPE_SERVICE()
            ->generatePrototype( STRINGIZE_STRING_LOCAL( "Node" ), STRINGIZE_STRING_LOCAL( "Figma" ), MENGINE_DOCUMENT_FACTORABLE );

        if( figmaNode == nullptr )
        {
            resource->finalize();
            this->clearMountedFileGroup_();
            m_status = "Figma node prototype is not registered";
            return;
        }

        UnknownFigmaInterface * figmaUnknown = figmaNode->getUnknown();

        if( figmaUnknown == nullptr )
        {
            figmaNode->dispose();
            resource->finalize();
            this->clearMountedFileGroup_();
            m_status = "Figma node has no UnknownFigmaInterface";
            return;
        }

        figmaNode->setName( STRINGIZE_STRING_LOCAL( "FigmaViewerExampleNode" ) );

        figmaUnknown->setViewportSize( mt::vec2f( m_viewportInput[0], m_viewportInput[1] ) );
        figmaUnknown->setViewportScale( m_scaleInput );
        figmaUnknown->setStartFrameId( m_startFrameIdInput );
        figmaUnknown->setFontSearchPath( m_fontsPathInput );
        figmaUnknown->setPlaybackRate( s_figmaViewerPlaybackRate( m_playbackRateIndex ).rate );
        figmaUnknown->setResourceFigma( resource );

        m_scene->addChildFront( figmaNode );

        m_figmaNode = figmaNode;
        m_figmaResource = resource;
        m_figmaUnknown = figmaUnknown;
        this->updateFigmaViewport_();
        m_status = "Opened";
    }
    //////////////////////////////////////////////////////////////////////////
    void FigmaViewerExampleSceneEventReceiver::updateFigmaViewport_()
    {
        const Resolution & windowResolution = APPLICATION_SERVICE()
            ->getCurrentWindowResolution();

        const Resolution & currentContentResolution = APPLICATION_SERVICE()
            ->getContentResolution();

        if( currentContentResolution != windowResolution )
        {
            APPLICATION_SERVICE()->setContentResolution( windowResolution );
        }

        if( m_figmaNode == nullptr || m_figmaUnknown == nullptr )
        {
            return;
        }

        const Resolution & contentResolution = APPLICATION_SERVICE()
            ->getContentResolution();

        const float contentWidth = contentResolution.getWidthF();
        const float contentHeight = contentResolution.getHeightF();
        float outputScale = std::max( 0.01f, m_scaleInput );
        float positionX = (contentWidth - m_viewportInput[0] * outputScale) * 0.5f;
        float positionY = (contentHeight - m_viewportInput[1] * outputScale) * 0.5f;

        if( m_autoViewportInput == true )
        {
            outputScale = s_calcAutoOutputScale( contentWidth, contentHeight, m_viewportInput[0], m_viewportInput[1] );

            const float outputWidth = m_viewportInput[0] * outputScale;
            const float outputHeight = m_viewportInput[1] * outputScale;

            positionX = (contentWidth - outputWidth) * 0.5f;
            positionY = (contentHeight - outputHeight) * 0.5f;
            m_scaleInput = outputScale;
        }

        m_figmaUnknown->setViewportSize( mt::vec2f( m_viewportInput[0], m_viewportInput[1] ) );
        m_figmaUnknown->setViewportScale( outputScale );

        TransformationInterface * transformation = m_figmaNode->getTransformation();

        transformation->setLocalPosition( mt::vec3f( positionX + m_positionInput[0], positionY + m_positionInput[1], 0.f ) );
        transformation->setLocalScale( mt::vec3f( outputScale, outputScale, 1.f ) );
    }
    //////////////////////////////////////////////////////////////////////////
    bool FigmaViewerExampleSceneEventReceiver::resolveInputPaths_( FileGroupInterfacePtr * const _fileGroup, FileGroupInterfacePtr * const _mountedFileGroup, FilePath * const _figPath )
    {
        const String figPath = s_normalizePathString( m_figPathInput );
        const String fontsPath = s_normalizePathString( m_fontsPathInput );

        if( figPath.empty() == true )
        {
            m_status = "Missing .fig path";
            return false;
        }

        const bool figPathAbsolute = s_isAbsolutePath( figPath );

        if( figPathAbsolute == false )
        {
            m_status = ".fig path must be absolute";
            return false;
        }

        if( fontsPath.empty() == false )
        {
            const String fontsDirectoryPath = s_makeDirectoryPath( fontsPath );

            if( s_isAbsolutePath( fontsDirectoryPath ) == false )
            {
                m_status = "Fonts folder must be absolute";
                return false;
            }

            if( FILE_SYSTEM()->existDirectory( "", fontsDirectoryPath.c_str() ) == false )
            {
                m_status = "Missing fonts folder";
                return false;
            }
        }

        FileGroupInterfacePtr fileGroup;
        FileGroupInterfacePtr mountedFileGroup;
        String resolvedFigPath;

        String folderPath;

        if( s_splitFullPath( figPath, &folderPath, &resolvedFigPath ) == false )
        {
            m_status = "Invalid .fig path";
            return false;
        }

        const FilePath mountedFolderPath = s_makeFilePath( folderPath );

        if( FILE_SERVICE()
            ->mountFileGroup( STRINGIZE_STRING_LOCAL( "FigmaViewerExampleFiles" ), nullptr, nullptr, mountedFolderPath, STRINGIZE_STRING_LOCAL( "global" ), &mountedFileGroup, false, MENGINE_DOCUMENT_FACTORABLE ) == false )
        {
            m_status = "Failed to mount .fig folder";
            return false;
        }

        fileGroup = mountedFileGroup;

        *_fileGroup = fileGroup;
        *_mountedFileGroup = mountedFileGroup;
        *_figPath = s_makeFilePath( resolvedFigPath );

        return true;
    }
    //////////////////////////////////////////////////////////////////////////
    ResourceFigmaPtr FigmaViewerExampleSceneEventReceiver::createFigmaResource_( const FileGroupInterfacePtr & _fileGroup, const FilePath & _figPath )
    {
        if( _fileGroup == nullptr )
        {
            m_status = "File group is not mounted";
            return nullptr;
        }

        if( _fileGroup->existFile( _figPath, false ) == false )
        {
            m_status = "Missing .fig file";
            return nullptr;
        }

        DataflowInterfacePtr dataflow = VOCABULARY_GET( STRINGIZE_STRING_LOCAL( "Dataflow" ), STRINGIZE_STRING_LOCAL( "figma" ) );

        if( dataflow == nullptr )
        {
            m_status = "Figma dataflow is not registered";
            return nullptr;
        }

        ContentInterfacePtr content = Helper::makeFileContent( _fileGroup, _figPath, MENGINE_DOCUMENT_FACTORABLE );

        MENGINE_ASSERTION_MEMORY_PANIC( content, "invalid create Figma viewer content" );

        content->setDataflow( dataflow );

        ResourceFigmaPtr resource = PROTOTYPE_SERVICE()
            ->generatePrototype( STRINGIZE_STRING_LOCAL( "Resource" ), STRINGIZE_STRING_LOCAL( "ResourceFigma" ), MENGINE_DOCUMENT_FACTORABLE );

        if( resource == nullptr )
        {
            m_status = "ResourceFigma prototype is not registered";
            return nullptr;
        }

        resource->setName( STRINGIZE_STRING_LOCAL( "FigmaViewerExampleResource" ) );
        resource->setContent( content );

        if( resource->initialize() == false )
        {
            m_status = "ResourceFigma initialize failed";
            return nullptr;
        }

        return resource;
    }
    //////////////////////////////////////////////////////////////////////////
    void FigmaViewerExampleSceneEventReceiver::clearFigma_()
    {
        if( m_figmaNode != nullptr )
        {
            m_figmaNode->dispose();
            m_figmaNode = nullptr;
            m_figmaUnknown = nullptr;
        }

        if( m_figmaResource != nullptr )
        {
            m_figmaResource->finalize();
            m_figmaResource = nullptr;
        }

        this->clearMountedFileGroup_();
    }
    //////////////////////////////////////////////////////////////////////////
    void FigmaViewerExampleSceneEventReceiver::clearMountedFileGroup_()
    {
        if( m_mountedFileGroup == nullptr )
        {
            return;
        }

        FILE_SERVICE()
            ->unmountFileGroup( STRINGIZE_STRING_LOCAL( "FigmaViewerExampleFiles" ) );

        m_mountedFileGroup = nullptr;
    }
    //////////////////////////////////////////////////////////////////////////
    void FigmaViewerExampleSceneEventReceiver::resetEntryParams_()
    {
        const FigmaViewerFileDesc & file = s_figmaViewerFile( m_fileIndex );
        const FigmaViewerEntryDesc & entry = s_figmaViewerEntry( file, m_entryIndex );

        m_viewportInput[0] = entry.width;
        m_viewportInput[1] = entry.height;
        m_scaleInput = entry.scale;
        s_copyPathInput( m_startFrameIdInput, sizeof( m_startFrameIdInput ), entry.frameId );
    }
    //////////////////////////////////////////////////////////////////////////
}
