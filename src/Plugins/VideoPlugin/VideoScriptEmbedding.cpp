#include "VideoScriptEmbedding.h"

#include "Interface/ScriptServiceInterface.h"
#include "Interface/VocabularyServiceInterface.h"

#include "Environment/Python/PythonIncluder.h"
#include "Environment/Python/PythonAnimatableEventReceiver.h"
#include "Environment/Python/PythonScriptWrapper.h"
#include "Environment/Python/PythonDocumentTraceback.h"

#include "ResourceVideo.h"
#include "SurfaceVideo.h"
#include "SurfaceMockupVideo.h"

#include "Kernel/Logger.h"
#include "Kernel/AssertionMemoryPanic.h"
#include "Kernel/ConstStringHelper.h"
#include "Kernel/ShapeQuadFixed.h"
#include "Kernel/PrototypeHelper.h"

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
        ShapePtr s_createVideo( const ConstString & _name, const ConstString & _shapeType, const ResourceVideoPtr & _resource )
        {
            MENGINE_ASSERTION_MEMORY_PANIC( _resource, "'%s' resource is nullptr"
                , _name.c_str()
            );

            SurfaceVideoPtr surface = Helper::generateFactorable<Surface, SurfaceVideo>( MENGINE_DOCUMENT_PYBIND );

            MENGINE_ASSERTION_MEMORY_PANIC( surface );

            surface->setName( _name );
            surface->setResourceVideo( _resource );

            ShapePtr shape = Helper::generatePrototype( Node::getFactorableType(), _shapeType, MENGINE_DOCUMENT_PYBIND );

            MENGINE_ASSERTION_MEMORY_PANIC( shape );

            shape->setName( _name );
            shape->setSurface( surface );

            return shape;
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
        SCRIPT_SERVICE()
            ->setAvailablePlugin( "Video", true );

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

        pybind::def_function( _kernel, "createVideo", &Detail::s_createVideo );

        Helper::registerScriptWrapping<ResourceVideo>( _kernel, MENGINE_DOCUMENT_FACTORABLE );
        Helper::registerScriptWrapping<SurfaceVideo>( _kernel, MENGINE_DOCUMENT_FACTORABLE );
        Helper::registerScriptWrapping<SurfaceMockupVideo>( _kernel, MENGINE_DOCUMENT_FACTORABLE );

        return true;
    }
    //////////////////////////////////////////////////////////////////////////
    void VideoScriptEmbedding::eject( pybind::kernel_interface * _kernel )
    {
        _kernel->remove_scope<ResourceVideo>();
        _kernel->remove_scope<SurfaceVideo>();
        _kernel->remove_scope<SurfaceMockupVideo>();

        Helper::unregisterScriptWrapping<ResourceVideo>();
        Helper::unregisterScriptWrapping<SurfaceVideo>();
        Helper::unregisterScriptWrapping<SurfaceMockupVideo>();
    }
    //////////////////////////////////////////////////////////////////////////
}
