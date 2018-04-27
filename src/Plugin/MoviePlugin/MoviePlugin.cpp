#include "MoviePlugin.h"

#include "Interface/PrototypeManagerInterface.h"
#include "Interface/StringizeInterface.h"
#include "Interface/ScriptSystemInterface.h"
#include "Interface/ConfigInterface.h"

#include "Kernel/ScriptWrapper.h"

#include "Kernel/NodePrototypeGenerator.h"
#include "Kernel/ResourcePrototypeGenerator.h"

#include "PythonScriptWrapper/PythonAnimatableEventReceiver.h"

#include "Core/ModuleFactory.h"

#include "Logger/Logger.h"

#include <stdlib.h>

//////////////////////////////////////////////////////////////////////////
PLUGIN_FACTORY( Movie, Mengine::MoviePlugin )
//////////////////////////////////////////////////////////////////////////
namespace Mengine
{
    //////////////////////////////////////////////////////////////////////////
    static void * stdex_movie_alloc( void * _data, size_t _size )
    {
        (void)_data;

        return stdex_malloc( _size, "Movie" );
        //return new uint8_t[_size];
        //return malloc( _size );
    }
    //////////////////////////////////////////////////////////////////////////
    static void * stdex_movie_alloc_n( void * _data, size_t _size, size_t _count )
    {
        (void)_data;

        size_t total = _size * _count;

        return stdex_malloc( total, "Movie" );
        //return new uint8_t[total];
        //return malloc( total );
    }
    //////////////////////////////////////////////////////////////////////////
    static void stdex_movie_free( void * _data, const void * _ptr )
    {
        (void)_data;

        stdex_free( (void *)_ptr, "Movie" );
        //delete[] _ptr;
        //free( (void *)_ptr );
    }
    //////////////////////////////////////////////////////////////////////////
    static void stdex_movie_free_n( void * _data, const void * _ptr )
    {
        (void)_data;

        stdex_free( (void *)_ptr, "Movie" );
        //delete[] _ptr;
        //free( (void *)_ptr );
    }
    //////////////////////////////////////////////////////////////////////////
    static void stdex_movie_logerror( void * _data, aeMovieErrorCode _code, const char * _format, ... )
    {
        (void)_data;

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
        char msg[4096];
        vsprintf( msg, _format, argList );
        va_end( argList );

        LOGGER_ERROR( "MoviePlugin error '%s' code '%d'"
            , msg
            , _code
        );
    }
    //////////////////////////////////////////////////////////////////////////
    MoviePlugin::MoviePlugin()
        : m_instance( nullptr )
    {
    }
    //////////////////////////////////////////////////////////////////////////
    namespace
    {
        class ResourceMovie2PrototypeGenerator
            : public ResourcePrototypeGenerator<ResourceMovie2, 128>
        {
        public:
            ResourceMovie2PrototypeGenerator( const aeMovieInstance * _instance )
                : m_instance( _instance )
            {
            }

            ~ResourceMovie2PrototypeGenerator() override
            {
            }

        protected:
            PointerFactorable generate() override
            {
                ResourceMovie2 * resource = m_factory->createObject();

                if( resource == nullptr )
                {
                    LOGGER_ERROR( "ResourcePrototypeGenerator can't generate %s %s"
                        , m_category.c_str()
                        , m_prototype.c_str()
                    );

                    return nullptr;
                }

                resource->setType( m_prototype );
                resource->setScriptWrapper( m_scriptWrapper );

                resource->setMovieInstance( m_instance );

                return resource;
            }

        protected:
            const aeMovieInstance * m_instance;
        };
    }
    //////////////////////////////////////////////////////////////////////////
    bool MoviePlugin::_avaliable()
    {
        return true;
    }
    //////////////////////////////////////////////////////////////////////////
    namespace External
    {
        //////////////////////////////////////////////////////////////////////////
        class PythonMovie2EventReceiver
            : public PythonAnimatableEventReceiver<Movie2EventReceiver>
        {
        public:
        };
        //////////////////////////////////////////////////////////////////////////
        static PyObject * Movie2_setEventListener( pybind::kernel_interface * _kernel, Movie2 * _node, PyObject * _args, PyObject * _kwds )
        {
            (void)_args;

            if( _kwds == nullptr )
            {
                return pybind::ret_none();
            }

            pybind::dict py_kwds( _kernel, _kwds );
            Helper::registerAnimatableEventReceiver<PythonMovie2EventReceiver>( py_kwds, _node );

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

                    LOGGER_ERROR( "Movie2::setEventListener invalid kwds '%s'\n"
                        , k.c_str()
                    );
                }

                throw;
            }
#endif

            return pybind::ret_none();
        }
        //////////////////////////////////////////////////////////////////////////
        class PythonVisitorMovie2Socket
            : public VisitorMovie2Layer
        {
        public:
            PythonVisitorMovie2Socket( pybind::kernel_interface * _kernel, const pybind::list & _list )
                : m_kernel( _kernel )
                , m_list( _list )
            {
            }

        protected:
            void visitMovieLayer( Movie2 * _movie, const ConstString & _name, Node * _node ) override
            {
                HotSpotPolygon * hotspot = static_node_cast<HotSpotPolygon *>(_node);

                if( hotspot == nullptr )
                {
                    return;
                }

                m_list.append( pybind::make_tuple_t( m_kernel, _movie, _name, hotspot ) );
            }

        protected:
            pybind::kernel_interface * m_kernel;
            pybind::list m_list;
        };
        //////////////////////////////////////////////////////////////////////////    
        static pybind::list Movie2_getSockets( pybind::kernel_interface * _kernel, Movie2 * _movie )
        {
            pybind::list py_list( _kernel );

            PythonVisitorMovie2Socket visitor( _kernel, py_list );
            _movie->visitSockets( &visitor );

            return py_list;
        }
        //////////////////////////////////////////////////////////////////////////
        class PythonVisitorMovie2Slots
            : public VisitorMovie2Layer
        {
        public:
            PythonVisitorMovie2Slots( pybind::kernel_interface * _kernel, const pybind::list & _list )
                : m_kernel( _kernel )
                , m_list( _list )
            {
            }

        protected:
            void visitMovieLayer( Movie2 * _movie, const ConstString & _name, Node * _node ) override
            {
                Movie2Slot * hotspot = static_node_cast<Movie2Slot *>(_node);

                if( hotspot == nullptr )
                {
                    return;
                }

                m_list.append( pybind::make_tuple_t( m_kernel, _movie, _name, hotspot ) );
            }

        protected:
            pybind::kernel_interface * m_kernel;
            pybind::list m_list;
        };
        //////////////////////////////////////////////////////////////////////////    
        static pybind::list Movie2_getSlots( pybind::kernel_interface * _kernel, Movie2 * _movie )
        {
            pybind::list py_list( _kernel );

            PythonVisitorMovie2Slots visitor( _kernel, py_list );
            _movie->visitSockets( &visitor );

            return py_list;
        }
    }
    //////////////////////////////////////////////////////////////////////////
    bool MoviePlugin::_initialize()
    {
        String hashkey = CONFIG_VALUE( "MoviePlugin", "HASHKEY", "" );

        if( hashkey.empty() == true )
        {
            LOGGER_ERROR( "MoviePlugin::_initialize not setup HASHKEY" );

            return false;
        }

        if( hashkey.size() != 40 )
        {
            LOGGER_ERROR( "MoviePlugin::_initialize invalid HASHKEY '%s'"
                , hashkey.c_str()
            );

            return false;
        }

        m_hashkey = hashkey;

        pybind::kernel_interface * kernel = pybind::get_kernel();

        m_instance = ae_create_movie_instance( m_hashkey.c_str(), &stdex_movie_alloc, &stdex_movie_alloc_n, &stdex_movie_free, &stdex_movie_free_n, 0, &stdex_movie_logerror, this );

        pybind::interface_<Movie2, pybind::bases<Node, Animatable> >( kernel, "Movie2", false )
            .def( "setResourceMovie2", &Movie2::setResourceMovie2 )
            .def( "getResourceMovie2", &Movie2::getResourceMovie2 )
            .def( "setCompositionName", &Movie2::setCompositionName )
            .def( "getCompositionName", &Movie2::getCompositionName )
            .def( "getDuration", &Movie2::getDuration )
            .def( "setWorkAreaFromEvent", &Movie2::setWorkAreaFromEvent )
            .def( "removeWorkArea", &Movie2::removeWorkArea )
            .def( "hasSubComposition", &Movie2::hasSubComposition )
            .def( "playSubComposition", &Movie2::playSubComposition )
            .def( "stopSubComposition", &Movie2::stopSubComposition )
            .def( "interruptSubComposition", &Movie2::interruptSubComposition )
            .def( "setLoopSubComposition", &Movie2::setLoopSubComposition )
            .def_static_native_kernel( "setEventListener", &External::Movie2_setEventListener )
            .def_static_kernel( "getSockets", &External::Movie2_getSockets )
            .def_static_kernel( "getSlots", &External::Movie2_getSlots )
            .def( "getSprite", &Movie2::getSprite )
            .def( "hasSprite", &Movie2::hasSprite )
            .def( "getParticle", &Movie2::getParticle )
            .def( "hasParticle", &Movie2::hasParticle )
            .def( "getSlot", &Movie2::getSlot )
            .def( "hasSlot", &Movie2::hasSlot )
            .def( "getSocket", &Movie2::getSocket )
            .def( "hasSocket", &Movie2::hasSocket )
            .def( "getText", &Movie2::getText )
            .def( "hasText", &Movie2::hasText )
            .def( "setEnableMovieLayers", &Movie2::setEnableMovieLayers )
            ;


        pybind::interface_<Movie2Slot, pybind::bases<Node> >( kernel, "Movie2Slot", false )            
            ;


        pybind::interface_<ResourceMovie2, pybind::bases<ResourceReference> >( kernel, "ResourceMovie2", false )
            .def( "hasComposition", &ResourceMovie2::hasComposition )
            .def( "getCompositionDuration", &ResourceMovie2::getCompositionDuration )
            .def( "getCompositionFrameDuration", &ResourceMovie2::getCompositionFrameDuration )
            ;

        SCRIPT_SERVICE()
            ->setWrapper( STRINGIZE_STRING_LOCAL( "Movie2" ), new ScriptWrapper<Movie2>() );

        SCRIPT_SERVICE()
            ->setWrapper( STRINGIZE_STRING_LOCAL( "Movie2Slot" ), new ScriptWrapper<Movie2Slot>() );

        SCRIPT_SERVICE()
            ->setWrapper( STRINGIZE_STRING_LOCAL( "ResourceMovie2" ), new ScriptWrapper<ResourceMovie2>() );

        if( PROTOTYPE_SERVICE()
            ->addPrototype( STRINGIZE_STRING_LOCAL( "Node" ), STRINGIZE_STRING_LOCAL( "Movie2" ), new NodePrototypeGenerator<Movie2, 128> ) == false )
        {
            return false;
        }

        if( PROTOTYPE_SERVICE()
            ->addPrototype( STRINGIZE_STRING_LOCAL( "Node" ), STRINGIZE_STRING_LOCAL( "Movie2Slot" ), new NodePrototypeGenerator<Movie2Slot, 128> ) == false )
        {
            return false;
        }

        if( PROTOTYPE_SERVICE()
            ->addPrototype( STRINGIZE_STRING_LOCAL( "Resource" ), STRINGIZE_STRING_LOCAL( "ResourceMovie2" ), new ResourceMovie2PrototypeGenerator( m_instance ) ) == false )
        {
            return false;
        }

        return true;
    }
    //////////////////////////////////////////////////////////////////////////
    void MoviePlugin::_finalize()
    {
        ae_delete_movie_instance( m_instance );
        m_instance = nullptr;

        SCRIPT_SERVICE()
            ->removeWrapper( STRINGIZE_STRING_LOCAL( "Movie2" ) );

        SCRIPT_SERVICE()
            ->removeWrapper( STRINGIZE_STRING_LOCAL( "ResourceMovie2" ) );

        PROTOTYPE_SERVICE()
            ->removePrototype( STRINGIZE_STRING_LOCAL( "Node" ), STRINGIZE_STRING_LOCAL( "Movie2" ) );

        PROTOTYPE_SERVICE()
            ->removePrototype( STRINGIZE_STRING_LOCAL( "Node" ), STRINGIZE_STRING_LOCAL( "Movie2Slot" ) );

        PROTOTYPE_SERVICE()
            ->removePrototype( STRINGIZE_STRING_LOCAL( "Resource" ), STRINGIZE_STRING_LOCAL( "ResourceMovie2" ) );
    }
}
