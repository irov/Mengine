#include "VideoScriptEmbedding.h"

#include "Interface/ScriptServiceInterface.h"
#include "Interface/VocabularyServiceInterface.h"
#include "Interface/StringizeServiceInterface.h"

#include "Environment/Python/PythonAnimatableEventReceiver.h"
#include "Environment/Python/PythonScriptWrapper.h"

#include "ResourceVideo.h"
#include "SurfaceVideo.h"

#include "Kernel/Logger.h"
#include "Kernel/ScriptWrapperInterface.h"

#include "pybind/pybind.hpp"

namespace Mengine
{
    //////////////////////////////////////////////////////////////////////////
    namespace Detail
    {
        //////////////////////////////////////////////////////////////////////////
        static PyObject * s_SurfaceVideo_setEventListener( pybind::kernel_interface * _kernel, SurfaceVideo * _surface, PyObject * _args, PyObject * _kwds )
        {
            MENGINE_UNUSED( _args );

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
    VideoScriptEmbedding::VideoScriptEmbedding()
    {
    }
    //////////////////////////////////////////////////////////////////////////
    VideoScriptEmbedding::~VideoScriptEmbedding()
    {
    }
    //////////////////////////////////////////////////////////////////////////
    bool VideoScriptEmbedding::embedding( pybind::kernel_interface * _kernel )
    {
        pybind::interface_<ResourceVideo, pybind::bases<Resource, Content> >( _kernel, "ResourceVideo", false )
            ;

        pybind::interface_<SurfaceVideo, pybind::bases<Surface, Eventable, Animatable> >( _kernel, "SurfaceVideo", false )
            .def( "setResourceVideo", &SurfaceVideo::setResourceVideo )
            .def( "getResourceVideo", &SurfaceVideo::getResourceVideo )
            .def( "getWidth", &SurfaceVideo::getWidth )
            .def( "getHeight", &SurfaceVideo::getHeight )
            .def_static_native_kernel( "setEventListener", &Detail::s_SurfaceVideo_setEventListener )
            ;

        VOCABULARY_SET( ScriptWrapperInterface, STRINGIZE_STRING_LOCAL( "ClassWrapping" ), STRINGIZE_STRING_LOCAL( "ResourceVideo" ), Helper::makeFactorableUnique<PythonScriptWrapper<ResourceVideo> >( _kernel ) );
        VOCABULARY_SET( ScriptWrapperInterface, STRINGIZE_STRING_LOCAL( "ClassWrapping" ), STRINGIZE_STRING_LOCAL( "SurfaceVideo" ), Helper::makeFactorableUnique<PythonScriptWrapper<SurfaceVideo> >( _kernel ) );

        return true;
    }
    //////////////////////////////////////////////////////////////////////////
    void VideoScriptEmbedding::ejecting( pybind::kernel_interface * _kernel )
    {
        _kernel->remove_scope<ResourceVideo>();
        _kernel->remove_scope<SurfaceVideo>();

        VOCABULARY_REMOVE( STRINGIZE_STRING_LOCAL( "ClassWrapping" ), STRINGIZE_STRING_LOCAL( "ResourceVideo" ) );
        VOCABULARY_REMOVE( STRINGIZE_STRING_LOCAL( "ClassWrapping" ), STRINGIZE_STRING_LOCAL( "SurfaceVideo" ) );
    }
}
