#include "FontEffectViewerExampleSceneEventReceiver.h"

#include "Interface/ApplicationInterface.h"
#include "Interface/PrototypeServiceInterface.h"
#include "Interface/FontServiceInterface.h"
#include "Interface/FileServiceInterface.h"
#include "Interface/RenderTextureServiceInterface.h"
#include "Interface/TransformationInterface.h"

#include "Plugins/FontEffectPlugin/FontEffectPluginInterface.h"
#include "Plugins/FontEffectPlugin/FontEffectDataInterface.h"
#include "Plugins/FontEffectPlugin/FontEffectSerialization.h"
#include "Plugins/TTFPlugin/TTFFont.h"
#include "Plugins/TTFPlugin/TTFFontGlyph.h"
#include "Plugins/MCPPlugin/MCPInterface.h"

#include "FontEffectViewerExampleFileDialog.h"
#include "FontEffectViewerExampleDescEditor.h"

#include "Kernel/AssertionMemoryPanic.h"
#include "Kernel/ConstStringHelper.h"
#include "Kernel/FilePathHelper.h"
#include "Kernel/DocumentHelper.h"
#include "Kernel/Entity.h"
#include "Kernel/NodeCast.h"
#include "Kernel/ContentHelper.h"
#include "Kernel/VocabularyHelper.h"
#include "Kernel/JSONHelper.h"
#include "Kernel/Dataflow.h"
#include "Kernel/Logger.h"
#include "Kernel/U32String.h"

#include "Config/StdString.h"
#include "Config/StdIO.h"

#include "math/utils.h"

#include "imgui.h"

namespace Mengine
{
    namespace Detail
    {
        //////////////////////////////////////////////////////////////////////////
        constexpr uint32_t SAMPLE_COUNT = 3;
        static const uint32_t SAMPLE_VALUES[SAMPLE_COUNT] = {1, 2, 4};
        static const Char * SAMPLE_NAMES[SAMPLE_COUNT] = {"1", "2", "4"};
        //////////////////////////////////////////////////////////////////////////
        static bool splitFullPath( const String & _fullPath, String * const _folder, String * const _file )
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
        static FilePath makeFilePath( const String & _path )
        {
            if( _path.empty() == true )
            {
                return FilePath::none();
            }

            FilePath filePath = Helper::stringizeFilePathSize( _path.c_str(), (FilePath::size_type)_path.size() );

            return filePath;
        }
        //////////////////////////////////////////////////////////////////////////
        static void unmountFolder( const ConstString & _groupName )
        {
            FileGroupInterfacePtr mountedFileGroup;
            if( FILE_SERVICE()
                ->hasFileGroup( _groupName, &mountedFileGroup ) == false )
            {
                return;
            }

            FILE_SERVICE()
                ->unmountFileGroup( _groupName );
        }
        //////////////////////////////////////////////////////////////////////////
        static bool mountFolder( const ConstString & _groupName, const String & _folder, FileGroupInterfacePtr * const _fileGroup )
        {
            unmountFolder( _groupName );

            FileGroupInterfacePtr mountedFileGroup;
            if( FILE_SERVICE()
                ->mountFileGroup( _groupName, nullptr, nullptr, makeFilePath( _folder ), STRINGIZE_STRING_LOCAL( "global" ), &mountedFileGroup, false, MENGINE_DOCUMENT_FUNCTION ) == false )
            {
                return false;
            }

            *_fileGroup = mountedFileGroup;

            return true;
        }
        //////////////////////////////////////////////////////////////////////////
        static int findString( const VectorString & _values, const Char * _value )
        {
            for( VectorString::size_type index = 0; index != _values.size(); ++index )
            {
                if( _values[index] == _value )
                {
                    return (int)index;
                }
            }

            return -1;
        }
        //////////////////////////////////////////////////////////////////////////
    }
    //////////////////////////////////////////////////////////////////////////
    FontEffectViewerExampleSceneEventReceiver::FontEffectViewerExampleSceneEventReceiver()
        : m_scene( nullptr )
        , m_previewZoom( 1.f )
        , m_previewPan( 0.f, 0.f )
        , m_previewPanning( false )
        , m_sidebarWidth( 440.f )
        , m_loadedGlyphCounter( 0 )
        , m_presetIndex( -1 )
        , m_glyphIndex( -1 )
        , m_height( 64 )
        , m_sampleIndex( 1 )
        , m_noEffect( false )
        , m_showGlyphPreview( true )
        , m_previewScale( 2.f )
        , m_dirty( false )
        , m_mcpHandlerRegistered( false )
    {
        m_presetNameInput[0] = '\0';
        m_presetsPathInput[0] = '\0';
        m_glyphPathInput[0] = '\0';

        StdString::strcpy_safe( m_textInput, "Font Effect\nAbc 123", sizeof( m_textInput ) );

        m_fontColor[0] = 1.f;
        m_fontColor[1] = 1.f;
        m_fontColor[2] = 1.f;
        m_fontColor[3] = 1.f;
    }
    //////////////////////////////////////////////////////////////////////////
    FontEffectViewerExampleSceneEventReceiver::~FontEffectViewerExampleSceneEventReceiver()
    {
    }
    //////////////////////////////////////////////////////////////////////////
    bool FontEffectViewerExampleSceneEventReceiver::onEntityCreate( const EntityBehaviorInterfacePtr & _behavior, Entity * _entity )
    {
        MENGINE_UNUSED( _behavior );

        m_scene = Helper::staticNodeCast<Scene *>( _entity );

        return true;
    }
    //////////////////////////////////////////////////////////////////////////
    void FontEffectViewerExampleSceneEventReceiver::onEntityDestroy( const EntityBehaviorInterfacePtr & _behavior )
    {
        MENGINE_UNUSED( _behavior );

        m_scene = nullptr;
    }
    //////////////////////////////////////////////////////////////////////////
    bool FontEffectViewerExampleSceneEventReceiver::onEntityPreparation( const EntityBehaviorInterfacePtr & _behavior )
    {
        MENGINE_UNUSED( _behavior );

        APPLICATION_SERVICE()
            ->setFixedContentResolution( false );

        APPLICATION_SERVICE()
            ->setFixedDisplayResolution( false );

        APPLICATION_SERVICE()
            ->setFixedViewportResolution( false );

        this->loadSettings_();

        if( this->loadBundledFonts_() == false )
        {
            m_status = "Failed to load bundled Glyphs.json/Fonts.json (run build/downloads/downloads.sh)";
        }

        if( this->loadBundledEffects_() == false )
        {
            m_status = "Failed to load bundled Effects.json";
        }

        if( m_glyphIndex < 0 && m_glyphNames.empty() == false )
        {
            m_glyphIndex = 0;
        }

        if( m_glyphIndex >= 0 )
        {
            this->selectGlyph_( Helper::stringizeString( m_glyphNames[m_glyphIndex].c_str() ) );
        }

        if( m_glyphPathInput[0] != '\0' )
        {
            this->loadGlyphFile_( m_glyphPathInput );
        }

        ImGUIRenderPtr imguiRender = PROTOTYPE_SERVICE()
            ->generatePrototype( STRINGIZE_STRING_LOCAL( "Node" ), STRINGIZE_STRING_LOCAL( "ImGUIRender" ), MENGINE_DOCUMENT_FACTORABLE );

        MENGINE_ASSERTION_MEMORY_PANIC( imguiRender, "invalid create ImGUIRender" );

        imguiRender->setName( STRINGIZE_STRING_LOCAL( "FontEffectViewerExampleImGUI" ) );
        imguiRender->setProvider( [this]( const ImGUIRenderProviderInterfacePtr & _provider )
        {
            this->renderControls_( _provider );
        } );

        RenderInterface * imguiRenderInterface = imguiRender->getRender();
        imguiRenderInterface->setZIndex( 1000 );

        m_scene->addChild( imguiRender );
        m_imguiRender = imguiRender;

        if( this->createPreviewCanvas_() == false )
        {
            m_status = "Failed to create preview canvas";
        }

        this->registerMCPHandler_();

        m_dirty = true;

        return true;
    }
    //////////////////////////////////////////////////////////////////////////
    void FontEffectViewerExampleSceneEventReceiver::onEntityDeactivate( const EntityBehaviorInterfacePtr & _behavior )
    {
        MENGINE_UNUSED( _behavior );

        this->unregisterMCPHandler_();

        this->saveSettings_();

        this->destroyPreview_();
        this->destroyPreviewCanvas_();

        if( m_font != nullptr )
        {
            const ConstString & fontName = m_font->getName();

            m_font = nullptr;

            FONT_SERVICE()
                ->removeFont( fontName );
        }

        m_glyph = nullptr;

        if( m_loadedGlyphName.empty() == false )
        {
            FONT_SERVICE()
                ->removeGlyph( m_loadedGlyphName );

            m_loadedGlyphName.clear();
        }

        if( m_imguiRender != nullptr )
        {
            m_imguiRender->dispose();
            m_imguiRender = nullptr;
        }

        m_glyphFileGroup = nullptr;
        m_presetsFileGroup = nullptr;

        Detail::unmountFolder( STRINGIZE_STRING_LOCAL( "FontEffectViewerGlyphs" ) );
        Detail::unmountFolder( STRINGIZE_STRING_LOCAL( "FontEffectViewerPresets" ) );
    }
    //////////////////////////////////////////////////////////////////////////
    void FontEffectViewerExampleSceneEventReceiver::registerMCPHandler_()
    {
        if( SERVICE_IS_INITIALIZE( MCPServiceInterface ) == false )
        {
            return;
        }

        if( MCP_SERVICE()
            ->addHandler( STRINGIZE_STRING_LOCAL( "font_effect_viewer" ), MCPHandlerInterfacePtr( this ) ) == false )
        {
            return;
        }

        m_mcpHandlerRegistered = true;
    }
    //////////////////////////////////////////////////////////////////////////
    void FontEffectViewerExampleSceneEventReceiver::unregisterMCPHandler_()
    {
        if( m_mcpHandlerRegistered == false )
        {
            return;
        }

        m_mcpHandlerRegistered = false;

        if( SERVICE_IS_INITIALIZE( MCPServiceInterface ) == false )
        {
            return;
        }

        MCP_SERVICE()
            ->removeHandler( MCPHandlerInterfacePtr( this ) );
    }
    //////////////////////////////////////////////////////////////////////////
    void FontEffectViewerExampleSceneEventReceiver::fillMCPState_( jpp::object * const _result ) const
    {
        jpp::array j_presets = jpp::make_array();

        for( const String & name : m_presetNames )
        {
            j_presets.push_back( name.c_str() );
        }

        jpp::array j_glyphs = jpp::make_array();

        for( const String & name : m_glyphNames )
        {
            j_glyphs.push_back( name.c_str() );
        }

        _result->set( "preset", m_presetNameInput );
        _result->set( "presets", j_presets.to_object() );
        _result->set( "presets_file", m_presetsPathInput );
        _result->set( "glyphs", j_glyphs.to_object() );
        _result->set( "glyph", m_glyph != nullptr ? m_glyph->getName().c_str() : "" );
        _result->set( "glyph_file", m_glyphPathInput );
        _result->set( "no_effect", m_noEffect );
        _result->set( "height", m_height );
        _result->set( "zoom", m_previewZoom );
        _result->set( "pan_x", m_previewPan.x );
        _result->set( "pan_y", m_previewPan.y );
        _result->set( "sample", Detail::SAMPLE_VALUES[m_sampleIndex] );
        _result->set( "text", m_textInput );
        _result->set( "dirty", m_dirty );
        _result->set( "status", m_status.c_str() );
        _result->set( "font_compiled", m_font != nullptr && m_font->isCompileFont() == true );
        _result->set( "layout_count", m_font != nullptr && m_font->isCompileFont() == true ? m_font->getLayoutCount() : 0U );

        jpp::object j_desc;
        Helper::dumpFontEffectDesc( m_desc, &j_desc );

        _result->set( "desc", j_desc );
    }
    //////////////////////////////////////////////////////////////////////////
    bool FontEffectViewerExampleSceneEventReceiver::onMCPCall( const jpp::object & _arguments, jpp::object * const _result )
    {
        *_result = jpp::make_object();

        const Char * action = _arguments.get( "action", "get" );

        if( StdString::strcmp( action, "set" ) == 0 )
        {
            jpp::object j_preset;
            if( _arguments.exist( "preset", &j_preset ) == true )
            {
                const Char * preset = j_preset;

                int index = Detail::findString( m_presetNames, preset );

                if( index < 0 )
                {
                    _result->set( "error", "preset not found" );

                    return false;
                }

                this->selectPreset_( index );
            }

            jpp::object j_glyph;
            if( _arguments.exist( "glyph", &j_glyph ) == true )
            {
                const Char * glyph = j_glyph;

                int index = Detail::findString( m_glyphNames, glyph );

                if( index < 0 )
                {
                    _result->set( "error", "glyph not found" );

                    return false;
                }

                m_glyphIndex = index;

                this->selectGlyph_( Helper::stringizeString( glyph ) );
            }

            jpp::object j_glyphFile;
            if( _arguments.exist( "glyph_file", &j_glyphFile ) == true )
            {
                const Char * glyphFile = j_glyphFile;

                if( this->loadGlyphFile_( glyphFile ) == false )
                {
                    _result->set( "error", m_status.c_str() );

                    return false;
                }
            }

            jpp::object j_noEffect;
            if( _arguments.exist( "no_effect", &j_noEffect ) == true )
            {
                m_noEffect = j_noEffect;
                m_dirty = true;
            }

            jpp::object j_height;
            if( _arguments.exist( "height", &j_height ) == true )
            {
                int height = j_height;

                m_height = mt::clamp( 8, height, 200 );
                m_dirty = true;
            }

            jpp::object j_zoom;
            if( _arguments.exist( "zoom", &j_zoom ) == true )
            {
                float zoom = j_zoom;

                jpp::object j_cursorX;
                jpp::object j_cursorY;
                bool hasCursorX = _arguments.exist( "cursor_x", &j_cursorX );
                bool hasCursorY = _arguments.exist( "cursor_y", &j_cursorY );

                bool hasCursor = hasCursorX == true && hasCursorY == true;

                if( hasCursor == true )
                {
                    this->zoomPreviewAt_( zoom, (float)j_cursorX, (float)j_cursorY );
                }
                else
                {
                    m_previewZoom = mt::clamp( 0.25f, zoom, 8.f );

                    if( m_previewCanvas != nullptr )
                    {
                        m_previewCanvas->setZoom( m_previewZoom );
                    }
                }
            }

            jpp::object j_panX;
            bool hasPanX = _arguments.exist( "pan_x", &j_panX );

            if( hasPanX == true )
            {
                m_previewPan.x = (float)j_panX;
            }

            jpp::object j_panY;
            bool hasPanY = _arguments.exist( "pan_y", &j_panY );

            if( hasPanY == true )
            {
                m_previewPan.y = (float)j_panY;
            }

            if( hasPanX == true || hasPanY == true )
            {
                this->clampPreviewPan_();

                this->updatePreviewPosition_();
            }

            jpp::object j_resetView;
            if( _arguments.exist( "reset_view", &j_resetView ) == true )
            {
                bool resetView = j_resetView;

                if( resetView == true )
                {
                    m_previewZoom = 1.f;
                    m_previewPan = mt::vec2f( 0.f, 0.f );
                    m_previewPanning = false;

                    if( m_previewCanvas != nullptr )
                    {
                        m_previewCanvas->setZoom( m_previewZoom );
                    }

                    this->updatePreviewPosition_();
                }
            }

            jpp::object j_sample;
            if( _arguments.exist( "sample", &j_sample ) == true )
            {
                uint32_t sample = j_sample;

                for( int sampleIndex = 0; sampleIndex != (int)Detail::SAMPLE_COUNT; ++sampleIndex )
                {
                    if( Detail::SAMPLE_VALUES[sampleIndex] == sample )
                    {
                        m_sampleIndex = sampleIndex;
                        m_dirty = true;
                    }
                }
            }

            jpp::object j_text;
            if( _arguments.exist( "text", &j_text ) == true )
            {
                const Char * text = j_text;

                StdString::strcpy_safe( m_textInput, text, sizeof( m_textInput ) );
                m_dirty = true;
            }

            jpp::object j_name;
            if( _arguments.exist( "name", &j_name ) == true )
            {
                const Char * name = j_name;

                StdString::strcpy_safe( m_presetNameInput, name, sizeof( m_presetNameInput ) );
            }

            if( m_dirty == true )
            {
                this->rebuildPreview_();
            }
        }
        else if( StdString::strcmp( action, "load" ) == 0 )
        {
            const Char * path = _arguments.get( "path", "" );

            if( this->loadPresetsFile_( path ) == false )
            {
                _result->set( "error", m_status.c_str() );

                return false;
            }

            if( m_dirty == true )
            {
                this->rebuildPreview_();
            }
        }
        else if( StdString::strcmp( action, "save" ) == 0 )
        {
            const Char * path = _arguments.get( "path", "" );

            if( this->savePresetsFile_( path ) == false )
            {
                _result->set( "error", m_status.c_str() );

                return false;
            }
        }
        else if( StdString::strcmp( action, "store" ) == 0 )
        {
            jpp::object j_name;
            if( _arguments.exist( "name", &j_name ) == true )
            {
                const Char * name = j_name;

                StdString::strcpy_safe( m_presetNameInput, name, sizeof( m_presetNameInput ) );
            }

            this->storeCurrentPreset_();
        }
        else if( StdString::strcmp( action, "reload_bundled" ) == 0 )
        {
            this->loadBundledEffects_();

            if( m_dirty == true )
            {
                this->rebuildPreview_();
            }
        }
        else if( StdString::strcmp( action, "get" ) != 0 )
        {
            _result->set( "error", "unknown action" );

            return false;
        }

        this->fillMCPState_( _result );

        return true;
    }
    //////////////////////////////////////////////////////////////////////////
    bool FontEffectViewerExampleSceneEventReceiver::loadBundledFonts_()
    {
        const FileGroupInterfacePtr & fileGroup = VOCABULARY_GET( STRINGIZE_STRING_LOCAL( "FileGroup" ), ConstString::none() );

        MENGINE_ASSERTION_MEMORY_PANIC( fileGroup, "invalid get default file group" );

        ContentInterfacePtr glyphsContent = Helper::makeFileContent( fileGroup, STRINGIZE_FILEPATH_LOCAL( "Glyphs.json" ), MENGINE_DOCUMENT_FACTORABLE );

        if( FONT_SERVICE()
            ->loadGlyphs( glyphsContent, MENGINE_DOCUMENT_FACTORABLE ) == false )
        {
            LOGGER_ERROR( "font effect viewer: failed to load 'Glyphs.json'" );

            return false;
        }

        ContentInterfacePtr fontsContent = Helper::makeFileContent( fileGroup, STRINGIZE_FILEPATH_LOCAL( "Fonts.json" ), MENGINE_DOCUMENT_FACTORABLE );

        if( FONT_SERVICE()
            ->loadFonts( fontsContent, MENGINE_DOCUMENT_FACTORABLE ) == false )
        {
            LOGGER_ERROR( "font effect viewer: failed to load 'Fonts.json'" );

            return false;
        }

        m_glyphNames.clear();

        FONT_SERVICE()
            ->foreachGlyphs( [this]( const FontGlyphInterfacePtr & _glyph )
        {
            const ConstString & name = _glyph->getName();

            m_glyphNames.emplace_back( String( name.c_str() ) );
        } );

        return true;
    }
    //////////////////////////////////////////////////////////////////////////
    bool FontEffectViewerExampleSceneEventReceiver::loadBundledEffects_()
    {
        const FileGroupInterfacePtr & fileGroup = VOCABULARY_GET( STRINGIZE_STRING_LOCAL( "FileGroup" ), ConstString::none() );

        MENGINE_ASSERTION_MEMORY_PANIC( fileGroup, "invalid get default file group" );

        ContentInterfacePtr content = Helper::makeFileContent( fileGroup, STRINGIZE_FILEPATH_LOCAL( "Effects.json" ), MENGINE_DOCUMENT_FACTORABLE );

        m_presetsFileGroup = nullptr;
        m_presetsFileName.clear();

        if( this->setPresetsContent_( content, m_presetNameInput ) == false )
        {
            return false;
        }

        return true;
    }
    //////////////////////////////////////////////////////////////////////////
    void FontEffectViewerExampleSceneEventReceiver::loadSettings_()
    {
        const FileGroupInterfacePtr & userFileGroup = FILE_SERVICE()
            ->getFileGroup( STRINGIZE_STRING_LOCAL( "user" ) );

        if( userFileGroup == nullptr )
        {
            return;
        }

        const FilePath settingsPath = STRINGIZE_FILEPATH_LOCAL( "font_effect_viewer.json" );

        if( userFileGroup->existFile( settingsPath, false ) == false )
        {
            return;
        }

        jpp::object settings = Helper::loadJSONFile( userFileGroup, settingsPath, MENGINE_DOCUMENT_FACTORABLE );

        if( settings.invalid() == true )
        {
            return;
        }

        StdString::strcpy_safe( m_presetsPathInput, settings.get( "presets_file", "" ), sizeof( m_presetsPathInput ) );
        StdString::strcpy_safe( m_glyphPathInput, settings.get( "glyph_file", "" ), sizeof( m_glyphPathInput ) );
        StdString::strcpy_safe( m_presetNameInput, settings.get( "preset", "" ), sizeof( m_presetNameInput ) );
        StdString::strcpy_safe( m_textInput, settings.get( "text", m_textInput ), sizeof( m_textInput ) );

        m_height = settings.get( "height", m_height );
        m_sampleIndex = settings.get( "sample_index", m_sampleIndex );
        m_previewScale = settings.get( "preview_scale", m_previewScale );
        m_showGlyphPreview = settings.get( "show_glyph_preview", m_showGlyphPreview );
        m_noEffect = settings.get( "no_effect", m_noEffect );

        if( m_sampleIndex < 0 || m_sampleIndex >= (int)Detail::SAMPLE_COUNT )
        {
            m_sampleIndex = 1;
        }

        if( m_height < 8 )
        {
            m_height = 8;
        }

        if( m_height > 200 )
        {
            m_height = 200;
        }
    }
    //////////////////////////////////////////////////////////////////////////
    void FontEffectViewerExampleSceneEventReceiver::saveSettings_() const
    {
        const FileGroupInterfacePtr & userFileGroup = FILE_SERVICE()
            ->getFileGroup( STRINGIZE_STRING_LOCAL( "user" ) );

        if( userFileGroup == nullptr )
        {
            return;
        }

        jpp::object settings = jpp::make_object();
        settings.set( "presets_file", m_presetsPathInput );
        settings.set( "glyph_file", m_glyphPathInput );
        settings.set( "preset", m_presetNameInput );
        settings.set( "text", m_textInput );
        settings.set( "height", m_height );
        settings.set( "sample_index", m_sampleIndex );
        settings.set( "preview_scale", m_previewScale );
        settings.set( "show_glyph_preview", m_showGlyphPreview );
        settings.set( "no_effect", m_noEffect );

        Helper::writeJSONFile( settings, userFileGroup, STRINGIZE_FILEPATH_LOCAL( "font_effect_viewer.json" ), true, MENGINE_DOCUMENT_FACTORABLE );
    }
    //////////////////////////////////////////////////////////////////////////
    bool FontEffectViewerExampleSceneEventReceiver::setPresetsContent_( const ContentInterfacePtr & _content, const Char * _preferredPreset )
    {
        DataflowInterfacePtr dataflow = VOCABULARY_GET( STRINGIZE_STRING_LOCAL( "Dataflow" ), STRINGIZE_STRING_LOCAL( "fontEffect" ) );

        if( dataflow == nullptr )
        {
            m_status = "FontEffectDesc dataflow is not registered";

            return false;
        }

        _content->setDataflow( dataflow );

        DataflowContext context;
        context.filePath = _content->getFilePath();

        FontEffectDataInterfacePtr data = Helper::getDataflow( _content, &context, MENGINE_DOCUMENT_FACTORABLE );

        if( data == nullptr )
        {
            m_status = "Failed to parse JSON";

            return false;
        }

        VectorFontEffectViewerExamplePresetEntries presets;
        VectorString names;

        data->foreachEffect( [&presets, &names]( const ConstString & _name, const FontEffectDesc & _desc, uint32_t _sample )
        {
            FontEffectViewerExamplePresetEntryDesc entry;
            entry.desc = _desc;
            entry.sample = _sample;

            presets.emplace_back( entry );
            names.emplace_back( String( _name.c_str() ) );
        } );

        m_presets = presets;
        m_presetNames = names;

        int index = Detail::findString( m_presetNames, _preferredPreset );

        if( index < 0 && m_presetNames.empty() == false )
        {
            index = 0;
        }

        if( index >= 0 )
        {
            this->selectPreset_( index );
        }
        else
        {
            m_presetIndex = -1;
        }

        return true;
    }
    //////////////////////////////////////////////////////////////////////////
    bool FontEffectViewerExampleSceneEventReceiver::selectPreset_( int _index )
    {
        if( _index < 0 || _index >= (int)m_presets.size() )
        {
            return false;
        }

        const String & name = m_presetNames[_index];
        const FontEffectViewerExamplePresetEntryDesc & entry = m_presets[_index];

        for( int sampleIndex = 0; sampleIndex != (int)Detail::SAMPLE_COUNT; ++sampleIndex )
        {
            if( Detail::SAMPLE_VALUES[sampleIndex] == entry.sample )
            {
                m_sampleIndex = sampleIndex;
            }
        }

        m_desc = entry.desc;
        m_presetIndex = _index;

        StdString::strcpy_safe( m_presetNameInput, name.c_str(), sizeof( m_presetNameInput ) );

        m_status = "Preset '" + name + "'";

        m_dirty = true;

        return true;
    }
    //////////////////////////////////////////////////////////////////////////
    bool FontEffectViewerExampleSceneEventReceiver::loadPresetsFile_( const Char * _fullPath )
    {
        String folder;
        String file;

        if( Detail::splitFullPath( String( _fullPath ), &folder, &file ) == false )
        {
            m_status = "Invalid JSON path";

            return false;
        }

        FileGroupInterfacePtr fileGroup;
        if( Detail::mountFolder( STRINGIZE_STRING_LOCAL( "FontEffectViewerPresets" ), folder, &fileGroup ) == false )
        {
            m_status = "Failed to mount JSON folder";

            return false;
        }

        ContentInterfacePtr content = Helper::makeFileContent( fileGroup, Detail::makeFilePath( file ), MENGINE_DOCUMENT_FACTORABLE );

        if( this->setPresetsContent_( content, m_presetNameInput ) == false )
        {
            return false;
        }

        m_presetsFileGroup = fileGroup;
        m_presetsFileName = file;

        StdString::strcpy_safe( m_presetsPathInput, _fullPath, sizeof( m_presetsPathInput ) );

        return true;
    }
    //////////////////////////////////////////////////////////////////////////
    void FontEffectViewerExampleSceneEventReceiver::storeCurrentPreset_()
    {
        FontEffectViewerExamplePresetEntryDesc entry;
        entry.desc = m_desc;
        entry.sample = Detail::SAMPLE_VALUES[m_sampleIndex];

        const Char * presetName = m_presetNameInput[0] != '\0' ? m_presetNameInput : "Effect";

        int index = Detail::findString( m_presetNames, presetName );

        if( index < 0 )
        {
            m_presetNames.emplace_back( String( presetName ) );
            m_presets.emplace_back( entry );

            index = (int)m_presetNames.size() - 1;
        }
        else
        {
            m_presets[index] = entry;
        }

        m_presetIndex = index;
    }
    //////////////////////////////////////////////////////////////////////////
    bool FontEffectViewerExampleSceneEventReceiver::savePresetsFile_( const Char * _fullPath )
    {
        String folder;
        String file;

        if( Detail::splitFullPath( String( _fullPath ), &folder, &file ) == false )
        {
            m_status = "Invalid save path";

            return false;
        }

        FileGroupInterfacePtr fileGroup;
        if( Detail::mountFolder( STRINGIZE_STRING_LOCAL( "FontEffectViewerPresets" ), folder, &fileGroup ) == false )
        {
            m_status = "Failed to mount save folder";

            return false;
        }

        this->storeCurrentPreset_();

        jpp::object root = jpp::make_object();
        root.set( "Version", 1 );

        jpp::object j_effects = jpp::make_object();

        for( VectorFontEffectViewerExamplePresetEntries::size_type index = 0; index != m_presets.size(); ++index )
        {
            const FontEffectViewerExamplePresetEntryDesc & entry = m_presets[index];

            jpp::object j_effect;
            Helper::dumpFontEffectDesc( entry.desc, &j_effect );

            j_effect.set( "Sample", entry.sample );

            j_effects.set( m_presetNames[index].c_str(), j_effect );
        }

        root.set( "Effects", j_effects );

        if( Helper::writeJSONFile( root, fileGroup, Detail::makeFilePath( file ), false, MENGINE_DOCUMENT_FACTORABLE ) == false )
        {
            m_status = "Failed to write JSON";

            return false;
        }

        m_presetsFileGroup = fileGroup;
        m_presetsFileName = file;

        StdString::strcpy_safe( m_presetsPathInput, _fullPath, sizeof( m_presetsPathInput ) );

        m_status = "Saved '" + file + "'";

        return true;
    }
    //////////////////////////////////////////////////////////////////////////
    bool FontEffectViewerExampleSceneEventReceiver::loadGlyphFile_( const Char * _fullPath )
    {
        String folder;
        String file;

        if( Detail::splitFullPath( String( _fullPath ), &folder, &file ) == false )
        {
            m_status = "Invalid TTF path";

            return false;
        }

        FileGroupInterfacePtr fileGroup;
        if( Detail::mountFolder( STRINGIZE_STRING_LOCAL( "FontEffectViewerGlyphs" ), folder, &fileGroup ) == false )
        {
            m_status = "Failed to mount TTF folder";

            return false;
        }

        DataflowInterfacePtr dataflow = VOCABULARY_GET( STRINGIZE_STRING_LOCAL( "Dataflow" ), STRINGIZE_STRING_LOCAL( "ttfFont" ) );

        if( dataflow == nullptr )
        {
            m_status = "TTF dataflow is not registered";

            return false;
        }

        ++m_loadedGlyphCounter;

        Char glyphName[64] = {'\0'};
        MENGINE_SNPRINTF( glyphName, sizeof( glyphName ), "ViewerGlyph_%u", m_loadedGlyphCounter );

        ConstString glyphNameConst = Helper::stringizeString( glyphName );

        FontGlyphInterfacePtr glyph = FONT_SERVICE()
            ->createGlyph( glyphNameConst, STRINGIZE_STRING_LOCAL( "TTF" ), MENGINE_DOCUMENT_FACTORABLE );

        if( glyph == nullptr )
        {
            m_status = "Failed to create TTF glyph";

            return false;
        }

        ContentInterfacePtr content = Helper::makeFileContent( fileGroup, Detail::makeFilePath( file ), MENGINE_DOCUMENT_FACTORABLE );

        content->setDataflow( dataflow );

        TTFFontGlyphPtr ttfGlyph = TTFFontGlyphPtr::from( glyph );

        ttfGlyph->setGlyphContent( content );

        if( glyph->initialize() == false )
        {
            FONT_SERVICE()
                ->removeGlyph( glyphNameConst );

            m_status = "Failed to initialize TTF glyph";

            return false;
        }

        this->destroyPreview_();

        if( m_loadedGlyphName.empty() == false )
        {
            FONT_SERVICE()
                ->removeGlyph( m_loadedGlyphName );
        }

        m_loadedGlyphName = glyphNameConst;
        m_glyphFileGroup = fileGroup;

        StdString::strcpy_safe( m_glyphPathInput, _fullPath, sizeof( m_glyphPathInput ) );

        this->selectGlyph_( glyphNameConst );

        m_glyphIndex = -1;

        m_status = "Loaded '" + file + "'";

        return true;
    }
    //////////////////////////////////////////////////////////////////////////
    void FontEffectViewerExampleSceneEventReceiver::selectGlyph_( const ConstString & _glyphName )
    {
        FontGlyphInterfacePtr glyph;
        if( FONT_SERVICE()
            ->hasGlyph( _glyphName, &glyph ) == false )
        {
            m_status = "Glyph not found";

            return;
        }

        this->destroyPreview_();

        m_glyph = glyph;

        m_dirty = true;
    }
    //////////////////////////////////////////////////////////////////////////
    void FontEffectViewerExampleSceneEventReceiver::destroyPreview_()
    {
        if( m_textField != nullptr )
        {
            m_textField->dispose();
            m_textField = nullptr;
        }
    }
    //////////////////////////////////////////////////////////////////////////
    bool FontEffectViewerExampleSceneEventReceiver::createPreviewCanvas_()
    {
        FontEffectViewerExamplePreviewCanvasPtr canvas = PROTOTYPE_SERVICE()
            ->generatePrototype( STRINGIZE_STRING_LOCAL( "Node" ), STRINGIZE_STRING_LOCAL( "FontEffectViewerExamplePreviewCanvas" ), MENGINE_DOCUMENT_FACTORABLE );

        if( canvas == nullptr )
        {
            return false;
        }

        canvas->setName( STRINGIZE_STRING_LOCAL( "FontEffectViewerPreviewCanvas" ) );

        if( canvas->setSize( 64, 64 ) == false )
        {
            return false;
        }

        m_scene->addChild( canvas );

        m_previewCanvas = canvas;

        return true;
    }
    //////////////////////////////////////////////////////////////////////////
    void FontEffectViewerExampleSceneEventReceiver::destroyPreviewCanvas_()
    {
        if( m_previewCanvas != nullptr )
        {
            m_previewCanvas->dispose();
            m_previewCanvas = nullptr;
        }
    }
    //////////////////////////////////////////////////////////////////////////
    void FontEffectViewerExampleSceneEventReceiver::rebuildPreview_()
    {
        m_dirty = false;

        this->destroyPreview_();

        if( m_glyph == nullptr )
        {
            m_status = "No glyph selected";

            return;
        }

        if( m_font == nullptr )
        {
            FontInterfacePtr font = FONT_SERVICE()
                ->createFont( STRINGIZE_STRING_LOCAL( "FontEffectViewerFont" ), STRINGIZE_STRING_LOCAL( "TTF" ), MENGINE_DOCUMENT_FACTORABLE );

            if( font == nullptr )
            {
                m_status = "Failed to create TTF font";

                return;
            }

            if( font->initialize() == false )
            {
                m_status = "Failed to initialize TTF font";

                return;
            }

            m_font = font;
        }

        if( m_font->isCompileFont() == true )
        {
            m_status = "Font is still compiled";

            return;
        }

        FontEffectInterfacePtr effect;

        if( m_noEffect == false )
        {
            effect = FONTEFFECT_SERVICE()
                ->createFontEffect( m_desc, Detail::SAMPLE_VALUES[m_sampleIndex], MENGINE_DOCUMENT_FACTORABLE );

            if( effect == nullptr || effect->isValid() == false )
            {
                m_status = "Invalid effect description";

                return;
            }
        }

        TTFFontPtr ttfFont = TTFFontPtr::from( m_font );
        TTFFontGlyphPtr ttfGlyph = TTFFontGlyphPtr::from( m_glyph );

        ttfFont->setTTFFontGlyph( ttfGlyph );

        m_font->setHeight( (uint32_t)m_height );
        m_font->setFontColor( Color( m_fontColor[0], m_fontColor[1], m_fontColor[2], m_fontColor[3] ) );
        m_font->setEffect( effect );

        if( m_font->compileFont() == false )
        {
            m_status = "Failed to compile font with effect";

            return;
        }

        m_font->releaseFont();

        if( m_previewCanvas == nullptr )
        {
            m_status = "No preview canvas";

            return;
        }

        TextFieldPtr textField = PROTOTYPE_SERVICE()
            ->generatePrototype( STRINGIZE_STRING_LOCAL( "Node" ), STRINGIZE_STRING_LOCAL( "TextField" ), MENGINE_DOCUMENT_FACTORABLE );

        MENGINE_ASSERTION_MEMORY_PANIC( textField, "invalid create TextField" );

        textField->setName( STRINGIZE_STRING_LOCAL( "FontEffectViewerText" ) );
        textField->setFont( m_font );
        textField->setText( String( m_textInput ) );
        textField->setHorizontAlign( ETFHA_CENTER );
        textField->setVerticalAlign( ETFVA_CENTER );
        textField->setPixelsnap( true );

        const mt::vec2f & canvasSize = m_previewCanvas->getSize();

        float centerX = canvasSize.x * 0.5f + m_previewPan.x;
        float centerY = canvasSize.y * 0.5f + m_previewPan.y;

        textField->getTransformation()->setLocalPosition( {centerX, centerY, 0.f} );

        m_previewCanvas->addChild( textField );

        m_textField = textField;

        if( m_status.empty() == true )
        {
            m_status = "OK";
        }
    }
    //////////////////////////////////////////////////////////////////////////
    void FontEffectViewerExampleSceneEventReceiver::renderGlyphPreview_( const ImGUIRenderProviderInterfacePtr & _provider )
    {
        if( m_font == nullptr || m_font->isCompileFont() == false )
        {
            ImGui::TextDisabled( "font is not compiled" );

            return;
        }

        U32String codes;
        size_t textLength = StdString::strlen( m_textInput );

        if( m_font->prepareText( m_textInput, textLength, &codes ) == false )
        {
            ImGui::TextDisabled( "no text" );

            return;
        }

        if( codes.empty() == true )
        {
            ImGui::TextDisabled( "no text" );

            return;
        }

        GlyphCode code = 0;

        for( Char32 c : codes )
        {
            if( c == 10 || c == 13 || c == 32 )
            {
                continue;
            }

            code = (GlyphCode)c;

            break;
        }

        if( code == 0 )
        {
            ImGui::TextDisabled( "no printable glyph" );

            return;
        }

        uint32_t layoutCount = m_font->getLayoutCount();

        for( uint32_t layout = 0; layout != layoutCount; ++layout )
        {
            Glyph glyph;
            if( m_font->getGlyph( layout, code, 0, &glyph ) == false )
            {
                continue;
            }

            ImGui::Text( "layout %u: offset (%.1f, %.1f) size (%.1f, %.1f)", layout, glyph.offset.x, glyph.offset.y, glyph.size.x, glyph.size.y );

            if( glyph.texture == nullptr )
            {
                continue;
            }

            ImTextureID textureId = _provider->getImTexture( glyph.texture );

            ImVec2 size( glyph.size.x * m_previewScale, glyph.size.y * m_previewScale );
            ImVec2 uv0( glyph.uv.p0.x, glyph.uv.p0.y );
            ImVec2 uv1( glyph.uv.p2.x, glyph.uv.p2.y );

            ImGui::Image( textureId, size, uv0, uv1 );
        }
    }
    //////////////////////////////////////////////////////////////////////////
    void FontEffectViewerExampleSceneEventReceiver::dumpGlyphs_()
    {
        if( m_font == nullptr || m_font->isCompileFont() == false )
        {
            m_status = "Font is not compiled";

            return;
        }

        const FileGroupInterfacePtr & userFileGroup = FILE_SERVICE()
            ->getFileGroup( STRINGIZE_STRING_LOCAL( "user" ) );

        if( userFileGroup == nullptr )
        {
            m_status = "No user file group";

            return;
        }

        U32String codes;
        size_t textLength = StdString::strlen( m_textInput );

        if( m_font->prepareText( m_textInput, textLength, &codes ) == false )
        {
            ImGui::TextDisabled( "no text" );

            return;
        }

        if( codes.empty() == true )
        {
            return;
        }

        GlyphCode code = 0;

        for( Char32 c : codes )
        {
            if( c == 10 || c == 13 || c == 32 )
            {
                continue;
            }

            code = (GlyphCode)c;

            break;
        }

        uint32_t layoutCount = m_font->getLayoutCount();

        uint32_t dumped = 0;

        for( uint32_t layout = 0; layout != layoutCount; ++layout )
        {
            Glyph glyph;
            if( m_font->getGlyph( layout, code, 0, &glyph ) == false )
            {
                continue;
            }

            if( glyph.texture == nullptr )
            {
                continue;
            }

            Char fileName[64] = {'\0'};
            MENGINE_SNPRINTF( fileName, sizeof( fileName ), "font_effect_atlas_L%u.png", layout );

            ContentInterfacePtr content = Helper::makeFileContent( userFileGroup, Helper::stringizeFilePath( fileName ), MENGINE_DOCUMENT_FACTORABLE );

            content->setCodecType( STRINGIZE_STRING_LOCAL( "pngImage" ) );

            if( RENDERTEXTURE_SERVICE()
                ->saveImage( glyph.texture, content ) == false )
            {
                continue;
            }

            ++dumped;
        }

        Char status[128] = {'\0'};
        MENGINE_SNPRINTF( status, sizeof( status ), "Dumped %u atlas(es) to user folder (font_effect_atlas_L*.png)", dumped );

        m_status = status;
    }
    //////////////////////////////////////////////////////////////////////////
    void FontEffectViewerExampleSceneEventReceiver::clampPreviewPan_()
    {
        if( m_previewCanvas == nullptr )
        {
            m_previewPan = mt::vec2f( 0.f, 0.f );

            return;
        }

        const mt::vec2f & canvasSize = m_previewCanvas->getSize();

        float maxPanX = canvasSize.x * 0.5f;
        float maxPanY = canvasSize.y * 0.5f;

        m_previewPan.x = mt::clamp( -maxPanX, m_previewPan.x, maxPanX );
        m_previewPan.y = mt::clamp( -maxPanY, m_previewPan.y, maxPanY );
    }
    //////////////////////////////////////////////////////////////////////////
    void FontEffectViewerExampleSceneEventReceiver::updatePreviewPosition_()
    {
        if( m_textField == nullptr || m_previewCanvas == nullptr )
        {
            return;
        }

        const mt::vec2f & canvasSize = m_previewCanvas->getSize();

        float centerX = canvasSize.x * 0.5f + m_previewPan.x;
        float centerY = canvasSize.y * 0.5f + m_previewPan.y;

        m_textField->getTransformation()->setLocalPosition( {centerX, centerY, 0.f} );
    }
    //////////////////////////////////////////////////////////////////////////
    void FontEffectViewerExampleSceneEventReceiver::zoomPreviewAt_( float _newZoom, float _cursorOffsetX, float _cursorOffsetY )
    {
        if( m_previewCanvas == nullptr )
        {
            return;
        }

        float oldZoom = m_previewZoom;
        float newZoom = mt::clamp( 0.25f, _newZoom, 8.f );

        if( newZoom == oldZoom )
        {
            return;
        }

        m_previewPan.x += _cursorOffsetX * (1.f / newZoom - 1.f / oldZoom);
        m_previewPan.y += _cursorOffsetY * (1.f / newZoom - 1.f / oldZoom);

        m_previewZoom = newZoom;

        this->clampPreviewPan_();

        m_previewCanvas->setZoom( m_previewZoom );

        this->updatePreviewPosition_();
    }
    //////////////////////////////////////////////////////////////////////////
    void FontEffectViewerExampleSceneEventReceiver::renderPreviewCanvas_( const ImGUIRenderProviderInterfacePtr & _provider )
    {
        if( m_previewCanvas == nullptr )
        {
            ImGui::TextDisabled( "preview canvas is not available" );

            return;
        }

        ImVec2 contentAvail = ImGui::GetContentRegionAvail();

        float bottomRowHeight = ImGui::GetFrameHeightWithSpacing();

        ImVec2 avail( contentAvail.x, contentAvail.y - bottomRowHeight );

        if( avail.x >= 16.f && avail.y >= 16.f )
        {
            uint32_t roundedWidth = ((uint32_t)avail.x / 16u) * 16u;
            uint32_t roundedHeight = ((uint32_t)avail.y / 16u) * 16u;

            roundedWidth = roundedWidth < 64u ? 64u : roundedWidth;
            roundedHeight = roundedHeight < 64u ? 64u : roundedHeight;

            const mt::vec2f & currentSize = m_previewCanvas->getSize();

            if( (uint32_t)currentSize.x != roundedWidth || (uint32_t)currentSize.y != roundedHeight )
            {
                if( m_previewCanvas->setSize( roundedWidth, roundedHeight ) == true )
                {
                    m_dirty = true;
                }
            }
        }

        const RenderTextureInterfacePtr & texture = m_previewCanvas->getTexture();
        const RenderTargetInterfacePtr & target = m_previewCanvas->getTarget();

        if( texture == nullptr || target == nullptr )
        {
            ImGui::TextDisabled( "preview canvas is not ready" );

            return;
        }

        ImTextureID textureId = _provider->getImTexture( texture );

        const mt::uv4f & uv = target->getUV();

        ImVec2 uv0( uv.p0.x, uv.p0.y );
        ImVec2 uv1( uv.p2.x, uv.p2.y );

        ImGui::Image( textureId, avail, uv0, uv1 );

        bool hovered = ImGui::IsItemHovered();

        if( hovered == true )
        {
            float wheel = ImGui::GetIO().MouseWheel;

            if( wheel != 0.f )
            {
                ImVec2 imageMin = ImGui::GetItemRectMin();
                ImVec2 mousePos = ImGui::GetIO().MousePos;

                float cursorOffsetX = (mousePos.x - imageMin.x) - avail.x * 0.5f;
                float cursorOffsetY = (mousePos.y - imageMin.y) - avail.y * 0.5f;

                this->zoomPreviewAt_( m_previewZoom * (1.f + wheel * 0.1f), cursorOffsetX, cursorOffsetY );
            }

            if( ImGui::IsMouseClicked( ImGuiMouseButton_Left ) == true )
            {
                m_previewPanning = true;
            }
        }

        if( m_previewPanning == true )
        {
            if( ImGui::IsMouseDown( ImGuiMouseButton_Left ) == false )
            {
                m_previewPanning = false;
            }
            else
            {
                ImVec2 delta = ImGui::GetIO().MouseDelta;

                if( delta.x != 0.f || delta.y != 0.f )
                {
                    m_previewPan.x += delta.x / m_previewZoom;
                    m_previewPan.y += delta.y / m_previewZoom;

                    this->clampPreviewPan_();

                    this->updatePreviewPosition_();
                }
            }
        }

        ImGui::TextDisabled( "zoom %.0f%% (wheel to zoom, drag with left button to pan)", m_previewZoom * 100.f );

        ImGui::SameLine();

        if( ImGui::SmallButton( "Reset view" ) == true )
        {
            m_previewZoom = 1.f;
            m_previewPan = mt::vec2f( 0.f, 0.f );
            m_previewPanning = false;

            m_previewCanvas->setZoom( m_previewZoom );

            this->updatePreviewPosition_();
        }
    }
    //////////////////////////////////////////////////////////////////////////
    void FontEffectViewerExampleSceneEventReceiver::renderControls_( const ImGUIRenderProviderInterfacePtr & _provider )
    {
        if( m_dirty == true )
        {
            this->rebuildPreview_();
        }

        const ImGuiIO & io = ImGui::GetIO();

        ImGui::SetNextWindowPos( ImVec2( 0.f, 0.f ) );
        ImGui::SetNextWindowSize( io.DisplaySize );

        ImGuiWindowFlags windowFlags = ImGuiWindowFlags_NoMove | ImGuiWindowFlags_NoResize | ImGuiWindowFlags_NoCollapse | ImGuiWindowFlags_NoBringToFrontOnFocus | ImGuiWindowFlags_NoSavedSettings;

        if( ImGui::Begin( "Font Effect Viewer", nullptr, windowFlags ) == false )
        {
            ImGui::End();

            return;
        }

        float minSidebarWidth = 320.f;
        float maxSidebarWidth = io.DisplaySize.x - 200.f;

        if( maxSidebarWidth < minSidebarWidth )
        {
            maxSidebarWidth = minSidebarWidth;
        }

        if( m_sidebarWidth < minSidebarWidth )
        {
            m_sidebarWidth = minSidebarWidth;
        }
        else if( m_sidebarWidth > maxSidebarWidth )
        {
            m_sidebarWidth = maxSidebarWidth;
        }

        ImGui::BeginChild( "FontEffectViewerSidebar", ImVec2( m_sidebarWidth, 0.f ), false );

        bool changed = false;

        if( ImGui::CollapsingHeader( "Presets", ImGuiTreeNodeFlags_DefaultOpen ) == true )
        {
            if( ImGui::Button( "Load JSON..." ) == true )
            {
                Char path[MENGINE_MAX_PATH] = {'\0'};

                if( FontEffectViewerExampleSelectFilePath( "Open effects JSON", m_presetsPathInput, path, sizeof( path ) ) == true )
                {
                    this->loadPresetsFile_( path );
                }
            }

            ImGui::SameLine();

            ImGui::BeginDisabled( m_presetsFileGroup == nullptr );

            if( ImGui::Button( "Save" ) == true )
            {
                this->savePresetsFile_( m_presetsPathInput );
            }

            ImGui::EndDisabled();

            ImGui::SameLine();

            if( ImGui::Button( "Save As..." ) == true )
            {
                Char path[MENGINE_MAX_PATH] = {'\0'};

                const Char * defaultName = m_presetsFileName.empty() == true ? "Effects.json" : m_presetsFileName.c_str();

                if( FontEffectViewerExampleSelectSaveFilePath( "Save effects JSON", m_presetsPathInput, defaultName, path, sizeof( path ) ) == true )
                {
                    this->savePresetsFile_( path );
                }
            }

            ImGui::SameLine();

            if( ImGui::Button( "Reload bundled" ) == true )
            {
                this->loadBundledEffects_();
            }

            ImGui::TextDisabled( "%s", m_presetsFileGroup == nullptr ? "bundled Effects.json (read only)" : m_presetsPathInput );

            if( m_presetNames.empty() == false )
            {
                const Char * currentName = m_presetIndex >= 0 ? m_presetNames[m_presetIndex].c_str() : "<none>";

                if( ImGui::BeginCombo( "Preset", currentName ) == true )
                {
                    for( VectorString::size_type index = 0; index != m_presetNames.size(); ++index )
                    {
                        bool selected = (m_presetIndex == (int)index);

                        if( ImGui::Selectable( m_presetNames[index].c_str(), selected ) == true )
                        {
                            this->selectPreset_( (int)index );
                        }
                    }

                    ImGui::EndCombo();
                }
            }

            ImGui::InputText( "Name", m_presetNameInput, sizeof( m_presetNameInput ) );

            if( ImGui::Button( "Store as preset" ) == true )
            {
                this->storeCurrentPreset_();
            }
        }

        if( ImGui::CollapsingHeader( "Font", ImGuiTreeNodeFlags_DefaultOpen ) == true )
        {
            if( m_glyphNames.empty() == false )
            {
                const Char * currentName = m_glyphIndex >= 0 ? m_glyphNames[m_glyphIndex].c_str() : (m_loadedGlyphName.empty() == false ? m_loadedGlyphName.c_str() : "<none>");

                if( ImGui::BeginCombo( "Glyph", currentName ) == true )
                {
                    for( VectorString::size_type index = 0; index != m_glyphNames.size(); ++index )
                    {
                        bool selected = (m_glyphIndex == (int)index);

                        if( ImGui::Selectable( m_glyphNames[index].c_str(), selected ) == true )
                        {
                            m_glyphIndex = (int)index;

                            this->selectGlyph_( Helper::stringizeString( m_glyphNames[index].c_str() ) );
                        }
                    }

                    ImGui::EndCombo();
                }
            }

            if( ImGui::Button( "Load TTF..." ) == true )
            {
                Char path[MENGINE_MAX_PATH] = {'\0'};

                if( FontEffectViewerExampleSelectFilePath( "Open TTF font", m_glyphPathInput, path, sizeof( path ) ) == true )
                {
                    this->loadGlyphFile_( path );
                }
            }

            if( m_glyphPathInput[0] != '\0' )
            {
                ImGui::SameLine();
                ImGui::TextDisabled( "%s", m_glyphPathInput );
            }

            changed |= ImGui::SliderInt( "Height", &m_height, 8, 200 );
            changed |= ImGui::Combo( "Sample", &m_sampleIndex, Detail::SAMPLE_NAMES, 3 );
            changed |= ImGui::ColorEdit4( "Font color", m_fontColor, ImGuiColorEditFlags_AlphaBar );
            changed |= ImGui::Checkbox( "No effect", &m_noEffect );

            if( ImGui::InputTextMultiline( "Text", m_textInput, sizeof( m_textInput ), ImVec2( -1.f, 60.f ) ) == true )
            {
                changed = true;
            }
        }

        if( ImGui::CollapsingHeader( "Layers", ImGuiTreeNodeFlags_DefaultOpen ) == true )
        {
            changed |= FontEffectViewerExampleRenderDescEditor( &m_desc );
        }

        if( ImGui::CollapsingHeader( "Glyph preview" ) == true )
        {
            ImGui::Checkbox( "Show", &m_showGlyphPreview );
            ImGui::SameLine();
            ImGui::SetNextItemWidth( 100.f );
            ImGui::SliderFloat( "Scale", &m_previewScale, 0.5f, 8.f, "%.1f" );
            ImGui::SameLine();

            if( ImGui::Button( "Dump PNG" ) == true )
            {
                this->dumpGlyphs_();
            }

            if( m_showGlyphPreview == true )
            {
                this->renderGlyphPreview_( _provider );
            }
        }

        ImGui::Separator();
        ImGui::TextWrapped( "%s", m_status.c_str() );

        ImGui::EndChild();

        ImGui::SameLine();

        ImGui::PushStyleVar( ImGuiStyleVar_ItemSpacing, ImVec2( 0.f, 0.f ) );

        ImGui::Button( "##FontEffectViewerSplitter", ImVec2( 6.f, -1.f ) );

        if( ImGui::IsItemHovered() == true || ImGui::IsItemActive() == true )
        {
            ImGui::SetMouseCursor( ImGuiMouseCursor_ResizeEW );
        }

        if( ImGui::IsItemActive() == true )
        {
            m_sidebarWidth += io.MouseDelta.x;
        }

        ImGui::PopStyleVar();

        ImGui::SameLine();

        ImGui::BeginChild( "FontEffectViewerPreview", ImVec2( 0.f, 0.f ), false, ImGuiWindowFlags_NoScrollbar | ImGuiWindowFlags_NoScrollWithMouse );

        ImGui::Text( "Preview" );
        ImGui::Separator();

        this->renderPreviewCanvas_( _provider );

        ImGui::EndChild();

        ImGui::End();

        if( changed == true )
        {
            m_dirty = true;
        }
    }
    //////////////////////////////////////////////////////////////////////////
}
