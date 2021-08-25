#include "VideoScriptEmbedding.h"

#include "Interface/ScriptServiceInterface.h"
#include "Interface/VocabularyServiceInterface.h"

#include "Environment/Python/PythonAnimatableEventReceiver.h"
#include "Environment/Python/PythonScriptWrapper.h"
#include "Environment/Python/PythonDocumentTraceback.h"

#include "ResourceVideo.h"
#include "SurfaceVideo.h"
#include "SurfaceMockupVideo.h"

#include "Kernel/Logger.h"
#include "Kernel/AssertionMemoryPanic.h"
#include "Kernel/ConstStringHelper.h"

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

            MENGINE_ASSERTION_MEMORY_PANIC( _kwds );

            pybind::dict py_kwds( _kernel, _kwds );
            Helper::registerAnimatableEventReceiver<>( _kernel, py_kwds, _surface, MENGINE_DOCUMENT_PYBIND );

            MENGINE_ASSERTION_PYTHON_EVENT_RECEIVER( _surface, py_kwds );

            return _kernel->ret_none();
        }
        //////////////////////////////////////////////////////////////////////////
        static PyObject * s_SurfaceMockupVideo_setEventListener( pybind::kernel_interface * _kernel, SurfaceMockupVideo * _surface, PyObject * _args, PyObject * _kwds )
        {
            MENGINE_UNUSED( _args );

            MENGINE_ASSERTION_MEMORY_PANIC( _kwds );

            pybind::dict py_kwds( _kernel, _kwds );
            Helper::registerAnimatableEventReceiver<>( _kernel, py_kwds, _surface, MENGINE_DOCUMENT_PYBIND );

            MENGINE_ASSERTION_PYTHON_EVENT_RECEIVER( _surface, py_kwds );

            return _kernel->ret_none();
        }
        //////////////////////////////////////////////////////////////////////////
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
    bool VideoScriptEmbedding::embed( pybind::kernel_interface * _kernel )
    {
        pybind::interface_<ResourceVideo, pybind::bases<Resource>>( _kernel, "ResourceVideo", false )
            .def( "setFrameRate", &ResourceVideo::setFrameRate )
            .def( "getFrameRate", &ResourceVideo::getFrameRate )
            .def( "setDuration", &ResourceVideo::setDuration )
            .def( "getDuration", &ResourceVideo::getDuration )
            .def( "setResize", &ResourceVideo::setResize )
            .def( "getResize", &ResourceVideo::getResize )
            .def( "setAlpha", &ResourceVideo::setAlpha )
            .def( "isAlpha", &ResourceVideo::isAlpha )
            .def( "setPremultiply", &ResourceVideo::setPremultiply )
            .def( "isPremultiply", &ResourceVideo::isPremultiply )
            .def( "setNoSeek", &ResourceVideo::setNoSeek )
            .def( "isNoSeek", &ResourceVideo::isNoSeek )
            ;

        pybind::interface_<SurfaceVideo, pybind::bases<Surface>>( _kernel, "SurfaceVideo", false )
            .def( "setResourceVideo", &SurfaceVideo::setResourceVideo )
            .def( "getResourceVideo", &SurfaceVideo::getResourceVideo )
            .def( "getWidth", &SurfaceVideo::getWidth )
            .def( "getHeight", &SurfaceVideo::getHeight )
            .def_static_native_kernel( "setEventListener", &Detail::s_SurfaceVideo_setEventListener )
            ;

        pybind::interface_<SurfaceMockupVideo, pybind::bases<Surface>>( _kernel, "SurfaceMockupVideo", false )
            .def( "setResourceVideo", &SurfaceMockupVideo::setResourceVideo )
            .def( "getResourceVideo", &SurfaceMockupVideo::getResourceVideo )
            .def( "getWidth", &SurfaceMockupVideo::getWidth )
            .def( "getHeight", &SurfaceMockupVideo::getHeight )
            .def_static_native_kernel( "setEventListener", &Detail::s_SurfaceMockupVideo_setEventListener )
            ;

        Helper::registerScriptWrapping<ResourceVideo>( _kernel, STRINGIZE_STRING_LOCAL( "ResourceVideo" ), MENGINE_DOCUMENT_FACTORABLE );
        Helper::registerScriptWrapping<SurfaceVideo>( _kernel, STRINGIZE_STRING_LOCAL( "SurfaceVideo" ), MENGINE_DOCUMENT_FACTORABLE );
        Helper::registerScriptWrapping<SurfaceMockupVideo>( _kernel, STRINGIZE_STRING_LOCAL( "SurfaceMockupVideo" ), MENGINE_DOCUMENT_FACTORABLE );

        return true;
    }
    //////////////////////////////////////////////////////////////////////////
    void VideoScriptEmbedding::eject( pybind::kernel_interface * _kernel )
    {
        _kernel->remove_scope<ResourceVideo>();
        _kernel->remove_scope<SurfaceVideo>();
        _kernel->remove_scope<SurfaceMockupVideo>();

        Helper::unregisterScriptWrapping( STRINGIZE_STRING_LOCAL( "ResourceVideo" ) );
        Helper::unregisterScriptWrapping( STRINGIZE_STRING_LOCAL( "SurfaceVideo" ) );
        Helper::unregisterScriptWrapping( STRINGIZE_STRING_LOCAL( "SurfaceMockupVideo" ) );
    }
    //////////////////////////////////////////////////////////////////////////
}
