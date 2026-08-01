#include "MCPCommandComposition.h"

#include "MCPCommandRegistry.h"
#include "Application/MCPAppStopCommand.h"
#include "Runtime/MCPLogsReadCommand.h"
#include "Runtime/MCPRuntimeControlCommand.h"
#include "Runtime/MCPFrameCaptureCommand.h"
#include "Runtime/MCPWaitForCommand.h"
#include "Runtime/MCPDiagnosticsGetCommand.h"
#include "Scene/MCPSceneSnapshotCommand.h"
#include "Scene/MCPSceneFindCommand.h"
#include "Scene/MCPSceneGetCommand.h"
#include "Scene/MCPSceneSetCommand.h"
#include "Input/MCPInputMouseCommand.h"
#include "Input/MCPInputKeyboardCommand.h"
#include "Input/MCPInputTouchCommand.h"
#include "Input/MCPInputSequenceCommand.h"
#include "Input/Steps/MCPDelayInputSequenceStep.h"
#include "Input/Steps/MCPFramesInputSequenceStep.h"
#include "Input/Steps/MCPMouseInputSequenceStep.h"
#include "Input/Steps/MCPKeyboardInputSequenceStep.h"
#include "Input/Steps/MCPTouchInputSequenceStep.h"
#include "Input/Steps/MCPWaitInputSequenceStep.h"
#include "Resource/MCPResourceReloadCommand.h"
#include "Resource/MCPResourceRevertCommand.h"
#include "Wait/Conditions/MCPFramesWaitCondition.h"
#include "Wait/Conditions/MCPRuntimeWaitCondition.h"
#include "Wait/Conditions/MCPSceneWaitCondition.h"
#include "Wait/Conditions/MCPNodeWaitCondition.h"
#include "Wait/Conditions/MCPLogWaitCondition.h"

#if defined(MENGINE_BUILD_MENGINE_SCRIPT_EMBEDDED)
#   include "Wait/Conditions/MCPDebuggerWaitCondition.h"
#   include "Wait/Conditions/MCPScriptWaitCondition.h"
#   include "Script/MCPScriptReloadModuleCommand.h"
#   include "Script/MCPScriptModulesCommand.h"
#   include "Script/MCPScriptSourceCommand.h"
#   include "Script/MCPScriptInspectCommand.h"
#   include "Script/MCPScriptGetCommand.h"
#   include "Script/MCPScriptSetCommand.h"
#   include "Script/MCPScriptCallCommand.h"
#   include "Script/MCPScriptReleaseCommand.h"
#   include "Script/MCPScriptEvalCommand.h"
#   include "Script/MCPScriptExecCommand.h"
#   include "Debugger/MCPDebugSetBreakpointsCommand.h"
#   include "Debugger/MCPDebugSetExceptionPolicyCommand.h"
#   include "Debugger/MCPDebugPauseCommand.h"
#   include "Debugger/MCPDebugContinueCommand.h"
#   include "Debugger/MCPDebugStepCommand.h"
#   include "Debugger/MCPDebugStackCommand.h"
#   include "Debugger/MCPDebugScopesCommand.h"
#   include "Debugger/MCPDebugVariablesCommand.h"
#   include "Debugger/MCPDebugEvaluateCommand.h"
#   include "Debugger/MCPDebugSetVariableCommand.h"
#endif

#include "../Contexts/MCPRuntimeContext.h"
#include "../Contexts/MCPSceneContext.h"
#include "../Contexts/MCPInputContext.h"
#include "../Contexts/MCPResourceContext.h"
#include "../Contexts/MCPHandlerRegistry.h"
#include "../Contexts/MCPScriptContext.h"
#include "../Contexts/MCPDebuggerContext.h"

#include "Kernel/ConstStringHelper.h"
#include "Kernel/FactorableUnique.h"

namespace Mengine
{
    //////////////////////////////////////////////////////////////////////////
    namespace Helper
    {
        //////////////////////////////////////////////////////////////////////////
        bool composeMCPCommands( MCPCommandHostInterface * _host, MCPRuntimeContext * _runtimeContext, MCPSceneContext * _sceneContext, MCPInputContext * _inputContext, MCPResourceContext * _resourceContext, MCPHandlerRegistry * _handlerRegistry, MCPScriptContext * _scriptContext, MCPDebuggerContext * _debuggerContext, MCPWaitConditionRegistry * _waitRegistry, MCPInputSequenceStepRegistry * _stepRegistry, MCPCommandRegistry * _registry )
        {
            DocumentInterfacePtr framesWaitDocument = MENGINE_DOCUMENT_FUNCTION;
            MCPFramesWaitConditionPtr framesWaitCondition = Helper::makeFactorableUnique<MCPFramesWaitCondition>( framesWaitDocument );
            ConstString framesWaitName = STRINGIZE_STRING_LOCAL( "frames" );

            if( _waitRegistry->addCondition( framesWaitName, framesWaitCondition ) == false )
            {
                return false;
            }

            DocumentInterfacePtr runtimeWaitDocument = MENGINE_DOCUMENT_FUNCTION;
            MCPRuntimeWaitConditionPtr runtimeWaitCondition = Helper::makeFactorableUnique<MCPRuntimeWaitCondition>( runtimeWaitDocument );
            ConstString runtimeWaitName = STRINGIZE_STRING_LOCAL( "runtime" );

            if( _waitRegistry->addCondition( runtimeWaitName, runtimeWaitCondition ) == false )
            {
                return false;
            }

            DocumentInterfacePtr sceneWaitDocument = MENGINE_DOCUMENT_FUNCTION;
            MCPSceneWaitConditionPtr sceneWaitCondition = Helper::makeFactorableUnique<MCPSceneWaitCondition>( sceneWaitDocument );
            ConstString sceneWaitName = STRINGIZE_STRING_LOCAL( "scene" );

            if( _waitRegistry->addCondition( sceneWaitName, sceneWaitCondition ) == false )
            {
                return false;
            }

            DocumentInterfacePtr nodeWaitDocument = MENGINE_DOCUMENT_FUNCTION;
            MCPNodeWaitConditionPtr nodeWaitCondition = Helper::makeFactorableUnique<MCPNodeWaitCondition>( nodeWaitDocument, _sceneContext );
            ConstString nodeWaitName = STRINGIZE_STRING_LOCAL( "node" );

            if( _waitRegistry->addCondition( nodeWaitName, nodeWaitCondition ) == false )
            {
                return false;
            }

            DocumentInterfacePtr logWaitDocument = MENGINE_DOCUMENT_FUNCTION;
            MCPLogWaitConditionPtr logWaitCondition = Helper::makeFactorableUnique<MCPLogWaitCondition>( logWaitDocument, _runtimeContext );
            ConstString logWaitName = STRINGIZE_STRING_LOCAL( "log" );

            if( _waitRegistry->addCondition( logWaitName, logWaitCondition ) == false )
            {
                return false;
            }

#if defined(MENGINE_BUILD_MENGINE_SCRIPT_EMBEDDED)
            DocumentInterfacePtr debuggerWaitDocument = MENGINE_DOCUMENT_FUNCTION;
            MCPDebuggerWaitConditionPtr debuggerWaitCondition = Helper::makeFactorableUnique<MCPDebuggerWaitCondition>( debuggerWaitDocument, _host );
            ConstString debuggerWaitName = STRINGIZE_STRING_LOCAL( "debugger" );

            if( _waitRegistry->addCondition( debuggerWaitName, debuggerWaitCondition ) == false )
            {
                return false;
            }

            DocumentInterfacePtr scriptWaitDocument = MENGINE_DOCUMENT_FUNCTION;
            MCPScriptWaitConditionPtr scriptWaitCondition = Helper::makeFactorableUnique<MCPScriptWaitCondition>( scriptWaitDocument, _scriptContext );
            ConstString scriptWaitName = STRINGIZE_STRING_LOCAL( "script" );

            if( _waitRegistry->addCondition( scriptWaitName, scriptWaitCondition ) == false )
            {
                return false;
            }
#endif

            DocumentInterfacePtr appStopDocument = MENGINE_DOCUMENT_FUNCTION;
            MCPAppStopCommandPtr appStopCommand = Helper::makeFactorableUnique<MCPAppStopCommand>( appStopDocument, _host );
            ConstString appStopName = STRINGIZE_STRING_LOCAL( "app_stop" );

            if( _registry->addCommand( appStopName, appStopCommand ) == false )
            {
                return false;
            }

            DocumentInterfacePtr logsReadDocument = MENGINE_DOCUMENT_FUNCTION;
            MCPLogsReadCommandPtr logsReadCommand = Helper::makeFactorableUnique<MCPLogsReadCommand>( logsReadDocument, _runtimeContext );
            ConstString logsReadName = STRINGIZE_STRING_LOCAL( "logs_read" );

            if( _registry->addCommand( logsReadName, logsReadCommand ) == false )
            {
                return false;
            }

            DocumentInterfacePtr runtimeControlDocument = MENGINE_DOCUMENT_FUNCTION;
            MCPRuntimeControlCommandPtr runtimeControlCommand = Helper::makeFactorableUnique<MCPRuntimeControlCommand>( runtimeControlDocument, _host, _runtimeContext );
            ConstString runtimeControlName = STRINGIZE_STRING_LOCAL( "runtime_control" );

            if( _registry->addCommand( runtimeControlName, runtimeControlCommand ) == false )
            {
                return false;
            }

            DocumentInterfacePtr frameCaptureDocument = MENGINE_DOCUMENT_FUNCTION;
            MCPFrameCaptureCommandPtr frameCaptureCommand = Helper::makeFactorableUnique<MCPFrameCaptureCommand>( frameCaptureDocument, _host );
            ConstString frameCaptureName = STRINGIZE_STRING_LOCAL( "frame_capture" );

            if( _registry->addCommand( frameCaptureName, frameCaptureCommand ) == false )
            {
                return false;
            }

            DocumentInterfacePtr waitForDocument = MENGINE_DOCUMENT_FUNCTION;
            MCPWaitForCommandPtr waitForCommand = Helper::makeFactorableUnique<MCPWaitForCommand>( waitForDocument, _host, _waitRegistry );
            ConstString waitForName = STRINGIZE_STRING_LOCAL( "wait_for" );

            if( _registry->addCommand( waitForName, waitForCommand ) == false )
            {
                return false;
            }

            DocumentInterfacePtr diagnosticsGetDocument = MENGINE_DOCUMENT_FUNCTION;
            MCPDiagnosticsGetCommandPtr diagnosticsGetCommand = Helper::makeFactorableUnique<MCPDiagnosticsGetCommand>( diagnosticsGetDocument, _host, _runtimeContext, _sceneContext, _resourceContext, _scriptContext );
            ConstString diagnosticsGetName = STRINGIZE_STRING_LOCAL( "diagnostics_get" );

            if( _registry->addCommand( diagnosticsGetName, diagnosticsGetCommand ) == false )
            {
                return false;
            }

            DocumentInterfacePtr sceneSnapshotDocument = MENGINE_DOCUMENT_FUNCTION;
            MCPSceneSnapshotCommandPtr sceneSnapshotCommand = Helper::makeFactorableUnique<MCPSceneSnapshotCommand>( sceneSnapshotDocument, _sceneContext );
            ConstString sceneSnapshotName = STRINGIZE_STRING_LOCAL( "scene_snapshot" );

            if( _registry->addCommand( sceneSnapshotName, sceneSnapshotCommand ) == false )
            {
                return false;
            }

            DocumentInterfacePtr sceneFindDocument = MENGINE_DOCUMENT_FUNCTION;
            MCPSceneFindCommandPtr sceneFindCommand = Helper::makeFactorableUnique<MCPSceneFindCommand>( sceneFindDocument, _sceneContext );
            ConstString sceneFindName = STRINGIZE_STRING_LOCAL( "scene_find" );

            if( _registry->addCommand( sceneFindName, sceneFindCommand ) == false )
            {
                return false;
            }

            DocumentInterfacePtr sceneGetDocument = MENGINE_DOCUMENT_FUNCTION;
            MCPSceneGetCommandPtr sceneGetCommand = Helper::makeFactorableUnique<MCPSceneGetCommand>( sceneGetDocument, _sceneContext );
            ConstString sceneGetName = STRINGIZE_STRING_LOCAL( "scene_get" );

            if( _registry->addCommand( sceneGetName, sceneGetCommand ) == false )
            {
                return false;
            }

            DocumentInterfacePtr sceneSetDocument = MENGINE_DOCUMENT_FUNCTION;
            MCPSceneSetCommandPtr sceneSetCommand = Helper::makeFactorableUnique<MCPSceneSetCommand>( sceneSetDocument, _sceneContext );
            ConstString sceneSetName = STRINGIZE_STRING_LOCAL( "scene_set" );

            if( _registry->addCommand( sceneSetName, sceneSetCommand ) == false )
            {
                return false;
            }

            DocumentInterfacePtr inputMouseDocument = MENGINE_DOCUMENT_FUNCTION;
            MCPInputMouseCommandPtr inputMouseCommand = Helper::makeFactorableUnique<MCPInputMouseCommand>( inputMouseDocument, _inputContext );
            ConstString inputMouseName = STRINGIZE_STRING_LOCAL( "input_mouse" );

            if( _registry->addCommand( inputMouseName, inputMouseCommand ) == false )
            {
                return false;
            }

            DocumentInterfacePtr inputKeyboardDocument = MENGINE_DOCUMENT_FUNCTION;
            MCPInputKeyboardCommandPtr inputKeyboardCommand = Helper::makeFactorableUnique<MCPInputKeyboardCommand>( inputKeyboardDocument, _inputContext );
            ConstString inputKeyboardName = STRINGIZE_STRING_LOCAL( "input_keyboard" );

            if( _registry->addCommand( inputKeyboardName, inputKeyboardCommand ) == false )
            {
                return false;
            }

            DocumentInterfacePtr inputTouchDocument = MENGINE_DOCUMENT_FUNCTION;
            MCPInputTouchCommandPtr inputTouchCommand = Helper::makeFactorableUnique<MCPInputTouchCommand>( inputTouchDocument, _inputContext );
            ConstString inputTouchName = STRINGIZE_STRING_LOCAL( "input_touch" );

            if( _registry->addCommand( inputTouchName, inputTouchCommand ) == false )
            {
                return false;
            }

            DocumentInterfacePtr delayStepDocument = MENGINE_DOCUMENT_FUNCTION;
            MCPDelayInputSequenceStepPtr delayStep = Helper::makeFactorableUnique<MCPDelayInputSequenceStep>( delayStepDocument );
            ConstString delayStepName = STRINGIZE_STRING_LOCAL( "delay" );

            if( _stepRegistry->addStep( delayStepName, delayStep ) == false )
            {
                return false;
            }

            DocumentInterfacePtr framesStepDocument = MENGINE_DOCUMENT_FUNCTION;
            MCPFramesInputSequenceStepPtr framesStep = Helper::makeFactorableUnique<MCPFramesInputSequenceStep>( framesStepDocument );
            ConstString framesStepName = STRINGIZE_STRING_LOCAL( "frames" );

            if( _stepRegistry->addStep( framesStepName, framesStep ) == false )
            {
                return false;
            }

            MCPCommandInterfacePtr inputMouseInterface = inputMouseCommand;
            DocumentInterfacePtr mouseStepDocument = MENGINE_DOCUMENT_FUNCTION;
            MCPMouseInputSequenceStepPtr mouseStep = Helper::makeFactorableUnique<MCPMouseInputSequenceStep>( mouseStepDocument, inputMouseInterface );
            ConstString mouseStepName = STRINGIZE_STRING_LOCAL( "mouse" );

            if( _stepRegistry->addStep( mouseStepName, mouseStep ) == false )
            {
                return false;
            }

            MCPCommandInterfacePtr inputKeyboardInterface = inputKeyboardCommand;
            DocumentInterfacePtr keyboardStepDocument = MENGINE_DOCUMENT_FUNCTION;
            MCPKeyboardInputSequenceStepPtr keyboardStep = Helper::makeFactorableUnique<MCPKeyboardInputSequenceStep>( keyboardStepDocument, inputKeyboardInterface );
            ConstString keyboardStepName = STRINGIZE_STRING_LOCAL( "keyboard" );

            if( _stepRegistry->addStep( keyboardStepName, keyboardStep ) == false )
            {
                return false;
            }

            MCPCommandInterfacePtr inputTouchInterface = inputTouchCommand;
            DocumentInterfacePtr touchStepDocument = MENGINE_DOCUMENT_FUNCTION;
            MCPTouchInputSequenceStepPtr touchStep = Helper::makeFactorableUnique<MCPTouchInputSequenceStep>( touchStepDocument, inputTouchInterface );
            ConstString touchStepName = STRINGIZE_STRING_LOCAL( "touch" );

            if( _stepRegistry->addStep( touchStepName, touchStep ) == false )
            {
                return false;
            }

            DocumentInterfacePtr waitStepDocument = MENGINE_DOCUMENT_FUNCTION;
            MCPWaitInputSequenceStepPtr waitStep = Helper::makeFactorableUnique<MCPWaitInputSequenceStep>( waitStepDocument, _waitRegistry );
            ConstString waitStepName = STRINGIZE_STRING_LOCAL( "wait" );

            if( _stepRegistry->addStep( waitStepName, waitStep ) == false )
            {
                return false;
            }

            DocumentInterfacePtr inputSequenceDocument = MENGINE_DOCUMENT_FUNCTION;
            MCPInputSequenceCommandPtr inputSequenceCommand = Helper::makeFactorableUnique<MCPInputSequenceCommand>( inputSequenceDocument, _host, _stepRegistry );
            ConstString inputSequenceName = STRINGIZE_STRING_LOCAL( "input_sequence" );

            if( _registry->addCommand( inputSequenceName, inputSequenceCommand ) == false )
            {
                return false;
            }

            DocumentInterfacePtr resourceReloadDocument = MENGINE_DOCUMENT_FUNCTION;
            MCPResourceReloadCommandPtr resourceReloadCommand = Helper::makeFactorableUnique<MCPResourceReloadCommand>( resourceReloadDocument, _resourceContext );
            ConstString resourceReloadName = STRINGIZE_STRING_LOCAL( "resource_reload" );

            if( _registry->addCommand( resourceReloadName, resourceReloadCommand ) == false )
            {
                return false;
            }

            DocumentInterfacePtr resourceRevertDocument = MENGINE_DOCUMENT_FUNCTION;
            MCPResourceRevertCommandPtr resourceRevertCommand = Helper::makeFactorableUnique<MCPResourceRevertCommand>( resourceRevertDocument, _resourceContext );
            ConstString resourceRevertName = STRINGIZE_STRING_LOCAL( "resource_revert" );

            if( _registry->addCommand( resourceRevertName, resourceRevertCommand ) == false )
            {
                return false;
            }

#if defined(MENGINE_BUILD_MENGINE_SCRIPT_EMBEDDED)
            DocumentInterfacePtr scriptReloadDocument = MENGINE_DOCUMENT_FUNCTION;
            MCPScriptReloadModuleCommandPtr scriptReloadCommand = Helper::makeFactorableUnique<MCPScriptReloadModuleCommand>( scriptReloadDocument, _scriptContext, _debuggerContext );
            ConstString scriptReloadName = STRINGIZE_STRING_LOCAL( "script_reload_module" );

            if( _registry->addCommand( scriptReloadName, scriptReloadCommand ) == false )
            {
                return false;
            }

            DocumentInterfacePtr scriptModulesDocument = MENGINE_DOCUMENT_FUNCTION;
            MCPScriptModulesCommandPtr scriptModulesCommand = Helper::makeFactorableUnique<MCPScriptModulesCommand>( scriptModulesDocument );
            ConstString scriptModulesName = STRINGIZE_STRING_LOCAL( "script_modules" );

            if( _registry->addCommand( scriptModulesName, scriptModulesCommand ) == false )
            {
                return false;
            }

            DocumentInterfacePtr scriptSourceDocument = MENGINE_DOCUMENT_FUNCTION;
            MCPScriptSourceCommandPtr scriptSourceCommand = Helper::makeFactorableUnique<MCPScriptSourceCommand>( scriptSourceDocument, _scriptContext );
            ConstString scriptSourceName = STRINGIZE_STRING_LOCAL( "script_source" );

            if( _registry->addCommand( scriptSourceName, scriptSourceCommand ) == false )
            {
                return false;
            }

            DocumentInterfacePtr scriptInspectDocument = MENGINE_DOCUMENT_FUNCTION;
            MCPScriptInspectCommandPtr scriptInspectCommand = Helper::makeFactorableUnique<MCPScriptInspectCommand>( scriptInspectDocument, _scriptContext );
            ConstString scriptInspectName = STRINGIZE_STRING_LOCAL( "script_inspect" );

            if( _registry->addCommand( scriptInspectName, scriptInspectCommand ) == false )
            {
                return false;
            }

            DocumentInterfacePtr scriptGetDocument = MENGINE_DOCUMENT_FUNCTION;
            MCPScriptGetCommandPtr scriptGetCommand = Helper::makeFactorableUnique<MCPScriptGetCommand>( scriptGetDocument, _scriptContext );
            ConstString scriptGetName = STRINGIZE_STRING_LOCAL( "script_get" );

            if( _registry->addCommand( scriptGetName, scriptGetCommand ) == false )
            {
                return false;
            }

            DocumentInterfacePtr scriptSetDocument = MENGINE_DOCUMENT_FUNCTION;
            MCPScriptSetCommandPtr scriptSetCommand = Helper::makeFactorableUnique<MCPScriptSetCommand>( scriptSetDocument, _scriptContext );
            ConstString scriptSetName = STRINGIZE_STRING_LOCAL( "script_set" );

            if( _registry->addCommand( scriptSetName, scriptSetCommand ) == false )
            {
                return false;
            }

            DocumentInterfacePtr scriptCallDocument = MENGINE_DOCUMENT_FUNCTION;
            MCPScriptCallCommandPtr scriptCallCommand = Helper::makeFactorableUnique<MCPScriptCallCommand>( scriptCallDocument, _handlerRegistry, _scriptContext );
            ConstString scriptCallName = STRINGIZE_STRING_LOCAL( "script_call" );

            if( _registry->addCommand( scriptCallName, scriptCallCommand ) == false )
            {
                return false;
            }

            DocumentInterfacePtr scriptReleaseDocument = MENGINE_DOCUMENT_FUNCTION;
            MCPScriptReleaseCommandPtr scriptReleaseCommand = Helper::makeFactorableUnique<MCPScriptReleaseCommand>( scriptReleaseDocument, _scriptContext );
            ConstString scriptReleaseName = STRINGIZE_STRING_LOCAL( "script_release" );

            if( _registry->addCommand( scriptReleaseName, scriptReleaseCommand ) == false )
            {
                return false;
            }

            DocumentInterfacePtr scriptEvalDocument = MENGINE_DOCUMENT_FUNCTION;
            MCPScriptEvalCommandPtr scriptEvalCommand = Helper::makeFactorableUnique<MCPScriptEvalCommand>( scriptEvalDocument, _scriptContext, _debuggerContext );
            ConstString scriptEvalName = STRINGIZE_STRING_LOCAL( "script_eval" );

            if( _registry->addCommand( scriptEvalName, scriptEvalCommand ) == false )
            {
                return false;
            }

            DocumentInterfacePtr scriptExecDocument = MENGINE_DOCUMENT_FUNCTION;
            MCPScriptExecCommandPtr scriptExecCommand = Helper::makeFactorableUnique<MCPScriptExecCommand>( scriptExecDocument, _scriptContext, _debuggerContext );
            ConstString scriptExecName = STRINGIZE_STRING_LOCAL( "script_exec" );

            if( _registry->addCommand( scriptExecName, scriptExecCommand ) == false )
            {
                return false;
            }

            DocumentInterfacePtr debugSetBreakpointsDocument = MENGINE_DOCUMENT_FUNCTION;
            MCPDebugSetBreakpointsCommandPtr debugSetBreakpointsCommand = Helper::makeFactorableUnique<MCPDebugSetBreakpointsCommand>( debugSetBreakpointsDocument, _debuggerContext );
            ConstString debugSetBreakpointsName = STRINGIZE_STRING_LOCAL( "debug_set_breakpoints" );

            if( _registry->addCommand( debugSetBreakpointsName, debugSetBreakpointsCommand ) == false )
            {
                return false;
            }

            DocumentInterfacePtr debugSetExceptionPolicyDocument = MENGINE_DOCUMENT_FUNCTION;
            MCPDebugSetExceptionPolicyCommandPtr debugSetExceptionPolicyCommand = Helper::makeFactorableUnique<MCPDebugSetExceptionPolicyCommand>( debugSetExceptionPolicyDocument, _debuggerContext );
            ConstString debugSetExceptionPolicyName = STRINGIZE_STRING_LOCAL( "debug_set_exception_policy" );

            if( _registry->addCommand( debugSetExceptionPolicyName, debugSetExceptionPolicyCommand ) == false )
            {
                return false;
            }

            DocumentInterfacePtr debugPauseDocument = MENGINE_DOCUMENT_FUNCTION;
            MCPDebugPauseCommandPtr debugPauseCommand = Helper::makeFactorableUnique<MCPDebugPauseCommand>( debugPauseDocument, _debuggerContext );
            ConstString debugPauseName = STRINGIZE_STRING_LOCAL( "debug_pause" );

            if( _registry->addCommand( debugPauseName, debugPauseCommand ) == false )
            {
                return false;
            }

            DocumentInterfacePtr debugContinueDocument = MENGINE_DOCUMENT_FUNCTION;
            MCPDebugContinueCommandPtr debugContinueCommand = Helper::makeFactorableUnique<MCPDebugContinueCommand>( debugContinueDocument, _debuggerContext );
            ConstString debugContinueName = STRINGIZE_STRING_LOCAL( "debug_continue" );

            if( _registry->addCommand( debugContinueName, debugContinueCommand ) == false )
            {
                return false;
            }

            DocumentInterfacePtr debugStepDocument = MENGINE_DOCUMENT_FUNCTION;
            MCPDebugStepCommandPtr debugStepCommand = Helper::makeFactorableUnique<MCPDebugStepCommand>( debugStepDocument, _debuggerContext );
            ConstString debugStepName = STRINGIZE_STRING_LOCAL( "debug_step" );

            if( _registry->addCommand( debugStepName, debugStepCommand ) == false )
            {
                return false;
            }

            DocumentInterfacePtr debugStackDocument = MENGINE_DOCUMENT_FUNCTION;
            MCPDebugStackCommandPtr debugStackCommand = Helper::makeFactorableUnique<MCPDebugStackCommand>( debugStackDocument, _debuggerContext );
            ConstString debugStackName = STRINGIZE_STRING_LOCAL( "debug_stack" );

            if( _registry->addCommand( debugStackName, debugStackCommand ) == false )
            {
                return false;
            }

            DocumentInterfacePtr debugScopesDocument = MENGINE_DOCUMENT_FUNCTION;
            MCPDebugScopesCommandPtr debugScopesCommand = Helper::makeFactorableUnique<MCPDebugScopesCommand>( debugScopesDocument, _debuggerContext, _scriptContext );
            ConstString debugScopesName = STRINGIZE_STRING_LOCAL( "debug_scopes" );

            if( _registry->addCommand( debugScopesName, debugScopesCommand ) == false )
            {
                return false;
            }

            DocumentInterfacePtr debugVariablesDocument = MENGINE_DOCUMENT_FUNCTION;
            MCPDebugVariablesCommandPtr debugVariablesCommand = Helper::makeFactorableUnique<MCPDebugVariablesCommand>( debugVariablesDocument, _scriptContext );
            ConstString debugVariablesName = STRINGIZE_STRING_LOCAL( "debug_variables" );

            if( _registry->addCommand( debugVariablesName, debugVariablesCommand ) == false )
            {
                return false;
            }

            DocumentInterfacePtr debugEvaluateDocument = MENGINE_DOCUMENT_FUNCTION;
            MCPDebugEvaluateCommandPtr debugEvaluateCommand = Helper::makeFactorableUnique<MCPDebugEvaluateCommand>( debugEvaluateDocument, _debuggerContext, _scriptContext );
            ConstString debugEvaluateName = STRINGIZE_STRING_LOCAL( "debug_evaluate" );

            if( _registry->addCommand( debugEvaluateName, debugEvaluateCommand ) == false )
            {
                return false;
            }

            DocumentInterfacePtr debugSetVariableDocument = MENGINE_DOCUMENT_FUNCTION;
            MCPDebugSetVariableCommandPtr debugSetVariableCommand = Helper::makeFactorableUnique<MCPDebugSetVariableCommand>( debugSetVariableDocument, _debuggerContext, _scriptContext );
            ConstString debugSetVariableName = STRINGIZE_STRING_LOCAL( "debug_set_variable" );

            if( _registry->addCommand( debugSetVariableName, debugSetVariableCommand ) == false )
            {
                return false;
            }
#endif

            return true;
        }
        //////////////////////////////////////////////////////////////////////////
    }
    //////////////////////////////////////////////////////////////////////////
}
