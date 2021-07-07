#include "GameScriptEmbedding.h"

#include "Interface/ScriptServiceInterface.h"
#include "Interface/GameServiceInterface.h"
#include "Interface/ConfigServiceInterface.h"
#include "Interface/OptionsServiceInterface.h"
#include "Interface/ScriptModuleInterface.h"

#include "PythonGameEventReceiver.h"

#include "Kernel/AssertionMemoryPanic.h"
#include "Kernel/Logger.h"
#include "Kernel/FilePathHelper.h"

namespace Mengine
{
    //////////////////////////////////////////////////////////////////////////
    namespace Detail
    {
        //////////////////////////////////////////////////////////////////////////
        static void registerGameEventMethods( pybind::kernel_interface * _kernel, const ScriptModuleInterfacePtr & _module, const DocumentPtr & _doc )
        {
            const pybind::module & base_module = _module->getModule();

            GameServiceInterface * game = GAME_SERVICE();

            Helper::registerPythonEventReceiverModule<PythonGameEventReceiver>( _kernel, base_module, game, "onFullscreen", EVENT_GAME_FULLSCREEN, _doc );
            Helper::registerPythonEventReceiverModule<PythonGameEventReceiver>( _kernel, base_module, game, "onFixedContentResolution", EVENT_GAME_FIXED_CONTENT_RESOLUTION, _doc );
            Helper::registerPythonEventReceiverModule<PythonGameEventReceiver>( _kernel, base_module, game, "onFixedDisplayResolution", EVENT_GAME_FIXED_DISPLAY_RESOLUTION, _doc );
            Helper::registerPythonEventReceiverModule<PythonGameEventReceiver>( _kernel, base_module, game, "onRenderViewport", EVENT_GAME_RENDER_VIEWPORT, _doc );
            Helper::registerPythonEventReceiverModule<PythonGameEventReceiver>( _kernel, base_module, game, "onGameViewport", EVENT_GAME_VIEWPORT, _doc );

            Helper::registerPythonEventReceiverModule<PythonGameEventReceiver>( _kernel, base_module, game, "onHandleKeyEvent", EVENT_GAME_KEY, _doc );
            Helper::registerPythonEventReceiverModule<PythonGameEventReceiver>( _kernel, base_module, game, "onHandleTextEvent", EVENT_GAME_TEXT, _doc );
            Helper::registerPythonEventReceiverModule<PythonGameEventReceiver>( _kernel, base_module, game, "onHandleMouseButtonEvent", EVENT_GAME_MOUSE_BUTTON, _doc );
            Helper::registerPythonEventReceiverModule<PythonGameEventReceiver>( _kernel, base_module, game, "onHandleMouseButtonEventBegin", EVENT_GAME_MOUSE_BUTTON_BEGIN, _doc );
            Helper::registerPythonEventReceiverModule<PythonGameEventReceiver>( _kernel, base_module, game, "onHandleMouseButtonEventEnd", EVENT_GAME_MOUSE_BUTTON_END, _doc );
            Helper::registerPythonEventReceiverModule<PythonGameEventReceiver>( _kernel, base_module, game, "onHandleMouseMove", EVENT_GAME_MOUSE_MOVE, _doc );
            Helper::registerPythonEventReceiverModule<PythonGameEventReceiver>( _kernel, base_module, game, "onHandleMouseWheel", EVENT_GAME_MOUSE_WHEEL, _doc );

            Helper::registerPythonEventReceiverModule<PythonGameEventReceiver>( _kernel, base_module, game, "onAppMouseEnter", EVENT_GAME_APP_MOUSE_ENTER, _doc );
            Helper::registerPythonEventReceiverModule<PythonGameEventReceiver>( _kernel, base_module, game, "onAppMouseLeave", EVENT_GAME_APP_MOUSE_LEAVE, _doc );
            Helper::registerPythonEventReceiverModule<PythonGameEventReceiver>( _kernel, base_module, game, "onTimeFactor", EVENT_GAME_ON_TIME_FACTOR, _doc );

            Helper::registerPythonEventReceiverModule<PythonGameEventReceiver>( _kernel, base_module, game, "onPreparation", EVENT_GAME_PREPARATION, _doc );
            Helper::registerPythonEventReceiverModule<PythonGameEventReceiver>( _kernel, base_module, game, "onRun", EVENT_GAME_RUN, _doc );
            Helper::registerPythonEventReceiverModule<PythonGameEventReceiver>( _kernel, base_module, game, "onStop", EVENT_GAME_STOP, _doc );
            Helper::registerPythonEventReceiverModule<PythonGameEventReceiver>( _kernel, base_module, game, "onInitialize", EVENT_GAME_INITIALIZE, _doc );
            Helper::registerPythonEventReceiverModule<PythonGameEventReceiver>( _kernel, base_module, game, "onInitializeRenderResources", EVENT_GAME_INITIALIZE_RENDER_RESOURCES, _doc );
            Helper::registerPythonEventReceiverModule<PythonGameEventReceiver>( _kernel, base_module, game, "onAccountFinalize", EVENT_GAME_ACCOUNT_FINALIZE, _doc );
            Helper::registerPythonEventReceiverModule<PythonGameEventReceiver>( _kernel, base_module, game, "onFinalize", EVENT_GAME_FINALIZE, _doc );
            Helper::registerPythonEventReceiverModule<PythonGameEventReceiver>( _kernel, base_module, game, "onDestroy", EVENT_GAME_DESTROY, _doc );

            Helper::registerPythonEventReceiverModule<PythonGameEventReceiver>( _kernel, base_module, game, "onFocus", EVENT_GAME_FOCUS, _doc );
            Helper::registerPythonEventReceiverModule<PythonGameEventReceiver>( _kernel, base_module, game, "onCreateDefaultAccount", EVENT_GAME_CREATE_DEFAULT_ACCOUNT, _doc );
            Helper::registerPythonEventReceiverModule<PythonGameEventReceiver>( _kernel, base_module, game, "onCreateGlobalAccount", EVENT_GAME_CREATE_GLOBAL_ACCOUNT, _doc );
            Helper::registerPythonEventReceiverModule<PythonGameEventReceiver>( _kernel, base_module, game, "onLoadAccounts", EVENT_GAME_LOAD_ACCOUNTS, _doc );

            Helper::registerPythonEventReceiverModule<PythonGameEventReceiver>( _kernel, base_module, game, "onCreateAccount", EVENT_GAME_CREATE_ACCOUNT, _doc );
            Helper::registerPythonEventReceiverModule<PythonGameEventReceiver>( _kernel, base_module, game, "onDeleteAccount", EVENT_GAME_DELETE_ACCOUNT, _doc );
            Helper::registerPythonEventReceiverModule<PythonGameEventReceiver>( _kernel, base_module, game, "onSelectAccount", EVENT_GAME_SELECT_ACCOUNT, _doc );
            Helper::registerPythonEventReceiverModule<PythonGameEventReceiver>( _kernel, base_module, game, "onUnselectAccount", EVENT_GAME_UNSELECT_ACCOUNT, _doc );

            Helper::registerPythonEventReceiverModule<PythonGameEventReceiver>( _kernel, base_module, game, "onChangeSoundVolume", EVENT_GAME_CHANGE_SOUND_VOLUME, _doc );
            Helper::registerPythonEventReceiverModule<PythonGameEventReceiver>( _kernel, base_module, game, "onCursorMode", EVENT_GAME_CURSOR_MODE, _doc );
            Helper::registerPythonEventReceiverModule<PythonGameEventReceiver>( _kernel, base_module, game, "onUserEvent", EVENT_GAME_USER, _doc );
            Helper::registerPythonEventReceiverModule<PythonGameEventReceiver>( _kernel, base_module, game, "onCloseWindow", EVENT_GAME_CLOSE, _doc );
            Helper::registerPythonEventReceiverModule<PythonGameEventReceiver>( _kernel, base_module, game, "onOverFillrate", EVENT_GAME_OVER_FILLRATE, _doc );
            Helper::registerPythonEventReceiverModule<PythonGameEventReceiver>( _kernel, base_module, game, "onFrameEnd", EVENT_GAME_FRAME_END, _doc );
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

        ConstString personality = CONFIG_VALUE( "Game", "PersonalityModule", STRINGIZE_FILEPATH_LOCAL( "Personality" ) );

        ScriptModuleInterfacePtr module = SCRIPT_SERVICE()
            ->importModule( personality );

        MENGINE_ASSERTION_MEMORY_PANIC( module, "invalid import module '%s'"
            , personality.c_str()
        );

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