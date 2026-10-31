#include "Movie2ScriptEmbedding.h"

#include "Interface/ScriptServiceInterface.h"
#include "Interface/PluginServiceInterface.h"

#include "Environment/Python/PythonIncluder.h"
#include "Environment/Python/PythonAnimatableEventReceiver.h"
#include "Environment/Python/PythonScriptWrapper.h"
#include "Environment/Python/PythonDocument.h"

#include "Movie2.h"
#include "PythonMovie2EventReceiver.h"
#include "PythonMovie2MarkerEventReceiver.h"
#include "PythonMovie2SubCompositionEventReceiver.h"

#include "Kernel/Logger.h"
#include "Kernel/AssertionMemoryPanic.h"
#include "Kernel/ConstStringHelper.h"

#include "Config/StdString.h"

namespace Mengine
{
    //////////////////////////////////////////////////////////////////////////
    namespace Detail
    {
        //////////////////////////////////////////////////////////////////////////
        static VectorConstString movie2GetMotionParameters( const Movie2 * _movie )
        {
            VectorConstString names;
            _movie->getMotionParameters( &names );

            return names;
        }
        //////////////////////////////////////////////////////////////////////////
        static VectorConstString movie2GetMotionRegisters( const Movie2 * _movie )
        {
            VectorConstString names;
            _movie->getMotionRegisters( &names );

            return names;
        }
        //////////////////////////////////////////////////////////////////////////
        static PyObject * movie2SetEventListener( pybind::kernel_interface * _kernel, Movie2 * _node, PyObject * _args, PyObject * _kwds )
        {
            MENGINE_UNUSED( _args );

            MENGINE_ASSERTION_MEMORY_PANIC( _kwds, "kwds is nullptr" );

            pybind::dict pyKwds( _kernel, _kwds );
            Helper::registerAnimatableEventReceiver<PythonMovie2EventReceiver>( _kernel, pyKwds, _node, MENGINE_DOCUMENT_PYTHON );
            Helper::registerPythonEventReceiver<PythonMovie2MarkerEventReceiver>( _kernel, pyKwds, _node, STRINGIZE_STRING_LOCAL( "onMovieMarker" ), EVENT_MOVIE2_MARKER, MENGINE_DOCUMENT_PYTHON );

            MENGINE_ASSERTION_PYTHON_EVENT_RECEIVER( _node, pyKwds );

            PyObject * result = _kernel->ret_none();

            return result;
        }
        //////////////////////////////////////////////////////////////////////////
        static mt::box2f movie2GetCompositionBoundsWM( Movie2 * _movie )
        {
            const mt::box2f & bounds = _movie->getCompositionBounds();

            const TransformationInterface * transformation = _movie->getTransformation();

            const mt::mat4f & wm = transformation->getWorldMatrix();

            mt::box2f boundsWM;
            mt::box2_mul_m4( &boundsWM, bounds, wm );

            return boundsWM;
        }
        //////////////////////////////////////////////////////////////////////////
        static pybind::list movie2GetSockets( pybind::kernel_interface * _kernel, Movie2 * _movie )
        {
            pybind::list pyList( _kernel );

            _movie->foreachSockets( [_kernel, &pyList]( Movie2 * _movie, uint32_t _index, const NodePtr & _node )
            {
                MENGINE_UNUSED( _index );

                HotSpotPolygonPtr hotspot = HotSpotPolygonPtr::from( _node );

                if( hotspot == nullptr )
                {
                    return;
                }

                const ConstString & name = _node->getName();

                pybind::tuple item = pybind::make_tuple_t( _kernel, _movie, name, hotspot );
                pyList.append( item );
            } );

            return pyList;
        }
        //////////////////////////////////////////////////////////////////////////
        static pybind::list movie2GetSlots( pybind::kernel_interface * _kernel, Movie2 * _movie )
        {
            pybind::list pyList( _kernel );

            _movie->foreachSlots( [_kernel, &pyList]( Movie2 * _movie, uint32_t _index, const NodePtr & _node )
            {
                MENGINE_UNUSED( _index );

                Movie2SlotPtr slot = Movie2SlotPtr::from( _node );

                if( slot == nullptr )
                {
                    return;
                }

                const ConstString & name = _node->getName();

                pybind::tuple item = pybind::make_tuple_t( _kernel, _movie, name, slot );
                pyList.append( item );
            } );

            return pyList;
        }
        //////////////////////////////////////////////////////////////////////////
        static PyObject * movie2SubCompositionSetEventListener( pybind::kernel_interface * _kernel, Movie2SubComposition * _node, PyObject * _args, PyObject * _kwds )
        {
            MENGINE_UNUSED( _args );

            MENGINE_ASSERTION_MEMORY_PANIC( _kwds, "kwds is nullptr" );

            pybind::dict pyKwds( _kernel, _kwds );
            Helper::registerAnimatableEventReceiver<PythonMovie2SubCompositionEventReceiver>( _kernel, pyKwds, _node, MENGINE_DOCUMENT_PYTHON );

            MENGINE_ASSERTION_PYTHON_EVENT_RECEIVER( _node, pyKwds );

            PyObject * result = _kernel->ret_none();

            return result;
        }
        //////////////////////////////////////////////////////////////////////////
        static pybind::list resourceMovie2GetCompositionResources( pybind::kernel_interface * _kernel, ResourceMovie2 * _resourceMovie2, const ConstString & _compositionName )
        {
            const Movie2DataInterfacePtr & data = _resourceMovie2->getData();

            const aeMovieCompositionData * compositionData = _resourceMovie2->getCompositionData( _compositionName );

            if( compositionData == nullptr )
            {
                LOGGER_ERROR( "ResourceMovie2 '%s' not found composition '%s'"
                    , _resourceMovie2->getName().c_str()
                    , _compositionName.c_str()
                );

                pybind::list resources( _kernel );

                return resources;
            }

            pybind::list items = pybind::make_list_t( _kernel );

            data->foreachCompositionDataResources( compositionData, [_kernel, &items]( uint32_t _index, const Char * _name, const ResourcePtr & _resource )
            {
                pybind::dict item = pybind::make_dict_t( _kernel );

                item["index"] = _index;
                item["name"] = _name;
                item["resource"] = _resource;

                items.append( item );
            } );

            return items;
        }
        //////////////////////////////////////////////////////////////////////////
        static pybind::list resourceMovie2GetCompositionLayers( pybind::kernel_interface * _kernel, ResourceMovie2 * _resourceMovie2, const ConstString & _compositionName )
        {
            pybind::list items = pybind::make_list_t( _kernel );

            _resourceMovie2->foreachCompositionLayers( _compositionName, [_kernel, &items]( const ResourceMovie2::CompositionLayer & _layer )
            {
                pybind::dict item = pybind::make_dict_t( _kernel );

                item["index"] = _layer.index;
                item["name"] = _layer.name;
                item["type"] = _layer.type;
                item["matrix"] = _layer.matrix;
                item["color"] = _layer.color;
                item["dimension"] = _layer.dimension;
                item["trackmatte"] = _layer.trackMatte;

                pybind::list options = pybind::make_list_t( _kernel );

                for( uint32_t o : _layer.options )
                {
                    Char oStr[5];

                    oStr[0] = (Char)(o >> 24) & 0xff;
                    oStr[1] = (Char)(o >> 16) & 0xff;
                    oStr[2] = (Char)(o >> 8) & 0xff;
                    oStr[3] = (Char)(o >> 0) & 0xff;
                    oStr[4] = '\0';

                    oStr[0] = oStr[0] == '\0' ? ' ' : oStr[0];
                    oStr[1] = oStr[1] == '\0' ? ' ' : oStr[1];
                    oStr[2] = oStr[2] == '\0' ? ' ' : oStr[2];
                    oStr[3] = oStr[3] == '\0' ? ' ' : oStr[3];

                    Char * oStrValue = StdString::strrchr( oStr, ' ' );

                    if( oStrValue == nullptr )
                    {
                        options.append( oStr );
                    }
                    else
                    {
                        oStrValue++;

                        options.append( oStrValue );
                    }
                }

                item["options"] = options;

                items.append( item );
            } );

            return items;
        }
        //////////////////////////////////////////////////////////////////////////
        static pybind::list resourceMovie2GetCompositions( pybind::kernel_interface * _kernel, ResourceMovie2 * _resourceMovie2 )
        {
            pybind::list items = pybind::make_list_t( _kernel );

            _resourceMovie2->foreachCompositionsDesc( [_kernel, &items]( const ConstString & _name, const ResourceMovie2::CompositionDesc & _layer )
            {
                pybind::dict item = pybind::make_dict_t( _kernel );

                item["name"] = _name;
                item["duration"] = _layer.duration;
                item["frameDuration"] = _layer.frameDuration;
                item["has_bounds"] = _layer.hasBounds;
                item["bounds"] = _layer.bounds;
                item["master"] = _layer.master;

                items.append( item );
            } );

            return items;
        }
        //////////////////////////////////////////////////////////////////////////
        struct MovieLayerFramesDesc
        {
            pybind::kernel_interface * kernel;
            pybind::dict result;
            pybind::list frames;
            ae_float_t frameDuration;
            ae_float_t inTime;
        };
        //////////////////////////////////////////////////////////////////////////
        static ae_void_t movieLayerDataMatrixVisitor( ae_uint32_t _index, const aeMovieLayerDataMatrixCallbackData * _callbackData, ae_userdata_t _userdata )
        {
            MovieLayerFramesDesc * desc = reinterpret_cast<MovieLayerFramesDesc *>(_userdata);

            pybind::dict frame( desc->kernel );

            ae_float_t time = desc->inTime + _index * desc->frameDuration;

            frame["time"] = time;

            ae_color_channel_t r = _callbackData->r;
            ae_color_channel_t g = _callbackData->g;
            ae_color_channel_t b = _callbackData->b;
            ae_color_channel_t opacity = _callbackData->opacity;

            frame["color"] = Color( r, g, b, opacity );

            ae_float_t volume = _callbackData->volume;
            frame["volume"] = volume;

            ae_float_t ax = _callbackData->anchor_point[0];
            ae_float_t ay = _callbackData->anchor_point[1];

            frame["anchor_point"] = mt::vec2f( ax, ay );

            ae_float_t px = _callbackData->position[0];
            ae_float_t py = _callbackData->position[1];

            frame["position"] = mt::vec2f( px, py );

            ae_float_t sx = _callbackData->scale[0];
            ae_float_t sy = _callbackData->scale[1];

            frame["scale"] = mt::vec2f( sx, sy );

            ae_float_t quaternionz = _callbackData->quaternion[0];
            ae_float_t quaternionw = _callbackData->quaternion[1];

            frame["quaternion"] = mt::vec2f( quaternionz, quaternionw );

            desc->frames.append( frame );
        }
        //////////////////////////////////////////////////////////////////////////
        static ae_bool_t movieLayerDataVisitor( const aeMovieCompositionData * _compositionData, const aeMovieLayerData * _layerData, ae_userdata_t _userdata )
        {
            aeMovieLayerTypeEnum layerType = ae_get_movie_layer_data_type( _layerData );

            if( layerType != AE_MOVIE_LAYER_TYPE_SLOT )
            {
                return AE_TRUE;
            }

            ae_float_t frameDuration = ae_get_movie_composition_data_frame_duration( _compositionData );
            ae_float_t inTime = ae_get_movie_layer_data_in_time( _layerData );

            MovieLayerFramesDesc * desc = reinterpret_cast<MovieLayerFramesDesc *>(_userdata);

            desc->frames = pybind::list( desc->kernel );
            desc->frameDuration = frameDuration;
            desc->inTime = inTime;

            ae_visit_movie_layer_data_matrix( _layerData, &movieLayerDataMatrixVisitor, _userdata );

            const ae_char_t * layerName = ae_get_movie_layer_data_name( _layerData );

            desc->result[layerName] = desc->frames;

            return AE_TRUE;
        }
        //////////////////////////////////////////////////////////////////////////
        static pybind::dict getNullObjectsFromResourceMovie2( pybind::kernel_interface * _kernel, ResourceMovie2 * _resourceMovie2, const ConstString & _compositionName )
        {
            MENGINE_ASSERTION_MEMORY_PANIC( _resourceMovie2, "resource is nullptr" );

            const aeMovieCompositionData * compositionData = _resourceMovie2->getCompositionData( _compositionName );

            MENGINE_ASSERTION_MEMORY_PANIC( compositionData, "not found composition '%s'"
                , _compositionName.c_str()
            );

            MovieLayerFramesDesc desc;
            desc.result = pybind::dict( _kernel );
            desc.kernel = _kernel;

            ae_visit_composition_layer_data( compositionData, &movieLayerDataVisitor, &desc );

            return desc.result;
        }
        //////////////////////////////////////////////////////////////////////////
    }
    //////////////////////////////////////////////////////////////////////////
    Movie2ScriptEmbedding::Movie2ScriptEmbedding()
    {
    }
    //////////////////////////////////////////////////////////////////////////
    Movie2ScriptEmbedding::~Movie2ScriptEmbedding()
    {
    }
    //////////////////////////////////////////////////////////////////////////
    bool Movie2ScriptEmbedding::embed( pybind::kernel_interface * _kernel )
    {
        pybind::enum_<aeMovieMotionParameter>( _kernel, "Movie2MotionParameter" )
            .def( "AE_MOVIE_MOTION_PARAMETER_SPEED", AE_MOVIE_MOTION_PARAMETER_SPEED )
            .def( "AE_MOVIE_MOTION_PARAMETER_INTENSITY", AE_MOVIE_MOTION_PARAMETER_INTENSITY )
            .def( "AE_MOVIE_MOTION_PARAMETER_AMPLITUDE", AE_MOVIE_MOTION_PARAMETER_AMPLITUDE )
            .def( "AE_MOVIE_MOTION_PARAMETER_FREQUENCY", AE_MOVIE_MOTION_PARAMETER_FREQUENCY )
            .def( "AE_MOVIE_MOTION_PARAMETER_PHASE", AE_MOVIE_MOTION_PARAMETER_PHASE )
            .def( "AE_MOVIE_MOTION_PARAMETER_PROGRESS", AE_MOVIE_MOTION_PARAMETER_PROGRESS )
            .def( "AE_MOVIE_MOTION_PARAMETER_BLEND", AE_MOVIE_MOTION_PARAMETER_BLEND )
            .def( "AE_MOVIE_MOTION_PARAMETER_DIRECTION_X", AE_MOVIE_MOTION_PARAMETER_DIRECTION_X )
            .def( "AE_MOVIE_MOTION_PARAMETER_DIRECTION_Y", AE_MOVIE_MOTION_PARAMETER_DIRECTION_Y )
            .def( "AE_MOVIE_MOTION_PARAMETER_TARGET_X", AE_MOVIE_MOTION_PARAMETER_TARGET_X )
            .def( "AE_MOVIE_MOTION_PARAMETER_TARGET_Y", AE_MOVIE_MOTION_PARAMETER_TARGET_Y )
            .def( "AE_MOVIE_MOTION_PARAMETER_DAMPING", AE_MOVIE_MOTION_PARAMETER_DAMPING )
            .def( "AE_MOVIE_MOTION_PARAMETER_SEED", AE_MOVIE_MOTION_PARAMETER_SEED );

        pybind::interface_<Movie2, pybind::bases<Node>>( _kernel, "Movie2", false )
            .def( "setResourceMovie2", &Movie2::setResourceMovie2 )
            .def( "getResourceMovie2", &Movie2::getResourceMovie2 )
            .def( "setCompositionName", &Movie2::setCompositionName )
            .def( "getCompositionName", &Movie2::getCompositionName )
            .def( "setMotionParameter", &Movie2::setMotionParameter )
            .def( "getMotionParameter", &Movie2::getMotionParameter )
            .def_static( "getMotionParameters", &Detail::movie2GetMotionParameters )
            .def( "setMotionRegister", &Movie2::setMotionRegister )
            .def( "getMotionRegister", &Movie2::getMotionRegister )
            .def_static( "getMotionRegisters", &Detail::movie2GetMotionRegisters )
            .def( "setTextAliasEnvironment", &Movie2::setTextAliasEnvironment )
            .def( "getTextAliasEnvironment", &Movie2::getTextAliasEnvironment )
            .def( "setWorkAreaFromEvent", &Movie2::setWorkAreaFromEvent )
            .def( "removeWorkArea", &Movie2::removeWorkArea )
            .def( "hasSubComposition", &Movie2::hasSubComposition )
            .def( "getSubComposition", &Movie2::getSubComposition )
            .def( "hasCompositionBounds", &Movie2::hasCompositionBounds )
            .def( "getCompositionBounds", &Movie2::getCompositionBounds )
            .def_static( "getCompositionBoundsWM", &Detail::movie2GetCompositionBoundsWM )
            .def_static_native_kernel( "setEventListener", &Detail::movie2SetEventListener )
            .def_static_kernel( "getSockets", &Detail::movie2GetSockets )
            .def_static_kernel( "getSlots", &Detail::movie2GetSlots )
            .def( "findSprite", &Movie2::findSprite )
            .def( "hasSprite", &Movie2::hasSprite )
            .def( "findParticle", &Movie2::findParticle )
            .def( "hasParticle", &Movie2::hasParticle )
            .def( "findSlot", &Movie2::findSlot )
            .def( "hasSlot", &Movie2::hasSlot )
            .def( "findSocket", &Movie2::findSocket )
            .def( "hasSocket", &Movie2::hasSocket )
            .def( "findText", &Movie2::findText )
            .def( "hasText", &Movie2::hasText )
            .def( "hasMovieLayers", &Movie2::hasMovieLayers )
            .def( "setEnableMovieLayers", &Movie2::setEnableMovieLayers )
            .def( "setExtraOpacityMovieLayers", &Movie2::setExtraOpacityMovieLayers )
            .def( "setExtraTransformation", &Movie2::setExtraTransformation )
            .def( "removeExtraTransformation", &Movie2::removeExtraTransformation );

        pybind::interface_<Movie2Slot, pybind::bases<Node>>( _kernel, "Movie2Slot", false )
            .def( "getMovieName", &Movie2Slot::getMovieName )
            .def( "getCompositionName", &Movie2Slot::getCompositionName )
            .def( "getLayerIndex", &Movie2Slot::getLayerIndex )
            .def( "getDimension", &Movie2Slot::getDimension )
            .def( "hasOption", &Movie2Slot::hasOption )
            ;

        pybind::interface_<Movie2SubComposition, pybind::bases<Eventable, Animatable, Scriptable, Identity>>( _kernel, "Movie2SubComposition", false )
            .def( "setEnable", &Movie2SubComposition::setSubCompositionEnable )
            .def( "getEnable", &Movie2SubComposition::getSubCompositionEnable )
            .def_static_native_kernel( "setEventListener", &Detail::movie2SubCompositionSetEventListener )
            ;

        pybind::interface_<ResourceMovie2, pybind::bases<Resource>>( _kernel, "ResourceMovie2", false )
            .def( "hasComposition", &ResourceMovie2::hasComposition )
            .def( "hasCompositionLayer", &ResourceMovie2::hasCompositionLayer )
            .def( "getCompositionDuration", &ResourceMovie2::getCompositionDuration )
            .def( "getCompositionFrameDuration", &ResourceMovie2::getCompositionFrameDuration )
            .def_static_kernel( "getCompositionResources", &Detail::resourceMovie2GetCompositionResources )
            .def_static_kernel( "getCompositionLayers", &Detail::resourceMovie2GetCompositionLayers )
            .def_static_kernel( "getCompositions", &Detail::resourceMovie2GetCompositions )
            ;

        pybind::def_function_kernel( _kernel, "getNullObjectsFromResourceMovie2", &Detail::getNullObjectsFromResourceMovie2 );

        Helper::registerScriptWrapping<Movie2>( _kernel, MENGINE_DOCUMENT_FACTORABLE );
        Helper::registerScriptWrapping<Movie2Slot>( _kernel, MENGINE_DOCUMENT_FACTORABLE );
        Helper::registerScriptWrapping<Movie2SubComposition>( _kernel, MENGINE_DOCUMENT_FACTORABLE );
        Helper::registerScriptWrapping<ResourceMovie2>( _kernel, MENGINE_DOCUMENT_FACTORABLE );

        return true;
    }
    //////////////////////////////////////////////////////////////////////////
    void Movie2ScriptEmbedding::eject( pybind::kernel_interface * _kernel )
    {
        _kernel->remove_scope<Movie2>();
        _kernel->remove_scope<Movie2Slot>();
        _kernel->remove_scope<Movie2SubComposition>();
        _kernel->remove_scope<ResourceMovie2>();

        Helper::unregisterScriptWrapping<Movie2>();
        Helper::unregisterScriptWrapping<Movie2Slot>();
        Helper::unregisterScriptWrapping<Movie2SubComposition>();
        Helper::unregisterScriptWrapping<ResourceMovie2>();
    }
    //////////////////////////////////////////////////////////////////////////
}
