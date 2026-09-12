#pragma once

#include "Interface/FontInterface.h"
#include "Interface/FontGlyphInterface.h"
#include "Interface/FileGroupInterface.h"
#include "Interface/ContentInterface.h"

#include "Engine/TextField.h"

#include "FontEffectViewerExamplePreviewCanvas.h"

#include "Frameworks/ImGUIFramework/ImGUIRender.h"

#include "Plugins/FontEffectPlugin/FontEffectDesc.h"
#include "Plugins/MCPPlugin/MCPHandlerInterface.h"

#include "Kernel/DummySceneEventReceiver.h"
#include "Kernel/Scene.h"
#include "Kernel/Vector.h"
#include "Kernel/String.h"
#include "Kernel/VectorString.h"
#include "Kernel/JSON.h"

namespace Mengine
{
    //////////////////////////////////////////////////////////////////////////
    struct FontEffectViewerExamplePresetEntryDesc
    {
        FontEffectDesc desc;
        uint32_t sample;
    };
    //////////////////////////////////////////////////////////////////////////
    typedef Vector<FontEffectViewerExamplePresetEntryDesc> VectorFontEffectViewerExamplePresetEntries;
    //////////////////////////////////////////////////////////////////////////
    class FontEffectViewerExampleSceneEventReceiver
        : public DummySceneEventReceiver
        , public MCPHandlerInterface
        , public Factorable
    {
    public:
        FontEffectViewerExampleSceneEventReceiver();
        ~FontEffectViewerExampleSceneEventReceiver() override;

    public:
        bool onEntityCreate( const EntityBehaviorInterfacePtr & _behavior, Entity * _entity ) override;
        void onEntityDestroy( const EntityBehaviorInterfacePtr & _behavior ) override;
        bool onEntityPreparation( const EntityBehaviorInterfacePtr & _behavior ) override;
        void onEntityDeactivate( const EntityBehaviorInterfacePtr & _behavior ) override;

    public:
        bool onMCPCall( const jpp::object & _arguments, jpp::object * const _result ) override;

    protected:
        void fillMCPState_( jpp::object * const _result ) const;
        void registerMCPHandler_();
        void unregisterMCPHandler_();

    protected:
        bool loadBundledFonts_();
        bool loadBundledEffects_();

    protected:
        void loadSettings_();
        void saveSettings_() const;

    protected:
        bool setPresetsContent_( const ContentInterfacePtr & _content, const Char * _preferredPreset );
        bool selectPreset_( int _index );
        bool loadPresetsFile_( const Char * _fullPath );
        bool savePresetsFile_( const Char * _fullPath );
        void storeCurrentPreset_();

    protected:
        bool loadGlyphFile_( const Char * _fullPath );
        void selectGlyph_( const ConstString & _glyphName );

    protected:
        void rebuildPreview_();
        void destroyPreview_();

    protected:
        void clampPreviewPan_();
        void updatePreviewPosition_();
        void zoomPreviewAt_( float _newZoom, float _cursorOffsetX, float _cursorOffsetY );

    protected:
        void renderControls_( const ImGUIRenderProviderInterfacePtr & _provider );
        void renderPreviewCanvas_( const ImGUIRenderProviderInterfacePtr & _provider );
        void renderGlyphPreview_( const ImGUIRenderProviderInterfacePtr & _provider );
        void dumpGlyphs_();

    protected:
        bool createPreviewCanvas_();
        void destroyPreviewCanvas_();

    protected:
        Scene * m_scene;

        ImGUIRenderPtr m_imguiRender;
        TextFieldPtr m_textField;

        FontEffectViewerExamplePreviewCanvasPtr m_previewCanvas;
        float m_previewZoom;
        mt::vec2f m_previewPan;
        bool m_previewPanning;
        float m_sidebarWidth;

        FontInterfacePtr m_font;
        FontGlyphInterfacePtr m_glyph;

        FileGroupInterfacePtr m_glyphFileGroup;
        FileGroupInterfacePtr m_presetsFileGroup;
        String m_presetsFileName;
        uint32_t m_loadedGlyphCounter;
        ConstString m_loadedGlyphName;

        VectorFontEffectViewerExamplePresetEntries m_presets;
        VectorString m_presetNames;
        int m_presetIndex;

        FontEffectDesc m_desc;

        VectorString m_glyphNames;
        int m_glyphIndex;

        Char m_presetNameInput[128];
        Char m_presetsPathInput[MENGINE_MAX_PATH];
        Char m_glyphPathInput[MENGINE_MAX_PATH];
        Char m_textInput[1024];

        int m_height;
        int m_sampleIndex;
        float m_fontColor[4];
        bool m_noEffect;
        bool m_showGlyphPreview;
        float m_previewScale;

        bool m_dirty;
        String m_status;

        bool m_mcpHandlerRegistered;
    };
    //////////////////////////////////////////////////////////////////////////
    typedef IntrusivePtr<FontEffectViewerExampleSceneEventReceiver> FontEffectViewerExampleSceneEventReceiverPtr;
    //////////////////////////////////////////////////////////////////////////
}
