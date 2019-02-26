#include "MoviePlugin.h"

#include "Interface/PrototypeServiceInterface.h"
#include "Interface/StringizeServiceInterface.h"
#include "Interface/ScriptServiceInterface.h"
#include "Interface/PlayerInterface.h"
#include "Interface/ConfigServiceInterface.h"
#include "Interface/ArchiveServiceInterface.h"
#include "Interface/LoaderServiceInterface.h"
#include "Interface/DataServiceInterface.h"
#include "Interface/VocabularyServiceInterface.h"

#include "Plugins/NodeDebugRenderPlugin/NodeDebugRenderServiceInterface.h"
#include "Plugins/ResourcePrefetcherPlugin/ResourcePrefetcherServiceInterface.h"
#include "Plugins/ResourceValidatePlugin/ResourceValidateServiceInterface.h"

#include "Environment/Python/PythonScriptWrapper.h"

#include "DataflowAEZ.h"
#include "Movie2DebugRender.h"

#include "Engine/ShapeQuadFixed.h"
#include "Engine/HotSpotPolygon.h"

#include "Kernel/DefaultPrototypeGenerator.h"
#include "Kernel/NodePrototypeGenerator.h"
#include "Kernel/ResourcePrototypeGenerator.h"

#include "Environment/Python/PythonAnimatableEventReceiver.h"

#include "Kernel/Logger.h"

#include "Movie2.h"
#include "ResourceMovie2.h"
#include "LoaderResourceMovie2.h"
#include "Movie2Slot.h"

#include "ResourceMovie2Validator.h"
#include "Movie2DebuggerBoundingBox.h"

#include "pybind/pybind.hpp"

#include <stdlib.h>
#include <stdarg.h>

//////////////////////////////////////////////////////////////////////////
PLUGIN_FACTORY( Movie, Mengine::MoviePlugin )
//////////////////////////////////////////////////////////////////////////
namespace Mengine
{
    //////////////////////////////////////////////////////////////////////////
    static ae_voidptr_t stdex_movie_alloc( ae_userdata_t _userdata, ae_size_t _size )
    {
        AE_UNUSED( _userdata );

        return stdex_malloc( _size, "Movie" );
        //return new uint8_t[_size];
        //return malloc( _size );
    }
    //////////////////////////////////////////////////////////////////////////
    static ae_voidptr_t stdex_movie_alloc_n( ae_userdata_t _userdata, ae_size_t _size, ae_size_t _count )
    {
        AE_UNUSED( _userdata );

        size_t total = _size * _count;

        return stdex_malloc( total, "Movie" );
        //return new uint8_t[total];
        //return malloc( total );
    }
    //////////////////////////////////////////////////////////////////////////
    static void stdex_movie_free( ae_userdata_t _userdata, ae_constvoidptr_t _ptr )
    {
        AE_UNUSED( _userdata );

        stdex_free( (void *)_ptr, "Movie" );
        //delete[] _ptr;
        //free( (void *)_ptr );
    }
    //////////////////////////////////////////////////////////////////////////
    static void stdex_movie_free_n( ae_userdata_t _userdata, ae_constvoidptr_t _ptr )
    {
        AE_UNUSED( _userdata );

        stdex_free( (void *)_ptr, "Movie" );
        //delete[] _ptr;
        //free( (void *)_ptr );
    }
    //////////////////////////////////////////////////////////////////////////
    static void stdex_movie_logerror( ae_userdata_t _userdata, aeMovieErrorCode _code, const ae_char_t * _format, ... )
    {
        AE_UNUSED( _userdata );

        switch( _code )
        {
        case AE_ERROR_STREAM:
            {
                return;
            }break;
        default:
            {
            }break;
        }

        va_list argList;

        va_start( argList, _format );
        Char msg[4096];
        vsprintf( msg, _format, argList );
        va_end( argList );

        LOGGER_ERROR( "MoviePlugin error '%s' code '%d'"
            , msg
            , _code
        );
    }
    //////////////////////////////////////////////////////////////////////////
    MoviePlugin::MoviePlugin()
        : m_movieInstance( nullptr )
    {
    }
    //////////////////////////////////////////////////////////////////////////
    MoviePlugin::~MoviePlugin()
    {
    }
    //////////////////////////////////////////////////////////////////////////
    namespace Detail
    {
        //////////////////////////////////////////////////////////////////////////
        class PythonMovie2EventReceiver
            : public PythonAnimatableEventReceiver<Movie2EventReceiver>
        {
        public:
        };
        //////////////////////////////////////////////////////////////////////////
        static PyObject * s_Movie2_setEventListener( pybind::kernel_interface * _kernel, Movie2 * _node, PyObject * _args, PyObject * _kwds )
        {
            (void)_args;

            if( _kwds == nullptr )
            {
                return _kernel->ret_none();
            }

            pybind::dict py_kwds( _kernel, _kwds );
            Helper::registerAnimatableEventReceiver<PythonMovie2EventReceiver>( _kernel, py_kwds, _node );

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

                    LOGGER_ERROR( "node '%s' invalid kwds '%s'"
                        , _node->getName().c_str()
                        , k.c_str()
                    );
                }

                throw;
            }
#endif

            return _kernel->ret_none();
        }
        //////////////////////////////////////////////////////////////////////////
        class PythonVisitorMovie2Socket
            : public VisitorMovie2LayerInterface
            , public Factorable
        {
        public:
            PythonVisitorMovie2Socket( pybind::kernel_interface * _kernel, const pybind::list & _list )
                : m_kernel( _kernel )
                , m_list( _list )
            {
            }

            ~PythonVisitorMovie2Socket() override
            {
            }

        protected:
            void visitMovieLayer( Movie2 * _movie, uint32_t _index, const NodePtr & _node ) override
            {
                (void)_index;

                HotSpotPolygonPtr hotspot = stdex::intrusive_static_cast<HotSpotPolygonPtr>(_node);

                if( hotspot == nullptr )
                {
                    return;
                }

                const ConstString & name = _node->getName();

                m_list.append( pybind::make_tuple_t( m_kernel, _movie, name, hotspot ) );
            }

        protected:
            pybind::kernel_interface * m_kernel;
            pybind::list m_list;
        };
        //////////////////////////////////////////////////////////////////////////
        static mt::box2f s_Movie2_getCompositionBoundsWM( Movie2 * _movie )
        {
            const mt::box2f & bounds = _movie->getCompositionBounds();

            const mt::mat4f & wm = _movie->getWorldMatrix();

            mt::box2f boundsWM;
            mt::mul_box2_m4( boundsWM, bounds, wm );

            return boundsWM;
        }
        //////////////////////////////////////////////////////////////////////////    
        static pybind::list s_Movie2_getSockets( pybind::kernel_interface * _kernel, Movie2 * _movie )
        {
            pybind::list py_list( _kernel );

            VisitorMovie2LayerInterfacePtr visitor = Helper::makeFactorableUnique<PythonVisitorMovie2Socket>( _kernel, py_list );
            _movie->visitSockets( visitor );

            return py_list;
        }
        //////////////////////////////////////////////////////////////////////////
        class PythonVisitorMovie2Slots
            : public VisitorMovie2LayerInterface
            , public Factorable
        {
        public:
            PythonVisitorMovie2Slots( pybind::kernel_interface * _kernel, const pybind::list & _list )
                : m_kernel( _kernel )
                , m_list( _list )
            {
            }

            ~PythonVisitorMovie2Slots() override
            {
            }

        protected:
            void visitMovieLayer( Movie2 * _movie, uint32_t _index, const NodePtr & _node ) override
            {
                (void)_index;

                Movie2SlotPtr hotspot = stdex::intrusive_static_cast<Movie2SlotPtr>(_node);

                if( hotspot == nullptr )
                {
                    return;
                }

                const ConstString & name = _node->getName();

                m_list.append( pybind::make_tuple_t( m_kernel, _movie, name, hotspot ) );
            }

        protected:
            pybind::kernel_interface * m_kernel;
            pybind::list m_list;
        };
        //////////////////////////////////////////////////////////////////////////    
        static pybind::list s_Movie2_getSlots( pybind::kernel_interface * _kernel, Movie2 * _movie )
        {
            pybind::list py_list( _kernel );

            VisitorMovie2LayerInterfacePtr visitor = Helper::makeFactorableUnique<PythonVisitorMovie2Slots>( _kernel, py_list );
            _movie->visitSockets( visitor );

            return py_list;
        }
        //////////////////////////////////////////////////////////////////////////
        class PythonMovie2SubCompositionEventReceiver
            : public PythonAnimatableEventReceiver<Movie2SubCompositionEventReceiver>
        {
        public:
        };
        //////////////////////////////////////////////////////////////////////////
        static PyObject * s_Movie2SubComposition_setEventListener( pybind::kernel_interface * _kernel, Movie2SubComposition * _node, PyObject * _args, PyObject * _kwds )
        {
            (void)_args;

            if( _kwds == nullptr )
            {
                return _kernel->ret_none();
            }

            pybind::dict py_kwds( _kernel, _kwds );
            Helper::registerAnimatableEventReceiver<PythonMovie2SubCompositionEventReceiver>( _kernel, py_kwds, _node );

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

                    LOGGER_ERROR( "node '%s' invalid kwds '%s'"
                        , _node->getName().c_str()
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
    bool MoviePlugin::_initializePlugin()
    {
        this->addDependencyService( "PrefetcherService" );

        const Char * hashkey = CONFIG_VALUE( "MoviePlugin", "HASHKEY", "NONE" );

        size_t hashkeylen = strlen( hashkey );

        if( hashkeylen != 40 )
        {
            LOGGER_ERROR( "invalid HASHKEY '%s'"
                , hashkey
            );

            return false;
        }

        m_hashkey = hashkey;

        if( SERVICE_EXIST( ScriptServiceInterface ) == true )
        {
            pybind::kernel_interface * kernel = SCRIPT_SERVICE()
                ->getKernel();

            pybind::set_kernel( kernel );

            m_movieInstance = ae_create_movie_instance( m_hashkey.c_str(), &stdex_movie_alloc, &stdex_movie_alloc_n, &stdex_movie_free, &stdex_movie_free_n, 0, &stdex_movie_logerror, this );

            pybind::interface_<Movie2, pybind::bases<Node, Animatable> >( kernel, "Movie2", false )
                .def( "setResourceMovie2", &Movie2::setResourceMovie2 )
                .def( "getResourceMovie2", &Movie2::getResourceMovie2 )
                .def( "setCompositionName", &Movie2::setCompositionName )
                .def( "getCompositionName", &Movie2::getCompositionName )
                .def( "setTextAliasEnvironment", &Movie2::setTextAliasEnvironment )
                .def( "getTextAliasEnvironment", &Movie2::getTextAliasEnvironment )
                .def_deprecated( "getDuration", &Movie2::getDuration, "Use animation" )
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
                .def( "setEnableMovieLayers", &Movie2::setEnableMovieLayers )
                ;

            pybind::interface_<Movie2Slot, pybind::bases<Node> >( kernel, "Movie2Slot", false )
                ;

            pybind::interface_<Movie2SubComposition, pybind::bases<Eventable, Animatable, Scriptable, Identity> >( kernel, "Movie2SubComposition", false )
                .def( "setEnable", &Movie2SubComposition::setEnable )
                .def( "getEnable", &Movie2SubComposition::getEnable )
                .def_static_native_kernel( "setEventListener", &Detail::s_Movie2SubComposition_setEventListener )
                ;

            pybind::interface_<ResourceMovie2, pybind::bases<Resource> >( kernel, "ResourceMovie2", false )
                .def( "hasComposition", &ResourceMovie2::hasComposition )
                .def( "getCompositionDuration", &ResourceMovie2::getCompositionDuration )
                .def( "getCompositionFrameDuration", &ResourceMovie2::getCompositionFrameDuration )
                ;

            VOCALUBARY_SET( ScriptWrapperInterface, STRINGIZE_STRING_LOCAL( "ClassWrapping" ), STRINGIZE_STRING_LOCAL( "Movie2" ), Helper::makeFactorableUnique<PythonScriptWrapper<Movie2> >( kernel ) );
            VOCALUBARY_SET( ScriptWrapperInterface, STRINGIZE_STRING_LOCAL( "ClassWrapping" ), STRINGIZE_STRING_LOCAL( "Movie2Slot" ), Helper::makeFactorableUnique<PythonScriptWrapper<Movie2Slot> >( kernel ) );
            VOCALUBARY_SET( ScriptWrapperInterface, STRINGIZE_STRING_LOCAL( "ClassWrapping" ), STRINGIZE_STRING_LOCAL( "Movie2SubComposition" ), Helper::makeFactorableUnique<PythonScriptWrapper<Movie2SubComposition> >( kernel ) );
            VOCALUBARY_SET( ScriptWrapperInterface, STRINGIZE_STRING_LOCAL( "ClassWrapping" ), STRINGIZE_STRING_LOCAL( "ResourceMovie2" ), Helper::makeFactorableUnique<PythonScriptWrapper<ResourceMovie2> >( kernel ) );
        }

        if( PROTOTYPE_SERVICE()
            ->addPrototype( STRINGIZE_STRING_LOCAL( "Node" ), STRINGIZE_STRING_LOCAL( "Movie2" ), Helper::makeFactorableUnique<NodePrototypeGenerator<Movie2, 128>>() ) == false )
        {
            return false;
        }

        if( PROTOTYPE_SERVICE()
            ->addPrototype( STRINGIZE_STRING_LOCAL( "Node" ), STRINGIZE_STRING_LOCAL( "Movie2Slot" ), Helper::makeFactorableUnique<NodePrototypeGenerator<Movie2Slot, 128>>() ) == false )
        {
            return false;
        }

        if( PROTOTYPE_SERVICE()
            ->addPrototype( STRINGIZE_STRING_LOCAL( "Node" ), STRINGIZE_STRING_LOCAL( "Movie2SubComposition" ), Helper::makeFactorableUnique<ScriptablePrototypeGenerator<Movie2SubComposition, 128>>() ) == false )
        {
            return false;
        }

        ArchivatorInterfacePtr archivator = ARCHIVE_SERVICE()
            ->getArchivator( STRINGIZE_STRING_LOCAL( "lz4" ) );

        if( archivator == nullptr )
        {
            return false;
        }

        DataflowAEZPtr dataflowAEZ = Helper::makeFactorableUnique<DataflowAEZ>();

        dataflowAEZ->setMovieInstance( m_movieInstance );
        dataflowAEZ->setArchivator( archivator );

        if( dataflowAEZ->initialize() == false )
        {
            return false;
        }

        VOCALUBARY_SET( DataflowInterface, STRINGIZE_STRING_LOCAL( "Dataflow" ), STRINGIZE_STRING_LOCAL( "aezMovie" ), dataflowAEZ );

        if( PROTOTYPE_SERVICE()
            ->addPrototype( STRINGIZE_STRING_LOCAL( "Resource" ), STRINGIZE_STRING_LOCAL( "ResourceMovie2" ), Helper::makeFactorableUnique<ResourcePrototypeGenerator<ResourceMovie2, 128>>() ) == false )
        {
            return false;
        }

        if( SERVICE_EXIST( ResourcePrefetcherServiceInterface ) == true )
        {
            ResourcePrefetcherInterfacePtr resourcePrefetcher = VOCALUBARY_GET( STRINGIZE_STRING_LOCAL( "ResourcePrefetcher" ), STRINGIZE_STRING_LOCAL( "Dataflow" ) );

            RESOURCEPREFETCHER_SERVICE()
                ->addResourcePrefetcher( STRINGIZE_STRING_LOCAL( "ResourceMovie2" ), resourcePrefetcher );
        }

        if( SERVICE_EXIST( NodeDebugRenderServiceInterface ) == true )
        {
            NODEDEBUGRENDER_SERVICE()
                ->addNodeDebugRender( STRINGIZE_STRING_LOCAL( "Movie2" ), Helper::makeFactorableUnique<Movie2DebugRender>() );
        }

        if( SERVICE_EXIST( ResourceValidateServiceInterface ) == true )
        {
            ResourceMovie2ValidatorPtr movie2Validator = Helper::makeFactorableUnique<ResourceMovie2Validator>();

            movie2Validator->setMovieInstance( m_movieInstance );
            movie2Validator->setArchivator( archivator );

            VOCALUBARY_SET( ResourceValidatorInterface, STRINGIZE_STRING_LOCAL( "Validator" ), STRINGIZE_STRING_LOCAL( "ResourceMovie2" ), movie2Validator );
        }

        if( SERVICE_EXIST( LoaderServiceInterface ) == true )
        {
            VOCALUBARY_SET( LoaderInterface, STRINGIZE_STRING_LOCAL( "Loader" ), STRINGIZE_STRING_LOCAL( "ResourceMovie2" ), Helper::makeFactorableUnique<LoaderResourceMovie2>() );
        }

        VOCALUBARY_SET( NodeDebuggerBoundingBoxInterface, STRINGIZE_STRING_LOCAL( "NodeDebuggerBoundingBox" ), STRINGIZE_STRING_LOCAL( "Movie2" ), Helper::makeFactorableUnique<Movie2DebuggerBoundingBox>() );

        return true;
    }
    //////////////////////////////////////////////////////////////////////////
    void MoviePlugin::_finalizePlugin()
    {
        ae_delete_movie_instance( m_movieInstance );
        m_movieInstance = nullptr;

        if( SERVICE_EXIST( ScriptServiceInterface ) == true )
        {
            VOCALUBARY_REMOVE( STRINGIZE_STRING_LOCAL( "ClassWrapping" ), STRINGIZE_STRING_LOCAL( "Movie2" ) );
            VOCALUBARY_REMOVE( STRINGIZE_STRING_LOCAL( "ClassWrapping" ), STRINGIZE_STRING_LOCAL( "Movie2Slot" ) );
            VOCALUBARY_REMOVE( STRINGIZE_STRING_LOCAL( "ClassWrapping" ), STRINGIZE_STRING_LOCAL( "Movie2SubComposition" ) );
            VOCALUBARY_REMOVE( STRINGIZE_STRING_LOCAL( "ClassWrapping" ), STRINGIZE_STRING_LOCAL( "ResourceMovie2" ) );
        }

        PROTOTYPE_SERVICE()
            ->removePrototype( STRINGIZE_STRING_LOCAL( "Node" ), STRINGIZE_STRING_LOCAL( "Movie2" ) );

        PROTOTYPE_SERVICE()
            ->removePrototype( STRINGIZE_STRING_LOCAL( "Node" ), STRINGIZE_STRING_LOCAL( "Movie2Slot" ) );

        PROTOTYPE_SERVICE()
            ->removePrototype( STRINGIZE_STRING_LOCAL( "Node" ), STRINGIZE_STRING_LOCAL( "Movie2SubComposition" ) );

        PROTOTYPE_SERVICE()
            ->removePrototype( STRINGIZE_STRING_LOCAL( "Resource" ), STRINGIZE_STRING_LOCAL( "ResourceMovie2" ) );

        DataflowInterfacePtr dataflow = VOCALUBARY_REMOVE( STRINGIZE_STRING_LOCAL( "Dataflow" ), STRINGIZE_STRING_LOCAL( "aezMovie" ) );
        dataflow->finalize();

        if( SERVICE_EXIST( ResourcePrefetcherServiceInterface ) == true )
        {
            RESOURCEPREFETCHER_SERVICE()
                ->removeResourcePrefetcher( STRINGIZE_STRING_LOCAL( "ResourceMovie2" ) );
        }

        if( SERVICE_EXIST( NodeDebugRenderServiceInterface ) == true )
        {
            NODEDEBUGRENDER_SERVICE()
                ->removeNodeDebugRender( STRINGIZE_STRING_LOCAL( "Movie2" ) );
        }

        if( SERVICE_EXIST( ResourceValidateServiceInterface ) == true )
        {
            VOCALUBARY_REMOVE( STRINGIZE_STRING_LOCAL( "Validator" ), STRINGIZE_STRING_LOCAL( "ResourceMovie2" ) );
        }

        if( SERVICE_EXIST( LoaderServiceInterface ) == true )
        {
            VOCALUBARY_REMOVE( STRINGIZE_STRING_LOCAL( "Loader" ), STRINGIZE_STRING_LOCAL( "ResourceMovie2" ) );
        }

        VOCALUBARY_REMOVE( STRINGIZE_STRING_LOCAL( "NodeDebuggerBoundingBox" ), STRINGIZE_STRING_LOCAL( "Movie2" ) );
    }
}
