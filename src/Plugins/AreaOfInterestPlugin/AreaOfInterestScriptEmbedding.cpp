#include "AreaOfInterestScriptEmbedding.h"

#include "Interface/VocabularyServiceInterface.h"
#include "Interface/ScriptWrapperInterface.h"

#include "Environment/Python/PythonEventReceiver.h"
#include "Environment/Python/PythonScriptWrapper.h"

#include "Frameworks/PythonFramework/DocumentTraceback.h"

#include "AreaOfInterestZone.h"
#include "NodeAreaOfInterestTrigger.h"
#include "NodeAreaOfInterestActor.h"

#include "Kernel/Document.h"
#include "Kernel/DocumentHelper.h"
#include "Kernel/ConstStringHelper.h"
#include "Kernel/FactoryPool.h"
#include "Kernel/AssertionFactory.h"
#include "Kernel/AssertionMemoryPanic.h"

#include "pybind/pybind.hpp"

namespace Mengine
{
    //////////////////////////////////////////////////////////////////////////
    namespace Detail
    {
        //////////////////////////////////////////////////////////////////////////
        class PythonNodeAreaOfInterestTriggerEventReceiver
            : public PythonEventReceiver
            , public NodeAreaOfInterestTriggerEventReceiverInterface
            , public Factorable
        {
        public:
            PythonNodeAreaOfInterestTriggerEventReceiver()
            {
            }

            ~PythonNodeAreaOfInterestTriggerEventReceiver() override
            {
            }

        public:
            void onAreaOfInterestTriggerEnter( const AreaOfInterestActorProviderInterfacePtr & _enemy, uint32_t _iff1, uint32_t _iff2 )
            {
                NodeAreaOfInterestActorPtr enemy = stdex::intrusive_static_cast<NodeAreaOfInterestActorPtr>(_enemy);

                m_cb.call( enemy, _iff1, _iff2 );
            }

            void onAreaOfInterestTriggerLeave( const AreaOfInterestActorProviderInterfacePtr & _enemy, uint32_t _iff1, uint32_t _iff2 )
            {
                NodeAreaOfInterestActorPtr enemy = stdex::intrusive_static_cast<NodeAreaOfInterestActorPtr>(_enemy);

                m_cb.call( enemy, _iff1, _iff2 );
            }
        };
        //////////////////////////////////////////////////////////////////////////
        static PyObject * s_NodeAOITrigger_setEventListener( pybind::kernel_interface * _kernel, NodeAreaOfInterestTrigger * _node, PyObject * _args, PyObject * _kwds )
        {
            MENGINE_UNUSED( _args );

            MENGINE_ASSERTION_MEMORY_PANIC( _kwds, _kernel->ret_none(), "invalid set event listener" );

            pybind::dict py_kwds( _kernel, _kwds );

            Helper::registerPythonEventReceiver<Detail::PythonNodeAreaOfInterestTriggerEventReceiver>( _kernel, py_kwds, _node, "onTriggerEnter", EVENT_AREAOFINTEREST_TRIGGER_ENTER, MENGINE_DOCUMENT_PYBIND );
            Helper::registerPythonEventReceiver<Detail::PythonNodeAreaOfInterestTriggerEventReceiver>( _kernel, py_kwds, _node, "onTriggerLeave", EVENT_AREAOFINTEREST_TRIGGER_LEAVE, MENGINE_DOCUMENT_PYBIND );

            MENGINE_ASSERTION_PYTHON_EVENT_RECEIVER( _node, py_kwds );

            return _kernel->ret_none();
        }
        //////////////////////////////////////////////////////////////////////////
        static AreaOfInterestZoneInterfacePtr s_createAreaOfInterestZone()
        {
            AreaOfInterestZoneInterfacePtr zone = AREAOFINTEREST_SERVICE()
                ->createZone( MENGINE_DOCUMENT_PYBIND );

            return zone;
        }
    }
    //////////////////////////////////////////////////////////////////////////
    AreaOfInterestScriptEmbedding::AreaOfInterestScriptEmbedding()
    {
    }
    //////////////////////////////////////////////////////////////////////////
    AreaOfInterestScriptEmbedding::~AreaOfInterestScriptEmbedding()
    {
    }    
    //////////////////////////////////////////////////////////////////////////
    bool AreaOfInterestScriptEmbedding::embedding( pybind::kernel_interface * _kernel )
    {
        pybind::interface_<AreaOfInterestZoneInterface, pybind::bases<Mixin>>( _kernel, "AreaOfInterestZoneInterface" )
            .def( "freeze", &AreaOfInterestZoneInterface::freeze )
            .def( "isFreeze", &AreaOfInterestZoneInterface::isFreeze )
            ;

        pybind::interface_<AreaOfInterestZone, pybind::bases<AreaOfInterestZoneInterface, Scriptable> >( _kernel, "AreaOfInterestZone", false )
            ;

        pybind::interface_<NodeAreaOfInterestTrigger, pybind::bases<Node, Eventable> >( _kernel, "NodeAOITrigger", false )
            .def( "setRadius", &NodeAreaOfInterestTrigger::setRadius )
            .def( "getRadius", &NodeAreaOfInterestTrigger::getRadius )
            .def( "setIFF", &NodeAreaOfInterestTrigger::setIFF )
            .def( "getIFF", &NodeAreaOfInterestTrigger::getIFF )
            .def( "setAreaOfInterestZone", &NodeAreaOfInterestTrigger::setAreaOfInterestZone )
            .def( "getAreaOfInterestZone", &NodeAreaOfInterestTrigger::getAreaOfInterestZone )
            .def( "setTriggerUserData", &NodeAreaOfInterestTrigger::setTriggerUserData )
            .def( "getTriggerUserData", &NodeAreaOfInterestTrigger::getTriggerUserData )
            .def_static_native_kernel( "setEventListener", &Detail::s_NodeAOITrigger_setEventListener )
            ;

        pybind::interface_<NodeAreaOfInterestActor, pybind::bases<Node> >( _kernel, "NodeAOIActor", false )
            .def( "setRadius", &NodeAreaOfInterestActor::setRadius )
            .def( "getRadius", &NodeAreaOfInterestActor::getRadius )
            .def( "setIFF", &NodeAreaOfInterestActor::setIFF )
            .def( "getIFF", &NodeAreaOfInterestActor::getIFF )
            .def( "setAreaOfInterestZone", &NodeAreaOfInterestActor::setAreaOfInterestZone )
            .def( "getAreaOfInterestZone", &NodeAreaOfInterestActor::getAreaOfInterestZone )
            .def( "setActorUserData", &NodeAreaOfInterestActor::setActorUserData )
            .def( "getActorUserData", &NodeAreaOfInterestActor::getActorUserData )
            ;

        pybind::def_function( _kernel, "createAreaOfInterestZone", &Detail::s_createAreaOfInterestZone );

        VOCABULARY_SET( ScriptWrapperInterface, STRINGIZE_STRING_LOCAL( "ClassWrapping" ), STRINGIZE_STRING_LOCAL( "NodeAreaOfInterestTrigger" ), Helper::makeFactorableUnique<PythonScriptWrapper<NodeAreaOfInterestTrigger> >( MENGINE_DOCUMENT_FACTORABLE, _kernel ) );
        VOCABULARY_SET( ScriptWrapperInterface, STRINGIZE_STRING_LOCAL( "ClassWrapping" ), STRINGIZE_STRING_LOCAL( "NodeAreaOfInterestActor" ), Helper::makeFactorableUnique<PythonScriptWrapper<NodeAreaOfInterestActor> >( MENGINE_DOCUMENT_FACTORABLE, _kernel ) );

        return true;
    }
    //////////////////////////////////////////////////////////////////////////
    void AreaOfInterestScriptEmbedding::ejecting( pybind::kernel_interface * _kernel )
    {
        _kernel->remove_scope<AreaOfInterestZoneInterface>();
        _kernel->remove_scope<AreaOfInterestZone>();
        _kernel->remove_scope<NodeAreaOfInterestTrigger>();
        _kernel->remove_scope<NodeAreaOfInterestActor>();

        VOCABULARY_REMOVE( STRINGIZE_STRING_LOCAL( "ClassWrapping" ), STRINGIZE_STRING_LOCAL( "NodeAreaOfInterestTrigger" ) );
        VOCABULARY_REMOVE( STRINGIZE_STRING_LOCAL( "ClassWrapping" ), STRINGIZE_STRING_LOCAL( "NodeAreaOfInterestActor" ) );
    }
}

