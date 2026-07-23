#include "FigmaScriptEmbedding.h"

#include "Figma.h"
#include "ResourceFigma.h"

#include "Environment/Python/PythonIncluder.h"
#include "Environment/Python/PythonScriptWrapper.h"
#include "Environment/Python/PythonEventReceiver.h"
#include "Environment/Python/PythonDocument.h"

#include "Kernel/AssertionMemoryPanic.h"
#include "Kernel/ConstStringHelper.h"

namespace Mengine
{
    //////////////////////////////////////////////////////////////////////////
    namespace Detail
    {
        //////////////////////////////////////////////////////////////////////////
        class PythonFigmaEventReceiver
            : public PythonEventReceiver
            , public FigmaEventReceiverInterface
            , public Factorable
        {
        public:
            void onFigmaTrigger( const FigmaTriggerEvent & _event ) override
            {
                m_cb.call( _event );
            }

            void onFigmaAction( const FigmaActionEvent & _event, FigmaActionResponse * const _response ) override
            {
                m_cb.call( _event, _response );
            }

            void onFigmaFrameChanged( const String & _previousFrameId, const String & _currentFrameId ) override
            {
                m_cb.call( _previousFrameId, _currentFrameId );
            }

            void onFigmaOverlayOpened( const String & _frameId ) override
            {
                m_cb.call( _frameId );
            }

            void onFigmaOverlayClosed( const String & _frameId ) override
            {
                m_cb.call( _frameId );
            }

            void onFigmaStateChanged( const String & _sourceNodeId, const String & _previousStateId, const String & _currentStateId ) override
            {
                m_cb.call( _sourceNodeId, _previousStateId, _currentStateId );
            }
        };
        //////////////////////////////////////////////////////////////////////////
        static PyObject * Figma_setEventListener( pybind::kernel_interface * _kernel, Figma * _node, PyObject * _args, PyObject * _kwds )
        {
            MENGINE_UNUSED( _args );

            MENGINE_ASSERTION_MEMORY_PANIC( _kwds, "invalid Figma set event listener" );

            pybind::dict pyKwds( _kernel, _kwds );
            Helper::registerPythonEventReceiver<PythonFigmaEventReceiver>( _kernel, pyKwds, _node, STRINGIZE_STRING_LOCAL( "onFigmaTrigger" ), EVENT_FIGMA_TRIGGER, MENGINE_DOCUMENT_PYTHON );
            Helper::registerPythonEventReceiver<PythonFigmaEventReceiver>( _kernel, pyKwds, _node, STRINGIZE_STRING_LOCAL( "onFigmaAction" ), EVENT_FIGMA_ACTION, MENGINE_DOCUMENT_PYTHON );
            Helper::registerPythonEventReceiver<PythonFigmaEventReceiver>( _kernel, pyKwds, _node, STRINGIZE_STRING_LOCAL( "onFigmaFrameChanged" ), EVENT_FIGMA_FRAME_CHANGED, MENGINE_DOCUMENT_PYTHON );
            Helper::registerPythonEventReceiver<PythonFigmaEventReceiver>( _kernel, pyKwds, _node, STRINGIZE_STRING_LOCAL( "onFigmaOverlayOpened" ), EVENT_FIGMA_OVERLAY_OPENED, MENGINE_DOCUMENT_PYTHON );
            Helper::registerPythonEventReceiver<PythonFigmaEventReceiver>( _kernel, pyKwds, _node, STRINGIZE_STRING_LOCAL( "onFigmaOverlayClosed" ), EVENT_FIGMA_OVERLAY_CLOSED, MENGINE_DOCUMENT_PYTHON );
            Helper::registerPythonEventReceiver<PythonFigmaEventReceiver>( _kernel, pyKwds, _node, STRINGIZE_STRING_LOCAL( "onFigmaStateChanged" ), EVENT_FIGMA_STATE_CHANGED, MENGINE_DOCUMENT_PYTHON );

            MENGINE_ASSERTION_PYTHON_EVENT_RECEIVER( _node, pyKwds );

            return _kernel->ret_none();
        }
        //////////////////////////////////////////////////////////////////////////
    }
    //////////////////////////////////////////////////////////////////////////
    FigmaScriptEmbedding::FigmaScriptEmbedding()
    {
    }
    //////////////////////////////////////////////////////////////////////////
    FigmaScriptEmbedding::~FigmaScriptEmbedding()
    {
    }
    //////////////////////////////////////////////////////////////////////////
    bool FigmaScriptEmbedding::embed( pybind::kernel_interface * _kernel )
    {
        pybind::enum_<::Figma::EActionInputKind>( _kernel, "FigmaActionInputKind" )
            .def( "FIGMA_ACTION_INPUT_POINTER", ::Figma::EActionInputKind::Pointer )
            .def( "FIGMA_ACTION_INPUT_KEY", ::Figma::EActionInputKind::Key )
            .def( "FIGMA_ACTION_INPUT_TIMER", ::Figma::EActionInputKind::Timer )
            .def( "FIGMA_ACTION_INPUT_PROGRAMMATIC", ::Figma::EActionInputKind::Programmatic )
            ;

        pybind::enum_<::Figma::ETriggerType>( _kernel, "FigmaTriggerType" )
            .def( "FIGMA_TRIGGER_CLICK", ::Figma::ETriggerType::Click )
            .def( "FIGMA_TRIGGER_HOVER_ENTER", ::Figma::ETriggerType::HoverEnter )
            .def( "FIGMA_TRIGGER_HOVER_LEAVE", ::Figma::ETriggerType::HoverLeave )
            .def( "FIGMA_TRIGGER_PRESS", ::Figma::ETriggerType::Press )
            .def( "FIGMA_TRIGGER_POINTER_DOWN", ::Figma::ETriggerType::PointerDown )
            .def( "FIGMA_TRIGGER_POINTER_UP", ::Figma::ETriggerType::PointerUp )
            .def( "FIGMA_TRIGGER_AFTER_TIMEOUT", ::Figma::ETriggerType::AfterTimeout )
            .def( "FIGMA_TRIGGER_KEY_DOWN", ::Figma::ETriggerType::KeyDown )
            ;

        pybind::enum_<::Figma::EActionResult>( _kernel, "FigmaActionResult" )
            .def( "FIGMA_ACTION_RESULT_ALLOW_DEFAULT", ::Figma::EActionResult::AllowDefault )
            .def( "FIGMA_ACTION_RESULT_CONSUME", ::Figma::EActionResult::Consume )
            .def( "FIGMA_ACTION_RESULT_NAVIGATE_FRAME", ::Figma::EActionResult::NavigateFrame )
            .def( "FIGMA_ACTION_RESULT_OPEN_OVERLAY", ::Figma::EActionResult::OpenOverlay )
            .def( "FIGMA_ACTION_RESULT_CLOSE_OVERLAY", ::Figma::EActionResult::CloseOverlay )
            ;

        pybind::enum_<::Figma::EConnectionType>( _kernel, "FigmaConnectionType" )
            .def( "FIGMA_CONNECTION_NONE", ::Figma::EConnectionType::None )
            .def( "FIGMA_CONNECTION_INTERNAL_NODE", ::Figma::EConnectionType::InternalNode )
            .def( "FIGMA_CONNECTION_BACK", ::Figma::EConnectionType::Back )
            .def( "FIGMA_CONNECTION_CLOSE", ::Figma::EConnectionType::Close )
            ;

        pybind::enum_<::Figma::ENavigationType>( _kernel, "FigmaNavigationType" )
            .def( "FIGMA_NAVIGATION_NAVIGATE", ::Figma::ENavigationType::Navigate )
            .def( "FIGMA_NAVIGATION_OVERLAY", ::Figma::ENavigationType::Overlay )
            .def( "FIGMA_NAVIGATION_SWAP", ::Figma::ENavigationType::Swap )
            .def( "FIGMA_NAVIGATION_SCROLL_TO", ::Figma::ENavigationType::ScrollTo )
            ;

        pybind::enum_<EFigmaBindingValueType>( _kernel, "FigmaBindingValueType" )
            .def( "FIGMA_BINDING_VALUE_TEXT", EFigmaBindingValueType::Text )
            .def( "FIGMA_BINDING_VALUE_NUMBER", EFigmaBindingValueType::Number )
            .def( "FIGMA_BINDING_VALUE_BOOLEAN", EFigmaBindingValueType::Boolean )
            .def( "FIGMA_BINDING_VALUE_IMAGE", EFigmaBindingValueType::Image )
            ;

        pybind::struct_<FigmaBindingValue>( _kernel, "FigmaBindingValue" )
            .def_constructor( pybind::init<>() )
            .def_member( "type", &FigmaBindingValue::type )
            .def_member( "stringValue", &FigmaBindingValue::stringValue )
            .def_member( "numberValue", &FigmaBindingValue::numberValue )
            .def_member( "boolValue", &FigmaBindingValue::boolValue )
            ;

        pybind::struct_<FigmaTriggerEvent>( _kernel, "FigmaTriggerEvent" )
            .def_member( "inputKind", &FigmaTriggerEvent::inputKind )
            .def_member( "triggerType", &FigmaTriggerEvent::triggerType )
            .def_member( "interactionId", &FigmaTriggerEvent::interactionId )
            .def_member( "sourceNodeId", &FigmaTriggerEvent::sourceNodeId )
            .def_member( "currentFrameId", &FigmaTriggerEvent::currentFrameId )
            .def_member( "pointerId", &FigmaTriggerEvent::pointerId )
            .def_member( "x", &FigmaTriggerEvent::x )
            .def_member( "y", &FigmaTriggerEvent::y )
            .def_member( "button", &FigmaTriggerEvent::button )
            .def_member( "keyCode", &FigmaTriggerEvent::keyCode )
            .def_member( "modifiers", &FigmaTriggerEvent::modifiers )
            ;

        pybind::struct_<FigmaActionEvent>( _kernel, "FigmaActionEvent" )
            .def_member( "inputKind", &FigmaActionEvent::inputKind )
            .def_member( "triggerType", &FigmaActionEvent::triggerType )
            .def_member( "connectionType", &FigmaActionEvent::connectionType )
            .def_member( "navigationType", &FigmaActionEvent::navigationType )
            .def_member( "actionId", &FigmaActionEvent::actionId )
            .def_member( "interactionId", &FigmaActionEvent::interactionId )
            .def_member( "sourceNodeId", &FigmaActionEvent::sourceNodeId )
            .def_member( "currentFrameId", &FigmaActionEvent::currentFrameId )
            .def_member( "targetFrameId", &FigmaActionEvent::targetFrameId )
            .def_member( "pointerId", &FigmaActionEvent::pointerId )
            .def_member( "x", &FigmaActionEvent::x )
            .def_member( "y", &FigmaActionEvent::y )
            .def_member( "button", &FigmaActionEvent::button )
            .def_member( "keyCode", &FigmaActionEvent::keyCode )
            .def_member( "modifiers", &FigmaActionEvent::modifiers )
            ;

        pybind::struct_<FigmaActionResponse>( _kernel, "FigmaActionResponse" )
            .def_constructor( pybind::init<>() )
            .def_member( "result", &FigmaActionResponse::result )
            .def_member( "targetFrameId", &FigmaActionResponse::targetFrameId )
            ;

        pybind::interface_<ResourceFigma, pybind::bases<Resource>>( _kernel, "ResourceFigma", false )
            ;

        pybind::interface_<Figma, pybind::bases<Node>>( _kernel, "Figma", false )
            .def( "setBindingText", &Figma::setBindingText )
            .def( "setBindingNumber", &Figma::setBindingNumber )
            .def( "setBindingVisible", &Figma::setBindingVisible )
            .def( "setBindingEnabled", &Figma::setBindingEnabled )
            .def( "setBindingImage", &Figma::setBindingImage )
            .def( "setBindingState", &Figma::setBindingState )
            .def( "setBindingValue", &Figma::setBindingValue )
            .def( "clearBindingValue", &Figma::clearBindingValue )
            .def( "navigateToFrame", &Figma::navigateToFrame )
            .def( "openOverlay", &Figma::openOverlay )
            .def( "closeOverlay", &Figma::closeOverlay )
            .def( "goBack", &Figma::goBack )
            .def_static_native_kernel( "setEventListener", &Detail::Figma_setEventListener )
            ;

        Helper::registerScriptWrapping<ResourceFigma>( _kernel, MENGINE_DOCUMENT_FACTORABLE );
        Helper::registerScriptWrapping<Figma>( _kernel, MENGINE_DOCUMENT_FACTORABLE );

        return true;
    }
    //////////////////////////////////////////////////////////////////////////
    void FigmaScriptEmbedding::eject( pybind::kernel_interface * _kernel )
    {
        _kernel->remove_from_module( "FIGMA_ACTION_INPUT_POINTER", nullptr );
        _kernel->remove_from_module( "FIGMA_ACTION_INPUT_KEY", nullptr );
        _kernel->remove_from_module( "FIGMA_ACTION_INPUT_TIMER", nullptr );
        _kernel->remove_from_module( "FIGMA_ACTION_INPUT_PROGRAMMATIC", nullptr );
        _kernel->remove_from_module( "FIGMA_TRIGGER_CLICK", nullptr );
        _kernel->remove_from_module( "FIGMA_TRIGGER_HOVER_ENTER", nullptr );
        _kernel->remove_from_module( "FIGMA_TRIGGER_HOVER_LEAVE", nullptr );
        _kernel->remove_from_module( "FIGMA_TRIGGER_PRESS", nullptr );
        _kernel->remove_from_module( "FIGMA_TRIGGER_POINTER_DOWN", nullptr );
        _kernel->remove_from_module( "FIGMA_TRIGGER_POINTER_UP", nullptr );
        _kernel->remove_from_module( "FIGMA_TRIGGER_AFTER_TIMEOUT", nullptr );
        _kernel->remove_from_module( "FIGMA_TRIGGER_KEY_DOWN", nullptr );
        _kernel->remove_from_module( "FIGMA_ACTION_RESULT_ALLOW_DEFAULT", nullptr );
        _kernel->remove_from_module( "FIGMA_ACTION_RESULT_CONSUME", nullptr );
        _kernel->remove_from_module( "FIGMA_ACTION_RESULT_NAVIGATE_FRAME", nullptr );
        _kernel->remove_from_module( "FIGMA_ACTION_RESULT_OPEN_OVERLAY", nullptr );
        _kernel->remove_from_module( "FIGMA_ACTION_RESULT_CLOSE_OVERLAY", nullptr );
        _kernel->remove_from_module( "FIGMA_CONNECTION_NONE", nullptr );
        _kernel->remove_from_module( "FIGMA_CONNECTION_INTERNAL_NODE", nullptr );
        _kernel->remove_from_module( "FIGMA_CONNECTION_BACK", nullptr );
        _kernel->remove_from_module( "FIGMA_CONNECTION_CLOSE", nullptr );
        _kernel->remove_from_module( "FIGMA_NAVIGATION_NAVIGATE", nullptr );
        _kernel->remove_from_module( "FIGMA_NAVIGATION_OVERLAY", nullptr );
        _kernel->remove_from_module( "FIGMA_NAVIGATION_SWAP", nullptr );
        _kernel->remove_from_module( "FIGMA_NAVIGATION_SCROLL_TO", nullptr );
        _kernel->remove_from_module( "FIGMA_BINDING_VALUE_TEXT", nullptr );
        _kernel->remove_from_module( "FIGMA_BINDING_VALUE_NUMBER", nullptr );
        _kernel->remove_from_module( "FIGMA_BINDING_VALUE_BOOLEAN", nullptr );
        _kernel->remove_from_module( "FIGMA_BINDING_VALUE_IMAGE", nullptr );

        _kernel->remove_scope<Figma>();
        _kernel->remove_scope<ResourceFigma>();
        _kernel->remove_scope<FigmaBindingValue>();
        _kernel->remove_scope<FigmaActionResponse>();
        _kernel->remove_scope<FigmaActionEvent>();
        _kernel->remove_scope<FigmaTriggerEvent>();

        Helper::unregisterScriptWrapping<Figma>();
        Helper::unregisterScriptWrapping<ResourceFigma>();
    }
    //////////////////////////////////////////////////////////////////////////
}
