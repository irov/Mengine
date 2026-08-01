#pragma once

namespace Mengine
{
    class MCPCommandHostInterface;
    class MCPCommandRegistry;
    class MCPRuntimeContext;
    class MCPSceneContext;
    class MCPInputContext;
    class MCPResourceContext;
    class MCPWaitConditionRegistry;
    class MCPInputSequenceStepRegistry;
    class MCPHandlerRegistry;
    class MCPScriptContext;
    class MCPDebuggerContext;

    //////////////////////////////////////////////////////////////////////////
    namespace Helper
    {
        bool composeMCPCommands( MCPCommandHostInterface * _host, MCPRuntimeContext * _runtimeContext, MCPSceneContext * _sceneContext, MCPInputContext * _inputContext, MCPResourceContext * _resourceContext, MCPHandlerRegistry * _handlerRegistry, MCPScriptContext * _scriptContext, MCPDebuggerContext * _debuggerContext, MCPWaitConditionRegistry * _waitRegistry, MCPInputSequenceStepRegistry * _stepRegistry, MCPCommandRegistry * _registry );
    }
    //////////////////////////////////////////////////////////////////////////
}
