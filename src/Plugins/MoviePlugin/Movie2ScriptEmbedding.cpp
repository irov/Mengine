#include "Movie2ScriptEmbedding.h"

#include "Interface/ScriptServiceInterface.h"

#include "Environment/Python/PythonIncluder.h"
#include "Environment/Python/PythonAnimatableEventReceiver.h"
#include "Environment/Python/PythonScriptWrapper.h"
#include "Environment/Python/PythonDocumentTraceback.h"

#include "Movie2.h"

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
        class PythonMovie2EventReceiver
            : public PythonAnimatableEventReceiver<Movie2EventReceiverInterface>
        {
        public:
            PythonMovie2EventReceiver()
            {
            }

            ~PythonMovie2EventReceiver() override
            {
            }
        };
        //////////////////////////////////////////////////////////////////////////
        static PyObject * s_Movie2_setEventListener( pybind::kernel_interface * _kernel, Movie2 * _node, PyObject * _args, PyObject * _kwds )
        {
            MENGINE_UNUSED( _args );

            MENGINE_ASSERTION_MEMORY_PANIC( _kwds, "kwds is nullptr" );

            pybind::dict py_kwds( _kernel, _kwds );
            Helper::registerAnimatableEventReceiver<PythonMovie2EventReceiver>( _kernel, py_kwds, _node, MENGINE_DOCUMENT_PYBIND );

            MENGINE_ASSERTION_PYTHON_EVENT_RECEIVER( _node, py_kwds );

            return _kernel->ret_none();
        }
        //////////////////////////////////////////////////////////////////////////
        static mt::box2f s_Movie2_getCompositionBoundsWM( Movie2 * _movie )
        {
            const mt::box2f & bounds = _movie->getCompositionBounds();

            const TransformationInterface * transformation = _movie->getTransformation();

            const mt::mat4f & wm = transformation->getWorldMatrix();

            mt::box2f boundsWM;
            mt::box2_mul_m4( &boundsWM, bounds, wm );

            return boundsWM;
        }
        //////////////////////////////////////////////////////////////////////////    
        static pybind::list s_Movie2_getSockets( pybind::kernel_interface * _kernel, Movie2 * _movie )
        {
            pybind::list py_list( _kernel );

            _movie->foreachSockets( [_kernel, &py_list]( Movie2 * _movie, uint32_t _index, const NodePtr & _node )
            {
                MENGINE_UNUSED( _index );

                HotSpotPolygonPtr hotspot = HotSpotPolygonPtr::from( _node );

                if( hotspot == nullptr )
                {
                    return;
                }

                const ConstString & name = _node->getName();

                py_list.append( pybind::make_tuple_t( _kernel, _movie, name, hotspot ) );
            } );

            return py_list;
        }
        //////////////////////////////////////////////////////////////////////////    
        static pybind::list s_Movie2_getSlots( pybind::kernel_interface * _kernel, Movie2 * _movie )
        {
            pybind::list py_list( _kernel );

            _movie->foreachSlots( [_kernel, &py_list]( Movie2 * _movie, uint32_t _index, const NodePtr & _node )
            {
                MENGINE_UNUSED( _index );

                Movie2SlotPtr hotspot = Movie2SlotPtr::from( _node );

                if( hotspot == nullptr )
                {
                    return;
                }

                const ConstString & name = _node->getName();

                py_list.append( pybind::make_tuple_t( _kernel, _movie, name, hotspot ) );
            } );

            return py_list;
        }
        //////////////////////////////////////////////////////////////////////////
        class PythonMovie2SubCompositionEventReceiver
            : public PythonAnimatableEventReceiver<Movie2SubCompositionEventReceiver>
        {
        public:
            PythonMovie2SubCompositionEventReceiver()
            {
            }

            ~PythonMovie2SubCompositionEventReceiver() override
            {
            }
        };
        //////////////////////////////////////////////////////////////////////////
        static PyObject * s_Movie2SubComposition_setEventListener( pybind::kernel_interface * _kernel, Movie2SubComposition * _node, PyObject * _args, PyObject * _kwds )
        {
            MENGINE_UNUSED( _args );

            MENGINE_ASSERTION_MEMORY_PANIC( _kwds, "kwds is nullptr" );

            pybind::dict py_kwds( _kernel, _kwds );
            Helper::registerAnimatableEventReceiver<PythonMovie2SubCompositionEventReceiver>( _kernel, py_kwds, _node, MENGINE_DOCUMENT_PYBIND );

            MENGINE_ASSERTION_PYTHON_EVENT_RECEIVER( _node, py_kwds );

            return _kernel->ret_none();
        }
        //////////////////////////////////////////////////////////////////////////
        static pybind::list s_ResourceMovie2_getCompositionResources( pybind::kernel_interface * _kernel, ResourceMovie2 * _resourceMovie2, const ConstString & _compositionName )
        {
            const Movie2DataInterfacePtr & data = _resourceMovie2->getData();

            const aeMovieCompositionData * compositionData = _resourceMovie2->getCompositionData( _compositionName );

            if( compositionData == nullptr )
            {
                LOGGER_ERROR( "ResourceMovie2 '%s' not found composition '%s'"
                    , _resourceMovie2->getName().c_str()
                    , _compositionName.c_str()
                );

                return pybind::list( _kernel );
            }

            pybind::list l = pybind::make_list_t( _kernel );

            data->foreachCompositionDataResources( compositionData, [_kernel, &l]( uint32_t _index, const Char * _name, const ResourcePtr & _resource )
            {
                pybind::dict d = pybind::make_dict_t( _kernel );

                d["index"] = _index;
                d["name"] = _name;
                d["resource"] = _resource;

                l.append( d );
            } );

            return l;
        }
        //////////////////////////////////////////////////////////////////////////
        static pybind::list s_ResourceMovie2_getCompositionLayers( pybind::kernel_interface * _kernel, ResourceMovie2 * _resourceMovie2, const ConstString & _compositionName )
        {
            pybind::list l = pybind::make_list_t( _kernel );

            _resourceMovie2->foreachCompositionLayers( _compositionName, [_kernel, &l]( const ResourceMovie2::CompositionLayer & _layer )
            {
                pybind::dict d = pybind::make_dict_t( _kernel );

                d["index"] = _layer.index;
                d["name"] = _layer.name;
                d["type"] = _layer.type;
                d["matrix"] = _layer.matrix;
                d["color"] = _layer.color;
                d["dimension"] = _layer.dimension;

                pybind::list options = pybind::make_list_t( _kernel );

                for( uint32_t o : _layer.options )
                {
                    Char o_str[5];

                    o_str[0] = (Char)(o >> 24) & 0xff;
                    o_str[1] = (Char)(o >> 16) & 0xff;
                    o_str[2] = (Char)(o >> 8) & 0xff;
                    o_str[3] = (Char)(o >> 0) & 0xff;
                    o_str[4] = '\0';

                    o_str[0] = o_str[0] == '\0' ? ' ' : o_str[0];
                    o_str[1] = o_str[1] == '\0' ? ' ' : o_str[1];
                    o_str[2] = o_str[2] == '\0' ? ' ' : o_str[2];
                    o_str[3] = o_str[3] == '\0' ? ' ' : o_str[3];

                    Char * o_str_value = StdString::strrchr( o_str, ' ' );

                    if( o_str_value == nullptr )
                    {
                        options.append( o_str );
                    }
                    else
                    {
                        o_str_value++;

                        options.append( o_str_value );
                    }                    
                }

                d["options"] = options;

                l.append( d );
            } );

            return l;
        }
        //////////////////////////////////////////////////////////////////////////
        static pybind::list s_ResourceMovie2_getCompositions( pybind::kernel_interface * _kernel, ResourceMovie2 * _resourceMovie2 )
        {
            pybind::list l = pybind::make_list_t( _kernel );

            _resourceMovie2->foreachCompositionsDesc( [_kernel, &l]( const ConstString & _name, const ResourceMovie2::CompositionDesc & _layer )
            {
                pybind::dict d = pybind::make_dict_t( _kernel );

                d["name"] = _name;
                d["duration"] = _layer.duration;
                d["frameDuration"] = _layer.frameDuration;
                d["has_bounds"] = _layer.has_bounds;
                d["bounds"] = _layer.bounds;
                d["master"] = _layer.master;

                l.append( d );
            } );

            return l;
        }
        //////////////////////////////////////////////////////////////////////////
        struct movie_desc_t
        {
            pybind::kernel_interface * kernel;
            pybind::dict result;
            pybind::list frames;
            ae_float_t frame_duration;
            ae_float_t in_time;
        };
        //////////////////////////////////////////////////////////////////////////
        static ae_void_t __movie_layer_data_matrix_visitor( ae_uint32_t _index, const aeMovieLayerDataMatrixCallbackData * _callbackData, ae_userdata_t _ud )
        {
            movie_desc_t * d = reinterpret_cast<movie_desc_t *>(_ud);

            pybind::dict frame( d->kernel );

            ae_float_t time = d->in_time + _index * d->frame_duration;

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

            d->frames.append( frame );
        }
        //////////////////////////////////////////////////////////////////////////
        static ae_bool_t __movie_layer_data_visitor( const aeMovieCompositionData * _compositionData, const aeMovieLayerData * _layerData, ae_userdata_t _ud )
        {
            aeMovieLayerTypeEnum layer_type = ae_get_movie_layer_data_type( _layerData );

            if( layer_type != AE_MOVIE_LAYER_TYPE_SLOT )
            {
                return AE_TRUE;
            }

            ae_float_t frame_duration = ae_get_movie_composition_data_frame_duration( _compositionData );
            ae_float_t in_time = ae_get_movie_layer_data_in_time( _layerData );

            movie_desc_t * d = reinterpret_cast<movie_desc_t *>(_ud);

            d->frames = pybind::list( d->kernel );
            d->frame_duration = frame_duration;
            d->in_time = in_time;

            ae_visit_movie_layer_data_matrix( _layerData, &__movie_layer_data_matrix_visitor, _ud );

            const ae_char_t * layer_name = ae_get_movie_layer_data_name( _layerData );

            d->result[layer_name] = d->frames;

            return AE_TRUE;
        }
        //////////////////////////////////////////////////////////////////////////
        static pybind::dict s_getNullObjectsFromResourceMovie2( pybind::kernel_interface * _kernel, ResourceMovie2 * _resourceMovie2, const ConstString & _compositionName )
        {
            MENGINE_ASSERTION_MEMORY_PANIC( _resourceMovie2, "resource is nullptr" );

            const aeMovieCompositionData * compositionData = _resourceMovie2->getCompositionData( _compositionName );

            MENGINE_ASSERTION_MEMORY_PANIC( compositionData, "not found composition '%s'"
                , _compositionName.c_str()
            );

            movie_desc_t desc;
            desc.result = pybind::dict( _kernel );
            desc.kernel = _kernel;

            ae_visit_composition_layer_data( compositionData, &__movie_layer_data_visitor, &desc );

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
        SCRIPT_SERVICE()
            ->setAvailablePlugin( "Movie2", true );

        pybind::interface_<Movie2, pybind::bases<Node>>( _kernel, "Movie2", false )
            .def( "setResourceMovie2", &Movie2::setResourceMovie2 )
            .def( "getResourceMovie2", &Movie2::getResourceMovie2 )
            .def( "setCompositionName", &Movie2::setCompositionName )
            .def( "getCompositionName", &Movie2::getCompositionName )
            .def( "setTextAliasEnvironment", &Movie2::setTextAliasEnvironment )
            .def( "getTextAliasEnvironment", &Movie2::getTextAliasEnvironment )
            .def( "setWorkAreaFromEvent", &Movie2::setWorkAreaFromEvent )
            .def( "removeWorkArea", &Movie2::removeWorkArea )
            .def( "hasSubComposition", &Movie2::hasSubComposition )
            .def( "getSubComposition", &Movie2::getSubComposition )
            .def( "hasCompositionBounds", &Movie2::hasCompositionBounds )
            .def( "getCompositionBounds", &Movie2::getCompositionBounds )
            .def_static( "getCompositionBoundsWM", &Detail::s_Movie2_getCompositionBoundsWM )
            .def_static_native_kernel( "setEventListener", &Detail::s_Movie2_setEventListener )
            .def_static_kernel( "getSockets", &Detail::s_Movie2_getSockets )
            .def_static_kernel( "getSlots", &Detail::s_Movie2_getSlots )
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
            ;

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
            .def_static_native_kernel( "setEventListener", &Detail::s_Movie2SubComposition_setEventListener )
            ;

        pybind::interface_<ResourceMovie2, pybind::bases<Resource>>( _kernel, "ResourceMovie2", false )
            .def( "hasComposition", &ResourceMovie2::hasComposition )
            .def( "hasCompositionLayer", &ResourceMovie2::hasCompositionLayer )
            .def( "getCompositionDuration", &ResourceMovie2::getCompositionDuration )
            .def( "getCompositionFrameDuration", &ResourceMovie2::getCompositionFrameDuration )
            .def_static_kernel( "getCompositionResources", &Detail::s_ResourceMovie2_getCompositionResources )
            .def_static_kernel( "getCompositionLayers", &Detail::s_ResourceMovie2_getCompositionLayers )
            .def_static_kernel( "getCompositions", &Detail::s_ResourceMovie2_getCompositions )
            ;

        pybind::def_function_kernel( _kernel, "getNullObjectsFromResourceMovie2", &Detail::s_getNullObjectsFromResourceMovie2 );

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

