#include "VideoPlugin.h"

#include "Interface/PrototypeServiceInterface.h"
#include "Interface/StringizeInterface.h"
#include "Interface/LoaderServiceInterface.h"

#include "Plugins/ResourceValidatePlugin/ResourceValidateInterface.h"

#include "SurfaceVideo.h"
#include "VideoResourceValidateVisitor.h"

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
            Helper::registerAnimatableEventReceiver<>( py_kwds, _surface );

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

                    LOGGER_ERROR( "SurfaceVideo::setEventListener invalid kwds '%s'\n"
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
    bool VideoPlugin::_initialize()
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
            .def( "getDuration", &SurfaceVideo::getDuration )
            .def_static_native_kernel( "setEventListener", &Detail::s_SurfaceVideo_setEventListener )
            ;

        if( PROTOTYPE_SERVICE()
            ->addPrototype( STRINGIZE_STRING_LOCAL( "Resource" ), STRINGIZE_STRING_LOCAL( "ResourceVideo" ), new FactorableUnique<ResourcePrototypeGenerator<ResourceVideo, 128> > ) == false )
        {
            return false;
        }

        if( PROTOTYPE_SERVICE()
            ->addPrototype( STRINGIZE_STRING_LOCAL( "Surface" ), STRINGIZE_STRING_LOCAL( "SurfaceVideo" ), new FactorableUnique<SurfacePrototypeGenerator<SurfaceVideo, 128> > ) == false )
        {
            return false;
        }

        if( SCRIPT_SERVICE()
            ->setWrapper( STRINGIZE_STRING_LOCAL( "ResourceVideo" ), new FactorableUnique<PythonScriptWrapper<ResourceVideo> >( kernel ) ) == false )
        {
            return false;
        }

        if( SCRIPT_SERVICE()
            ->setWrapper( STRINGIZE_STRING_LOCAL( "SurfaceVideo" ), new FactorableUnique<PythonScriptWrapper<SurfaceVideo> >( kernel ) ) == false )
        {
            return false;
        }

        if( SERVICE_EXIST( ResourceValidateServiceInterface ) == true )
        {
            VisitorPtr videoValidateVisitor = new FactorableUnique<VideoResourceValidateVisitor>();

            RESOURCEVALIDATE_SERVICE()
                ->addResourceValidateVisitor( videoValidateVisitor );

            m_videoValidateVisitor = videoValidateVisitor;
        }

        LOADER_SERVICE()
            ->addLoader( STRINGIZE_STRING_LOCAL( "ResourceVideo" ), new FactorableUnique<LoaderResourceVideo>() );

        return true;
    }
    //////////////////////////////////////////////////////////////////////////
    void VideoPlugin::_finalize()
    {
        if( PROTOTYPE_SERVICE()
            ->removePrototype( STRINGIZE_STRING_LOCAL( "Resource" ), STRINGIZE_STRING_LOCAL( "ResourceVideo" ) ) == false )
        {
            return;
        }

        if( PROTOTYPE_SERVICE()
            ->removePrototype( STRINGIZE_STRING_LOCAL( "Surface" ), STRINGIZE_STRING_LOCAL( "SurfaceVideo" ) ) == false )
        {
            return;
        }

        if( SCRIPT_SERVICE()
            ->removeWrapper( STRINGIZE_STRING_LOCAL( "ResourceVideo" ) ) == false )
        {
            return;
        }

        if( SCRIPT_SERVICE()
            ->removeWrapper( STRINGIZE_STRING_LOCAL( "SurfaceVideo" ) ) == false )
        {
            return;
        }

        if( SERVICE_EXIST( ResourceValidateServiceInterface ) == true )
        {
            RESOURCEVALIDATE_SERVICE()
                ->removeResourceValidateVisitor( m_videoValidateVisitor );

            m_videoValidateVisitor = nullptr;
        }

        LOADER_SERVICE()
            ->removeLoader( STRINGIZE_STRING_LOCAL( "ResourceVideo" ) );
    }
}