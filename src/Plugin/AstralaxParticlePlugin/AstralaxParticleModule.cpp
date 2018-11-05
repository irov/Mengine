#include "AstralaxParticleModule.h"

#include "AstralaxInterface.h"

#include "Interface/PrototypeServiceInterface.h"
#include "Interface/StringizeInterface.h"
#include "Interface/ScriptSystemInterface.h"
#include "Interface/LoaderServiceInterface.h"

#include "ResourceAstralax.h"
#include "LoaderResourceAstralax.h"

#include "AstralaxEmitter.h"

#include "Plugin/ResourceValidatePlugin/ResourceValidateInterface.h"
#include "ParticleResourceValidateVisitor.h"

#include "Environment/Python/PythonAnimatableEventReceiver.h"
#include "Environment/Python/PythonScriptWrapper.h"

#include "Kernel/NodePrototypeGenerator.h"
#include "Kernel/ResourcePrototypeGenerator.h"
#include "Kernel/Logger.h"

#include "pybind/pybind.hpp"

//////////////////////////////////////////////////////////////////////////
namespace Mengine
{
    //////////////////////////////////////////////////////////////////////////
    namespace Detail
    {
        ////////////////////////////////////////////////////////////////////////////        
        static PyObject * s_ParticleEmitter2_setEventListener( pybind::kernel_interface * _kernel, AstralaxEmitter * _node, PyObject * _args, PyObject * _kwds )
        {
            (void)_args;

            if( _kwds == nullptr )
            {
                return _kernel->ret_none();
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
                    String k = it.key();

                    LOGGER_ERROR( "invalid kwds '%s'\n"
                        , k.c_str()
                    );
                }

                throw;
            }
#endif

            return _kernel->ret_none();
        }
    }
    //////////////////////////////////////////////////////////////////////////
    AstralaxParticleModule::AstralaxParticleModule()
    {
    }
    //////////////////////////////////////////////////////////////////////////
    AstralaxParticleModule::~AstralaxParticleModule()
    {
    }
    //////////////////////////////////////////////////////////////////////////
    bool AstralaxParticleModule::_initialize()
    {
        LOGGER_INFO( "Initializing Particle System ..." );

        pybind::kernel_interface * kernel = SCRIPT_SERVICE()
            ->getKernel();

        pybind::set_kernel( kernel );

        pybind::interface_<ResourceAstralax, pybind::bases<Resource, Content> >( kernel, "ResourceAstralax", false )
            .def( "getResourceImageCount", &ResourceAstralax::getResourceImageCount )
            .def( "getResourceImage", &ResourceAstralax::getResourceImage )
            ;

        pybind::interface_<AstralaxEmitter, pybind::bases<Node, Eventable, Animatable> >( kernel, "ParticleEmitter2", false )
            .def( "setResourceParticle", &AstralaxEmitter::setResourceParticle )
            .def( "getResourceParticle", &AstralaxEmitter::getResourceParticle )

            .def( "setEmitterTranslateWithParticle", &AstralaxEmitter::setEmitterTranslateWithParticle )
            .def( "setEmitterPositionRelative", &AstralaxEmitter::setEmitterPositionRelative )
            .def( "setEmitterCameraRelative", &AstralaxEmitter::setEmitterCameraRelative )
            .def( "setEmitterPositionProviderOriginOffset", &AstralaxEmitter::setEmitterPositionProviderOriginOffset )

            .def( "changeEmitterImage", &AstralaxEmitter::changeEmitterImage )
            .def( "removeEmitterImage", &AstralaxEmitter::removeEmitterImage )
            .def( "changeEmitterPolygon", &AstralaxEmitter::changeEmitterPolygon )
            .def( "removeEmitterPolygon", &AstralaxEmitter::removeEmitterPolygon )

            .def( "getDuration", &AstralaxEmitter::getDuration )

            .def( "setEmitterRandomMode", &AstralaxEmitter::setEmitterRandomMode )
            .def( "getEmitterRandomMode", &AstralaxEmitter::getEmitterRandomMode )
            .def_static_native_kernel( "setEventListener", &Detail::s_ParticleEmitter2_setEventListener )
            ;

        if( SCRIPT_SERVICE()
            ->setWrapper( STRINGIZE_STRING_LOCAL( "ParticleEmitter2" ), new FactorableUnique<PythonScriptWrapper<AstralaxEmitter> >( kernel ) ) == false )
        {
            return false;
        }

        if( SCRIPT_SERVICE()
            ->setWrapper( STRINGIZE_STRING_LOCAL( "AstralaxEmitter" ), new FactorableUnique<PythonScriptWrapper<AstralaxEmitter> >( kernel ) ) == false )
        {
            return false;
        }

        if( PROTOTYPE_SERVICE()
            ->addPrototype( STRINGIZE_STRING_LOCAL( "Node" ), STRINGIZE_STRING_LOCAL( "ParticleEmitter2" ), new FactorableUnique<NodePrototypeGenerator<AstralaxEmitter, 128> > ) == false )
        {
            return false;
        }

        if( PROTOTYPE_SERVICE()
            ->addPrototype( STRINGIZE_STRING_LOCAL( "Node" ), STRINGIZE_STRING_LOCAL( "AstralaxEmitter" ), new FactorableUnique<NodePrototypeGenerator<AstralaxEmitter, 128> > ) == false )
        {
            return false;
        }

        if( SCRIPT_SERVICE()
            ->setWrapper( STRINGIZE_STRING_LOCAL( "ResourceAstralax" ), new FactorableUnique<PythonScriptWrapper<ResourceAstralax> >( kernel ) ) == false )
        {
            return false;
        }


        if( SCRIPT_SERVICE()
            ->setWrapper( STRINGIZE_STRING_LOCAL( "ResourceParticle" ), new FactorableUnique<PythonScriptWrapper<ResourceAstralax> >( kernel ) ) == false )
        {
            return false;
        }

        if( PROTOTYPE_SERVICE()
            ->addPrototype( STRINGIZE_STRING_LOCAL( "Resource" ), STRINGIZE_STRING_LOCAL( "ResourceParticle" ), new FactorableUnique<ResourcePrototypeGenerator<ResourceAstralax, 128> > ) == false )
        {
            return false;
        }

        if( PROTOTYPE_SERVICE()
            ->addPrototype( STRINGIZE_STRING_LOCAL( "Resource" ), STRINGIZE_STRING_LOCAL( "ResourceAstralax" ), new FactorableUnique<ResourcePrototypeGenerator<ResourceAstralax, 128> > ) == false )
        {
            return false;
        }

        VisitorPtr particleValidateVisitor = new FactorableUnique<ParticleResourceValidateVisitor>();

        RESOURCEVALIDATE_SERVICE()
            ->addResourceValidateVisitor( particleValidateVisitor );

        m_particleValidateVisitor = particleValidateVisitor;

        LOADER_SERVICE()
            ->addLoader( STRINGIZE_STRING_LOCAL( "ResourceAstralax" ), new FactorableUnique<LoaderResourceAstralax>() );

        LOADER_SERVICE()
            ->addLoader( STRINGIZE_STRING_LOCAL( "ResourceParticle" ), new FactorableUnique<LoaderResourceAstralax>() );

        return true;
    }
    //////////////////////////////////////////////////////////////////////////
    void AstralaxParticleModule::_finalize()
    {
        SCRIPT_SERVICE()
            ->removeWrapper( STRINGIZE_STRING_LOCAL( "ParticleEmitter2" ) );

        SCRIPT_SERVICE()
            ->removeWrapper( STRINGIZE_STRING_LOCAL( "AstralaxEmitter" ) );        

        PROTOTYPE_SERVICE()
            ->removePrototype( STRINGIZE_STRING_LOCAL( "Node" ), STRINGIZE_STRING_LOCAL( "ParticleEmitter2" ) );

        PROTOTYPE_SERVICE()
            ->removePrototype( STRINGIZE_STRING_LOCAL( "Node" ), STRINGIZE_STRING_LOCAL( "AstralaxEmitter" ) );

        SCRIPT_SERVICE()
            ->removeWrapper( STRINGIZE_STRING_LOCAL( "ResourceParticle" ) );

        SCRIPT_SERVICE()
            ->removeWrapper( STRINGIZE_STRING_LOCAL( "ResourceAstralax" ) );

        PROTOTYPE_SERVICE()
            ->removePrototype( STRINGIZE_STRING_LOCAL( "Resource" ), STRINGIZE_STRING_LOCAL( "ResourceParticle" ) );

        PROTOTYPE_SERVICE()
            ->removePrototype( STRINGIZE_STRING_LOCAL( "Resource" ), STRINGIZE_STRING_LOCAL( "ResourceAstralax" ) );

        pybind::kernel_interface * kernel = pybind::get_kernel();

        kernel->remove_scope<ResourceAstralax>();
        kernel->remove_scope<AstralaxEmitter>();

        RESOURCEVALIDATE_SERVICE()
            ->removeResourceValidateVisitor( m_particleValidateVisitor );
        m_particleValidateVisitor = nullptr;

        LOADER_SERVICE()
            ->removeLoader( STRINGIZE_STRING_LOCAL( "ResourceAstralax" ) );

        LOADER_SERVICE()
            ->removeLoader( STRINGIZE_STRING_LOCAL( "ResourceParticle" ) );
    }
    //////////////////////////////////////////////////////////////////////////
    void AstralaxParticleModule::_destroy()
    {
    }
}