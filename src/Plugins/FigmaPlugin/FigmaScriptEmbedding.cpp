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
            .def( "Pointer", ::Figma::EActionInputKind::Pointer )
            .def( "Key", ::Figma::EActionInputKind::Key )
            .def( "Timer", ::Figma::EActionInputKind::Timer )
            .def( "Programmatic", ::Figma::EActionInputKind::Programmatic )
            ;

        pybind::enum_<::Figma::ETriggerType>( _kernel, "FigmaTriggerType" )
            .def( "Click", ::Figma::ETriggerType::Click )
            .def( "HoverEnter", ::Figma::ETriggerType::HoverEnter )
            .def( "HoverLeave", ::Figma::ETriggerType::HoverLeave )
            .def( "Press", ::Figma::ETriggerType::Press )
            .def( "PointerDown", ::Figma::ETriggerType::PointerDown )
            .def( "PointerUp", ::Figma::ETriggerType::PointerUp )
            .def( "AfterTimeout", ::Figma::ETriggerType::AfterTimeout )
            .def( "KeyDown", ::Figma::ETriggerType::KeyDown )
            ;

        pybind::enum_<::Figma::EActionResult>( _kernel, "FigmaActionResult" )
            .def( "AllowDefault", ::Figma::EActionResult::AllowDefault )
            .def( "Consume", ::Figma::EActionResult::Consume )
            .def( "NavigateFrame", ::Figma::EActionResult::NavigateFrame )
            .def( "OpenOverlay", ::Figma::EActionResult::OpenOverlay )
            .def( "CloseOverlay", ::Figma::EActionResult::CloseOverlay )
            ;

        pybind::enum_<::Figma::EConnectionType>( _kernel, "FigmaConnectionType" )
            .def( "None", ::Figma::EConnectionType::None )
            .def( "InternalNode", ::Figma::EConnectionType::InternalNode )
            .def( "Back", ::Figma::EConnectionType::Back )
            .def( "Close", ::Figma::EConnectionType::Close )
            ;

        pybind::enum_<::Figma::ENavigationType>( _kernel, "FigmaNavigationType" )
            .def( "Navigate", ::Figma::ENavigationType::Navigate )
            .def( "Overlay", ::Figma::ENavigationType::Overlay )
            .def( "Swap", ::Figma::ENavigationType::Swap )
            .def( "ScrollTo", ::Figma::ENavigationType::ScrollTo )
            ;

        pybind::enum_<EFigmaBindingValueType>( _kernel, "FigmaBindingValueType" )
            .def( "Text", EFigmaBindingValueType::Text )
            .def( "Number", EFigmaBindingValueType::Number )
            .def( "Boolean", EFigmaBindingValueType::Boolean )
            .def( "Image", EFigmaBindingValueType::Image )
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
