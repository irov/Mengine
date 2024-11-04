#include "AreaOfInterestScriptEmbedding.h"

#include "Environment/Python/PythonIncluder.h"
#include "Environment/Python/PythonEventReceiver.h"
#include "Environment/Python/PythonScriptWrapper.h"
#include "Environment/Python/PythonDocumentTraceback.h"

#include "AreaOfInterestZone.h"
#include "NodeAreaOfInterestTrigger.h"
#include "NodeAreaOfInterestActor.h"

#include "Kernel/DocumentHelper.h"
#include "Kernel/ConstStringHelper.h"
#include "Kernel/FactoryPool.h"
#include "Kernel/AssertionFactory.h"
#include "Kernel/AssertionMemoryPanic.h"

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
            void onAreaOfInterestTriggerEnter( const AreaOfInterestActorProviderInterfacePtr & _enemy, uint32_t _iff1, uint32_t _iff2 ) override
            {
                NodeAreaOfInterestActorPtr enemy = NodeAreaOfInterestActorPtr::from( _enemy );

                m_cb.call( enemy, _iff1, _iff2 );
            }

            void onAreaOfInterestTriggerLeave( const AreaOfInterestActorProviderInterfacePtr & _enemy, uint32_t _iff1, uint32_t _iff2 ) override
            {
                NodeAreaOfInterestActorPtr enemy = NodeAreaOfInterestActorPtr::from( _enemy );

                m_cb.call( enemy, _iff1, _iff2 );
            }
        };
        //////////////////////////////////////////////////////////////////////////
        static PyObject * NodeAOITrigger_setEventListener(pybind::kernel_interface * _kernel, NodeAreaOfInterestTrigger * _node, PyObject * _args, PyObject * _kwds )
        {
            MENGINE_UNUSED( _args );

            MENGINE_ASSERTION_MEMORY_PANIC( _kwds, "invalid set event listener" );

            pybind::dict py_kwds( _kernel, _kwds );

            Helper::registerPythonEventReceiver<Detail::PythonNodeAreaOfInterestTriggerEventReceiver>( _kernel, py_kwds, _node, STRINGIZE_STRING_LOCAL( "onTriggerEnter" ), EVENT_AREAOFINTEREST_TRIGGER_ENTER, MENGINE_DOCUMENT_PYBIND );
            Helper::registerPythonEventReceiver<Detail::PythonNodeAreaOfInterestTriggerEventReceiver>( _kernel, py_kwds, _node, STRINGIZE_STRING_LOCAL( "onTriggerLeave" ), EVENT_AREAOFINTEREST_TRIGGER_LEAVE, MENGINE_DOCUMENT_PYBIND );

            MENGINE_ASSERTION_PYTHON_EVENT_RECEIVER( _node, py_kwds );

            return _kernel->ret_none();
        }
        //////////////////////////////////////////////////////////////////////////
        static AreaOfInterestZoneInterfacePtr createAreaOfInterestZone()
        {
            AreaOfInterestZoneInterfacePtr zone = AREAOFINTEREST_SERVICE()
                ->createZone( MENGINE_DOCUMENT_PYBIND );

            return zone;
        }
        //////////////////////////////////////////////////////////////////////////
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
    bool AreaOfInterestScriptEmbedding::embed( pybind::kernel_interface * _kernel )
    {
        SCRIPT_SERVICE()
            ->setAvailablePlugin( "AreaOfInterest", true );

        pybind::interface_<AreaOfInterestZoneInterface, pybind::bases<Mixin>>( _kernel, "AreaOfInterestZoneInterface" )
            .def( "freeze", &AreaOfInterestZoneInterface::freeze )
            .def( "isFreeze", &AreaOfInterestZoneInterface::isFreeze )
            ;

        pybind::interface_<AreaOfInterestZone, pybind::bases<AreaOfInterestZoneInterface, Scriptable>>( _kernel, "AreaOfInterestZone", false )
            ;

        pybind::interface_<NodeAreaOfInterestTrigger, pybind::bases<Node>>( _kernel, "NodeAOITrigger", false )
            .def( "setRadius", &NodeAreaOfInterestTrigger::setRadius )
            .def( "getRadius", &NodeAreaOfInterestTrigger::getRadius )
            .def( "setIFF", &NodeAreaOfInterestTrigger::setIFF )
            .def( "getIFF", &NodeAreaOfInterestTrigger::getIFF )
            .def( "setAreaOfInterestZone", &NodeAreaOfInterestTrigger::setAreaOfInterestZone )
            .def( "getAreaOfInterestZone", &NodeAreaOfInterestTrigger::getAreaOfInterestZone )
            .def( "setTriggerUserData", &NodeAreaOfInterestTrigger::setTriggerUserData )
            .def( "getTriggerUserData", &NodeAreaOfInterestTrigger::getTriggerUserData )
            .def_static_native_kernel( "setEventListener", &Detail::NodeAOITrigger_setEventListener )
            ;

        pybind::interface_<NodeAreaOfInterestActor, pybind::bases<Node>>( _kernel, "NodeAOIActor", false )
            .def( "setRadius", &NodeAreaOfInterestActor::setRadius )
            .def( "getRadius", &NodeAreaOfInterestActor::getRadius )
            .def( "setIFF", &NodeAreaOfInterestActor::setIFF )
            .def( "getIFF", &NodeAreaOfInterestActor::getIFF )
            .def( "setAreaOfInterestZone", &NodeAreaOfInterestActor::setAreaOfInterestZone )
            .def( "getAreaOfInterestZone", &NodeAreaOfInterestActor::getAreaOfInterestZone )
            .def( "setActorUserData", &NodeAreaOfInterestActor::setActorUserData )
            .def( "getActorUserData", &NodeAreaOfInterestActor::getActorUserData )
            ;

        pybind::def_function( _kernel, "createAreaOfInterestZone", &Detail::createAreaOfInterestZone );

        Helper::registerScriptWrapping<NodeAreaOfInterestTrigger>( _kernel, MENGINE_DOCUMENT_FACTORABLE );
        Helper::registerScriptWrapping<NodeAreaOfInterestActor>( _kernel, MENGINE_DOCUMENT_FACTORABLE );

        return true;
    }
    //////////////////////////////////////////////////////////////////////////
    void AreaOfInterestScriptEmbedding::eject( pybind::kernel_interface * _kernel )
    {
        _kernel->remove_scope<AreaOfInterestZoneInterface>();
        _kernel->remove_scope<AreaOfInterestZone>();
        _kernel->remove_scope<NodeAreaOfInterestTrigger>();
        _kernel->remove_scope<NodeAreaOfInterestActor>();

        Helper::unregisterScriptWrapping<NodeAreaOfInterestTrigger>();
        Helper::unregisterScriptWrapping<NodeAreaOfInterestActor>();
    }
}

