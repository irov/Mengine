#pragma once

#include "Interface/PlatformServiceInterface.h"
#include "Interface/FactoryInterface.h"

#include "Environment/Unix/UnixPlatformServiceExtensionInterface.h"

#include "UnixInput.h"

#include "Kernel/ServiceBase.h"
#include "Kernel/StaticString.h"
#include "Kernel/String.h"
#include "Kernel/Vector.h"

#include <GL/glx.h>

namespace Mengine
{
    class UnixPlatformService
        : public ServiceBase<PlatformServiceInterface>
        , public UnixPlatformServiceExtensionInterface
    {
        DECLARE_UNKNOWABLE();

    public:
        UnixPlatformService();
        ~UnixPlatformService() override;

    public:
        bool _initializeService() override;
        void _finalizeService() override;
        bool _runService() override;
        void _stopService() override;

    public:
        bool initializeFileService() override;
        void finalizeFileService() override;

    public:
        bool runPlatform() override;
        void loopPlatform() override;
        bool updatePlatform() override;
        void tickPlatform( float _frameTime ) override;
        bool renderPlatform() override;
        void stopPlatform() override;

    public:
        Timestamp getPlatfomTime() const override;
        Timestamp getInputTimestamp() const override;

    public:
        void setProjectTitle( const Char * _projectTitle ) override;
        size_t getProjectTitle( Char * const _projectTitle ) const override;
        bool alreadyRunningMonitor() override;
        bool createWindow( const Resolution & _resolution, bool _fullscreen ) override;

    public:
        const Tags & getPlatformTags() const override;
        bool hasPlatformTag( const ConstString & _tag ) const override;
        bool isDesktop() const override;
        bool hasTouchpad() const override;

    public:
        int32_t dpToWidthPx( int32_t _dp ) const override;
        int32_t dpToHeightPx( int32_t _dp ) const override;
        int32_t pxToWidthDp( int32_t _px ) const override;
        int32_t pxToHeightDp( int32_t _px ) const override;

    public:
        DynamicLibraryInterfacePtr loadDynamicLibrary( const Char * _dynamicLibraryName, const DocumentInterfacePtr & _doc ) override;

    public:
        bool getDesktopResolution( Resolution * const _resolution ) const override;
        bool getNoFullscreen() const override;
        bool getAlwaysFullscreen() const override;
        bool isDebuggerPresent() const override;
        void debugBreak() override;

        size_t getCurrentPath( Char * const _currentPath ) const override;
        size_t getUserPath( Char * const _userPath ) const override;
        size_t getExtraPreferencesFolderName( Char * const _folderName ) const override;
        bool getUserLocaleLanguage( Char * const _userLocaleLanguage ) const override;

        void closeWindow() override;
        void minimizeWindow() override;
        void setCursorPosition( const mt::vec2f & _cursorPosition ) override;
        bool setCursorCapture( bool _capture ) override;
        void setCursorIcon( const ConstString & _icon ) override;
        bool hasCursorIcon( const ConstString & _icon ) const override;

        void showKeyboard() override;
        void hideKeyboard() override;
        bool isShowKeyboard() const override;

    public:
        bool notifyWindowModeChanged( const Resolution & _resolution, bool _fullscreen ) override;
        void notifyVsyncChanged( bool _vsync ) override;
        void notifyCursorModeChanged( bool _mode ) override;
        bool notifyCursorIconSetup( const ConstString & _name, const ContentInterfacePtr & _content, const MemoryInterfacePtr & _memory ) override;

    public:
        void onEvent( const ConstString & _event, const Params & _params ) override;
        size_t getSystemFontPath( ConstString * const _groupName, const Char * _fontName, Char * const _fontPath ) const override;
        bool getMaxClientResolution( Resolution * const _resolution ) const override;

    public:
        bool openUrlInDefaultBrowser( const Char * _url ) override;
        bool openMail( const Char * _email, const Char * _subject, const Char * _body, const Char * _technically ) override;
        bool openDeleteAccount() override;
        bool updateDesktopWallpaper( const Char * _directoryPath, const Char * _filePath ) override;
        bool createDirectoryUserPicture( const Char * _directoryPath, const Char * _filePath, const void * _data, size_t _size ) override;
        bool createDirectoryUserMusic( const Char * _directoryPath, const Char * _filePath, const void * _data, size_t _size ) override;

    public:
        void messageBox( const Char * _caption, const Char * _format, ... ) const override;
        bool setClipboardText( const Char * _value ) const override;
        bool getClipboardText( Char * _value, size_t _capacity ) const override;

    public:
        Display * getX11Display() const override;
        Window getX11Window() const override;
        UnixOpenGLProc getOpenGLProcAddress( const Char * _name ) const override;
        UniqueId addUnixEventHandler( const LambdaUnixEventHandler & _lambda, const DocumentInterfacePtr & _doc ) override;
        void removeUnixEventHandler( UniqueId _id ) override;

    protected:
        bool createWindow_( const Resolution & _resolution, bool _fullscreen );
        void destroyWindow_();
        bool createOpenGLContext_( GLXFBConfig _config );
        bool changeWindow_( const Resolution & _resolution, bool _fullscreen );
        void setFullscreen_( bool _fullscreen );
        bool processEvents_();
        size_t getEventText_( const XEvent & _event, Char * const _text, size_t _capacity ) const;
        void pushQuitEvent_();
        void setActive_( bool _active, Timestamp _timestamp );
        bool isNeedWindowRender_() const;
        void handleClipboardRequest_( const XSelectionRequestEvent & _request );
        bool writeUserFile_( const Char * _baseFolder, const Char * _directoryPath, const Char * _filePath, const void * _data, size_t _size ) const;

    protected:
        Timestamp m_beginTime;
        Tags m_platformTags;
        StaticString<MENGINE_PLATFORM_PROJECT_TITLE_MAXNAME> m_projectTitle;

        Display * m_display;
        int m_screen;
        Window m_window;
        Window m_clipboardWindow;
        Colormap m_colormap;
        GLXContext m_glContext;
        XIM m_inputMethod;
        XIC m_inputContext;

        Atom m_wmProtocolsAtom;
        Atom m_wmDeleteWindowAtom;
        Atom m_netWmStateAtom;
        Atom m_netWmStateFullscreenAtom;
        Atom m_netWorkAreaAtom;
        Atom m_clipboardAtom;
        Atom m_targetsAtom;
        Atom m_utf8StringAtom;
        Atom m_textPlainAtom;
        Atom m_clipboardPropertyAtom;

        ::Cursor m_defaultCursor;
        ::Cursor m_hiddenCursor;

        FactoryInterfacePtr m_factoryDynamicLibraries;
        UnixInputPtr m_input;

        struct UnixEventHandlerDesc
        {
            UniqueId id;
            LambdaUnixEventHandler lambda;

#if defined(MENGINE_DOCUMENT_ENABLE)
            DocumentInterfacePtr doc;
#endif
        };

        typedef Vector<UnixEventHandlerDesc> VectorUnixEventHandlers;
        VectorUnixEventHandlers m_eventHandlers;

        mutable String m_clipboardText;

        Resolution m_windowResolution;
        double m_prevTime;
        float m_pauseUpdatingTime;
        float m_displayScale;

        bool m_active;
        bool m_fullscreen;
        bool m_quitRequested;
        bool m_windowAttached;
        bool m_windowExposed;
        bool m_cursorVisible;
        bool m_keyboardVisible;
    };
}
