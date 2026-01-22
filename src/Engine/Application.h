#pragma once

#include "Interface/ApplicationInterface.h"

#include "Kernel/Resolution.h"
#include "Kernel/Viewport.h"
#include "Kernel/StaticString.h"
#include "Kernel/ConstString.h"
#include "Kernel/ServiceBase.h"
#include "Kernel/Resource.h"
#include "Kernel/ResourceImage.h"
#include "Kernel/ResourceCursor.h"
#include "Kernel/MapAspectRatioViewports.h"

#include "Interface/MixerBooleanInterface.h"

#include "math/vec4.h"

namespace Mengine
{
    //////////////////////////////////////////////////////////////////////////
    class Application
        : public ServiceBase<ApplicationInterface>
    {
        DECLARE_FACTORABLE( Application );

    public:
        Application();
        ~Application() override;

    public:
        bool _initializeService() override;
        void _finalizeService() override;
        void _stopService() override;

    public:
        bool getAllowFullscreenSwitchShortcut() const override;

    public:
        bool initializeGame( const FileGroupInterfacePtr & _fileGroup, const VectorFilePath & _packagesPaths ) override;
        void finalizeGame() override;

    public:
        void setWindowResolution( const Resolution & _resolution ) override;
        const Resolution & getWindowResolution() const override;

    public:
        void setFullscreenMode( bool _fullscreen ) override;
        bool getFullscreenMode() const override;

        uint32_t getWindowBits() const override;
        int32_t getWindowFSAAType() const override;
        int32_t getWindowFSAAQuality() const override;

    public:
        void setFocus( bool _focus ) override;
        bool isFocus() const override;

    public:        
        void setUpdateFreeze( const ConstString & _owner, bool _freeze ) override;
        bool isUpdateFrozen() const override;

    public:
        void setRenderFreeze( const ConstString & _owner, bool _freeze ) override;
        bool isRenderFrozen() const override;

    public:
        void setNopause( bool _nopause ) override;
        bool getNopause() const override;

    protected:
        bool registerBaseTypes_();
        bool registerBaseNodeTypes_();
        bool registerBaseResourceTypes_();
        bool registerEntityGenerator_();
        bool registerSceneGenerator_();

    protected:
        void unregisterBaseTypes_();
        void unregisterBaseNodeTypes_();
        void unregisterBaseResourceTypes_();
        void unregisterEntityGenerator_();
        void unregisterSceneGenerator_();

    public:
        bool createRenderWindow() override;
        void quit() override;

    public:
        bool calcWindowResolution( bool _fullscreen, Resolution * const _windowResolution ) const override;

        const Resolution & getCurrentWindowResolution() const override;

        const Viewport & getRenderViewport() const override;
        const Resolution & getContentResolution() const override;

        void getGameViewport( float * const _aspect, Viewport * const _viewport ) const override;

        EContentEdgeMode getAspectRatioContentEdgeMode() const override;

    protected:
        void calcRenderViewport_( float * const _aspect, Viewport * const _viewport ) const;

    public:
        bool render() override;
        void flush() override;
        bool beginUpdate() override;
        void tick( float _time ) override;
        void endUpdate() override;
        void close() override;

    public:
        void turnSound( bool _turn ) override;

    public:
        bool handleKeyEvent( const InputKeyEvent & _event ) override;
        bool handleTextEvent( const InputTextEvent & _event ) override;

    public:
        bool handleAccelerometerEvent( const InputAccelerometerEvent & _event ) override;

    public:
        bool handleMouseButtonEvent( const InputMouseButtonEvent & _event ) override;
        bool handleMouseButtonEventBegin( const InputMouseButtonEvent & _event ) override;
        bool handleMouseButtonEventEnd( const InputMouseButtonEvent & _event ) override;
        bool handleMouseMove( const InputMouseMoveEvent & _event ) override;
        bool handleMouseWheel( const InputMouseWheelEvent & _event ) override;

    public:
        bool handleMouseEnter( const InputMouseEnterEvent & _event ) override;
        void handleMouseLeave( const InputMouseLeaveEvent & _event ) override;

    public:
        void setParticleEnable( bool _enabled ) override;
        bool getParticleEnable() const override;

        void setTextEnable( bool _enable ) override;
        bool getTextEnable() const override;

        void setVideoEnable( bool _enabled ) override;
        bool getVideoEnable() const override;

    public:
        void setInputMouseButtonEventBlock( bool _block ) override;
        bool getInputMouseButtonEventBlock() const override;

    public:
        void minimizeWindow() override;

        uint32_t getDebugMask() const override;

    public:
        size_t getCompanyName( Char * const _companyName ) const override;
        size_t getProjectName( Char * const _projectName ) const override;

    public:
        const ConstString & getProjectCodename() const override;
        uint32_t getProjectVersion() const override;

    public:
        void setLocale( const ConstString & _locale ) override;
        const ConstString & getLocale() const override;

    public:
        void setFixedContentResolution( bool _fixedContetResolution ) override;
        bool getFixedContentResolution() const override;

    public:
        void setFixedDisplayResolution( bool _fixedDisplayResolution ) override;
        bool getFixedDisplayResolution() const override;

    public:
        void setFixedViewportResolution( bool _fixedViewportResolution ) override;
        bool getFixedViewportResolution() const override;

    public:
        void updateNotification();
        void setVSync( bool _vsync ) override;
        bool getVSync() const override;
        void setCursorMode( bool _cursorMode ) override;
        bool getCursorMode() const override;
        void setCursorIcon( const ConstString & _resourceName ) override;

    public:
        void debugPause( bool _pause ) override;

    protected:
        void invalidateWindow_();

    protected:
        void updateDebugOpenFile_();
        void notifyDebugOpenFile_( const FilePath & _folder, const FilePath & _filePath, bool _read, bool _streaming );

    protected:
        void updateDebugResourceCompile_();
        void notifyDebugResourceCompile_( Resource * _resource );
        void notifyDebugResourceRelease_( Resource * _resource );

    protected:
        void notifyEnginePrepareFinalize_();

    protected:
        RenderPipelineInterfacePtr m_renderPipeline;
        ResourceImagePtr m_resourceWhitePixel;

        Resolution m_currentWindowResolution;

        Viewport m_renderViewport;

        bool m_particleEnable;
        bool m_textEnable;
        bool m_videoEnable;

        bool m_focus;
        bool m_updateGraceful;
        bool m_nopause;

        MixerBooleanInterfacePtr m_updateFreezer;
        MixerBooleanInterfacePtr m_renderFreezer;

        Resolution m_windowResolution;

        uint32_t m_bits;
        bool m_fullscreen;
        bool m_alwaysfullscreen;
        bool m_nofullscreen;
        bool m_vsync;

        int32_t m_FSAAType;
        int32_t m_FSAAQuality;

        Resolution m_contentResolution;

        MapAspectRatioViewports m_aspectRatioViewports;

        uint32_t m_updateRevision;

        bool m_fixedContentResolution;
        bool m_fixedDisplayResolution;
        bool m_fixedViewportResolution;
        bool m_createRenderWindow;

        uint32_t m_debugMask;

        float m_maxFrameTime;

        ConstString m_cursorResourceName;
        ResourceCursorPtr m_cursorResource;

        StaticString<MENGINE_APPLICATION_COMPANY_MAXNAME> m_companyName;
        StaticString<MENGINE_APPLICATION_PROJECT_MAXNAME> m_projectName;

        ConstString m_locale;

        ConstString m_projectCodename;
        uint32_t m_projectVersion;

        bool m_invalidateVsync;
        bool m_cursorMode;
        bool m_invalidateCursorMode;

        bool m_inputMouseButtonEventBlock;

        bool m_debugPause;
        bool m_debugFileOpen;
        bool m_debugResourceCompile;

        bool m_initailizeGame;
    };
    //////////////////////////////////////////////////////////////////////////
}
