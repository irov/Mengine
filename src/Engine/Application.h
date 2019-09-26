#pragma once

#include "Interface/ApplicationInterface.h"

#include "Kernel/Observable.h"
#include "Kernel/Resolution.h"
#include "Kernel/Viewport.h"
#include "Kernel/ConstString.h"
#include "Kernel/ServiceBase.h"

#include "math/vec4.h"

#include "Config/Map.h"

namespace Mengine
{
    typedef IntrusivePtr<class ResourceCursor> ResourceCursorPtr;

    class Application
        : public ServiceBase<ApplicationInterface>
        , public Observable
    {
    public:
        Application();
        ~Application() override;

    public:
        bool _initializeService() override;
        void _finalizeService() override;

    public:
        bool getAllowFullscreenSwitchShortcut() const override;

    public:
        bool initializeGame( const FileGroupInterfacePtr & _fileGroup, const VectorFilePath & _packagesPaths ) override;

    public:
        void changeWindowResolution( const Resolution & _resolution ) override;

    public:
        void setFullscreenMode( bool _fullscreen ) override;
        bool getFullscreenMode() const override;

        uint32_t getWindowBits() const override;
        int32_t getWindowFSAAType() const override;
        int32_t getWindowFSAAQuality() const override;

    public:
        bool isFocus() const override;
        bool isFrozen() const override;

    public:
        void setNopause( bool _nopause ) override;
        bool getNopause() const override;

    protected:
        bool registerBaseTypes_();
        bool registerBaseNodeTypes_();
        bool registerBaseResourceTypes_();
        bool registerEntityGenerator_();
        bool registerSceneGenerator_();
        bool registerArrowGenerator_();

    protected:
        void unregisterBaseTypes_();
        void unregisterBaseNodeTypes_();
        void unregisterBaseResourceTypes_();
        void unregisterEntityGenerator_();
        void unregisterSceneGenerator_();
        void unregisterArrowGenerator_();

    public:
        bool createRenderWindow() override;
        void quit() override;

    public:
        void calcWindowResolution( Resolution & _windowResolution ) const override;

        const Resolution & getCurrentResolution() const override;

        const Viewport & getRenderViewport() const override;
        const Resolution & getContentResolution() const override;

        void getGameViewport( float & _aspect, Viewport & _viewport ) const override;

    public:
        bool render() override;
        void flush() override;
        bool beginUpdate() override;
        void tick( float _time ) override;
        void endUpdate() override;
        void setFocus( bool _focus ) override;
        void setFreeze( bool _freeze ) override;
        void close() override;

        void turnSound( bool _turn ) override;

    public:
        bool keyEvent( const InputKeyEvent & _event ) override;
        bool textEvent( const InputTextEvent & _event ) override;

        bool mouseButtonEvent( const InputMouseButtonEvent & _event ) override;
        bool mouseMove( const InputMouseMoveEvent & _event ) override;
        bool mouseWheel( const InputMouseWheelEvent & _event ) override;
        void mousePosition( const InputMousePositionEvent & _event ) override;
        void mouseEnter( const InputMouseEnterEvent & _event ) override;
        void mouseLeave( const InputMouseLeaveEvent & _event ) override;

    public:
        void paint() override;

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
        void getCompanyName( Char * _companyName ) const override;
        void getProjectName( Char * _projectName ) const override;

    public:
        const ConstString & getProjectCodename() const override;
        uint32_t getProjectVersion() const override;

    public:
        void setLocale( const ConstString & _locale ) override;
        const ConstString & getLocale() const override;

    public:
        const Resolution & getWindowResolution() const;

    public:
        void setFixedContentResolution( bool _fixedContetResolution ) override;
        bool getFixedContentResolution() const override;

    public:
        void setFixedDisplayResolution( bool _fixedDisplayResolution ) override;
        bool getFixedDisplayResolution() override;

    public:
        bool isValidWindowMode() const override;

    public:
        void updateNotification();
        void setVSync( bool _vsync ) override;
        bool getVSync() const override;
        void setCursorMode( bool _mode ) override;
        bool getCursorMode() const override;
        void setCursorIcon( const ConstString & _resourceName ) override;

    public:
        void debugPause( bool _pause ) override;

    protected:
        bool findBestAspectViewport_( float _aspect, float & _bestAspect, Viewport & _viewport ) const;

    protected:
        void calcRenderViewport_( const Resolution & _resolution, Viewport & _viewport );
        void invalidateWindow_();

    protected:
        void updateDebugOpenFile_();
        void notifyDebugOpenFile_( const Char * _folder, const Char * _filePath, bool _streaming );

    protected:
        Resolution m_currentResolution;

        Viewport m_renderViewport;

        bool m_particleEnable;
        bool m_textEnable;
        bool m_videoEnable;

        bool m_focus;
        bool m_freeze;
        bool m_update;
        bool m_nopause;

        Resolution m_windowResolution;
        uint32_t m_bits;
        bool m_fullscreen;
        bool m_nofullscreen;
        bool m_vsync;

        int32_t	m_FSAAType;
        int32_t m_FSAAQuality;

        Resolution m_contentResolution;

        typedef Map<float, Viewport> MapAspectRatioViewports;
        MapAspectRatioViewports m_aspectRatioViewports;

        uint32_t m_updateRevision;

        bool m_fixedContentResolution;
        bool m_fixedDisplayResolution;
        bool m_createRenderWindow;

        uint32_t m_debugMask;

        float m_maxFrameTime;

        ResourceCursorPtr m_cursorResource;

        String m_companyName;
        String m_projectName;

        ConstString m_locale;

        ConstString m_projectCodename;
        uint32_t m_projectVersion;

        bool m_invalidateVsync;
        bool m_cursorMode;
        bool m_invalidateCursorMode;
        bool m_mouseEnter;

        bool m_inputMouseButtonEventBlock;

        bool m_debugPause;
        bool m_debugFileOpen;        
    };
}
