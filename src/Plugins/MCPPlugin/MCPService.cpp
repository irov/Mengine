#include "MCPService.h"

#include "Interface/ApplicationInterface.h"
#include "Interface/CodecServiceInterface.h"
#include "Interface/FileServiceInterface.h"
#include "Interface/LifecycleServiceInterface.h"
#include "Interface/LoggerServiceInterface.h"
#include "Interface/MemoryServiceInterface.h"
#include "Interface/PlatformServiceInterface.h"
#include "Interface/PlayerServiceInterface.h"
#include "Interface/RenderSystemInterface.h"
#include "Interface/ResourceServiceInterface.h"
#include "Interface/SceneServiceInterface.h"
#include "Interface/SocketSystemInterface.h"
#include "Interface/ThreadSystemInterface.h"
#include "Interface/TimelineServiceInterface.h"

#if defined(MENGINE_BUILD_MENGINE_SCRIPT_EMBEDDED)
#   include "Interface/ScriptProviderServiceInterface.h"
#   include "Interface/ScriptServiceInterface.h"
#endif

#if defined(MENGINE_PLATFORM_ANDROID)
#   include "Interface/NotificationServiceInterface.h"
#endif

#include "Commands/Application/MCPAppStopCommand.h"
#include "Commands/Input/MCPInputAccelerometerCommand.h"
#include "Commands/Input/MCPInputKeyboardCommand.h"
#include "Commands/Input/MCPInputMouseCommand.h"
#include "Commands/Input/MCPInputSequenceCommand.h"
#include "Commands/Input/MCPInputTouchCommand.h"
#include "Commands/Input/Steps/MCPAccelerometerInputSequenceStep.h"
#include "Commands/Input/Steps/MCPDelayInputSequenceStep.h"
#include "Commands/Input/Steps/MCPFramesInputSequenceStep.h"
#include "Commands/Input/Steps/MCPKeyboardInputSequenceStep.h"
#include "Commands/Input/Steps/MCPMouseInputSequenceStep.h"
#include "Commands/Input/Steps/MCPTouchInputSequenceStep.h"
#include "Commands/Input/Steps/MCPWaitInputSequenceStep.h"
#include "Commands/MCPCommandRegistry.h"
#include "Commands/Resource/MCPResourceReloadCommand.h"
#include "Commands/Resource/MCPResourceRevertCommand.h"
#include "Commands/Runtime/MCPDiagnosticsGetCommand.h"
#include "Commands/Runtime/MCPFrameCaptureCommand.h"
#include "Commands/Runtime/MCPLogsReadCommand.h"
#include "Commands/Runtime/MCPRuntimeControlCommand.h"
#include "Commands/Runtime/MCPWaitForCommand.h"
#include "Commands/Scene/MCPSceneFindCommand.h"
#include "Commands/Scene/MCPSceneGetCommand.h"
#include "Commands/Scene/MCPSceneSetCommand.h"
#include "Commands/Scene/MCPSceneSnapshotCommand.h"
#include "Commands/Wait/Conditions/MCPFramesWaitCondition.h"
#include "Commands/Wait/Conditions/MCPLogWaitCondition.h"
#include "Commands/Wait/Conditions/MCPNodeWaitCondition.h"
#include "Commands/Wait/Conditions/MCPRuntimeWaitCondition.h"
#include "Commands/Wait/Conditions/MCPSceneWaitCondition.h"
#include "MCPLogger.h"

#if defined(MENGINE_BUILD_MENGINE_SCRIPT_EMBEDDED)
#   include "Commands/Debugger/MCPDebugContinueCommand.h"
#   include "Commands/Debugger/MCPDebugEvaluateCommand.h"
#   include "Commands/Debugger/MCPDebugPauseCommand.h"
#   include "Commands/Debugger/MCPDebugScopesCommand.h"
#   include "Commands/Debugger/MCPDebugSetBreakpointsCommand.h"
#   include "Commands/Debugger/MCPDebugSetExceptionPolicyCommand.h"
#   include "Commands/Debugger/MCPDebugSetVariableCommand.h"
#   include "Commands/Debugger/MCPDebugStackCommand.h"
#   include "Commands/Debugger/MCPDebugStepCommand.h"
#   include "Commands/Debugger/MCPDebugVariablesCommand.h"
#   include "Commands/Script/MCPScriptCallCommand.h"
#   include "Commands/Script/MCPScriptEvalCommand.h"
#   include "Commands/Script/MCPScriptExecCommand.h"
#   include "Commands/Script/MCPScriptGetCommand.h"
#   include "Commands/Script/MCPScriptHandlersCommand.h"
#   include "Commands/Script/MCPScriptInspectCommand.h"
#   include "Commands/Script/MCPScriptModulesCommand.h"
#   include "Commands/Script/MCPScriptReleaseCommand.h"
#   include "Commands/Script/MCPScriptReloadModuleCommand.h"
#   include "Commands/Script/MCPScriptSetCommand.h"
#   include "Commands/Script/MCPScriptSourceCommand.h"
#   include "Commands/Wait/Conditions/MCPDebuggerWaitCondition.h"
#   include "Commands/Wait/Conditions/MCPScriptWaitCondition.h"
#endif

#include "Kernel/Assertion.h"
#include "Kernel/AssertionMemoryPanic.h"
#include "Kernel/BuildMode.h"
#include "Kernel/ConstStringHelper.h"
#include "Kernel/FactorableUnique.h"
#include "Kernel/JSONHelper.h"
#include "Kernel/Logger.h"
#include "Kernel/NotificationHelper.h"
#include "Kernel/OptionHelper.h"
#include "Kernel/ThreadHelper.h"
#include "Kernel/ThreadMutexHelper.h"
#include "Kernel/ThreadMutexScope.h"
#include "Kernel/TimestampHelper.h"
#include "Kernel/VectorConstString.h"

#if defined(MENGINE_PLATFORM_ANDROID)
#   include "Kernel/ParamsHelper.h"
#endif

#include "Config/StdString.h"
#include "Config/StdUtility.h"

#if defined(MENGINE_PLATFORM_ANDROID)
#   include <sys/system_properties.h>
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
    const Timestamp MCP_CONNECT_RETRY_TIMEOUT = 30000;
    const uint32_t MCP_CONNECT_RETRY_DELAY = 100;
    const uint32_t MCP_ACCEPT_POLL_DELAY = 50;
    const size_t MCP_BINARY_CHUNK_SIZE = 1024 * 1024;
    //////////////////////////////////////////////////////////////////////////
    MCPService::MCPService()
        : m_outgoingOffset( 0 )
        , m_socketConnected( false )
        , m_connected( false )
        , m_debuggerPaused( false )
        , m_sessionEnded( false )
        , m_running( false )
        , m_listen( false )
        , m_operationsUpdating( false )
        , m_connectDeadline( 0 )
        , m_updateFrame( 0 )
        , m_renderFrame( 0 )
    {
    }
    //////////////////////////////////////////////////////////////////////////
    MCPService::~MCPService()
    {
    }
    //////////////////////////////////////////////////////////////////////////
    const ServiceRequiredList & MCPService::requiredServices() const
    {
        static const ServiceRequiredList required = {
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

        MCPFramesWaitConditionPtr framesWaitCondition = Helper::makeFactorableUnique<MCPFramesWaitCondition>( MENGINE_DOCUMENT_FACTORABLE );

        if( m_waitConditionRegistry.addCondition( STRINGIZE_STRING_LOCAL( "frames" ), framesWaitCondition ) == false )
        {
            return false;
        }

        MCPRuntimeWaitConditionPtr runtimeWaitCondition = Helper::makeFactorableUnique<MCPRuntimeWaitCondition>( MENGINE_DOCUMENT_FACTORABLE );

        if( m_waitConditionRegistry.addCondition( STRINGIZE_STRING_LOCAL( "runtime" ), runtimeWaitCondition ) == false )
        {
            return false;
        }

        MCPSceneWaitConditionPtr sceneWaitCondition = Helper::makeFactorableUnique<MCPSceneWaitCondition>( MENGINE_DOCUMENT_FACTORABLE );

        if( m_waitConditionRegistry.addCondition( STRINGIZE_STRING_LOCAL( "scene" ), sceneWaitCondition ) == false )
        {
            return false;
        }

        MCPNodeWaitConditionPtr nodeWaitCondition = Helper::makeFactorableUnique<MCPNodeWaitCondition>( MENGINE_DOCUMENT_FACTORABLE, &m_sceneContext );

        if( m_waitConditionRegistry.addCondition( STRINGIZE_STRING_LOCAL( "node" ), nodeWaitCondition ) == false )
        {
            return false;
        }

        MCPLogWaitConditionPtr logWaitCondition = Helper::makeFactorableUnique<MCPLogWaitCondition>( MENGINE_DOCUMENT_FACTORABLE, &m_runtimeContext );

        if( m_waitConditionRegistry.addCondition( STRINGIZE_STRING_LOCAL( "log" ), logWaitCondition ) == false )
        {
            return false;
        }

#if defined(MENGINE_BUILD_MENGINE_SCRIPT_EMBEDDED)
        MCPScriptWaitConditionPtr scriptWaitCondition = Helper::makeFactorableUnique<MCPScriptWaitCondition>( MENGINE_DOCUMENT_FACTORABLE, &m_scriptContext );

        if( m_waitConditionRegistry.addCondition( STRINGIZE_STRING_LOCAL( "script" ), scriptWaitCondition ) == false )
        {
            return false;
        }
#endif

        MCPAppStopCommandPtr appStopCommand = Helper::makeFactorableUnique<MCPAppStopCommand>( MENGINE_DOCUMENT_FACTORABLE, this );

        if( m_commandRegistry.addCommand( STRINGIZE_STRING_LOCAL( "app_stop" ), appStopCommand ) == false )
        {
            return false;
        }

        MCPLogsReadCommandPtr logsReadCommand = Helper::makeFactorableUnique<MCPLogsReadCommand>( MENGINE_DOCUMENT_FACTORABLE, &m_runtimeContext );

        if( m_commandRegistry.addCommand( STRINGIZE_STRING_LOCAL( "logs_read" ), logsReadCommand ) == false )
        {
            return false;
        }

        MCPRuntimeControlCommandPtr runtimeControlCommand = Helper::makeFactorableUnique<MCPRuntimeControlCommand>( MENGINE_DOCUMENT_FACTORABLE, &m_runtimeContext );

        if( m_commandRegistry.addCommand( STRINGIZE_STRING_LOCAL( "runtime_control" ), runtimeControlCommand ) == false )
        {
            return false;
        }

        MCPFrameCaptureCommandPtr frameCaptureCommand = Helper::makeFactorableUnique<MCPFrameCaptureCommand>( MENGINE_DOCUMENT_FACTORABLE, this );

        if( m_commandRegistry.addCommand( STRINGIZE_STRING_LOCAL( "frame_capture" ), frameCaptureCommand ) == false )
        {
            return false;
        }

        MCPWaitForCommandPtr waitForCommand = Helper::makeFactorableUnique<MCPWaitForCommand>( MENGINE_DOCUMENT_FACTORABLE, this, &m_waitConditionRegistry );

        if( m_commandRegistry.addCommand( STRINGIZE_STRING_LOCAL( "wait_for" ), waitForCommand ) == false )
        {
            return false;
        }

        MCPDiagnosticsGetCommandPtr diagnosticsGetCommand = Helper::makeFactorableUnique<MCPDiagnosticsGetCommand>( MENGINE_DOCUMENT_FACTORABLE, this, &m_runtimeContext, &m_sceneContext, &m_resourceContext, &m_scriptContext );

        if( m_commandRegistry.addCommand( STRINGIZE_STRING_LOCAL( "diagnostics_get" ), diagnosticsGetCommand ) == false )
        {
            return false;
        }

        MCPSceneSnapshotCommandPtr sceneSnapshotCommand = Helper::makeFactorableUnique<MCPSceneSnapshotCommand>( MENGINE_DOCUMENT_FACTORABLE, &m_sceneContext );

        if( m_commandRegistry.addCommand( STRINGIZE_STRING_LOCAL( "scene_snapshot" ), sceneSnapshotCommand ) == false )
        {
            return false;
        }

        MCPSceneFindCommandPtr sceneFindCommand = Helper::makeFactorableUnique<MCPSceneFindCommand>( MENGINE_DOCUMENT_FACTORABLE, &m_sceneContext );

        if( m_commandRegistry.addCommand( STRINGIZE_STRING_LOCAL( "scene_find" ), sceneFindCommand ) == false )
        {
            return false;
        }

        MCPSceneGetCommandPtr sceneGetCommand = Helper::makeFactorableUnique<MCPSceneGetCommand>( MENGINE_DOCUMENT_FACTORABLE, &m_sceneContext );

        if( m_commandRegistry.addCommand( STRINGIZE_STRING_LOCAL( "scene_get" ), sceneGetCommand ) == false )
        {
            return false;
        }

        MCPSceneSetCommandPtr sceneSetCommand = Helper::makeFactorableUnique<MCPSceneSetCommand>( MENGINE_DOCUMENT_FACTORABLE, &m_sceneContext );

        if( m_commandRegistry.addCommand( STRINGIZE_STRING_LOCAL( "scene_set" ), sceneSetCommand ) == false )
        {
            return false;
        }

        MCPInputMouseCommandPtr inputMouseCommand = Helper::makeFactorableUnique<MCPInputMouseCommand>( MENGINE_DOCUMENT_FACTORABLE, &m_inputContext, &m_sceneContext );

        if( m_commandRegistry.addCommand( STRINGIZE_STRING_LOCAL( "input_mouse" ), inputMouseCommand ) == false )
        {
            return false;
        }

        MCPInputKeyboardCommandPtr inputKeyboardCommand = Helper::makeFactorableUnique<MCPInputKeyboardCommand>( MENGINE_DOCUMENT_FACTORABLE, &m_inputContext );

        if( m_commandRegistry.addCommand( STRINGIZE_STRING_LOCAL( "input_keyboard" ), inputKeyboardCommand ) == false )
        {
            return false;
        }

        MCPInputTouchCommandPtr inputTouchCommand = Helper::makeFactorableUnique<MCPInputTouchCommand>( MENGINE_DOCUMENT_FACTORABLE, &m_inputContext, &m_sceneContext );

        if( m_commandRegistry.addCommand( STRINGIZE_STRING_LOCAL( "input_touch" ), inputTouchCommand ) == false )
        {
            return false;
        }

        MCPInputAccelerometerCommandPtr inputAccelerometerCommand = Helper::makeFactorableUnique<MCPInputAccelerometerCommand>( MENGINE_DOCUMENT_FACTORABLE );

        if( m_commandRegistry.addCommand( STRINGIZE_STRING_LOCAL( "input_accelerometer" ), inputAccelerometerCommand ) == false )
        {
            return false;
        }

        MCPDelayInputSequenceStepPtr delayInputSequenceStep = Helper::makeFactorableUnique<MCPDelayInputSequenceStep>( MENGINE_DOCUMENT_FACTORABLE );

        if( m_inputSequenceStepRegistry.addStep( STRINGIZE_STRING_LOCAL( "delay" ), delayInputSequenceStep ) == false )
        {
            return false;
        }

        MCPFramesInputSequenceStepPtr framesInputSequenceStep = Helper::makeFactorableUnique<MCPFramesInputSequenceStep>( MENGINE_DOCUMENT_FACTORABLE );

        if( m_inputSequenceStepRegistry.addStep( STRINGIZE_STRING_LOCAL( "frames" ), framesInputSequenceStep ) == false )
        {
            return false;
        }

        MCPMouseInputSequenceStepPtr mouseInputSequenceStep = Helper::makeFactorableUnique<MCPMouseInputSequenceStep>( MENGINE_DOCUMENT_FACTORABLE, inputMouseCommand );

        if( m_inputSequenceStepRegistry.addStep( STRINGIZE_STRING_LOCAL( "mouse" ), mouseInputSequenceStep ) == false )
        {
            return false;
        }

        MCPKeyboardInputSequenceStepPtr keyboardInputSequenceStep = Helper::makeFactorableUnique<MCPKeyboardInputSequenceStep>( MENGINE_DOCUMENT_FACTORABLE, inputKeyboardCommand );

        if( m_inputSequenceStepRegistry.addStep( STRINGIZE_STRING_LOCAL( "keyboard" ), keyboardInputSequenceStep ) == false )
        {
            return false;
        }

        MCPTouchInputSequenceStepPtr touchInputSequenceStep = Helper::makeFactorableUnique<MCPTouchInputSequenceStep>( MENGINE_DOCUMENT_FACTORABLE, inputTouchCommand );

        if( m_inputSequenceStepRegistry.addStep( STRINGIZE_STRING_LOCAL( "touch" ), touchInputSequenceStep ) == false )
        {
            return false;
        }

        MCPAccelerometerInputSequenceStepPtr accelerometerInputSequenceStep = Helper::makeFactorableUnique<MCPAccelerometerInputSequenceStep>( MENGINE_DOCUMENT_FACTORABLE, inputAccelerometerCommand );

        if( m_inputSequenceStepRegistry.addStep( STRINGIZE_STRING_LOCAL( "accelerometer" ), accelerometerInputSequenceStep ) == false )
        {
            return false;
        }

        MCPWaitInputSequenceStepPtr waitInputSequenceStep = Helper::makeFactorableUnique<MCPWaitInputSequenceStep>( MENGINE_DOCUMENT_FACTORABLE, &m_waitConditionRegistry );

        if( m_inputSequenceStepRegistry.addStep( STRINGIZE_STRING_LOCAL( "wait" ), waitInputSequenceStep ) == false )
        {
            return false;
        }

        MCPInputSequenceCommandPtr inputSequenceCommand = Helper::makeFactorableUnique<MCPInputSequenceCommand>( MENGINE_DOCUMENT_FACTORABLE, this, &m_inputSequenceStepRegistry );

        if( m_commandRegistry.addCommand( STRINGIZE_STRING_LOCAL( "input_sequence" ), inputSequenceCommand ) == false )
        {
            return false;
        }

        MCPResourceReloadCommandPtr resourceReloadCommand = Helper::makeFactorableUnique<MCPResourceReloadCommand>( MENGINE_DOCUMENT_FACTORABLE, &m_resourceContext );

        if( m_commandRegistry.addCommand( STRINGIZE_STRING_LOCAL( "resource_reload" ), resourceReloadCommand ) == false )
        {
            return false;
        }

        MCPResourceRevertCommandPtr resourceRevertCommand = Helper::makeFactorableUnique<MCPResourceRevertCommand>( MENGINE_DOCUMENT_FACTORABLE, &m_resourceContext );

        if( m_commandRegistry.addCommand( STRINGIZE_STRING_LOCAL( "resource_revert" ), resourceRevertCommand ) == false )
        {
            return false;
        }

#if defined(MENGINE_BUILD_MENGINE_SCRIPT_EMBEDDED)
        MCPScriptReloadModuleCommandPtr scriptReloadModuleCommand = Helper::makeFactorableUnique<MCPScriptReloadModuleCommand>( MENGINE_DOCUMENT_FACTORABLE, &m_scriptContext, &m_debuggerContext );

        if( m_commandRegistry.addCommand( STRINGIZE_STRING_LOCAL( "script_reload_module" ), scriptReloadModuleCommand ) == false )
        {
            return false;
        }

        MCPScriptModulesCommandPtr scriptModulesCommand = Helper::makeFactorableUnique<MCPScriptModulesCommand>( MENGINE_DOCUMENT_FACTORABLE );

        if( m_commandRegistry.addCommand( STRINGIZE_STRING_LOCAL( "script_modules" ), scriptModulesCommand ) == false )
        {
            return false;
        }

        MCPScriptSourceCommandPtr scriptSourceCommand = Helper::makeFactorableUnique<MCPScriptSourceCommand>( MENGINE_DOCUMENT_FACTORABLE, &m_scriptContext );

        if( m_commandRegistry.addCommand( STRINGIZE_STRING_LOCAL( "script_source" ), scriptSourceCommand ) == false )
        {
            return false;
        }

        MCPScriptInspectCommandPtr scriptInspectCommand = Helper::makeFactorableUnique<MCPScriptInspectCommand>( MENGINE_DOCUMENT_FACTORABLE, &m_scriptContext );

        if( m_commandRegistry.addCommand( STRINGIZE_STRING_LOCAL( "script_inspect" ), scriptInspectCommand ) == false )
        {
            return false;
        }

        MCPScriptGetCommandPtr scriptGetCommand = Helper::makeFactorableUnique<MCPScriptGetCommand>( MENGINE_DOCUMENT_FACTORABLE, &m_scriptContext );

        if( m_commandRegistry.addCommand( STRINGIZE_STRING_LOCAL( "script_get" ), scriptGetCommand ) == false )
        {
            return false;
        }

        MCPScriptSetCommandPtr scriptSetCommand = Helper::makeFactorableUnique<MCPScriptSetCommand>( MENGINE_DOCUMENT_FACTORABLE, &m_scriptContext );

        if( m_commandRegistry.addCommand( STRINGIZE_STRING_LOCAL( "script_set" ), scriptSetCommand ) == false )
        {
            return false;
        }

        MCPScriptCallCommandPtr scriptCallCommand = Helper::makeFactorableUnique<MCPScriptCallCommand>( MENGINE_DOCUMENT_FACTORABLE, &m_handlerRegistry, &m_scriptContext );

        if( m_commandRegistry.addCommand( STRINGIZE_STRING_LOCAL( "script_call" ), scriptCallCommand ) == false )
        {
            return false;
        }

        MCPScriptHandlersCommandPtr scriptHandlersCommand = Helper::makeFactorableUnique<MCPScriptHandlersCommand>( MENGINE_DOCUMENT_FACTORABLE, &m_handlerRegistry );

        if( m_commandRegistry.addCommand( STRINGIZE_STRING_LOCAL( "script_handlers" ), scriptHandlersCommand ) == false )
        {
            return false;
        }

        MCPScriptReleaseCommandPtr scriptReleaseCommand = Helper::makeFactorableUnique<MCPScriptReleaseCommand>( MENGINE_DOCUMENT_FACTORABLE, &m_scriptContext );

        if( m_commandRegistry.addCommand( STRINGIZE_STRING_LOCAL( "script_release" ), scriptReleaseCommand ) == false )
        {
            return false;
        }

        MCPScriptEvalCommandPtr scriptEvalCommand = Helper::makeFactorableUnique<MCPScriptEvalCommand>( MENGINE_DOCUMENT_FACTORABLE, &m_scriptContext, &m_debuggerContext );

        if( m_commandRegistry.addCommand( STRINGIZE_STRING_LOCAL( "script_eval" ), scriptEvalCommand ) == false )
        {
            return false;
        }

        MCPScriptExecCommandPtr scriptExecCommand = Helper::makeFactorableUnique<MCPScriptExecCommand>( MENGINE_DOCUMENT_FACTORABLE, &m_scriptContext, &m_debuggerContext );

        if( m_commandRegistry.addCommand( STRINGIZE_STRING_LOCAL( "script_exec" ), scriptExecCommand ) == false )
        {
            return false;
        }

        if( Helper::isDebugMode() == true )
        {
            if( this->initializeDebuggerCommands_() == false )
            {
                return false;
            }
        }
#endif

        MCPLoggerPtr runtimeLogger = Helper::makeFactorableUnique<MCPLogger>( MENGINE_DOCUMENT_FACTORABLE );
        runtimeLogger->setService( this );
        runtimeLogger->setWriteHistory( true );
        runtimeLogger->setVerboseLevel( LM_VERBOSE );

        if( LOGGER_SERVICE()
            ->registerLogger( runtimeLogger ) == false )
        {
            return false;
        }

        m_runtimeLogger = runtimeLogger;

        NOTIFICATION_ADDOBSERVERMETHOD_THIS( NOTIFICATOR_APPLICATION_BEGIN_UPDATE, &MCPService::notifyApplicationBeginUpdate_, MENGINE_DOCUMENT_FACTORABLE );
        NOTIFICATION_ADDOBSERVERMETHOD_THIS( NOTIFICATOR_RENDER_SCENE_END, &MCPService::notifyRenderSceneEnd_, MENGINE_DOCUMENT_FACTORABLE );
        NOTIFICATION_ADDOBSERVERMETHOD_THIS( NOTIFICATOR_BOOTSTRAPPER_FINALIZE_GAME, &MCPService::notifyBootstrapperFinalizeGame_, MENGINE_DOCUMENT_FACTORABLE );

#if defined(MENGINE_PLATFORM_ANDROID)
        NOTIFICATION_ADDOBSERVERMETHOD_THIS( NOTIFICATOR_APPLICATION_INTENT_START, &MCPService::notifyApplicationIntent_, MENGINE_DOCUMENT_FACTORABLE );
        NOTIFICATION_ADDOBSERVERMETHOD_THIS( NOTIFICATOR_APPLICATION_INTENT_NEW, &MCPService::notifyApplicationIntent_, MENGINE_DOCUMENT_FACTORABLE );
#endif

        return true;
    }
#if defined(MENGINE_BUILD_MENGINE_SCRIPT_EMBEDDED)
    //////////////////////////////////////////////////////////////////////////
    bool MCPService::initializeDebuggerCommands_()
    {
        MCPDebuggerWaitConditionPtr debuggerWaitCondition = Helper::makeFactorableUnique<MCPDebuggerWaitCondition>( MENGINE_DOCUMENT_FACTORABLE, this );

        if( m_waitConditionRegistry.addCondition( STRINGIZE_STRING_LOCAL( "debugger" ), debuggerWaitCondition ) == false )
        {
            return false;
        }

        MCPDebugSetBreakpointsCommandPtr debugSetBreakpointsCommand = Helper::makeFactorableUnique<MCPDebugSetBreakpointsCommand>( MENGINE_DOCUMENT_FACTORABLE, &m_debuggerContext );

        if( m_commandRegistry.addCommand( STRINGIZE_STRING_LOCAL( "debug_set_breakpoints" ), debugSetBreakpointsCommand ) == false )
        {
            return false;
        }

        MCPDebugSetExceptionPolicyCommandPtr debugSetExceptionPolicyCommand = Helper::makeFactorableUnique<MCPDebugSetExceptionPolicyCommand>( MENGINE_DOCUMENT_FACTORABLE, &m_debuggerContext );

        if( m_commandRegistry.addCommand( STRINGIZE_STRING_LOCAL( "debug_set_exception_policy" ), debugSetExceptionPolicyCommand ) == false )
        {
            return false;
        }

        MCPDebugPauseCommandPtr debugPauseCommand = Helper::makeFactorableUnique<MCPDebugPauseCommand>( MENGINE_DOCUMENT_FACTORABLE, &m_debuggerContext );

        if( m_commandRegistry.addCommand( STRINGIZE_STRING_LOCAL( "debug_pause" ), debugPauseCommand ) == false )
        {
            return false;
        }

        MCPDebugContinueCommandPtr debugContinueCommand = Helper::makeFactorableUnique<MCPDebugContinueCommand>( MENGINE_DOCUMENT_FACTORABLE, &m_debuggerContext );

        if( m_commandRegistry.addCommand( STRINGIZE_STRING_LOCAL( "debug_continue" ), debugContinueCommand ) == false )
        {
            return false;
        }

        MCPDebugStepCommandPtr debugStepCommand = Helper::makeFactorableUnique<MCPDebugStepCommand>( MENGINE_DOCUMENT_FACTORABLE, &m_debuggerContext );

        if( m_commandRegistry.addCommand( STRINGIZE_STRING_LOCAL( "debug_step" ), debugStepCommand ) == false )
        {
            return false;
        }

        MCPDebugStackCommandPtr debugStackCommand = Helper::makeFactorableUnique<MCPDebugStackCommand>( MENGINE_DOCUMENT_FACTORABLE, &m_debuggerContext );

        if( m_commandRegistry.addCommand( STRINGIZE_STRING_LOCAL( "debug_stack" ), debugStackCommand ) == false )
        {
            return false;
        }

        MCPDebugScopesCommandPtr debugScopesCommand = Helper::makeFactorableUnique<MCPDebugScopesCommand>( MENGINE_DOCUMENT_FACTORABLE, &m_debuggerContext, &m_scriptContext );

        if( m_commandRegistry.addCommand( STRINGIZE_STRING_LOCAL( "debug_scopes" ), debugScopesCommand ) == false )
        {
            return false;
        }

        MCPDebugVariablesCommandPtr debugVariablesCommand = Helper::makeFactorableUnique<MCPDebugVariablesCommand>( MENGINE_DOCUMENT_FACTORABLE, &m_scriptContext );

        if( m_commandRegistry.addCommand( STRINGIZE_STRING_LOCAL( "debug_variables" ), debugVariablesCommand ) == false )
        {
            return false;
        }

        MCPDebugEvaluateCommandPtr debugEvaluateCommand = Helper::makeFactorableUnique<MCPDebugEvaluateCommand>( MENGINE_DOCUMENT_FACTORABLE, &m_debuggerContext, &m_scriptContext );

        if( m_commandRegistry.addCommand( STRINGIZE_STRING_LOCAL( "debug_evaluate" ), debugEvaluateCommand ) == false )
        {
            return false;
        }

        MCPDebugSetVariableCommandPtr debugSetVariableCommand = Helper::makeFactorableUnique<MCPDebugSetVariableCommand>( MENGINE_DOCUMENT_FACTORABLE, &m_debuggerContext, &m_scriptContext );

        if( m_commandRegistry.addCommand( STRINGIZE_STRING_LOCAL( "debug_set_variable" ), debugSetVariableCommand ) == false )
        {
            return false;
        }

        return true;
    }
#endif
    //////////////////////////////////////////////////////////////////////////
    bool MCPService::_runService()
    {
#if defined(MENGINE_PLATFORM_ANDROID)
        return true;
#else
        const String host = GET_OPTION_VALUE( "mcp-host", "127.0.0.1" );
        const String port = GET_OPTION_VALUE( "mcp-port", "" );
        const String token = GET_OPTION_VALUE( "mcp-token", "" );
        const String mode = GET_OPTION_VALUE( "mcp-mode", "visible" );
        bool listen = HAS_OPTION( "mcp-listen" );

        if( this->run( host, port, token, mode, listen ) == false )
        {
            LOGGER_ERROR( "MCP activation failed" );

            return false;
        }

        return true;
#endif
    }
    //////////////////////////////////////////////////////////////////////////
    bool MCPService::run( const String & _host, const String & _port, const String & _token, const String & _mode, bool _listen )
    {
        if( _token.size() != 64 )
        {
            LOGGER_ERROR( "MCP requires a 256-bit hexadecimal session token" );

            return false;
        }

        m_host = _host;
        m_port = _port;
        m_token = _token;
        m_mode = _mode;
        m_listen = _listen;

        m_socketConnected = false;
        m_connected = false;
        m_debuggerPaused = false;
        m_sessionEnded = false;

        Timestamp timestamp = Helper::getSystemTimestamp();
        m_connectDeadline = timestamp + MCP_CONNECT_RETRY_TIMEOUT;

        m_thread = Helper::createThreadIdentity( MENGINE_THREAD_DESCRIPTION( "MNGMCP" ), ETP_ABOVE_NORMAL, [this]( const ThreadIdentityRunnerInterfacePtr & _runner )
        {
            if( this->processSocket_( _runner ) == false )
            {
                return false;
            }

            return true;
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

        LOGGER_INFO( "mcp", "%s MNCP v1 '%s:%s' in '%s' mode"
            , m_listen == true ? "listening on" : "connecting to"
            , m_host.c_str()
            , m_port.c_str()
            , m_mode.c_str()
        );

        return true;
    }
    //////////////////////////////////////////////////////////////////////////
    void MCPService::notifyApplicationBeginUpdate_()
    {
        ++m_updateFrame;
    }
    //////////////////////////////////////////////////////////////////////////
    void MCPService::notifyRenderSceneEnd_( const RenderPipelineInterfacePtr & _pipeline )
    {
        MENGINE_UNUSED( _pipeline );

        ++m_renderFrame;
    }
    //////////////////////////////////////////////////////////////////////////
    void MCPService::notifyBootstrapperFinalizeGame_()
    {
        m_scriptContext.finalize();
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

        m_sessionEnded = false;

        this->releaseSession_();

        m_scriptContext.finalize();
        m_sceneContext.finalize();

        m_incomingRequests.clear();
        m_outgoingFrames.clear();
        m_cancelledRequests.clear();
        m_incomingAssemblies.clear();

        m_host.clear();
        m_port.clear();
        m_token.clear();
        m_mode.clear();
    }
    //////////////////////////////////////////////////////////////////////////
    void MCPService::_finalizeService()
    {
        NOTIFICATION_REMOVEOBSERVER_THIS( NOTIFICATOR_APPLICATION_BEGIN_UPDATE );
        NOTIFICATION_REMOVEOBSERVER_THIS( NOTIFICATOR_RENDER_SCENE_END );
        NOTIFICATION_REMOVEOBSERVER_THIS( NOTIFICATOR_BOOTSTRAPPER_FINALIZE_GAME );

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

            LOGGER_SERVICE()
                ->unregisterLogger( m_runtimeLogger );

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

        Char permission[PROP_VALUE_MAX] = {'\0'};
        ::__system_property_get( "debug.mengine.mcp", permission );

        if( StdString::strcmp( permission, "1" ) != 0 )
        {
            LOGGER_ERROR( "MCP intent ignored: allow it with 'adb shell setprop debug.mengine.mcp 1'" );

            return;
        }

        this->stop();

        if( this->run( host, port, token, mode, false ) == false )
        {
            LOGGER_ERROR( "MCP Android intent activation failed" );
        }
    }
#endif
    //////////////////////////////////////////////////////////////////////////
    bool MCPService::addHandler( const ConstString & _name, const MCPHandlerInterfacePtr & _handler )
    {
        if( m_handlerRegistry.addHandler( _name, _handler ) == false )
        {
            return false;
        }

        return true;
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
        return m_mode;
    }
    //////////////////////////////////////////////////////////////////////////
    uint64_t MCPService::getMCPUpdateFrame() const
    {
        return m_updateFrame;
    }
    //////////////////////////////////////////////////////////////////////////
    uint64_t MCPService::getMCPRenderFrame() const
    {
        return m_renderFrame;
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
    void MCPService::captureRuntimeLog( const LoggerMessage & _message )
    {
        m_runtimeContext.captureLog( _message );
    }
    //////////////////////////////////////////////////////////////////////////
    void MCPService::_preUpdate()
    {
        if( m_running == false )
        {
            return;
        }

        this->processConnection_();
        this->processRequests_();
    }
    //////////////////////////////////////////////////////////////////////////
    void MCPService::processConnection_()
    {
        if( m_sessionEnded.load() == false )
        {
            return;
        }

        this->releaseSession_();

        m_sessionEnded = false;

        if( m_listen == true )
        {
            return;
        }

        if( m_mode == "visible" )
        {
            return;
        }

        LOGGER_MESSAGE( "MCP controller is unavailable, quit '%s' application"
            , m_mode.c_str()
        );

        APPLICATION_SERVICE()
            ->quit();
    }
    //////////////////////////////////////////////////////////////////////////
    void MCPService::releaseSession_()
    {
        this->releaseOperations_();

        m_debuggerContext.disconnect();
        m_runtimeContext.restoreRuntime();
        m_resourceContext.revertAllOverlays();
    }
    //////////////////////////////////////////////////////////////////////////
    void MCPService::processRequests_()
    {
        if( m_connected.load() == false )
        {
            return;
        }

        this->dispatchRequests_();

        if( m_connected.load() == false )
        {
            return;
        }

        if( m_operationsUpdating == true )
        {
            return;
        }

        m_operationsUpdating = true;

        this->cancelOperations_();
        this->updateOperations_();

        m_operationsUpdating = false;
    }
    //////////////////////////////////////////////////////////////////////////
    void MCPService::dispatchRequests_()
    {
        for( ;; )
        {
            if( m_connected.load() == false )
            {
                break;
            }

            IncomingRequest request;

            {
                MENGINE_THREAD_MUTEX_SCOPE( m_incomingMutex );

                if( m_incomingRequests.empty() == true )
                {
                    break;
                }

                request = StdUtility::move( m_incomingRequests.front() );
                m_incomingRequests.pop_front();

                if( m_cancelledRequests.erase( request.requestId ) != 0 )
                {
                    continue;
                }
            }

            this->processRequest_( request );
        }
    }
    //////////////////////////////////////////////////////////////////////////
    void MCPService::cancelOperations_()
    {
        Set<uint32_t> cancelled;

        {
            MENGINE_THREAD_MUTEX_SCOPE( m_incomingMutex );

            cancelled.swap( m_cancelledRequests );
        }

        for( uint32_t requestId : cancelled )
        {
            MapOperations::iterator it = m_operations.find( requestId );

            if( it == m_operations.end() )
            {
                continue;
            }

            MCPOperationInterfacePtr operation = it->second;
            m_operations.erase( it );

            operation->cancel();
        }
    }
    //////////////////////////////////////////////////////////////////////////
    void MCPService::updateOperations_()
    {
        for( MapOperations::iterator it = m_operations.begin(); it != m_operations.end(); )
        {
            uint32_t requestId = it->first;
            MCPOperationInterfacePtr operation = it->second;

            MCPCommandResponse response;
            EMCPCommandStatus status = operation->update( &response );

            if( status == EMCPCommandStatus::DEFERRED )
            {
                ++it;

                continue;
            }

            it = m_operations.erase( it );

            this->sendCommandResult_( requestId, status, response );
        }
    }
    //////////////////////////////////////////////////////////////////////////
    void MCPService::releaseOperations_()
    {
        MapOperations operations;
        operations.swap( m_operations );

        for( const MapOperations::value_type & value : operations )
        {
            const MCPOperationInterfacePtr & operation = value.second;

            operation->cancel();
        }
    }
    //////////////////////////////////////////////////////////////////////////
    bool MCPService::processSocket_( const ThreadIdentityRunnerInterfacePtr & _runner )
    {
        if( _runner->isCancel() == true )
        {
            return false;
        }

        if( m_sessionEnded.load() == true )
        {
            return true;
        }

        if( m_socketConnected.load() == false )
        {
            if( this->openSocket_() == false )
            {
                return false;
            }

            return true;
        }

        if( this->flushOutgoing_() == false )
        {
            this->disconnectSocket_();

            return m_listen;
        }

        if( this->receiveIncoming_() == false )
        {
            this->disconnectSocket_();

            return m_listen;
        }

        return true;
    }
    //////////////////////////////////////////////////////////////////////////
    bool MCPService::openSocket_()
    {
        if( m_listen == true )
        {
            if( this->acceptSocket_() == false )
            {
                return false;
            }

            return true;
        }

        if( this->connectSocket_() == false )
        {
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
            Timestamp timestamp = Helper::getSystemTimestamp();

            if( timestamp >= m_connectDeadline )
            {
                LOGGER_ERROR( "MCP failed to connect to '%s:%s'"
                    , host
                    , port
                );

                m_sessionEnded = true;

                return false;
            }

            THREAD_SYSTEM()
                ->sleep( MCP_CONNECT_RETRY_DELAY );

            return true;
        }

        m_socket = socket;
        m_socketConnected = true;

        this->queueHandshake_();

        return true;
    }
    //////////////////////////////////////////////////////////////////////////
    bool MCPService::acceptSocket_()
    {
        if( m_socket == nullptr )
        {
            SocketInterfacePtr socket = SOCKET_SYSTEM()
                ->createSocket( MENGINE_DOCUMENT_FACTORABLE );

            if( socket == nullptr )
            {
                return false;
            }

            const Char * host = m_host.c_str();
            const Char * port = m_port.c_str();
            SocketConnectInfo bindInfo = {host, port};

            if( socket->bind( bindInfo, false ) == false )
            {
                LOGGER_ERROR( "MCP failed to listen on '%s:%s'"
                    , host
                    , port
                );

                return false;
            }

            m_socket = socket;
        }

        int32_t accepted = m_socket->checkForClientConnection();

        if( accepted < 0 )
        {
            LOGGER_ERROR( "MCP failed to accept a client on '%s:%s'"
                , m_host.c_str()
                , m_port.c_str()
            );

            m_socket = nullptr;

            return false;
        }

        if( accepted == 0 )
        {
            THREAD_SYSTEM()
                ->sleep( MCP_ACCEPT_POLL_DELAY );

            return true;
        }

        m_socketConnected = true;

        LOGGER_INFO( "mcp", "MNCP client connected, waiting for the handshake" );

        return true;
    }
    //////////////////////////////////////////////////////////////////////////
    bool MCPService::flushOutgoing_()
    {
        for( ;; )
        {
            if( m_outgoingFrame.empty() == true )
            {
                MENGINE_THREAD_MUTEX_SCOPE( m_outgoingMutex );

                if( m_outgoingFrames.empty() == true )
                {
                    return true;
                }

                m_outgoingFrame = StdUtility::move( m_outgoingFrames.front() );
                m_outgoingFrames.pop_front();
                m_outgoingOffset = 0;
            }

            size_t frameSize = m_outgoingFrame.size();
            const uint8_t * frameData = m_outgoingFrame.data();

            while( m_outgoingOffset != frameSize )
            {
                size_t sent = 0;
                const uint8_t * sendData = frameData + m_outgoingOffset;
                size_t sendSize = frameSize - m_outgoingOffset;

                if( m_socket->send( sendData, sendSize, &sent ) == false )
                {
                    return false;
                }

                if( sent == 0 )
                {
                    return true;
                }

                m_outgoingOffset += sent;
            }

            m_outgoingFrame.clear();
        }
    }
    //////////////////////////////////////////////////////////////////////////
    bool MCPService::receiveIncoming_()
    {
        if( m_socket->waitForData( 5 ) == false )
        {
            return true;
        }

        constexpr size_t bufferSize = 64 * 1024;
        uint8_t buffer[bufferSize];
        size_t received = 0;

        if( m_socket->receive( buffer, sizeof( buffer ), &received ) == false )
        {
            return false;
        }

        if( received == 0 )
        {
            return false;
        }

        VectorMCPFrames frames;
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
        if( m_connected.load() == false )
        {
            if( this->processHandshake_( _frame ) == false )
            {
                return false;
            }

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

            if( request.is_type_object() == false )
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
    bool MCPService::processHandshake_( const MCPFrame & _frame )
    {
        const uint8_t * payloadBuffer = _frame.payload.data();
        size_t payloadSize = _frame.payload.size();
        jpp::object payload = Helper::loadJSONBuffer( payloadBuffer, payloadSize, MENGINE_DOCUMENT_FACTORABLE );

        if( payload.invalid() == true )
        {
            return false;
        }

        if( payload.is_type_object() == false )
        {
            return false;
        }

        if( m_listen == false )
        {
            if( _frame.type != EMCPFrameType::Response )
            {
                return false;
            }

            if( _frame.requestId != Detail::MCP_HANDSHAKE_REQUEST_ID )
            {
                return false;
            }

            if( payload.exist( "error", nullptr ) == true )
            {
                return false;
            }

            m_connected = true;

            LOGGER_INFO( "mcp", "MNCP session authenticated" );

            return true;
        }

        if( _frame.type != EMCPFrameType::Request )
        {
            return false;
        }

        jpp::object methodObject;
        if( payload.exist( "method", &methodObject ) == false )
        {
            return false;
        }

        if( methodObject.is_type_string() == false )
        {
            return false;
        }

        const Char * method = methodObject;

        if( StdString::strcmp( method, "handshake" ) != 0 )
        {
            return false;
        }

        jpp::object params;
        if( payload.exist( "params", &params ) == false )
        {
            return false;
        }

        if( params.is_type_object() == false )
        {
            return false;
        }

        jpp::object tokenObject;
        if( params.exist( "token", &tokenObject ) == false )
        {
            return false;
        }

        if( tokenObject.is_type_string() == false )
        {
            return false;
        }

        const Char * token = tokenObject;

        if( m_token != token )
        {
            LOGGER_ERROR( "MCP client rejected: invalid session token" );

            return false;
        }

        jpp::array capabilities = jpp::make_array();
        this->makeCapabilities_( &capabilities );

        jpp::object result = jpp::make_object();
        result.set( "protocol", MCP_PROTOCOL_VERSION );
        result.set( "authenticated", true );
        result.set( "capabilities", capabilities );
        result.set( "mode", m_mode );

        jpp::object response = jpp::make_object();
        response.set( "result", result );

        this->queueResponse_( _frame.requestId, response );

        m_connected = true;

        LOGGER_INFO( "mcp", "MNCP client authenticated" );

        return true;
    }
    //////////////////////////////////////////////////////////////////////////
    void MCPService::disconnectSocket_()
    {
        m_connected = false;
        m_socketConnected = false;
        m_debuggerPaused = false;

        if( m_socket != nullptr )
        {
            m_socket->disconnect();
            m_socket = nullptr;
        }

        m_decoder.reset();
        m_incomingAssemblies.clear();
        m_outgoingFrame.clear();
        m_outgoingOffset = 0;

        {
            MENGINE_THREAD_MUTEX_SCOPE( m_incomingMutex );

            m_incomingRequests.clear();
            m_cancelledRequests.clear();
        }

        {
            MENGINE_THREAD_MUTEX_SCOPE( m_outgoingMutex );

            m_outgoingFrames.clear();
        }

        m_sessionEnded = true;
    }
    //////////////////////////////////////////////////////////////////////////
    void MCPService::makeCapabilities_( jpp::array * const _capabilities ) const
    {
        VectorConstString names;
        m_commandRegistry.getCommandNames( &names );

        for( const ConstString & name : names )
        {
            const Char * nameString = name.c_str();

            _capabilities->push_back( nameString );
        }
    }
    //////////////////////////////////////////////////////////////////////////
    void MCPService::queueHandshake_()
    {
        jpp::array capabilities = jpp::make_array();
        this->makeCapabilities_( &capabilities );

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

        if( m_socketConnected.load() == false )
        {
            return;
        }

        if( m_sessionEnded.load() == true )
        {
            return;
        }

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

        if( request.is_type_object() == false )
        {
            this->queueError_( _request.requestId, "execution_error", "request must be a JSON object" );

            return;
        }

        jpp::object methodObject;
        if( request.exist( "method", &methodObject ) == false )
        {
            this->queueError_( _request.requestId, "execution_error", "request method is required" );

            return;
        }

        if( methodObject.is_type_string() == false )
        {
            this->queueError_( _request.requestId, "execution_error", "request method must be a string" );

            return;
        }

        const Char * method = methodObject;
        ConstString methodName = Helper::stringizeString( method );
        jpp::object params;
        if( request.exist( "params", &params ) == false )
        {
            params = jpp::make_object();
        }

        if( params.is_type_object() == false )
        {
            this->queueError_( _request.requestId, "execution_error", "request params must be an object" );

            return;
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

        if( status == EMCPCommandStatus::DEFERRED )
        {
            MENGINE_ASSERTION_MEMORY_PANIC( commandResponse.operation, "command '%s' deferred without an operation"
                , method
            );

            bool inserted = m_operations.emplace( _request.requestId, commandResponse.operation ).second;

            if( inserted == false )
            {
                commandResponse.operation->cancel();

                this->queueError_( _request.requestId, "execution_error", "request id is already pending" );
            }

            return;
        }

        this->sendCommandResult_( _request.requestId, status, commandResponse );
    }
    //////////////////////////////////////////////////////////////////////////
    void MCPService::sendCommandResult_( uint32_t _requestId, EMCPCommandStatus _status, const MCPCommandResponse & _response )
    {
        MENGINE_ASSERTION_FATAL( _status != EMCPCommandStatus::DEFERRED, "deferred status has no result" );

        if( _status == EMCPCommandStatus::FAILURE )
        {
            const Char * errorCode = "execution_error";

            if( _response.errorCode.empty() == false )
            {
                errorCode = _response.errorCode.c_str();
            }

            const Char * errorMessage = _response.errorMessage.c_str();
            this->queueError_( _requestId, errorCode, errorMessage );

            return;
        }

        jpp::object result = _response.result;

        if( _response.attachment.empty() == false )
        {
            size_t attachmentSize = _response.attachment.size();

            jpp::object attachment = jpp::make_object();
            attachment.set( "size", (uint32_t)attachmentSize );
            result.set( "attachment", attachment );
        }

        jpp::object response = jpp::make_object();
        response.set( "result", result );
        this->queueResponse_( _requestId, response );

        if( _response.attachment.empty() == true )
        {
            return;
        }

        this->queueBinary_( _requestId, _response.attachment );
    }
    //////////////////////////////////////////////////////////////////////////
    void MCPService::queueBinary_( uint32_t _requestId, const Data & _data )
    {
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
