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

#include "Kernel/TimestampHelper.h"

#include "Config/StdIO.h"
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



#include "Mosaic/Mosaic.hpp"

namespace Mengine
{
    namespace Detail
    {
        //////////////////////////////////////////////////////////////////////////
        constexpr uint32_t SAMPLE_COUNT = 3;
        constexpr float PREVIEW_ZOOM_WHEEL_SPEED = 0.002f;
        static const uint32_t SAMPLE_VALUES[SAMPLE_COUNT] = {1, 2, 4};
        static const Char * SAMPLE_NAMES[SAMPLE_COUNT] = {"1", "2", "4"};
        //////////////////////////////////////////////////////////////////////////
        static Mosaic::Rect makeMosaicUV( const RenderTextureInterfacePtr & _texture, const mt::uv4f & _uv )
        {
            const mt::uv4f & textureUV = _texture->getUV();

            float textureWidth = textureUV.p2.x - textureUV.p0.x;
            float textureHeight = textureUV.p2.y - textureUV.p0.y;

            Mosaic::Rect uv;
            uv.x = (_uv.p0.x - textureUV.p0.x) / textureWidth;
            uv.y = (_uv.p0.y - textureUV.p0.y) / textureHeight;
            uv.width = (_uv.p2.x - _uv.p0.x) / textureWidth;
            uv.height = (_uv.p2.y - _uv.p0.y) / textureHeight;

            return uv;
        }
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
        , m_sidebarRatio( 0.43f )
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

        MosaicRenderPtr mosaicRender = PROTOTYPE_SERVICE()
            ->generatePrototype( STRINGIZE_STRING_LOCAL( "Node" ), STRINGIZE_STRING_LOCAL( "MosaicRender" ), MENGINE_DOCUMENT_FACTORABLE );

        MENGINE_ASSERTION_MEMORY_PANIC( mosaicRender, "invalid create MosaicRender" );

        mosaicRender->setName( STRINGIZE_STRING_LOCAL( "FontEffectViewerExampleMosaic" ) );
        mosaicRender->setProvider( [this]( Mosaic::Context * _ui )
        {
            this->renderControls_( _ui );
        } );

                m_scene->addChild( mosaicRender );
        m_mosaicRender = mosaicRender;


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

        if( m_mosaicRender != nullptr )
        {
            m_mosaicRender->dispose();
            m_mosaicRender = nullptr;
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

        m_font->setGlyph( m_glyph );

        m_font->setHeight( (uint32_t)m_height );
        m_font->setFontColor( Color( m_fontColor[0], m_fontColor[1], m_fontColor[2], m_fontColor[3] ) );
        m_font->setEffect( effect );

        if( m_font->compileFont() == false )
        {
            m_status = "Failed to compile font with effect";

            return;
        }

        this->measureGlyphTime_();

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
    void FontEffectViewerExampleSceneEventReceiver::measureGlyphTime_()
    {
        U32String codes;
        size_t textLength = StdString::strlen( m_textInput );

        if( m_font->prepareText( m_textInput, textLength, &codes ) == false )
        {
            return;
        }

        if( codes.empty() == true )
        {
            return;
        }

        double timeBegin = Helper::getElapsedTime();

        if( m_font->prepareGlyph( codes, MENGINE_DOCUMENT_FACTORABLE ) == false )
        {
            return;
        }

        double timeEnd = Helper::getElapsedTime();

        double microseconds = (timeEnd - timeBegin) * 1000.0 / (double)codes.size();

        Char buffer[64] = {'\0'};
        MENGINE_SNPRINTF( buffer, 64, "%.0f us/glyph", microseconds );

        m_glyphTime = buffer;
    }
    //////////////////////////////////////////////////////////////////////////
    void FontEffectViewerExampleSceneEventReceiver::renderGlyphPreview_( Mosaic::Context * _ui )
    {
        if( m_font == nullptr || m_font->isCompileFont() == false )
        {
            this->textLine_( _ui, true, "font is not compiled" );

            return;
        }

        U32String codes;
        size_t textLength = StdString::strlen( m_textInput );

        if( m_font->prepareText( m_textInput, textLength, &codes ) == false )
        {
            this->textLine_( _ui, true, "no text" );

            return;
        }

        if( codes.empty() == true )
        {
            this->textLine_( _ui, true, "no text" );

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
            this->textLine_( _ui, true, "no printable glyph" );

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

            this->textLine_( _ui, false, "layout %u: offset (%.1f, %.1f) size (%.1f, %.1f)", layout, glyph.offset.x, glyph.offset.y, glyph.size.x, glyph.size.y );

            if( glyph.texture == nullptr )
            {
                continue;
            }

            MosaicTextureHandle textureId = MOSAIC_SERVICE()
                ->getTextureHandle( glyph.texture );

            Mosaic::Vec2 size = {glyph.size.x * m_previewScale, glyph.size.y * m_previewScale};
            Mosaic::Rect uv = Detail::makeMosaicUV( glyph.texture, glyph.uv );

            Mosaic::image( _ui, textureId, size, uv );
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
            m_status = "No text";

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
    void FontEffectViewerExampleSceneEventReceiver::renderPreviewCanvas_( Mosaic::Context * _ui )
    {
        if( m_previewCanvas == nullptr )
        {
            this->textLine_( _ui, true, "preview canvas is not available" );

            return;
        }

        Mosaic::Rect previewBounds;
        Mosaic::Vec2 localPointer;
        Mosaic::Response previewResponse;

        bool hasBounds = false;
        bool hasLocalPointer = false;
        bool hasResponse = false;

        {
            Mosaic::LayoutOptions previewLayout;
            previewLayout.width = Mosaic::SizeRule::Fill;
            previewLayout.height = Mosaic::SizeRule::Fill;

            Mosaic::Canvas previewCanvas = Mosaic::canvas( _ui, "FontEffectViewerPreviewCanvas", previewLayout );

            hasBounds = previewCanvas.contentRect( &previewBounds );

            if( hasBounds == true )
            {
                uint32_t roundedWidth = ((uint32_t)previewBounds.width / 16u) * 16u;
                uint32_t roundedHeight = ((uint32_t)previewBounds.height / 16u) * 16u;

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

                const RenderTextureInterfacePtr & texture = m_previewCanvas->getTexture();

                if( texture != nullptr )
                {
                    MosaicTextureHandle textureId = MOSAIC_SERVICE()
                        ->getTextureHandle( texture );

                    previewCanvas.image( textureId, previewBounds );
                }
            }

            hasLocalPointer = previewCanvas.localPointerPosition( &localPointer );

            hasResponse = Mosaic::itemResponse( _ui, previewCanvas.id(), &previewResponse );
        }

        if( hasBounds == true && hasResponse == true && hasLocalPointer == true )
        {
            Mosaic::Vec2 wheel;

            if( Mosaic::consumeWheel( _ui, previewResponse.id, &wheel ) == true )
            {
                if( wheel.y != 0.f )
                {
                    float cursorOffsetX = localPointer.x - previewBounds.width * 0.5f;
                    float cursorOffsetY = localPointer.y - previewBounds.height * 0.5f;

                    float zoomScale = 1.f + wheel.y * Detail::PREVIEW_ZOOM_WHEEL_SPEED;

                    this->zoomPreviewAt_( m_previewZoom * zoomScale, cursorOffsetX, cursorOffsetY );
                }
            }
        }

        if( hasResponse == true && hasLocalPointer == true && previewResponse.active() == true )
        {
            if( m_previewPanning == true )
            {
                float deltaX = localPointer.x - m_previewPanAnchor.x;
                float deltaY = localPointer.y - m_previewPanAnchor.y;

                if( deltaX != 0.f || deltaY != 0.f )
                {
                    m_previewPan.x += deltaX / m_previewZoom;
                    m_previewPan.y += deltaY / m_previewZoom;

                    this->clampPreviewPan_();
                    this->updatePreviewPosition_();
                }
            }

            m_previewPanAnchor = mt::vec2f( localPointer.x, localPointer.y );
            m_previewPanning = true;
        }
        else
        {
            m_previewPanning = false;
        }

        Mosaic::Scope footer = Mosaic::row( _ui, Mosaic::Key( "FontEffectViewerPreviewFooter" ) );

        this->textLine_( _ui, true, "zoom %.0f%% (wheel to zoom, drag with left button to pan)", m_previewZoom * 100.f );

        if( Mosaic::smallButton( _ui, "Reset view" ).clicked() == true )
        {
            m_previewZoom = 1.f;
            m_previewPan = mt::vec2f( 0.f, 0.f );
            m_previewPanning = false;

            m_previewCanvas->setZoom( m_previewZoom );

            this->updatePreviewPosition_();
        }
    }
    //////////////////////////////////////////////////////////////////////////
    void FontEffectViewerExampleSceneEventReceiver::textLine_( Mosaic::Context * _ui, bool _disabled, const Char * _format, ... )
    {
        Char buffer[1024] = {'\0'};

        MENGINE_VA_LIST_TYPE args;
        MENGINE_VA_LIST_START( args, _format );
        MENGINE_VSNPRINTF( buffer, sizeof( buffer ) - 1, _format, args );
        MENGINE_VA_LIST_END( args );

        m_readout.emplace_back( buffer );

        const String & line = m_readout.back();

        Mosaic::Scope lineScope = Mosaic::scope( _ui, Mosaic::Key( m_readout.size() ) );

        Mosaic::Scope disabled = Mosaic::disabledScope( _ui, _disabled );

        Mosaic::text( _ui, Mosaic::StringView( line.c_str(), line.size() ) );
    }
    //////////////////////////////////////////////////////////////////////////
    void FontEffectViewerExampleSceneEventReceiver::renderControls_( Mosaic::Context * _ui )
    {
        if( m_dirty == true )
        {
            this->rebuildPreview_();
        }

        const Resolution & contentResolution = APPLICATION_SERVICE()
            ->getContentResolution();

        Mosaic::WindowOptions windowOptions;
        windowOptions.initialBounds = {0.f, 0.f, (float)contentResolution.getWidth(), (float)contentResolution.getHeight()};
        windowOptions.movable = false;
        windowOptions.resizable = false;
        windowOptions.saveSettings = false;
        windowOptions.bringToFront = false;

        Mosaic::WindowScope window = Mosaic::window( _ui, "Font Effect Viewer", windowOptions );

        if( window.visible() == false )
        {
            return;
        }

        m_readout.clear();

        Mosaic::SplitOptions splitOptions;
        splitOptions.minimumFirst = 320.f;
        splitOptions.minimumSecond = 200.f;

        Mosaic::Scope workspace = Mosaic::split( _ui, "FontEffectViewerWorkspace", Mosaic::Orientation::Horizontal, &m_sidebarRatio, splitOptions );

        bool changed = false;

        {
            Mosaic::Scope sidebar = Mosaic::scrollArea( _ui, "FontEffectViewerSidebar" );

            if( Mosaic::TreeScope headerScope = Mosaic::collapsingHeader( _ui, "Presets", true ); headerScope.expanded() == true )
            {
                if( Mosaic::button( _ui, "Load JSON..." ).clicked() == true )
                {
                    Char path[MENGINE_MAX_PATH] = {'\0'};

                    if( FontEffectViewerExampleSelectFilePath( "Open effects JSON", m_presetsPathInput, path, sizeof( path ) ) == true )
                    {
                        this->loadPresetsFile_( path );
                    }
                }

                Mosaic::sameLine( _ui );

                {
                    Mosaic::Scope disabledScope = Mosaic::disabledScope( _ui, m_presetsFileGroup == nullptr );

                    if( Mosaic::button( _ui, "Save" ).clicked() == true )
                    {
                        this->savePresetsFile_( m_presetsPathInput );
                    }

                }

                Mosaic::sameLine( _ui );

                if( Mosaic::button( _ui, "Save As..." ).clicked() == true )
                {
                    Char path[MENGINE_MAX_PATH] = {'\0'};

                    const Char * defaultName = m_presetsFileName.empty() == true ? "Effects.json" : m_presetsFileName.c_str();

                    if( FontEffectViewerExampleSelectSaveFilePath( "Save effects JSON", m_presetsPathInput, defaultName, path, sizeof( path ) ) == true )
                    {
                        this->savePresetsFile_( path );
                    }
                }

                Mosaic::sameLine( _ui );

                if( Mosaic::button( _ui, "Reload bundled" ).clicked() == true )
                {
                    this->loadBundledEffects_();
                }

                this->textLine_( _ui, true, "%s", m_presetsFileGroup == nullptr ? "bundled Effects.json (read only)" : m_presetsPathInput );

                if( m_presetNames.empty() == false )
                {
                    const Char * currentName = m_presetIndex >= 0 ? m_presetNames[m_presetIndex].c_str() : "<none>";

                    if( Mosaic::TreeScope comboScope = Mosaic::beginCombo( _ui, "Preset", currentName ); comboScope.visible() == true )
                    {
                        for( VectorString::size_type index = 0; index != m_presetNames.size(); ++index )
                        {
                            bool selected = (m_presetIndex == (int)index);

                            if( Mosaic::selectable( _ui, Mosaic::Key( (uint32_t)index ), m_presetNames[index].c_str(), selected ).clicked() == true )
                            {
                                this->selectPreset_( (int)index );
                            }
                        }

                    }
                }

                {
                    Mosaic::String presetName( m_presetNameInput );

                    if( Mosaic::property( _ui, "Name", &presetName ).changed() == true )
                    {
                        StdString::strcpy_safe( m_presetNameInput, presetName.c_str(), sizeof( m_presetNameInput ) );
                    }
                }

                if( Mosaic::button( _ui, "Store as preset" ).clicked() == true )
                {
                    this->storeCurrentPreset_();
                }
            }

            if( Mosaic::TreeScope headerScope = Mosaic::collapsingHeader( _ui, "Font", true ); headerScope.expanded() == true )
            {
                if( m_glyphNames.empty() == false )
                {
                    const Char * currentName = m_glyphIndex >= 0 ? m_glyphNames[m_glyphIndex].c_str() : (m_loadedGlyphName.empty() == false ? m_loadedGlyphName.c_str() : "<none>");

                    if( Mosaic::TreeScope comboScope = Mosaic::beginCombo( _ui, "Glyph", currentName ); comboScope.visible() == true )
                    {
                        for( VectorString::size_type index = 0; index != m_glyphNames.size(); ++index )
                        {
                            bool selected = (m_glyphIndex == (int)index);

                            if( Mosaic::selectable( _ui, Mosaic::Key( (uint32_t)index ), m_glyphNames[index].c_str(), selected ).clicked() == true )
                            {
                                m_glyphIndex = (int)index;

                                this->selectGlyph_( Helper::stringizeString( m_glyphNames[index].c_str() ) );
                            }
                        }

                    }
                }

                if( Mosaic::button( _ui, "Load TTF..." ).clicked() == true )
                {
                    Char path[MENGINE_MAX_PATH] = {'\0'};

                    if( FontEffectViewerExampleSelectFilePath( "Open TTF font", m_glyphPathInput, path, sizeof( path ) ) == true )
                    {
                        this->loadGlyphFile_( path );
                    }
                }

                if( m_glyphPathInput[0] != '\0' )
                {
                    Mosaic::sameLine( _ui );
                    this->textLine_( _ui, true, "%s", m_glyphPathInput );
                }

                changed |= [&]() { Mosaic::SliderOptions o; o.precision = 0; int32_t height = (int32_t)m_height; bool c = Mosaic::slider( _ui, "Height", &height, 8, 200, o ).changed(); m_height = (int)height; return c; }();
                {
                    Mosaic::StringView sampleItems[3];

                    for( uint32_t sampleIndex = 0; sampleIndex != 3; ++sampleIndex )
                    {
                        sampleItems[sampleIndex] = Detail::SAMPLE_NAMES[sampleIndex];
                    }

                    changed |= Mosaic::comboBox( _ui, "Sample", &m_sampleIndex, Mosaic::StringViewSpan( sampleItems, 3 ) ).changed();
                }
                {
                    Mosaic::Color fontColor = {m_fontColor[0], m_fontColor[1], m_fontColor[2], m_fontColor[3]};

                    if( Mosaic::colorEditorRgba( _ui, "Font color", &fontColor ).changed() == true )
                    {
                        m_fontColor[0] = fontColor.r;
                        m_fontColor[1] = fontColor.g;
                        m_fontColor[2] = fontColor.b;
                        m_fontColor[3] = fontColor.a;

                        changed = true;
                    }
                }
                changed |= Mosaic::checkbox( _ui, "No effect", &m_noEffect ).changed();

                {
                    Mosaic::String text( m_textInput );

                    Mosaic::LayoutOptions textLayout;
                    textLayout.height = Mosaic::Dimension::fixed( 60.f );

                    if( Mosaic::inputMultiline( _ui, "Text", &text, {}, textLayout ).changed() == true )
                    {
                        StdString::strcpy_safe( m_textInput, text.c_str(), sizeof( m_textInput ) );

                        changed = true;
                    }
                }
            }

            if( Mosaic::TreeScope headerScope = Mosaic::collapsingHeader( _ui, "Layers", true ); headerScope.expanded() == true )
            {
                changed |= FontEffectViewerExampleRenderDescEditor( _ui, &m_desc );
            }

            if( Mosaic::TreeScope headerScope = Mosaic::collapsingHeader( _ui, "Glyph preview" ); headerScope.expanded() == true )
            {
                Mosaic::checkbox( _ui, "Show", &m_showGlyphPreview );
                Mosaic::sameLine( _ui );
                Mosaic::setNextItemWidth( _ui, 100.f );
                MENGINE_UNUSED( Mosaic::slider( _ui, "Scale", &m_previewScale, [&]() { Mosaic::SliderOptions o; o.minimum = 0.5; o.maximum = 8.0; o.precision = 1; return o; }() ) );
                Mosaic::sameLine( _ui );

                if( Mosaic::button( _ui, "Dump PNG" ).clicked() == true )
                {
                    this->dumpGlyphs_();
                }

                if( m_showGlyphPreview == true )
                {
                    this->renderGlyphPreview_( _ui );
                }
            }

            Mosaic::separator( _ui );
            this->textLine_( _ui, false, "%s", m_status.c_str() );

            if( m_glyphTime.empty() == false )
            {
                Mosaic::sameLine( _ui );
                this->textLine_( _ui, true, "%s", m_glyphTime.c_str() );
            }

        }

        {
            Mosaic::LayoutOptions previewLayout;
            previewLayout.width = Mosaic::SizeRule::Fill;
            previewLayout.height = Mosaic::SizeRule::Fill;
            previewLayout.orientation = Mosaic::Orientation::Vertical;

            Mosaic::Scope preview = Mosaic::column( _ui, Mosaic::Key( "FontEffectViewerPreview" ), previewLayout );

            this->textLine_( _ui, false, "Preview" );
            Mosaic::separator( _ui );

            this->renderPreviewCanvas_( _ui );
        }

        if( changed == true )
        {
            m_dirty = true;
        }
    }
    //////////////////////////////////////////////////////////////////////////
}
