#include "VideoPlugin.h"

#include "Interface/PrototypeServiceInterface.h"
#include "Interface/StringizeServiceInterface.h"
#include "Interface/LoaderServiceInterface.h"
#include "Interface/VocabularyServiceInterface.h"
#include "Interface/ConfigServiceInterface.h"

#include "Plugins/ResourceValidatePlugin/ResourceValidateServiceInterface.h"

#include "SurfaceVideo.h"
#include "ResourceVideoValidator.h"

#include "LoaderResourceVideo.h"

#include "Environment/Python/PythonAnimatableEventReceiver.h"
#include "Environment/Python/PythonScriptWrapper.h"

#include "Kernel/ResourcePrototypeGenerator.h"
#include "Kernel/SurfacePrototypeGenerator.h"

//////////////////////////////////////////////////////////////////////////
PLUGIN_FACTORY( Video, Mengine::VideoPlugin );
//////////////////////////////////////////////////////////////////////////
namespace Mengine
{
    //////////////////////////////////////////////////////////////////////////
    VideoPlugin::VideoPlugin()
    {
    }
    //////////////////////////////////////////////////////////////////////////
    VideoPlugin::~VideoPlugin()
    {
    }
    //////////////////////////////////////////////////////////////////////////
    namespace Detail
    {
        //////////////////////////////////////////////////////////////////////////
        static PyObject * s_SurfaceVideo_setEventListener( pybind::kernel_interface * _kernel, SurfaceVideo * _surface, PyObject * _args, PyObject * _kwds )
        {
            (void)_args;

            if( _kwds == nullptr )
            {
                return _kernel->ret_none();
            }

            pybind::dict py_kwds( _kernel, _kwds );
            Helper::registerAnimatableEventReceiver<>( _kernel, py_kwds, _surface );

#ifdef MENGINE_DEBUG
            if( py_kwds.empty() == false )
            {
                for( pybind::dict::iterator
                    it = py_kwds.begin(),
                    it_end = py_kwds.end();
                    it != it_end;
                    ++it )
                {
                    String k = it.key();

                    LOGGER_ERROR( "surface '%s' invalid kwds '%s'"
                        , _surface->getName().c_str()
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
    bool VideoPlugin::_availablePlugin() const
    {
        bool available = CONFIG_VALUE( "Engine", "VideoPluginAvailable", true );

        return available;
    }
    //////////////////////////////////////////////////////////////////////////
    bool VideoPlugin::_initializePlugin()
    {
        pybind::kernel_interface * kernel = SCRIPT_SERVICE()
            ->getKernel();

        pybind::set_kernel( kernel );

        pybind::interface_<ResourceVideo, pybind::bases<Resource, Content> >( kernel, "ResourceVideo", false )
            ;

        pybind::interface_<SurfaceVideo, pybind::bases<Surface, Eventable, Animatable> >( kernel, "SurfaceVideo", false )
            .def( "setResourceVideo", &SurfaceVideo::setResourceVideo )
            .def( "getResourceVideo", &SurfaceVideo::getResourceVideo )
            .def( "getWidth", &SurfaceVideo::getWidth )
            .def( "getHeight", &SurfaceVideo::getHeight )
            .def_static_native_kernel( "setEventListener", &Detail::s_SurfaceVideo_setEventListener )
            ;

        VOCALUBARY_SET( ScriptWrapperInterface, STRINGIZE_STRING_LOCAL( "ClassWrapping" ), STRINGIZE_STRING_LOCAL( "ResourceVideo" ), Helper::makeFactorableUnique<PythonScriptWrapper<ResourceVideo> >( kernel ) );
        VOCALUBARY_SET( ScriptWrapperInterface, STRINGIZE_STRING_LOCAL( "ClassWrapping" ), STRINGIZE_STRING_LOCAL( "SurfaceVideo" ), Helper::makeFactorableUnique<PythonScriptWrapper<SurfaceVideo> >( kernel ) );

        if( PROTOTYPE_SERVICE()
            ->addPrototype( STRINGIZE_STRING_LOCAL( "Resource" ), STRINGIZE_STRING_LOCAL( "ResourceVideo" ), Helper::makeFactorableUnique<ResourcePrototypeGenerator<ResourceVideo, 128>>() ) == false )
        {
            return false;
        }

        if( PROTOTYPE_SERVICE()
            ->addPrototype( STRINGIZE_STRING_LOCAL( "Surface" ), STRINGIZE_STRING_LOCAL( "SurfaceVideo" ), Helper::makeFactorableUnique<SurfacePrototypeGenerator<SurfaceVideo, 128>>() ) == false )
        {
            return false;
        }

        SERVICE_WAIT( ResourceValidateServiceInterface, []()
        {
            VOCALUBARY_SET( ResourceValidatorInterface, STRINGIZE_STRING_LOCAL( "Validator" ), STRINGIZE_STRING_LOCAL( "ResourceVideo" ), Helper::makeFactorableUnique<ResourceVideoValidator>() );
        } );

        SERVICE_WAIT( LoaderServiceInterface, []()
        {
            VOCALUBARY_SET( LoaderInterface, STRINGIZE_STRING_LOCAL( "Loader" ), STRINGIZE_STRING_LOCAL( "ResourceVideo" ), Helper::makeFactorableUnique<LoaderResourceVideo>() );
        } );

        return true;
    }
    //////////////////////////////////////////////////////////////////////////
    void VideoPlugin::_finalizePlugin()
    {
        PROTOTYPE_SERVICE()
            ->removePrototype( STRINGIZE_STRING_LOCAL( "Resource" ), STRINGIZE_STRING_LOCAL( "ResourceVideo" ) );

        PROTOTYPE_SERVICE()
            ->removePrototype( STRINGIZE_STRING_LOCAL( "Surface" ), STRINGIZE_STRING_LOCAL( "SurfaceVideo" ) );

        VOCALUBARY_REMOVE( STRINGIZE_STRING_LOCAL( "ClassWrapping" ), STRINGIZE_STRING_LOCAL( "ResourceVideo" ) );
        VOCALUBARY_REMOVE( STRINGIZE_STRING_LOCAL( "ClassWrapping" ), STRINGIZE_STRING_LOCAL( "SurfaceVideo" ) );

        SERVICE_WAIT( ResourceValidateServiceInterface, []()
        {
            VOCALUBARY_REMOVE( STRINGIZE_STRING_LOCAL( "Validator" ), STRINGIZE_STRING_LOCAL( "ResourceVideo" ) );
        } );

        SERVICE_WAIT( LoaderServiceInterface, []()
        {
            VOCALUBARY_REMOVE( STRINGIZE_STRING_LOCAL( "Loader" ), STRINGIZE_STRING_LOCAL( "ResourceVideo" ) );
        } );
    }
}