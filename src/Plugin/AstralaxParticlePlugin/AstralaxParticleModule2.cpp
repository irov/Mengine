#include "AstralaxParticleModule2.h"

#include "Interface/ParticleSystemInterface.h"
#include "Interface/PrototypeManagerInterface.h"
#include "Interface/ScriptSystemInterface.h"

#include "ResourceParticle.h"
#include "ParticleEmitter2.h"

#include "PythonScriptWrapper/PythonAnimatableEventReceiver.h"

#include "Kernel/NodePrototypeGenerator.h"
#include "Kernel/ResourcePrototypeGenerator.h"
#include "Kernel/ScriptWrapper.h"
#include "Kernel/Logger.h"

#include "pybind/pybind.hpp"

//////////////////////////////////////////////////////////////////////////
namespace Mengine
{
    //////////////////////////////////////////////////////////////////////////
    namespace Detail
    {
        ////////////////////////////////////////////////////////////////////////////        
        PyObject * s_ParticleEmitter2_setEventListener( pybind::kernel_interface * _kernel, ParticleEmitter2 * _node, PyObject * _args, PyObject * _kwds )
        {
            (void)_args;

            if( _kwds == nullptr )
            {
                return pybind::ret_none();
            }

            pybind::dict py_kwds( _kernel, _kwds );
            Helper::registerAnimatableEventReceiver<>( py_kwds, _node );

#ifndef NDEBUG
            if( py_kwds.empty() == false )
            {
                for( pybind::dict::iterator
                    it = py_kwds.begin(),
                    it_end = py_kwds.end();
                    it != it_end;
                    ++it )
                {
                    std::string k = it.key();

                    LOGGER_ERROR( "ParticleEmitter2::setEventListener invalid kwds '%s'\n"
                        , k.c_str()
                    );
                }

                throw;
            }
#endif

            return pybind::ret_none();
        }
    }
	//////////////////////////////////////////////////////////////////////////
	AstralaxParticleModule2::AstralaxParticleModule2()
	{
	}
    //////////////////////////////////////////////////////////////////////////
    AstralaxParticleModule2::~AstralaxParticleModule2()
    {
    }
	//////////////////////////////////////////////////////////////////////////
	bool AstralaxParticleModule2::_initialize()
	{
		LOGGER_INFO( "Initializing Particle System ..." );

        if( SCRIPT_SERVICE()
            ->setWrapper( STRINGIZE_STRING_LOCAL( "ParticleEmitter2" ), new ScriptWrapper<ParticleEmitter2>() ) == false )
        {
            return false;
        }
        
        if( PROTOTYPE_SERVICE()
            ->addPrototype( STRINGIZE_STRING_LOCAL( "Node" ), STRINGIZE_STRING_LOCAL( "ParticleEmitter2" ), new NodePrototypeGenerator<ParticleEmitter2, 128> ) == false )
        {
            return false;
        }

        if( SCRIPT_SERVICE()
            ->setWrapper( STRINGIZE_STRING_LOCAL( "ResourceParticle" ), new ScriptWrapper<ResourceParticle>() ) == false )
        {
            return false;
        }

        if( PROTOTYPE_SERVICE()
            ->addPrototype( STRINGIZE_STRING_LOCAL( "Resource" ), STRINGIZE_STRING_LOCAL( "ResourceParticle" ), new ResourcePrototypeGenerator<ResourceParticle, 128> ) == false )
        {
            return false;
        }

        pybind::kernel_interface * kernel = pybind::get_kernel();

        pybind::interface_<ParticleEmitter2, pybind::bases<Node, Eventable, Animatable> >( kernel, "ParticleEmitter2", false )
            .def( "setResourceParticle", &ParticleEmitter2::setResourceParticle )
            .def( "getResourceParticle", &ParticleEmitter2::getResourceParticle )

            .def( "setEmitterTranslateWithParticle", &ParticleEmitter2::setEmitterTranslateWithParticle )
            .def( "setEmitterPositionRelative", &ParticleEmitter2::setEmitterPositionRelative )
            .def( "setEmitterCameraRelative", &ParticleEmitter2::setEmitterCameraRelative )
            .def( "setEmitterPositionProviderOriginOffset", &ParticleEmitter2::setEmitterPositionProviderOriginOffset )

            .def( "changeEmitterImage", &ParticleEmitter2::changeEmitterImage )
            .def( "removeEmitterImage", &ParticleEmitter2::removeEmitterImage )
            .def( "changeEmitterPolygon", &ParticleEmitter2::changeEmitterPolygon )
            .def( "removeEmitterPolygon", &ParticleEmitter2::removeEmitterPolygon )

            .def( "getDuration", &ParticleEmitter2::getDuration )

            .def( "setEmitterRandomMode", &ParticleEmitter2::setEmitterRandomMode )
            .def( "getEmitterRandomMode", &ParticleEmitter2::getEmitterRandomMode )
            .def_static_native_kernel( "setEventListener", &Detail::s_ParticleEmitter2_setEventListener )
            ;

		return true;
	}
	//////////////////////////////////////////////////////////////////////////
	void AstralaxParticleModule2::_finalize()
	{
        SCRIPT_SERVICE()
            ->removeWrapper( STRINGIZE_STRING_LOCAL( "ParticleEmitter2" ) );

        PROTOTYPE_SERVICE()
            ->removePrototype( STRINGIZE_STRING_LOCAL( "Node" ), STRINGIZE_STRING_LOCAL( "ParticleEmitter2" ) );

        SCRIPT_SERVICE()
            ->removeWrapper( STRINGIZE_STRING_LOCAL( "ResourceParticle" ) );

        PROTOTYPE_SERVICE()
            ->removePrototype( STRINGIZE_STRING_LOCAL( "Resource" ), STRINGIZE_STRING_LOCAL( "ResourceParticle" ) );

        pybind::kernel_interface * kernel = pybind::get_kernel();

        kernel->remove_scope<ParticleEmitter2>();
	}
	//////////////////////////////////////////////////////////////////////////
	void AstralaxParticleModule2::_destroy()
	{
	}
}