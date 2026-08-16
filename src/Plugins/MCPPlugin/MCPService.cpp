#include "MCPService.h"
#include "MCPLogger.h"

#include "Commands/MCPCommandRegistry.h"
#include "Commands/Application/MCPAppStopCommand.h"
#include "Commands/Runtime/MCPLogsReadCommand.h"
#include "Commands/Runtime/MCPRuntimeControlCommand.h"
#include "Commands/Runtime/MCPFrameCaptureCommand.h"
#include "Commands/Runtime/MCPWaitForCommand.h"
#include "Commands/Runtime/MCPDiagnosticsGetCommand.h"
#include "Commands/Scene/MCPSceneSnapshotCommand.h"
#include "Commands/Scene/MCPSceneFindCommand.h"
#include "Commands/Scene/MCPSceneGetCommand.h"
#include "Commands/Scene/MCPSceneSetCommand.h"
#include "Commands/Input/MCPInputMouseCommand.h"
#include "Commands/Input/MCPInputKeyboardCommand.h"
#include "Commands/Input/MCPInputTouchCommand.h"
#include "Commands/Input/MCPInputAccelerometerCommand.h"
#include "Commands/Input/MCPInputSequenceCommand.h"
#include "Commands/Input/Steps/MCPDelayInputSequenceStep.h"
#include "Commands/Input/Steps/MCPFramesInputSequenceStep.h"
#include "Commands/Input/Steps/MCPMouseInputSequenceStep.h"
#include "Commands/Input/Steps/MCPKeyboardInputSequenceStep.h"
#include "Commands/Input/Steps/MCPTouchInputSequenceStep.h"
#include "Commands/Input/Steps/MCPAccelerometerInputSequenceStep.h"
#include "Commands/Input/Steps/MCPWaitInputSequenceStep.h"
#include "Commands/Resource/MCPResourceReloadCommand.h"
#include "Commands/Resource/MCPResourceRevertCommand.h"
#include "Commands/Wait/Conditions/MCPFramesWaitCondition.h"
#include "Commands/Wait/Conditions/MCPRuntimeWaitCondition.h"
#include "Commands/Wait/Conditions/MCPSceneWaitCondition.h"
#include "Commands/Wait/Conditions/MCPNodeWaitCondition.h"
#include "Commands/Wait/Conditions/MCPLogWaitCondition.h"

#if defined(MENGINE_BUILD_MENGINE_SCRIPT_EMBEDDED)
#   include "Commands/Wait/Conditions/MCPDebuggerWaitCondition.h"
#   include "Commands/Wait/Conditions/MCPScriptWaitCondition.h"
#   include "Commands/Script/MCPScriptReloadModuleCommand.h"
#   include "Commands/Script/MCPScriptModulesCommand.h"
#   include "Commands/Script/MCPScriptSourceCommand.h"
#   include "Commands/Script/MCPScriptInspectCommand.h"
#   include "Commands/Script/MCPScriptGetCommand.h"
#   include "Commands/Script/MCPScriptSetCommand.h"
#   include "Commands/Script/MCPScriptCallCommand.h"
#   include "Commands/Script/MCPScriptReleaseCommand.h"
#   include "Commands/Script/MCPScriptEvalCommand.h"
#   include "Commands/Script/MCPScriptExecCommand.h"
#   include "Commands/Debugger/MCPDebugSetBreakpointsCommand.h"
#   include "Commands/Debugger/MCPDebugSetExceptionPolicyCommand.h"
#   include "Commands/Debugger/MCPDebugPauseCommand.h"
#   include "Commands/Debugger/MCPDebugContinueCommand.h"
#   include "Commands/Debugger/MCPDebugStepCommand.h"
#   include "Commands/Debugger/MCPDebugStackCommand.h"
#   include "Commands/Debugger/MCPDebugScopesCommand.h"
#   include "Commands/Debugger/MCPDebugVariablesCommand.h"
#   include "Commands/Debugger/MCPDebugEvaluateCommand.h"
#   include "Commands/Debugger/MCPDebugSetVariableCommand.h"
#endif

#include "Interface/ApplicationInterface.h"
#include "Interface/CodecServiceInterface.h"
#include "Interface/FileServiceInterface.h"
#include "Interface/MemoryServiceInterface.h"
#include "Interface/LoggerServiceInterface.h"
#include "Interface/LifecycleServiceInterface.h"
#include "Interface/PlayerServiceInterface.h"
#include "Interface/PlatformServiceInterface.h"
#include "Interface/RenderSystemInterface.h"
#include "Interface/ResourceServiceInterface.h"
#include "Interface/SceneServiceInterface.h"
#include "Interface/SocketSystemInterface.h"
#include "Interface/ThreadSystemInterface.h"
#include "Interface/TimelineServiceInterface.h"
#include "Kernel/ConstStringHelper.h"
#include "Kernel/FactorableUnique.h"
#include "Kernel/JSONHelper.h"
#include "Kernel/Logger.h"
#include "Kernel/OptionHelper.h"
#include "Kernel/ThreadHelper.h"
#include "Kernel/ThreadMutexHelper.h"
#include "Kernel/ThreadMutexScope.h"

#include "Config/StdUtility.h"

#if defined(MENGINE_PLATFORM_ANDROID)
#   include "Interface/NotificationServiceInterface.h"
#   include "Kernel/NotificationHelper.h"
#   include "Kernel/ParamsHelper.h"
#endif

#if defined(MENGINE_BUILD_MENGINE_SCRIPT_EMBEDDED)
#   include "Interface/ScriptProviderServiceInterface.h"
#   include "Interface/ScriptServiceInterface.h"
#endif

//////////////////////////////////////////////////////////////////////////
SERVICE_FACTORY( MCPService, Mengine::MCPService );
//////////////////////////////////////////////////////////////////////////
namespace Mengine
{
    //////////////////////////////////////////////////////////////////////////
    namespace Detail
    {
        //////////////////////////////////////////////////////////////////////////
        static const uint32_t MCP_HANDSHAKE_REQUEST_ID = 1;
        //////////////////////////////////////////////////////////////////////////
    }
    //////////////////////////////////////////////////////////////////////////
    MCPService::MCPService()
        : m_socketConnected( false )
        , m_connected( false )
        , m_debuggerPaused( false )
        , m_revertOverlays( false )
        , m_disconnectCommands( false )
        , m_running( false )
        , m_updateGeneration( 0 )
    {
    }
    //////////////////////////////////////////////////////////////////////////
    MCPService::~MCPService()
    {
    }
    //////////////////////////////////////////////////////////////////////////
    const ServiceRequiredList & MCPService::requiredServices() const
    {
        static ServiceRequiredList required = {
            SERVICE_ID( ApplicationInterface ),
            SERVICE_ID( CodecServiceInterface ),
            SERVICE_ID( FileServiceInterface ),
            SERVICE_ID( LifecycleServiceInterface ),
            SERVICE_ID( LoggerServiceInterface ),
            SERVICE_ID( MemoryServiceInterface ),
#if defined(MENGINE_PLATFORM_ANDROID)
            SERVICE_ID( NotificationServiceInterface ),
#endif
            SERVICE_ID( PlayerServiceInterface ),
            SERVICE_ID( PlatformServiceInterface ),
            SERVICE_ID( RenderSystemInterface ),
            SERVICE_ID( ResourceServiceInterface ),
            SERVICE_ID( SceneServiceInterface ),
            SERVICE_ID( SocketSystemInterface ),
            SERVICE_ID( ThreadSystemInterface ),
            SERVICE_ID( TimelineServiceInterface )
#if defined(MENGINE_BUILD_MENGINE_SCRIPT_EMBEDDED)
            , SERVICE_ID( ScriptProviderServiceInterface )
            , SERVICE_ID( ScriptServiceInterface )
#endif
        };

        return required;
    }
    //////////////////////////////////////////////////////////////////////////
    void MCPService::_dependencyService()
    {
        SERVICE_DEPENDENCY( MCPService, ApplicationInterface );
    }
    //////////////////////////////////////////////////////////////////////////
    bool MCPService::_initializeService()
    {
        m_incomingMutex = Helper::createThreadMutex( MENGINE_DOCUMENT_FACTORABLE );
        m_outgoingMutex = Helper::createThreadMutex( MENGINE_DOCUMENT_FACTORABLE );

        if( m_incomingMutex == nullptr || m_outgoingMutex == nullptr )
        {
            return false;
        }

        if( m_runtimeContext.initialize() == false )
        {
            return false;
        }

        m_debuggerContext.initialize( this );

        if( m_waitConditionRegistry.addCondition( STRINGIZE_STRING_LOCAL( "frames" ), Helper::makeFactorableUnique<MCPFramesWaitCondition>( MENGINE_DOCUMENT_FACTORABLE ) ) == false )
        {
            return false;
        }

        if( m_waitConditionRegistry.addCondition( STRINGIZE_STRING_LOCAL( "runtime" ), Helper::makeFactorableUnique<MCPRuntimeWaitCondition>( MENGINE_DOCUMENT_FACTORABLE ) ) == false )
        {
            return false;
        }

        if( m_waitConditionRegistry.addCondition( STRINGIZE_STRING_LOCAL( "scene" ), Helper::makeFactorableUnique<MCPSceneWaitCondition>( MENGINE_DOCUMENT_FACTORABLE ) ) == false )
        {
            return false;
        }

        if( m_waitConditionRegistry.addCondition( STRINGIZE_STRING_LOCAL( "node" ), Helper::makeFactorableUnique<MCPNodeWaitCondition>( MENGINE_DOCUMENT_FACTORABLE, &m_sceneContext ) ) == false )
        {
            return false;
        }

        if( m_waitConditionRegistry.addCondition( STRINGIZE_STRING_LOCAL( "log" ), Helper::makeFactorableUnique<MCPLogWaitCondition>( MENGINE_DOCUMENT_FACTORABLE, &m_runtimeContext ) ) == false )
        {
            return false;
        }

#if defined(MENGINE_BUILD_MENGINE_SCRIPT_EMBEDDED)
        if( m_waitConditionRegistry.addCondition( STRINGIZE_STRING_LOCAL( "debugger" ), Helper::makeFactorableUnique<MCPDebuggerWaitCondition>( MENGINE_DOCUMENT_FACTORABLE, this ) ) == false )
        {
            return false;
        }

        if( m_waitConditionRegistry.addCondition( STRINGIZE_STRING_LOCAL( "script" ), Helper::makeFactorableUnique<MCPScriptWaitCondition>( MENGINE_DOCUMENT_FACTORABLE, &m_scriptContext ) ) == false )
        {
            return false;
        }
#endif

        if( m_commandRegistry.addCommand( STRINGIZE_STRING_LOCAL( "app_stop" ), Helper::makeFactorableUnique<MCPAppStopCommand>( MENGINE_DOCUMENT_FACTORABLE, this ) ) == false )
        {
            return false;
        }

        if( m_commandRegistry.addCommand( STRINGIZE_STRING_LOCAL( "logs_read" ), Helper::makeFactorableUnique<MCPLogsReadCommand>( MENGINE_DOCUMENT_FACTORABLE, &m_runtimeContext ) ) == false )
        {
            return false;
        }

        if( m_commandRegistry.addCommand( STRINGIZE_STRING_LOCAL( "runtime_control" ), Helper::makeFactorableUnique<MCPRuntimeControlCommand>( MENGINE_DOCUMENT_FACTORABLE, this, &m_runtimeContext ) ) == false )
        {
            return false;
        }

        if( m_commandRegistry.addCommand( STRINGIZE_STRING_LOCAL( "frame_capture" ), Helper::makeFactorableUnique<MCPFrameCaptureCommand>( MENGINE_DOCUMENT_FACTORABLE, this ) ) == false )
        {
            return false;
        }

        if( m_commandRegistry.addCommand( STRINGIZE_STRING_LOCAL( "wait_for" ), Helper::makeFactorableUnique<MCPWaitForCommand>( MENGINE_DOCUMENT_FACTORABLE, this, &m_waitConditionRegistry ) ) == false )
        {
            return false;
        }

        if( m_commandRegistry.addCommand( STRINGIZE_STRING_LOCAL( "diagnostics_get" ), Helper::makeFactorableUnique<MCPDiagnosticsGetCommand>( MENGINE_DOCUMENT_FACTORABLE, this, &m_runtimeContext, &m_sceneContext, &m_resourceContext, &m_scriptContext ) ) == false )
        {
            return false;
        }

        if( m_commandRegistry.addCommand( STRINGIZE_STRING_LOCAL( "scene_snapshot" ), Helper::makeFactorableUnique<MCPSceneSnapshotCommand>( MENGINE_DOCUMENT_FACTORABLE, &m_sceneContext ) ) == false )
        {
            return false;
        }

        if( m_commandRegistry.addCommand( STRINGIZE_STRING_LOCAL( "scene_find" ), Helper::makeFactorableUnique<MCPSceneFindCommand>( MENGINE_DOCUMENT_FACTORABLE, &m_sceneContext ) ) == false )
        {
            return false;
        }

        if( m_commandRegistry.addCommand( STRINGIZE_STRING_LOCAL( "scene_get" ), Helper::makeFactorableUnique<MCPSceneGetCommand>( MENGINE_DOCUMENT_FACTORABLE, &m_sceneContext ) ) == false )
        {
            return false;
        }

        if( m_commandRegistry.addCommand( STRINGIZE_STRING_LOCAL( "scene_set" ), Helper::makeFactorableUnique<MCPSceneSetCommand>( MENGINE_DOCUMENT_FACTORABLE, &m_sceneContext ) ) == false )
        {
            return false;
        }

        MCPInputMouseCommandPtr inputMouseCommand;

        if( m_commandRegistry.addCommand( STRINGIZE_STRING_LOCAL( "input_mouse" ), inputMouseCommand = Helper::makeFactorableUnique<MCPInputMouseCommand>( MENGINE_DOCUMENT_FACTORABLE, &m_inputContext ) ) == false )
        {
            return false;
        }

        MCPInputKeyboardCommandPtr inputKeyboardCommand;

        if( m_commandRegistry.addCommand( STRINGIZE_STRING_LOCAL( "input_keyboard" ), inputKeyboardCommand = Helper::makeFactorableUnique<MCPInputKeyboardCommand>( MENGINE_DOCUMENT_FACTORABLE, &m_inputContext ) ) == false )
        {
            return false;
        }

        MCPInputTouchCommandPtr inputTouchCommand;

        if( m_commandRegistry.addCommand( STRINGIZE_STRING_LOCAL( "input_touch" ), inputTouchCommand = Helper::makeFactorableUnique<MCPInputTouchCommand>( MENGINE_DOCUMENT_FACTORABLE, &m_inputContext ) ) == false )
        {
            return false;
        }

        MCPInputAccelerometerCommandPtr inputAccelerometerCommand;

        if( m_commandRegistry.addCommand( STRINGIZE_STRING_LOCAL( "input_accelerometer" ), inputAccelerometerCommand = Helper::makeFactorableUnique<MCPInputAccelerometerCommand>( MENGINE_DOCUMENT_FACTORABLE ) ) == false )
        {
            return false;
        }

        if( m_inputSequenceStepRegistry.addStep( STRINGIZE_STRING_LOCAL( "delay" ), Helper::makeFactorableUnique<MCPDelayInputSequenceStep>( MENGINE_DOCUMENT_FACTORABLE ) ) == false )
        {
            return false;
        }

        if( m_inputSequenceStepRegistry.addStep( STRINGIZE_STRING_LOCAL( "frames" ), Helper::makeFactorableUnique<MCPFramesInputSequenceStep>( MENGINE_DOCUMENT_FACTORABLE ) ) == false )
        {
            return false;
        }

        if( m_inputSequenceStepRegistry.addStep( STRINGIZE_STRING_LOCAL( "mouse" ), Helper::makeFactorableUnique<MCPMouseInputSequenceStep>( MENGINE_DOCUMENT_FACTORABLE, inputMouseCommand ) ) == false )
        {
            return false;
        }

        if( m_inputSequenceStepRegistry.addStep( STRINGIZE_STRING_LOCAL( "keyboard" ), Helper::makeFactorableUnique<MCPKeyboardInputSequenceStep>( MENGINE_DOCUMENT_FACTORABLE, inputKeyboardCommand ) ) == false )
        {
            return false;
        }

        if( m_inputSequenceStepRegistry.addStep( STRINGIZE_STRING_LOCAL( "touch" ), Helper::makeFactorableUnique<MCPTouchInputSequenceStep>( MENGINE_DOCUMENT_FACTORABLE, inputTouchCommand ) ) == false )
        {
            return false;
        }

        if( m_inputSequenceStepRegistry.addStep( STRINGIZE_STRING_LOCAL( "accelerometer" ), Helper::makeFactorableUnique<MCPAccelerometerInputSequenceStep>( MENGINE_DOCUMENT_FACTORABLE, inputAccelerometerCommand ) ) == false )
        {
            return false;
        }

        if( m_inputSequenceStepRegistry.addStep( STRINGIZE_STRING_LOCAL( "wait" ), Helper::makeFactorableUnique<MCPWaitInputSequenceStep>( MENGINE_DOCUMENT_FACTORABLE, &m_waitConditionRegistry ) ) == false )
        {
            return false;
        }

        if( m_commandRegistry.addCommand( STRINGIZE_STRING_LOCAL( "input_sequence" ), Helper::makeFactorableUnique<MCPInputSequenceCommand>( MENGINE_DOCUMENT_FACTORABLE, this, &m_inputSequenceStepRegistry ) ) == false )
        {
            return false;
        }

        if( m_commandRegistry.addCommand( STRINGIZE_STRING_LOCAL( "resource_reload" ), Helper::makeFactorableUnique<MCPResourceReloadCommand>( MENGINE_DOCUMENT_FACTORABLE, &m_resourceContext ) ) == false )
        {
            return false;
        }

        if( m_commandRegistry.addCommand( STRINGIZE_STRING_LOCAL( "resource_revert" ), Helper::makeFactorableUnique<MCPResourceRevertCommand>( MENGINE_DOCUMENT_FACTORABLE, &m_resourceContext ) ) == false )
        {
            return false;
        }

#if defined(MENGINE_BUILD_MENGINE_SCRIPT_EMBEDDED)
        if( m_commandRegistry.addCommand( STRINGIZE_STRING_LOCAL( "script_reload_module" ), Helper::makeFactorableUnique<MCPScriptReloadModuleCommand>( MENGINE_DOCUMENT_FACTORABLE, &m_scriptContext, &m_debuggerContext ) ) == false )
        {
            return false;
        }

        if( m_commandRegistry.addCommand( STRINGIZE_STRING_LOCAL( "script_modules" ), Helper::makeFactorableUnique<MCPScriptModulesCommand>( MENGINE_DOCUMENT_FACTORABLE ) ) == false )
        {
            return false;
        }

        if( m_commandRegistry.addCommand( STRINGIZE_STRING_LOCAL( "script_source" ), Helper::makeFactorableUnique<MCPScriptSourceCommand>( MENGINE_DOCUMENT_FACTORABLE, &m_scriptContext ) ) == false )
        {
            return false;
        }

        if( m_commandRegistry.addCommand( STRINGIZE_STRING_LOCAL( "script_inspect" ), Helper::makeFactorableUnique<MCPScriptInspectCommand>( MENGINE_DOCUMENT_FACTORABLE, &m_scriptContext ) ) == false )
        {
            return false;
        }

        if( m_commandRegistry.addCommand( STRINGIZE_STRING_LOCAL( "script_get" ), Helper::makeFactorableUnique<MCPScriptGetCommand>( MENGINE_DOCUMENT_FACTORABLE, &m_scriptContext ) ) == false )
        {
            return false;
        }

        if( m_commandRegistry.addCommand( STRINGIZE_STRING_LOCAL( "script_set" ), Helper::makeFactorableUnique<MCPScriptSetCommand>( MENGINE_DOCUMENT_FACTORABLE, &m_scriptContext ) ) == false )
        {
            return false;
        }

        if( m_commandRegistry.addCommand( STRINGIZE_STRING_LOCAL( "script_call" ), Helper::makeFactorableUnique<MCPScriptCallCommand>( MENGINE_DOCUMENT_FACTORABLE, &m_handlerRegistry, &m_scriptContext ) ) == false )
        {
            return false;
        }

        if( m_commandRegistry.addCommand( STRINGIZE_STRING_LOCAL( "script_release" ), Helper::makeFactorableUnique<MCPScriptReleaseCommand>( MENGINE_DOCUMENT_FACTORABLE, &m_scriptContext ) ) == false )
        {
            return false;
        }

        if( m_commandRegistry.addCommand( STRINGIZE_STRING_LOCAL( "script_eval" ), Helper::makeFactorableUnique<MCPScriptEvalCommand>( MENGINE_DOCUMENT_FACTORABLE, &m_scriptContext, &m_debuggerContext ) ) == false )
        {
            return false;
        }

        if( m_commandRegistry.addCommand( STRINGIZE_STRING_LOCAL( "script_exec" ), Helper::makeFactorableUnique<MCPScriptExecCommand>( MENGINE_DOCUMENT_FACTORABLE, &m_scriptContext, &m_debuggerContext ) ) == false )
        {
            return false;
        }

        if( m_commandRegistry.addCommand( STRINGIZE_STRING_LOCAL( "debug_set_breakpoints" ), Helper::makeFactorableUnique<MCPDebugSetBreakpointsCommand>( MENGINE_DOCUMENT_FACTORABLE, &m_debuggerContext ) ) == false )
        {
            return false;
        }

        if( m_commandRegistry.addCommand( STRINGIZE_STRING_LOCAL( "debug_set_exception_policy" ), Helper::makeFactorableUnique<MCPDebugSetExceptionPolicyCommand>( MENGINE_DOCUMENT_FACTORABLE, &m_debuggerContext ) ) == false )
        {
            return false;
        }

        if( m_commandRegistry.addCommand( STRINGIZE_STRING_LOCAL( "debug_pause" ), Helper::makeFactorableUnique<MCPDebugPauseCommand>( MENGINE_DOCUMENT_FACTORABLE, &m_debuggerContext ) ) == false )
        {
            return false;
        }

        if( m_commandRegistry.addCommand( STRINGIZE_STRING_LOCAL( "debug_continue" ), Helper::makeFactorableUnique<MCPDebugContinueCommand>( MENGINE_DOCUMENT_FACTORABLE, &m_debuggerContext ) ) == false )
        {
            return false;
        }

        if( m_commandRegistry.addCommand( STRINGIZE_STRING_LOCAL( "debug_step" ), Helper::makeFactorableUnique<MCPDebugStepCommand>( MENGINE_DOCUMENT_FACTORABLE, &m_debuggerContext ) ) == false )
        {
            return false;
        }

        if( m_commandRegistry.addCommand( STRINGIZE_STRING_LOCAL( "debug_stack" ), Helper::makeFactorableUnique<MCPDebugStackCommand>( MENGINE_DOCUMENT_FACTORABLE, &m_debuggerContext ) ) == false )
        {
            return false;
        }

        if( m_commandRegistry.addCommand( STRINGIZE_STRING_LOCAL( "debug_scopes" ), Helper::makeFactorableUnique<MCPDebugScopesCommand>( MENGINE_DOCUMENT_FACTORABLE, &m_debuggerContext, &m_scriptContext ) ) == false )
        {
            return false;
        }

        if( m_commandRegistry.addCommand( STRINGIZE_STRING_LOCAL( "debug_variables" ), Helper::makeFactorableUnique<MCPDebugVariablesCommand>( MENGINE_DOCUMENT_FACTORABLE, &m_scriptContext ) ) == false )
        {
            return false;
        }

        if( m_commandRegistry.addCommand( STRINGIZE_STRING_LOCAL( "debug_evaluate" ), Helper::makeFactorableUnique<MCPDebugEvaluateCommand>( MENGINE_DOCUMENT_FACTORABLE, &m_debuggerContext, &m_scriptContext ) ) == false )
        {
            return false;
        }

        if( m_commandRegistry.addCommand( STRINGIZE_STRING_LOCAL( "debug_set_variable" ), Helper::makeFactorableUnique<MCPDebugSetVariableCommand>( MENGINE_DOCUMENT_FACTORABLE, &m_debuggerContext, &m_scriptContext ) ) == false )
        {
            return false;
        }
#endif

        MCPLoggerPtr runtimeLogger = Helper::makeFactorableUnique<MCPLogger>( MENGINE_DOCUMENT_FACTORABLE );
        runtimeLogger->setService( this );
        runtimeLogger->setWriteHistory( true );
        runtimeLogger->setVerboseLevel( LM_VERBOSE );

        if( LOGGER_SERVICE()->registerLogger( runtimeLogger ) == false )
        {
            return false;
        }

        m_runtimeLogger = runtimeLogger;

#if defined(MENGINE_PLATFORM_ANDROID)
        NOTIFICATION_ADDOBSERVERMETHOD_THIS( NOTIFICATOR_APPLICATION_INTENT_START, &MCPService::notifyApplicationIntent_, MENGINE_DOCUMENT_FACTORABLE );
        NOTIFICATION_ADDOBSERVERMETHOD_THIS( NOTIFICATOR_APPLICATION_INTENT_NEW, &MCPService::notifyApplicationIntent_, MENGINE_DOCUMENT_FACTORABLE );
#endif

        return true;
    }
    //////////////////////////////////////////////////////////////////////////
    bool MCPService::_runService()
    {
#if defined(MENGINE_PLATFORM_WINDOWS) || defined(MENGINE_PLATFORM_MACOS) || defined(MENGINE_PLATFORM_LINUX) || defined(MENGINE_PLATFORM_IOS)
        if( HAS_OPTION( "mcp" ) == true )
        {
            const String host = GET_OPTION_VALUE( "mcp-host", "127.0.0.1" );
            const String port = GET_OPTION_VALUE( "mcp-port", "" );
            const String token = GET_OPTION_VALUE( "mcp-token", "" );
            const String mode = GET_OPTION_VALUE( "mcp-mode", "visible" );

            if( this->run( host, port, token, mode ) == false )
            {
                LOGGER_ERROR( "MCP activation failed" );

                return false;
            }
        }
#endif

        return true;
    }
    //////////////////////////////////////////////////////////////////////////
    bool MCPService::run( const String & _host, const String & _port, const String & _token, const String & _mode )
    {
        m_host = _host;
        m_port = _port;
        m_token = _token;
        m_mode = _mode;

        if( m_token.size() != 64 )
        {
            LOGGER_ERROR( "MCP requires a 256-bit hexadecimal session token" );

            m_host.clear();
            m_port.clear();
            m_token.clear();
            m_mode.clear();

            return false;
        }

        m_socketConnected = false;
        m_connected = false;
        m_debuggerPaused = false;
        m_revertOverlays = false;
        m_disconnectCommands = false;
        m_updateGeneration = 0;

        m_thread = Helper::createThreadIdentity( MENGINE_THREAD_DESCRIPTION( "MNGMCP" ), ETP_ABOVE_NORMAL, [this]( const ThreadIdentityRunnerInterfacePtr & _runner )
        {
            bool successful = this->processSocket_( _runner );

            return successful;
        }, 1, MENGINE_DOCUMENT_FACTORABLE );

        if( m_thread == nullptr )
        {
            m_host.clear();
            m_port.clear();
            m_token.clear();
            m_mode.clear();

            return false;
        }

        m_running = true;

        const Char * host = m_host.c_str();
        const Char * port = m_port.c_str();
        const Char * mode = m_mode.c_str();
        LOGGER_INFO( "mcp", "connecting MNCP v1 to '%s:%s' in '%s' mode"
            , host
            , port
            , mode
        );

        return true;
    }
    //////////////////////////////////////////////////////////////////////////
    void MCPService::stop()
    {
        if( m_running == false )
        {
            return;
        }

        m_running = false;

        if( m_thread != nullptr )
        {
            m_thread->join( true );
            m_thread = nullptr;
        }

        this->disconnectSocket_();

        m_commandRegistry.disconnect();
        m_debuggerContext.disconnect();
        m_runtimeContext.restoreRuntime();
        m_resourceContext.revertAllOverlays();
        m_scriptContext.finalize();
        m_sceneContext.finalize();

        ConstString freezeName = STRINGIZE_STRING_LOCAL( "MCP" );
        APPLICATION_SERVICE()->setUpdateFreeze( freezeName, false );

        m_incomingRequests.clear();
        m_outgoingFrames.clear();
        m_cancelledRequests.clear();
        m_incomingAssemblies.clear();

        m_revertOverlays = false;
        m_disconnectCommands = false;

        m_host.clear();
        m_port.clear();
        m_token.clear();
        m_mode.clear();
    }
    //////////////////////////////////////////////////////////////////////////
    void MCPService::_finalizeService()
    {
#if defined(MENGINE_PLATFORM_ANDROID)
        NOTIFICATION_REMOVEOBSERVER_THIS( NOTIFICATOR_APPLICATION_INTENT_START );
        NOTIFICATION_REMOVEOBSERVER_THIS( NOTIFICATOR_APPLICATION_INTENT_NEW );
#endif

        this->stop();

        m_commandRegistry.clear();
        m_waitConditionRegistry.clear();
        m_inputSequenceStepRegistry.clear();

        if( m_runtimeLogger != nullptr )
        {
            LoggerInterface * loggerInterface = m_runtimeLogger.get();
            MCPLogger * logger = static_cast<MCPLogger *>(loggerInterface);
            logger->setService( nullptr );
            LOGGER_SERVICE()->unregisterLogger( m_runtimeLogger );
            m_runtimeLogger = nullptr;
        }

        m_incomingMutex = nullptr;
        m_outgoingMutex = nullptr;
        m_debuggerContext.finalize();
        m_scriptContext.finalize();
        m_handlerRegistry.clear();
        m_sceneContext.finalize();
        m_runtimeContext.finalize();
    }
    //////////////////////////////////////////////////////////////////////////
#if defined(MENGINE_PLATFORM_ANDROID)
    void MCPService::notifyApplicationIntent_( const String & _action, const String & _data, const String & _type, uint32_t _flags, const Params & _extras )
    {
        MENGINE_UNUSED( _action );
        MENGINE_UNUSED( _data );
        MENGINE_UNUSED( _type );
        MENGINE_UNUSED( _flags );

        String host = Helper::getParam( _extras, STRINGIZE_STRING_LOCAL( "mengine.mcp.host" ), "" );
        String port = Helper::getParam( _extras, STRINGIZE_STRING_LOCAL( "mengine.mcp.port" ), "" );
        String token = Helper::getParam( _extras, STRINGIZE_STRING_LOCAL( "mengine.mcp.token" ), "" );
        String mode = Helper::getParam( _extras, STRINGIZE_STRING_LOCAL( "mengine.mcp.mode" ), "" );

        if( host.empty() == true || port.empty() == true || token.size() != 64 || mode.empty() == true )
        {
            return;
        }

        this->stop();

        if( this->run( host, port, token, mode ) == false )
        {
            LOGGER_ERROR( "MCP Android intent activation failed" );
        }
    }
#endif
    //////////////////////////////////////////////////////////////////////////
    bool MCPService::addHandler( const ConstString & _name, const MCPHandlerInterfacePtr & _handler )
    {
        bool successful = m_handlerRegistry.addHandler( _name, _handler );

        return successful;
    }
    //////////////////////////////////////////////////////////////////////////
    void MCPService::removeHandler( const MCPHandlerInterfacePtr & _handler )
    {
        m_handlerRegistry.removeHandler( _handler );
    }
    //////////////////////////////////////////////////////////////////////////
    bool MCPService::isConnected() const
    {
        bool connected = m_connected.load();

        return connected;
    }
    //////////////////////////////////////////////////////////////////////////
    bool MCPService::isDebuggerPaused() const
    {
        bool paused = m_debuggerPaused.load();

        return paused;
    }
    //////////////////////////////////////////////////////////////////////////
    const String & MCPService::getMCPMode() const
    {
        const String & mode = m_mode;

        return mode;
    }
    //////////////////////////////////////////////////////////////////////////
    uint64_t MCPService::getMCPUpdateGeneration() const
    {
        uint64_t generation = m_updateGeneration;

        return generation;
    }
    //////////////////////////////////////////////////////////////////////////
    bool MCPService::isMCPConnected() const
    {
        bool connected = m_connected.load();

        return connected;
    }
    //////////////////////////////////////////////////////////////////////////
    bool MCPService::isMCPDebuggerPaused() const
    {
        bool paused = m_debuggerPaused.load();

        return paused;
    }
    //////////////////////////////////////////////////////////////////////////
    void MCPService::setMCPDebuggerPaused( bool _paused )
    {
        m_debuggerPaused = _paused;
    }
    //////////////////////////////////////////////////////////////////////////
    void MCPService::processMCPDebuggerRequests()
    {
        this->processRequests_();
    }
    //////////////////////////////////////////////////////////////////////////
    void MCPService::sendMCPCommandResponse( uint32_t _requestId, const jpp::object & _result )
    {
        jpp::object response = jpp::make_object();
        response.set( "result", _result );
        this->queueResponse_( _requestId, response );
    }
    //////////////////////////////////////////////////////////////////////////
    void MCPService::sendMCPCommandError( uint32_t _requestId, const Char * _code, const Char * _message )
    {
        this->queueError_( _requestId, _code, _message );
    }
    //////////////////////////////////////////////////////////////////////////
    void MCPService::sendMCPCommandBinary( uint32_t _requestId, const Data & _data )
    {
        this->queueBinary_( _requestId, _data );
    }
    //////////////////////////////////////////////////////////////////////////
    void MCPService::captureRuntimeLog( const LoggerMessage & _message )
    {
        m_runtimeContext.captureLog( _message );
    }
    //////////////////////////////////////////////////////////////////////////
    void MCPService::_update()
    {
        if( m_running == false )
        {
            return;
        }

        this->processRequests_();
    }
    //////////////////////////////////////////////////////////////////////////
    void MCPService::processRequests_()
    {
        ++m_updateGeneration;

        if( m_disconnectCommands.exchange( false ) == true )
        {
            m_commandRegistry.disconnect();
            m_debuggerContext.disconnect();
        }

        if( m_revertOverlays.exchange( false ) == true )
        {
            m_resourceContext.revertAllOverlays();
        }

        Set<uint32_t> cancelled;
        for( ;; )
        {
            DequeIncomingRequests requests;
            Set<uint32_t> newlyCancelled;

            {
                MENGINE_THREAD_MUTEX_SCOPE( m_incomingMutex );
                newlyCancelled.swap( m_cancelledRequests );

                if( m_incomingRequests.empty() == false )
                {
                    requests.resize( 1 );
                    requests.front() = StdUtility::move( m_incomingRequests.front() );
                    m_incomingRequests.pop_front();
                }
            }

            cancelled.insert( newlyCancelled.begin(), newlyCancelled.end() );

            for( uint32_t requestId : newlyCancelled )
            {
                m_commandRegistry.cancel( requestId );
            }

            m_commandRegistry.update();

            if( requests.empty() == true )
            {
                break;
            }

            IncomingRequest & request = requests.front();
            if( cancelled.find( request.requestId ) != cancelled.end() )
            {
                continue;
            }

            this->processRequest_( request );
        }
    }
    //////////////////////////////////////////////////////////////////////////
    bool MCPService::processSocket_( const ThreadIdentityRunnerInterfacePtr & _runner )
    {
        if( _runner->isCancel() == true )
        {
            return false;
        }

        if( m_socketConnected.load() == false )
        {
            bool successful = this->connectSocket_();

            return successful;
        }

        if( this->flushOutgoing_() == false )
        {
            this->disconnectSocket_();

            return false;
        }

        if( this->receiveIncoming_() == false )
        {
            this->disconnectSocket_();

            return false;
        }

        return true;
    }
    //////////////////////////////////////////////////////////////////////////
    bool MCPService::connectSocket_()
    {
        SocketInterfacePtr socket = SOCKET_SYSTEM()
            ->createSocket( MENGINE_DOCUMENT_FACTORABLE );

        if( socket == nullptr )
        {
            return false;
        }

        const Char * host = m_host.c_str();
        const Char * port = m_port.c_str();
        SocketConnectInfo connectInfo = {host, port};
        if( socket->connect( connectInfo ) == false )
        {
            LOGGER_ERROR( "MCP failed to connect to '%s:%s'"
                , host
                , port
            );

            return false;
        }

        m_socket = socket;
        m_socketConnected = true;
        this->queueHandshake_();

        return true;
    }
    //////////////////////////////////////////////////////////////////////////
    bool MCPService::flushOutgoing_()
    {
        DequeOutgoingFrames frames;

        {
            MENGINE_THREAD_MUTEX_SCOPE( m_outgoingMutex );
            frames.swap( m_outgoingFrames );
        }

        for( const Data & frame : frames )
        {
            size_t offset = 0;
            size_t frameSize = frame.size();
            const uint8_t * frameData = frame.data();

            while( offset != frameSize )
            {
                size_t sent = 0;
                const uint8_t * sendData = frameData + offset;
                size_t sendSize = frameSize - offset;
                bool successful = m_socket->send( sendData, sendSize, &sent );

                if( successful == false || sent == 0 )
                {
                    return false;
                }

                offset += sent;
            }
        }

        return true;
    }
    //////////////////////////////////////////////////////////////////////////
    bool MCPService::receiveIncoming_()
    {
        if( m_socket->waitForData( 5 ) == false )
        {
            return true;
        }

        uint8_t buffer[64 * 1024];
        size_t received = 0;

        if( m_socket->receive( buffer, sizeof( buffer ), &received ) == false || received == 0 )
        {
            return false;
        }

        Vector<MCPFrame> frames;
        const Char * error = nullptr;

        if( m_decoder.append( buffer, received, &frames, &error ) == false )
        {
            LOGGER_ERROR( "MCP protocol error: %s", error );

            return false;
        }

        for( MCPFrame & frame : frames )
        {
            if( this->processFrame_( frame ) == false )
            {
                return false;
            }
        }

        return true;
    }
    //////////////////////////////////////////////////////////////////////////
    bool MCPService::processFrame_( MCPFrame & _frame )
    {
        if( _frame.type == EMCPFrameType::Response && _frame.requestId == Detail::MCP_HANDSHAKE_REQUEST_ID )
        {
            const uint8_t * payloadBuffer = _frame.payload.data();
            size_t payloadSize = _frame.payload.size();
            jpp::object response = Helper::loadJSONBuffer( payloadBuffer, payloadSize, MENGINE_DOCUMENT_FACTORABLE );
            if( response.invalid() == true || response.exist( "error", nullptr ) == true )
            {
                return false;
            }

            m_connected = true;
            LOGGER_INFO( "mcp", "MNCP session authenticated" );

            return true;
        }

        if( _frame.type == EMCPFrameType::Cancel )
        {
            MENGINE_THREAD_MUTEX_SCOPE( m_incomingMutex );
            m_cancelledRequests.emplace( _frame.requestId );
            m_incomingAssemblies.erase( _frame.requestId );

            return true;
        }

        if( _frame.type == EMCPFrameType::Request )
        {
            if( (_frame.flags & MCP_FRAME_FLAG_JSON) == 0 )
            {
                return false;
            }

            const uint8_t * payloadBuffer = _frame.payload.data();
            size_t payloadSize = _frame.payload.size();
            jpp::object request = Helper::loadJSONBuffer( payloadBuffer, payloadSize, MENGINE_DOCUMENT_FACTORABLE );
            if( request.invalid() == true )
            {
                return false;
            }

            size_t expectedSize = 0;
            jpp::object attachment;
            if( request.exist( "attachment", &attachment ) == true && attachment.is_type_object() == true )
            {
                expectedSize = attachment.get( "size", MENGINE_UINT32_C(0) );
            }

            if( expectedSize > MCP_PROTOCOL_MAX_ATTACHMENT )
            {
                return false;
            }

            if( expectedSize == 0 )
            {
                IncomingRequest incoming;
                incoming.requestId = _frame.requestId;
                incoming.payload = StdUtility::move( _frame.payload );

                MENGINE_THREAD_MUTEX_SCOPE( m_incomingMutex );
                m_incomingRequests.emplace_back();
                m_incomingRequests.back() = StdUtility::move( incoming );

                return true;
            }

            IncomingAssembly assembly;
            assembly.payload = StdUtility::move( _frame.payload );
            assembly.chunkCount = 0;
            assembly.expectedSize = expectedSize;
            m_incomingAssemblies[_frame.requestId] = StdUtility::move( assembly );

            return true;
        }

        if( _frame.type == EMCPFrameType::Binary )
        {
            MapIncomingAssemblies::iterator it = m_incomingAssemblies.find( _frame.requestId );
            if( it == m_incomingAssemblies.end() )
            {
                return false;
            }

            IncomingAssembly & assembly = it->second;
            if( assembly.chunkCount == 0 )
            {
                assembly.chunkCount = _frame.chunkCount;
            }
            else if( assembly.chunkCount != _frame.chunkCount )
            {
                return false;
            }

            if( assembly.chunks.emplace( _frame.chunkIndex ).second == false )
            {
                return false;
            }

            assembly.attachment.insert( assembly.attachment.end(), _frame.payload.begin(), _frame.payload.end() );

            if( assembly.chunks.size() != assembly.chunkCount )
            {
                return true;
            }

            if( assembly.attachment.size() != assembly.expectedSize )
            {
                return false;
            }

            IncomingRequest incoming;
            incoming.requestId = _frame.requestId;
            incoming.payload = StdUtility::move( assembly.payload );
            incoming.attachment = StdUtility::move( assembly.attachment );
            m_incomingAssemblies.erase( it );

            MENGINE_THREAD_MUTEX_SCOPE( m_incomingMutex );
            m_incomingRequests.emplace_back();
            m_incomingRequests.back() = StdUtility::move( incoming );

            return true;
        }

        return true;
    }
    //////////////////////////////////////////////////////////////////////////
    void MCPService::disconnectSocket_()
    {
        m_connected = false;
        m_socketConnected = false;
        m_debuggerPaused = false;
        m_revertOverlays = true;
        m_disconnectCommands = true;

        if( m_socket != nullptr )
        {
            m_socket->disconnect();
            m_socket = nullptr;
        }

        m_decoder.reset();
        m_incomingAssemblies.clear();
    }
    //////////////////////////////////////////////////////////////////////////
    void MCPService::queueHandshake_()
    {
        jpp::array capabilities = jpp::make_array();
        capabilities.push_back( "scene" );
        capabilities.push_back( "virtual_input" );
        capabilities.push_back( "game_extensions" );
        capabilities.push_back( "diagnostics" );
        capabilities.push_back( "frame_capture" );
        capabilities.push_back( "explicit_reload" );
#if defined(MENGINE_BUILD_MENGINE_SCRIPT_EMBEDDED)
        capabilities.push_back( "script" );
        capabilities.push_back( "debugger" );
#endif

        jpp::object params = jpp::make_object();
        params.set( "token", m_token );
        params.set( "capabilities", capabilities );
        params.set( "mode", m_mode );

        jpp::object request = jpp::make_object();
        request.set( "method", "handshake" );
        request.set( "params", params );

        Data data;
        Helper::writeJSONDataCompact( request, &data );
        const uint8_t * dataBuffer = data.data();
        size_t dataSize = data.size();
        Data frame = Helper::makeMCPJSONFrame( EMCPFrameType::Request, Detail::MCP_HANDSHAKE_REQUEST_ID, dataBuffer, dataSize );
        this->queueFrame_( frame );
    }
    //////////////////////////////////////////////////////////////////////////
    void MCPService::queueResponse_( uint32_t _requestId, const jpp::object & _response )
    {
        Data data;
        Helper::writeJSONDataCompact( _response, &data );
        const uint8_t * dataBuffer = data.data();
        size_t dataSize = data.size();
        Data frame = Helper::makeMCPJSONFrame( EMCPFrameType::Response, _requestId, dataBuffer, dataSize );
        this->queueFrame_( frame );
    }
    //////////////////////////////////////////////////////////////////////////
    void MCPService::queueError_( uint32_t _requestId, const Char * _code, const Char * _message )
    {
        jpp::object error = jpp::make_object();
        error.set( "code", _code );
        error.set( "message", _message );

        jpp::object response = jpp::make_object();
        response.set( "error", error );
        this->queueResponse_( _requestId, response );
    }
    //////////////////////////////////////////////////////////////////////////
    void MCPService::queueFrame_( Data & _frame )
    {
        MENGINE_THREAD_MUTEX_SCOPE( m_outgoingMutex );
        m_outgoingFrames.emplace_back();
        m_outgoingFrames.back() = StdUtility::move( _frame );
    }
    //////////////////////////////////////////////////////////////////////////
    void MCPService::processRequest_( IncomingRequest & _request )
    {
        const uint8_t * payloadBuffer = _request.payload.data();
        size_t payloadSize = _request.payload.size();
        jpp::object request = Helper::loadJSONBuffer( payloadBuffer, payloadSize, MENGINE_DOCUMENT_FACTORABLE );
        if( request.invalid() == true )
        {
            this->queueError_( _request.requestId, "execution_error", "invalid JSON request" );

            return;
        }

        const Char * method = request.get( "method", "" );
        ConstString methodName = Helper::stringizeString( method );
        jpp::object params;
        if( request.exist( "params", &params ) == false || params.is_type_object() == false )
        {
            params = jpp::make_object();
        }

        MCPCommandInterfacePtr command = m_commandRegistry.findCommand( methodName );

        if( command == nullptr )
        {
            this->queueError_( _request.requestId, "unsupported", "runtime method is not available in this build" );

            return;
        }

        bool debuggerPaused = m_debuggerPaused.load();
        bool availableWhilePaused = command->availableWhilePaused( params );

        if( debuggerPaused == true && availableWhilePaused == false )
        {
            this->queueError_( _request.requestId, "runtime_paused", "runtime is paused in the Python debugger" );

            return;
        }

        MCPCommandRequest commandRequest = {_request.requestId, params, _request.attachment};
        MCPCommandResponse commandResponse;
        EMCPCommandStatus status = command->execute( commandRequest, &commandResponse );

        switch( status )
        {
        case EMCPCommandStatus::SUCCESS:
            {
                this->sendMCPCommandResponse( _request.requestId, commandResponse.result );

                return;
            }break;
        case EMCPCommandStatus::FAILURE:
            {
                const Char * errorCode = "execution_error";

                if( commandResponse.errorCode.empty() == false )
                {
                    errorCode = commandResponse.errorCode.c_str();
                }

                const Char * errorMessage = commandResponse.errorMessage.c_str();
                this->queueError_( _request.requestId, errorCode, errorMessage );

                return;
            }break;
        case EMCPCommandStatus::DEFERRED:
            {
                return;
            }break;
        }
    }
    //////////////////////////////////////////////////////////////////////////
    void MCPService::queueBinary_( uint32_t _requestId, const Data & _data )
    {
        static const size_t MCP_BINARY_CHUNK_SIZE = 1024 * 1024;

        uint32_t chunkCount = (uint32_t)((_data.size() + MCP_BINARY_CHUNK_SIZE - 1) / MCP_BINARY_CHUNK_SIZE);
        for( uint32_t chunkIndex = 0; chunkIndex != chunkCount; ++chunkIndex )
        {
            size_t offset = (size_t)chunkIndex * MCP_BINARY_CHUNK_SIZE;
            size_t size = _data.size() - offset;
            if( size > MCP_BINARY_CHUNK_SIZE )
            {
                size = MCP_BINARY_CHUNK_SIZE;
            }

            uint8_t flags = chunkIndex + 1 == chunkCount ? MCP_FRAME_FLAG_FINAL : MCP_FRAME_FLAG_NONE;
            const uint8_t * data = _data.data();
            const uint8_t * chunk = data + offset;
            Data frame = Helper::makeMCPFrame( EMCPFrameType::Binary, flags, _requestId, chunkIndex, chunkCount, chunk, size );
            this->queueFrame_( frame );
        }
    }
    //////////////////////////////////////////////////////////////////////////
}
