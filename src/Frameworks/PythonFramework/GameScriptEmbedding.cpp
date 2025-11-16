#include "GameScriptEmbedding.h"

#include "Interface/ScriptServiceInterface.h"
#include "Interface/GameServiceInterface.h"
#include "Interface/OptionsServiceInterface.h"
#include "Interface/ScriptModuleInterface.h"

#include "PythonGameEventReceiver.h"

#include "Kernel/AssertionMemoryPanic.h"
#include "Kernel/Logger.h"
#include "Kernel/FilePathHelper.h"
#include "Kernel/ConfigHelper.h"
#include "Kernel/BuildMode.h"

namespace Mengine
{
    //////////////////////////////////////////////////////////////////////////
    namespace Detail
    {
        //////////////////////////////////////////////////////////////////////////
        static void registerGameEventMethods( pybind::kernel_interface * _kernel, const ScriptModuleInterfacePtr & _module, const DocumentInterfacePtr & _doc )
        {
            GameServiceInterface * game = GAME_SERVICE();

            const pybind::module & py_module = _module->getModule();

            Helper::registerPythonEventReceiverModule<PythonGameEventReceiver>( _kernel, py_module, game, STRINGIZE_STRING_LOCAL( "onFullscreen" ), EVENT_GAME_FULLSCREEN, _doc );
            Helper::registerPythonEventReceiverModule<PythonGameEventReceiver>( _kernel, py_module, game, STRINGIZE_STRING_LOCAL( "onFixedContentResolution" ), EVENT_GAME_FIXED_CONTENT_RESOLUTION, _doc );
            Helper::registerPythonEventReceiverModule<PythonGameEventReceiver>( _kernel, py_module, game, STRINGIZE_STRING_LOCAL( "onFixedDisplayResolution" ), EVENT_GAME_FIXED_DISPLAY_RESOLUTION, _doc );
            Helper::registerPythonEventReceiverModule<PythonGameEventReceiver>( _kernel, py_module, game, STRINGIZE_STRING_LOCAL( "onFixedViewportResolution" ), EVENT_GAME_FIXED_VIEWPORT_RESOLUTION, _doc );
            Helper::registerPythonEventReceiverModule<PythonGameEventReceiver>( _kernel, py_module, game, STRINGIZE_STRING_LOCAL( "onRenderViewport" ), EVENT_GAME_RENDER_VIEWPORT, _doc );
            Helper::registerPythonEventReceiverModule<PythonGameEventReceiver>( _kernel, py_module, game, STRINGIZE_STRING_LOCAL( "onGameViewport" ), EVENT_GAME_VIEWPORT, _doc );

            Helper::registerPythonEventReceiverModule<PythonGameEventReceiver>( _kernel, py_module, game, STRINGIZE_STRING_LOCAL( "onHandleKeyEvent" ), EVENT_GAME_KEY, _doc );
            Helper::registerPythonEventReceiverModule<PythonGameEventReceiver>( _kernel, py_module, game, STRINGIZE_STRING_LOCAL( "onHandleTextEvent" ), EVENT_GAME_TEXT, _doc );
            Helper::registerPythonEventReceiverModule<PythonGameEventReceiver>( _kernel, py_module, game, STRINGIZE_STRING_LOCAL( "onHandleMouseButtonEvent" ), EVENT_GAME_MOUSE_BUTTON, _doc );
            Helper::registerPythonEventReceiverModule<PythonGameEventReceiver>( _kernel, py_module, game, STRINGIZE_STRING_LOCAL( "onHandleMouseButtonEventBegin" ), EVENT_GAME_MOUSE_BUTTON_BEGIN, _doc );
            Helper::registerPythonEventReceiverModule<PythonGameEventReceiver>( _kernel, py_module, game, STRINGIZE_STRING_LOCAL( "onHandleMouseButtonEventEnd" ), EVENT_GAME_MOUSE_BUTTON_END, _doc );
            Helper::registerPythonEventReceiverModule<PythonGameEventReceiver>( _kernel, py_module, game, STRINGIZE_STRING_LOCAL( "onHandleMouseMove" ), EVENT_GAME_MOUSE_MOVE, _doc );
            Helper::registerPythonEventReceiverModule<PythonGameEventReceiver>( _kernel, py_module, game, STRINGIZE_STRING_LOCAL( "onHandleMouseWheel" ), EVENT_GAME_MOUSE_WHEEL, _doc );

            Helper::registerPythonEventReceiverModule<PythonGameEventReceiver>( _kernel, py_module, game, STRINGIZE_STRING_LOCAL( "onAppMouseEnter" ), EVENT_GAME_APP_MOUSE_ENTER, _doc );
            Helper::registerPythonEventReceiverModule<PythonGameEventReceiver>( _kernel, py_module, game, STRINGIZE_STRING_LOCAL( "onAppMouseLeave" ), EVENT_GAME_APP_MOUSE_LEAVE, _doc );
            Helper::registerPythonEventReceiverModule<PythonGameEventReceiver>( _kernel, py_module, game, STRINGIZE_STRING_LOCAL( "onTimeFactorChange" ), EVENT_GAME_TIME_FACTOR_CHANGE, _doc );
            Helper::registerPythonEventReceiverModule<PythonGameEventReceiver>( _kernel, py_module, game, STRINGIZE_STRING_LOCAL( "onSettingChange" ), EVENT_GAME_SETTING_CHANGE, _doc );

            Helper::registerPythonEventReceiverModule<PythonGameEventReceiver>( _kernel, py_module, game, STRINGIZE_STRING_LOCAL( "onPreparation" ), EVENT_GAME_PREPARATION, _doc );
            Helper::registerPythonEventReceiverModule<PythonGameEventReceiver>( _kernel, py_module, game, STRINGIZE_STRING_LOCAL( "onRun" ), EVENT_GAME_RUN, _doc );
            Helper::registerPythonEventReceiverModule<PythonGameEventReceiver>( _kernel, py_module, game, STRINGIZE_STRING_LOCAL( "onInterruption" ), EVENT_GAME_INTERRUPTION, _doc );
            Helper::registerPythonEventReceiverModule<PythonGameEventReceiver>( _kernel, py_module, game, STRINGIZE_STRING_LOCAL( "onStop" ), EVENT_GAME_STOP, _doc );
            Helper::registerPythonEventReceiverModule<PythonGameEventReceiver>( _kernel, py_module, game, STRINGIZE_STRING_LOCAL( "onInitialize" ), EVENT_GAME_INITIALIZE, _doc );
            Helper::registerPythonEventReceiverModule<PythonGameEventReceiver>( _kernel, py_module, game, STRINGIZE_STRING_LOCAL( "onInitializeRenderResources" ), EVENT_GAME_INITIALIZE_RENDER_RESOURCES, _doc );
            Helper::registerPythonEventReceiverModule<PythonGameEventReceiver>( _kernel, py_module, game, STRINGIZE_STRING_LOCAL( "onFinalizeRenderResources" ), EVENT_GAME_FINALIZE_RENDER_RESOURCES, _doc );
            Helper::registerPythonEventReceiverModule<PythonGameEventReceiver>( _kernel, py_module, game, STRINGIZE_STRING_LOCAL( "onAccountFinalize" ), EVENT_GAME_ACCOUNT_FINALIZE, _doc );
            Helper::registerPythonEventReceiverModule<PythonGameEventReceiver>( _kernel, py_module, game, STRINGIZE_STRING_LOCAL( "onFinalize" ), EVENT_GAME_FINALIZE, _doc );
            Helper::registerPythonEventReceiverModule<PythonGameEventReceiver>( _kernel, py_module, game, STRINGIZE_STRING_LOCAL( "onDestroy" ), EVENT_GAME_DESTROY, _doc );

            Helper::registerPythonEventReceiverModule<PythonGameEventReceiver>( _kernel, py_module, game, STRINGIZE_STRING_LOCAL( "onFocus" ), EVENT_GAME_FOCUS, _doc );
            Helper::registerPythonEventReceiverModule<PythonGameEventReceiver>( _kernel, py_module, game, STRINGIZE_STRING_LOCAL( "onCreateDefaultAccount" ), EVENT_GAME_CREATE_DEFAULT_ACCOUNT, _doc );
            Helper::registerPythonEventReceiverModule<PythonGameEventReceiver>( _kernel, py_module, game, STRINGIZE_STRING_LOCAL( "onCreateGlobalAccount" ), EVENT_GAME_CREATE_GLOBAL_ACCOUNT, _doc );
            Helper::registerPythonEventReceiverModule<PythonGameEventReceiver>( _kernel, py_module, game, STRINGIZE_STRING_LOCAL( "onLoadAccounts" ), EVENT_GAME_LOAD_ACCOUNTS, _doc );

            Helper::registerPythonEventReceiverModule<PythonGameEventReceiver>( _kernel, py_module, game, STRINGIZE_STRING_LOCAL( "onCreateAccount" ), EVENT_GAME_CREATE_ACCOUNT, _doc );
            Helper::registerPythonEventReceiverModule<PythonGameEventReceiver>( _kernel, py_module, game, STRINGIZE_STRING_LOCAL( "onDeleteAccount" ), EVENT_GAME_DELETE_ACCOUNT, _doc );
            Helper::registerPythonEventReceiverModule<PythonGameEventReceiver>( _kernel, py_module, game, STRINGIZE_STRING_LOCAL( "onSelectAccount" ), EVENT_GAME_SELECT_ACCOUNT, _doc );
            Helper::registerPythonEventReceiverModule<PythonGameEventReceiver>( _kernel, py_module, game, STRINGIZE_STRING_LOCAL( "onUnselectAccount" ), EVENT_GAME_UNSELECT_ACCOUNT, _doc );

            Helper::registerPythonEventReceiverModule<PythonGameEventReceiver>( _kernel, py_module, game, STRINGIZE_STRING_LOCAL( "onChangeSoundVolume" ), EVENT_GAME_CHANGE_SOUND_VOLUME, _doc );
            Helper::registerPythonEventReceiverModule<PythonGameEventReceiver>( _kernel, py_module, game, STRINGIZE_STRING_LOCAL( "onCursorMode" ), EVENT_GAME_CURSOR_MODE, _doc );
            Helper::registerPythonEventReceiverModule<PythonGameEventReceiver>( _kernel, py_module, game, STRINGIZE_STRING_LOCAL( "onUserEvent" ), EVENT_GAME_USER, _doc );
            Helper::registerPythonEventReceiverModule<PythonGameEventReceiver>( _kernel, py_module, game, STRINGIZE_STRING_LOCAL( "onCloseWindow" ), EVENT_GAME_CLOSE, _doc );
            Helper::registerPythonEventReceiverModule<PythonGameEventReceiver>( _kernel, py_module, game, STRINGIZE_STRING_LOCAL( "onOverFillrate" ), EVENT_GAME_OVER_FILLRATE, _doc );
            Helper::registerPythonEventReceiverModule<PythonGameEventReceiver>( _kernel, py_module, game, STRINGIZE_STRING_LOCAL( "onFrameEnd" ), EVENT_GAME_FRAME_END, _doc );

            Helper::registerPythonEventReceiverModule<PythonGameEventReceiver>( _kernel, py_module, game, STRINGIZE_STRING_LOCAL( "onApplicationDidBecomeActive" ), EVENT_GAME_APPLICATION_DID_BECOME_ACTIVE, _doc );
            Helper::registerPythonEventReceiverModule<PythonGameEventReceiver>( _kernel, py_module, game, STRINGIZE_STRING_LOCAL( "onApplicationWillEnterForeground" ), EVENT_GAME_APPLICATION_WILL_ENTER_FOREGROUND, _doc );
            Helper::registerPythonEventReceiverModule<PythonGameEventReceiver>( _kernel, py_module, game, STRINGIZE_STRING_LOCAL( "onApplicationDidEnterBackground" ), EVENT_GAME_APPLICATION_DID_ENTER_BACKGROUD, _doc );
            Helper::registerPythonEventReceiverModule<PythonGameEventReceiver>( _kernel, py_module, game, STRINGIZE_STRING_LOCAL( "onApplicationWillResignActive" ), EVENT_GAME_APPLICATION_WILL_RESIGN_ACTIVE, _doc );
            Helper::registerPythonEventReceiverModule<PythonGameEventReceiver>( _kernel, py_module, game, STRINGIZE_STRING_LOCAL( "onApplicationWillTerminate" ), EVENT_GAME_APPLICATION_WILL_TERMINATE, _doc );

            Helper::registerPythonEventReceiverModule<PythonGameEventReceiver>( _kernel, py_module, game, STRINGIZE_STRING_LOCAL( "onAnalyticsEvent" ), EVENT_GAME_ANALYTICS_EVENT, _doc );
            Helper::registerPythonEventReceiverModule<PythonGameEventReceiver>( _kernel, py_module, game, STRINGIZE_STRING_LOCAL( "onAnalyticsScreenView" ), EVENT_GAME_ANALYTICS_SCREENVIEW, _doc );
            Helper::registerPythonEventReceiverModule<PythonGameEventReceiver>( _kernel, py_module, game, STRINGIZE_STRING_LOCAL( "onAnalyticsFlush" ), EVENT_GAME_ANALYTICS_FLUSH, _doc );
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
    bool GameScriptEmbedding::embed( pybind::kernel_interface * _kernel )
    {
        if( SCRIPT_SERVICE()
            ->bootstrapModules() == false )
        {
            LOGGER_ERROR( "invalid bootstrap modules" );

            return false;
        }

        ConstString Game_PersonalityModule = CONFIG_VALUE_CONSTSTRING( "Game", "PersonalityModule", STRINGIZE_STRING_LOCAL( "Personality" ) );

        ScriptModuleInterfacePtr module = SCRIPT_SERVICE()
            ->importModule( Game_PersonalityModule );

        if( module == nullptr )
        {
            LOGGER_ERROR("invalid import personality module '%s'"
                , Game_PersonalityModule.c_str()
            );

            return false;
        }

        Detail::registerGameEventMethods( _kernel, module, MENGINE_DOCUMENT_FACTORABLE );

        return true;
    }
    //////////////////////////////////////////////////////////////////////////
    void GameScriptEmbedding::eject( pybind::kernel_interface * _kernel )
    {
        MENGINE_UNUSED( _kernel );

        //Empty
    }
    //////////////////////////////////////////////////////////////////////////
}