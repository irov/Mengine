#include "GameScriptEmbedding.h"

#include "Interface/ScriptServiceInterface.h"
#include "Interface/GameServiceInterface.h"
#include "Interface/ConfigServiceInterface.h"
#include "Interface/OptionsInterface.h"

#include "Environment/Python/PythonEventReceiver.h"

#include "Kernel/Logger.h"

namespace Mengine
{
    //////////////////////////////////////////////////////////////////////////
    namespace Detail
    {
        class PythonGameEventReceiver
            : public GameEventReceiver
            , public PythonEventReceiver
            , public Factorable
        {
        protected:
            void onGameFullscreen( bool _fullscreen ) override
            {
                m_cb.call( _fullscreen );
            }

            void onGameFixedContentResolution( bool _fixed ) override
            {
                m_cb.call( _fixed );
            }

            void onGameFixedDisplayResolution( bool _fixed ) override
            {
                m_cb.call( _fixed );
            }

            void onGameRenderViewport( const Viewport & _viewport, const Resolution & _contentResolution ) override
            {
                m_cb.call( _viewport, _contentResolution );
            }

            void onGameViewport( const Viewport & _viewport, float _aspect ) override
            {
                m_cb.call( _viewport, _aspect );
            }

            bool onGameKey( KeyCode _code, float _x, float _y, bool _isDown, bool _isRepeat ) override
            {
                return m_cb.call( (uint32_t)_code, _x, _y, _isDown, _isRepeat );
            }

            bool onGameText( WChar _key, float _x, float _y ) override
            {
                return m_cb.call( _key, _x, _y );
            }

            bool onGameMouseButton( uint32_t _touchId, float _x, float _y, uint32_t _button, bool _isDown ) override
            {
                return m_cb.call( _touchId, _x, _y, _button, _isDown );
            }

            bool onGameMouseButtonBegin( uint32_t _touchId, float _x, float _y, uint32_t _button, bool _isDown ) override
            {
                return m_cb.call( _touchId, _x, _y, _button, _isDown );
            }

            bool onGameMouseButtonEnd( uint32_t _touchId, float _x, float _y, uint32_t _button, bool _isDown ) override
            {
                return m_cb.call( _touchId, _x, _y, _button, _isDown );
            }

            bool onGameMouseMove( uint32_t _touchId, float _x, float _y, float _dx, float _dy ) override
            {
                return m_cb.call( _touchId, _x, _y, _dx, _dy );
            }

            bool onGameMouseWheel( uint32_t _button, float _x, float _y, int32_t _wheel ) override
            {
                return m_cb.call( _button, _x, _y, _wheel );
            }

            void onGameAppMouseEnter( float _x, float _y ) override
            {
                m_cb.call( _x, _y );
            }

            void onGameAppMouseLeave() override
            {
                m_cb.call();
            }

            void onGameTimingFactor( float _timingFactor ) override
            {
                m_cb.call( _timingFactor );
            }

            bool onGamePreparation( bool _debug ) override
            {
                return m_cb.call( _debug );
            }

            void onGameRun() override
            {
                m_cb.call();
            }

            bool onGameInitialize() override
            {
                return m_cb.call();
            }

            void onGameInitializeRenderResources() override
            {
                m_cb.call();
            }

            void onGameFinalizeRenderResources() override
            {
                m_cb.call();
            }

            void onGameAccountFinalize() override
            {
                m_cb.call();
            }

            void onGameFinalize() override
            {
                m_cb.call();
            }

            void onGameDestroy() override
            {
                m_cb.call();
            }

            void onGameFocus( bool _focus ) override
            {
                m_cb.call( _focus );
            }

            void onGameCreateDefaultAccount() override
            {
                m_cb.call();
            }

            void onGameCreateGlobalAccount() override
            {
                m_cb.call();
            }

            void onGameLoadAccounts() override
            {
                m_cb.call();
            }

            void onGameCreateAccount( const ConstString & _accountID, bool _global ) override
            {
                m_cb.call( _accountID, _global );
            }

            void onGameDeleteAccount( const ConstString & _accountID ) override
            {
                m_cb.call( _accountID );
            }

            void onGameSelectAccount( const ConstString & _accountID ) override
            {
                m_cb.call( _accountID );
            }

            void onGameUselectAccount( const ConstString & _accountID ) override
            {
                m_cb.call( _accountID );
            }

            void onGameChangeSoundVolume( float _sound, float _music, float _voice ) override
            {
                m_cb.call( _sound, _music, _voice );
            }

            void onGameCursorMode( bool _mode ) override
            {
                m_cb.call( _mode );
            }

            void onGameUser( const ConstString & _event, const MapWParams & _params ) override
            {
                m_cb.call( _event, _params );
            }

            bool onGameClose() override
            {
                return m_cb.call();
            }

            void onGameOverFillrate( double _fillrate ) override
            {
                m_cb.call( _fillrate );
            }

            void onGameFrameEnd() override
            {
                m_cb.call();
            }
        };
        //////////////////////////////////////////////////////////////////////////
        static void registerGameEventMethods( pybind::kernel_interface * _kernel, const ScriptModuleInterfacePtr & _module )
        {
            const pybind::module & base_module = _module->getModule();

            pybind::module py_module( base_module );

            GameServiceInterface * game = GAME_SERVICE();

            Helper::registerPythonEventReceiverModule<PythonGameEventReceiver>( _kernel, py_module, game, "onFullscreen", EVENT_GAME_FULLSCREEN );
            Helper::registerPythonEventReceiverModule<PythonGameEventReceiver>( _kernel, py_module, game, "onFixedContentResolution", EVENT_GAME_FIXED_CONTENT_RESOLUTION );
            Helper::registerPythonEventReceiverModule<PythonGameEventReceiver>( _kernel, py_module, game, "onFixedDisplayResolution", EVENT_GAME_FIXED_DISPLAY_RESOLUTION );
            Helper::registerPythonEventReceiverModule<PythonGameEventReceiver>( _kernel, py_module, game, "onRenderViewport", EVENT_GAME_RENDER_VIEWPORT );
            Helper::registerPythonEventReceiverModule<PythonGameEventReceiver>( _kernel, py_module, game, "onGameViewport", EVENT_GAME_VIEWPORT );

            Helper::registerPythonEventReceiverModule<PythonGameEventReceiver>( _kernel, py_module, game, "onHandleKeyEvent", EVENT_GAME_KEY );
            Helper::registerPythonEventReceiverModule<PythonGameEventReceiver>( _kernel, py_module, game, "onHandleTextEvent", EVENT_GAME_TEXT );
            Helper::registerPythonEventReceiverModule<PythonGameEventReceiver>( _kernel, py_module, game, "onHandleMouseButtonEvent", EVENT_GAME_MOUSE_BUTTON );
            Helper::registerPythonEventReceiverModule<PythonGameEventReceiver>( _kernel, py_module, game, "onHandleMouseButtonEventBegin", EVENT_GAME_MOUSE_BUTTON_BEGIN );
            Helper::registerPythonEventReceiverModule<PythonGameEventReceiver>( _kernel, py_module, game, "onHandleMouseButtonEventEnd", EVENT_GAME_MOUSE_BUTTON_END );
            Helper::registerPythonEventReceiverModule<PythonGameEventReceiver>( _kernel, py_module, game, "onHandleMouseMove", EVENT_GAME_MOUSE_MOVE );
            Helper::registerPythonEventReceiverModule<PythonGameEventReceiver>( _kernel, py_module, game, "onHandleMouseWheel", EVENT_GAME_MOUSE_WHEEL );

            Helper::registerPythonEventReceiverModule<PythonGameEventReceiver>( _kernel, py_module, game, "onAppMouseEnter", EVENT_GAME_APP_MOUSE_ENTER );
            Helper::registerPythonEventReceiverModule<PythonGameEventReceiver>( _kernel, py_module, game, "onAppMouseLeave", EVENT_GAME_APP_MOUSE_LEAVE );
            Helper::registerPythonEventReceiverModule<PythonGameEventReceiver>( _kernel, py_module, game, "onTimingFactor", EVENT_GAME_ON_TIMING_FACTOR );

            Helper::registerPythonEventReceiverModule<PythonGameEventReceiver>( _kernel, py_module, game, "onPreparation", EVENT_GAME_PREPARATION );
            Helper::registerPythonEventReceiverModule<PythonGameEventReceiver>( _kernel, py_module, game, "onRun", EVENT_GAME_RUN );
            Helper::registerPythonEventReceiverModule<PythonGameEventReceiver>( _kernel, py_module, game, "onInitialize", EVENT_GAME_INITIALIZE );
            Helper::registerPythonEventReceiverModule<PythonGameEventReceiver>( _kernel, py_module, game, "onInitializeRenderResources", EVENT_GAME_INITIALIZE_RENDER_RESOURCES );
            Helper::registerPythonEventReceiverModule<PythonGameEventReceiver>( _kernel, py_module, game, "onAccountFinalize", EVENT_GAME_ACCOUNT_FINALIZE );
            Helper::registerPythonEventReceiverModule<PythonGameEventReceiver>( _kernel, py_module, game, "onFinalize", EVENT_GAME_FINALIZE );
            Helper::registerPythonEventReceiverModule<PythonGameEventReceiver>( _kernel, py_module, game, "onDestroy", EVENT_GAME_DESTROY );

            Helper::registerPythonEventReceiverModule<PythonGameEventReceiver>( _kernel, py_module, game, "onFocus", EVENT_GAME_FOCUS );
            Helper::registerPythonEventReceiverModule<PythonGameEventReceiver>( _kernel, py_module, game, "onCreateDefaultAccount", EVENT_GAME_CREATE_DEFAULT_ACCOUNT );
            Helper::registerPythonEventReceiverModule<PythonGameEventReceiver>( _kernel, py_module, game, "onCreateGlobalAccount", EVENT_GAME_CREATE_GLOBAL_ACCOUNT );
            Helper::registerPythonEventReceiverModule<PythonGameEventReceiver>( _kernel, py_module, game, "onLoadAccounts", EVENT_GAME_LOAD_ACCOUNTS );

            Helper::registerPythonEventReceiverModule<PythonGameEventReceiver>( _kernel, py_module, game, "onCreateAccount", EVENT_GAME_CREATE_ACCOUNT );
            Helper::registerPythonEventReceiverModule<PythonGameEventReceiver>( _kernel, py_module, game, "onDeleteAccount", EVENT_GAME_DELETE_ACCOUNT );
            Helper::registerPythonEventReceiverModule<PythonGameEventReceiver>( _kernel, py_module, game, "onSelectAccount", EVENT_GAME_SELECT_ACCOUNT );
            Helper::registerPythonEventReceiverModule<PythonGameEventReceiver>( _kernel, py_module, game, "onUnselectAccount", EVENT_GAME_UNSELECT_ACCOUNT );

            Helper::registerPythonEventReceiverModule<PythonGameEventReceiver>( _kernel, py_module, game, "onChangeSoundVolume", EVENT_GAME_CHANGE_SOUND_VOLUME );
            Helper::registerPythonEventReceiverModule<PythonGameEventReceiver>( _kernel, py_module, game, "onCursorMode", EVENT_GAME_CURSOR_MODE );
            Helper::registerPythonEventReceiverModule<PythonGameEventReceiver>( _kernel, py_module, game, "onUserEvent", EVENT_GAME_USER );
            Helper::registerPythonEventReceiverModule<PythonGameEventReceiver>( _kernel, py_module, game, "onCloseWindow", EVENT_GAME_CLOSE );

            Helper::registerPythonEventReceiverModule<PythonGameEventReceiver>( _kernel, py_module, game, "onOverFillrate", EVENT_GAME_OVER_FILLRATE );

            Helper::registerPythonEventReceiverModule<PythonGameEventReceiver>( _kernel, py_module, game, "onFrameEnd", EVENT_GAME_FRAME_END );
        }
    }
    //////////////////////////////////////////////////////////////////////////
    GameScriptEmbedding::GameScriptEmbedding()
    {
    }
    //////////////////////////////////////////////////////////////////////////
    GameScriptEmbedding::~GameScriptEmbedding()
    {
    }
    //////////////////////////////////////////////////////////////////////////
    bool GameScriptEmbedding::embedding( pybind::kernel_interface * _kernel )
    {
        bool developmentMode = HAS_OPTION( "dev" );

        SCRIPT_SERVICE()
            ->addGlobalModuleT( "_DEVELOPMENT", developmentMode );

#ifdef MENGINE_DEBUG
        SCRIPT_SERVICE()
            ->addGlobalModuleT( "_DEBUG", true );
#else
        SCRIPT_SERVICE()
            ->addGlobalModuleT( "_DEBUG", false );
#endif

#ifdef WIN32
        SCRIPT_SERVICE()
            ->addGlobalModuleT( "_WIN32", true );
#else
        SCRIPT_SERVICE()
            ->addGlobalModuleT( "_WIN32", false );
#endif

#ifdef __ANDROID__
        SCRIPT_SERVICE()
            ->addGlobalModuleT( "_ANDROID", true );
#else
        SCRIPT_SERVICE()
            ->addGlobalModuleT( "_ANDROID", false );
#endif

#ifdef MENGINE_MASTER_RELEASE
        SCRIPT_SERVICE()
            ->addGlobalModuleT( "_MASTER_RELEASE", true );
#else
        SCRIPT_SERVICE()
            ->addGlobalModuleT( "_MASTER_RELEASE", false );
#endif

        if( SCRIPT_SERVICE()
            ->bootstrapModules() == false )
        {
            LOGGER_ERROR( "invalid bootstrap modules"
            );

            return false;
        }

        ConstString personality = CONFIG_VALUE( "Game", "PersonalityModule", STRINGIZE_FILEPATH_LOCAL( "Personality" ) );

        ScriptModuleInterfacePtr module = SCRIPT_SERVICE()
            ->importModule( personality );

        if( module == nullptr )
        {
            LOGGER_ERROR( "invalid import module '%s'"
                , personality.c_str()
            );

            return false;
        }

        Detail::registerGameEventMethods( _kernel, module );

        return true;
    }
    //////////////////////////////////////////////////////////////////////////
    void GameScriptEmbedding::ejecting( pybind::kernel_interface * _kernel )
    {
        MENGINE_UNUSED( _kernel );

        //Empty
    }
}