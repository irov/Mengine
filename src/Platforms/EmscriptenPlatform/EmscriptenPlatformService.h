#pragma once

#include "Interface/PlatformServiceInterface.h"
#include "Interface/RenderPlatformInterface.h"

#include "Environment/Emscripten/EmscriptenIncluder.h"

#include "Kernel/KeyCode.h"
#include "Kernel/MouseButtonCode.h"
#include "Kernel/ServiceBase.h"
#include "Kernel/String.h"
#include "Kernel/TouchCode.h"

namespace Mengine
{
    class EmscriptenPlatformService
        : public ServiceBase<PlatformServiceInterface>
        , public RenderPlatformInterface
    {
    public:
        EmscriptenPlatformService();
        ~EmscriptenPlatformService() override;

    public:
        RenderPlatformInterface * getRenderPlatform() override;

    public:
        Timestamp getPlatfomTime() const override;
        Timestamp getInputTimestamp() const override;

    public:
        void setProjectTitle( const Char * _projectTitle ) override;
        size_t getProjectTitle( Char * const _projectTitle ) const override;

    public:
        const Tags & getPlatformTags() const override;
        bool hasPlatformTag( const ConstString & _tag ) const override;
        bool isDesktop() const override;
        bool hasTouchpad() const override;

    public:
        bool getDesktopResolution( Resolution * const _resolution ) const override;
        bool getMaxClientResolution( Resolution * const _resolution ) const override;
        bool getNoFullscreen() const override;
        bool getAlwaysFullscreen() const override;

    public:
        bool getSafeAreaViewport( Viewport * const _viewport ) const override;
        void setSafeAreaViewportChangedCallback( const LambdaSafeAreaViewportChanged & _callback ) override;
        bool getDisplayCutoutViewport( Viewport * const _viewport ) const override;

    public:
        size_t getCurrentPath( Char * const _filePath ) const override;
        size_t getUserPath( Char * const _filePath ) const override;
        size_t getTemporaryPath( Char * const _filePath ) const override;
        size_t getExtraPreferencesFolderName( Char * const _folderName ) const override;

    public:
        void setCursorPosition( const mt::vec2f & _cursorPosition ) override;
        bool setCursorCapture( bool _capture ) override;
        void setCursorIcon( const ConstString & _icon ) override;
        bool hasCursorIcon( const ConstString & _icon ) const override;

    public:
        size_t getSystemFontPath( ConstString * const _groupName, const Char * _fontName, Char * const _fontPath ) const override;

    public:
        bool setClipboardText( const Char * _value ) const override;
        bool getClipboardText( Char * const _value, size_t _capacity ) const override;

    public:
        bool _initializeService() override;
        void _finalizeService() override;

    public:
        bool initializeFileService() override;
        void finalizeFileService() override;

    public:
        bool runPlatform() override;
        void loopPlatform() override;
        bool updatePlatform() override;
        void tickPlatform( float _frameTime ) override;
        void stopPlatform() override;

    public:
        bool alreadyRunningMonitor() override;
        bool createWindow( const Resolution & _resolution, bool _fullscreen ) override;
        void closeWindow() override;
        void minimizeWindow() override;

    public:
        int32_t dpToWidthPx( int32_t _dp ) const override;
        int32_t dpToHeightPx( int32_t _dp ) const override;
        int32_t pxToWidthDp( int32_t _px ) const override;
        int32_t pxToHeightDp( int32_t _px ) const override;

    public:
        DynamicLibraryInterfacePtr loadDynamicLibrary( const Char * _dynamicLibraryName, const DocumentInterfacePtr & _doc ) override;

    public:
        bool createProcess( const Char * _executable, const ArgumentStrings & _arguments, uint32_t * const _exitCode ) const override;

    public:
        bool isDebuggerPresent() const override;
        void debugBreak() override;

    public:
        void showKeyboard() override;
        void hideKeyboard() override;
        bool isShowKeyboard() const override;

    public:
        bool notifyWindowModeChanged( const Resolution & _resolution, bool _fullscreen ) override;
        void notifyCursorModeChanged( bool _mode ) override;
        bool notifyCursorIconSetup( const ConstString & _name, const ContentInterfacePtr & _content, const MemoryInterfacePtr & _memory ) override;

    public:
        void onEvent( const ConstString & _event, const Params & _params ) override;

    public:
        bool openUrlInDefaultBrowser( const Char * _url ) override;
        bool openMail( const Char * _email, const Char * _subject, const Char * _body, const Char * _technically ) override;

    public:
        bool openDeleteAccount( const LambdaDeleteAccountAccepted & _accepted, const LambdaDeleteAccountCanceled & _canceled ) override;
        bool completeDeleteAccount( EDeleteAccountResult _result ) override;

    public:
        bool isNetworkAvailable() const override;

    public:
        bool updateDesktopWallpaper( const Char * _directoryPath, const Char * _filePath ) override;

    public:
        bool createDirectoryUserPicture( const Char * _directoryPath, const Char * _filePath, const void * _data, size_t _size ) override;
        bool createDirectoryUserMusic( const Char * _directoryPath, const Char * _filePath, const void * _data, size_t _size ) override;

    public:
        void messageBox( const Char * _caption, const Char * _format, ... ) const override;
        bool showSystemDialog( const Char * _title, const Char * _message, const LambdaSystemDialog & _callback ) override;

    public:
        bool renderPlatform() override;
        void notifyVsyncChanged( bool _vsync ) override;

    protected:
        void setActive_();
        void releaseInput_();
        void resize_();
        void getPointer_( double _clientX, double _clientY, mt::vec2f * const _point ) const;

    protected:
        static bool onMouse_( int _type, const EmscriptenMouseEvent * _event, void * _user );
        static bool onKey_( int _type, const EmscriptenKeyboardEvent * _event, void * _user );
        static bool onWheel_( int _type, const EmscriptenWheelEvent * _event, void * _user );
        static bool onTouch_( int _type, const EmscriptenTouchEvent * _event, void * _user );

    protected:
        static bool onFocus_( int _type, const EmscriptenFocusEvent * _event, void * _user );
        static bool onCanvasBlur_( int _type, const EmscriptenFocusEvent * _event, void * _user );
        static bool onVisibility_( int _type, const EmscriptenVisibilityChangeEvent * _event, void * _user );

    protected:
        static bool onResize_( int _type, const EmscriptenUiEvent * _event, void * _user );
        static bool onContext_( int _type, const void * _event, void * _user );

    protected:
        EMSCRIPTEN_WEBGL_CONTEXT_HANDLE m_context;

        Tags m_tags;
        String m_title;

        Resolution m_resolution;
        LambdaSafeAreaViewportChanged m_safeAreaCallback;
        double m_ratio;

        double m_beginTime;

        bool m_running;
        bool m_active;
        bool m_focused;
        bool m_hidden;
        bool m_contextLost;
        bool m_resetFrame;
        bool m_started;

        bool m_keyDown[MENGINE_INPUT_MAX_KEY_CODE] = {false};
        bool m_mouseButtonDown[MENGINE_INPUT_MAX_MOUSE_BUTTON_CODE] = {false};

        int32_t m_touchIds[MENGINE_INPUT_MAX_TOUCH];
        mt::vec2f m_touchPositions[MENGINE_INPUT_MAX_TOUCH];
    };
}
