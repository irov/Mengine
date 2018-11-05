#include "MoviePlugin.h"

#include "Interface/PrototypeServiceInterface.h"
#include "Interface/StringizeInterface.h"
#include "Interface/ScriptSystemInterface.h"
#include "Interface/PlayerInterface.h"
#include "Interface/ConfigServiceInterface.h"
#include "Interface/ArchiveServiceInterface.h"
#include "Interface/LoaderServiceInterface.h"

#include "Plugin/DebugRenderPlugin/DebugRenderInterface.h"
#include "Plugin/ResourceValidatePlugin/ResourceValidateInterface.h"

#include "Environment/Python/PythonScriptWrapper.h"

#include "Movie2DebugRenderVisitor.h"

#include "Engine/ShapeQuadFixed.h"
#include "Engine/HotSpotPolygon.h"
#include "Engine/TextField.h"

#include "Kernel/DefaultPrototypeGenerator.h"
#include "Kernel/NodePrototypeGenerator.h"
#include "Kernel/ResourcePrototypeGenerator.h"

#include "Environment/Python/PythonAnimatableEventReceiver.h"

#include "Kernel/Logger.h"

#include "Movie2.h"
#include "ResourceMovie2.h"
#include "LoaderResourceMovie2.h"
#include "Movie2Slot.h"

#include "Movie2ResourceValidateVisitor.h"

#include "pybind/pybind.hpp"

#include <stdlib.h>
#include <stdarg.h>

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
    MoviePlugin::~MoviePlugin()
    {
    }
    //////////////////////////////////////////////////////////////////////////
    namespace
    {
        class ResourceMovie2PrototypeGenerator
            : public ResourcePrototypeGenerator<ResourceMovie2, 128>
        {
        public:
            ResourceMovie2PrototypeGenerator( const aeMovieInstance * _instance, const ArchivatorInterfacePtr & _archivator )
                : m_instance( _instance )
                , m_archivator( _archivator )
            {
            }

            ~ResourceMovie2PrototypeGenerator() override
            {
            }

        protected:
            FactorablePointer generate() override
            {
                const FactoryPtr & factory = this->getFactory();

                ResourceMovie2Ptr resource = factory->createObject();

                if( resource == nullptr )
                {
                    LOGGER_ERROR( "ResourcePrototypeGenerator can't generate %s %s"
                        , m_category.c_str()
                        , m_prototype.c_str()
                    );

                    return nullptr;
                }

                resource->setType( m_prototype );

                this->setupScriptable( resource );

                resource->setMovieInstance( m_instance );
                resource->setMovieArchivator( m_archivator );

                return resource;
            }

        protected:
            const aeMovieInstance * m_instance;

            ArchivatorInterfacePtr m_archivator;
        };
    }
    //////////////////////////////////////////////////////////////////////////
    bool MoviePlugin::_avaliable()
    {
        return true;
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
                    String k = it.key();

                    LOGGER_ERROR( "Movie2::setEventListener invalid kwds '%s'\n"
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
            : public VisitorMovie2Layer
        {
        public:
            PythonVisitorMovie2Socket( pybind::kernel_interface * _kernel, const pybind::list & _list )
                : m_kernel( _kernel )
                , m_list( _list )
            {
            }

        protected:
            void visitMovieLayer( const Movie2Ptr & _movie, uint32_t _index, const NodePtr & _node ) override
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
        static Viewport s_Movie2_getCompositionBoundsWM( Movie2 * _movie )
        {
            const Viewport & bounds = _movie->getCompositionBounds();

            const mt::mat4f & wm = _movie->getWorldMatrix();

            Viewport boundsWM;
            mt::mul_v2_v2_m4( boundsWM.begin, bounds.begin, wm );
            mt::mul_v2_v2_m4( boundsWM.end, bounds.end, wm );

            return boundsWM;
        }
        //////////////////////////////////////////////////////////////////////////    
        static pybind::list s_Movie2_getSockets( pybind::kernel_interface * _kernel, Movie2 * _movie )
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
            void visitMovieLayer( const Movie2Ptr & _movie, uint32_t _index, const NodePtr & _node ) override
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

            PythonVisitorMovie2Slots visitor( _kernel, py_list );
            _movie->visitSockets( &visitor );

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
            Helper::registerAnimatableEventReceiver<PythonMovie2SubCompositionEventReceiver>( py_kwds, _node );

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

                    LOGGER_ERROR( "Movie2SubComposition::setEventListener invalid kwds '%s'\n"
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

        pybind::kernel_interface * kernel = SCRIPT_SERVICE()
            ->getKernel();

        pybind::set_kernel( kernel );

        m_instance = ae_create_movie_instance( m_hashkey.c_str(), &stdex_movie_alloc, &stdex_movie_alloc_n, &stdex_movie_free, &stdex_movie_free_n, 0, &stdex_movie_logerror, this );

        pybind::interface_<Movie2, pybind::bases<Node, Animatable> >( kernel, "Movie2", false )
            .def( "setResourceMovie2", &Movie2::setResourceMovie2 )
            .def( "getResourceMovie2", &Movie2::getResourceMovie2 )
            .def( "setCompositionName", &Movie2::setCompositionName )
            .def( "getCompositionName", &Movie2::getCompositionName )
            .def( "setTextAliasEnvironment", &Movie2::setTextAliasEnvironment )
            .def( "getTextAliasEnvironment", &Movie2::getTextAliasEnvironment )
            .def( "getDuration", &Movie2::getDuration )
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

        pybind::interface_<Movie2SubComposition, pybind::bases<Eventable, Animatable, Scriptable> >( kernel, "Movie2SubComposition", false )
            .def( "setSubMovieCompositionName", &Movie2SubComposition::setSubMovieCompositionName )
            .def( "getSubMovieCompositionName", &Movie2SubComposition::getSubMovieCompositionName )
            .def_static_native_kernel( "setEventListener", &Detail::s_Movie2SubComposition_setEventListener )
            ;

        pybind::interface_<ResourceMovie2, pybind::bases<Resource> >( kernel, "ResourceMovie2", false )
            .def( "hasComposition", &ResourceMovie2::hasComposition )
            .def( "getCompositionDuration", &ResourceMovie2::getCompositionDuration )
            .def( "getCompositionFrameDuration", &ResourceMovie2::getCompositionFrameDuration )
            ;

        SCRIPT_SERVICE()
            ->setWrapper( STRINGIZE_STRING_LOCAL( "Movie2" ), new FactorableUnique<PythonScriptWrapper<Movie2> >( kernel ) );

        SCRIPT_SERVICE()
            ->setWrapper( STRINGIZE_STRING_LOCAL( "Movie2Slot" ), new FactorableUnique<PythonScriptWrapper<Movie2Slot> >( kernel ) );

        SCRIPT_SERVICE()
            ->setWrapper( STRINGIZE_STRING_LOCAL( "Movie2SubComposition" ), new FactorableUnique<PythonScriptWrapper<Movie2SubComposition> >( kernel ) );

        SCRIPT_SERVICE()
            ->setWrapper( STRINGIZE_STRING_LOCAL( "ResourceMovie2" ), new FactorableUnique<PythonScriptWrapper<ResourceMovie2> >( kernel ) );

        if( PROTOTYPE_SERVICE()
            ->addPrototype( STRINGIZE_STRING_LOCAL( "Node" ), STRINGIZE_STRING_LOCAL( "Movie2" ), new FactorableUnique<NodePrototypeGenerator<Movie2, 128> > ) == false )
        {
            return false;
        }

        if( PROTOTYPE_SERVICE()
            ->addPrototype( STRINGIZE_STRING_LOCAL( "Node" ), STRINGIZE_STRING_LOCAL( "Movie2Slot" ), new FactorableUnique<NodePrototypeGenerator<Movie2Slot, 128> > ) == false )
        {
            return false;
        }

        if( PROTOTYPE_SERVICE()
            ->addPrototype( STRINGIZE_STRING_LOCAL( "Node" ), STRINGIZE_STRING_LOCAL( "Movie2SubComposition" ), new FactorableUnique<ScriptablePrototypeGenerator<Movie2SubComposition, 128> > ) == false )
        {
            return false;
        }

        ArchivatorInterfacePtr archivator = ARCHIVE_SERVICE()
            ->getArchivator( STRINGIZE_STRING_LOCAL( "lz4" ) );

        if( archivator == nullptr )
        {
            return false;
        }

        if( PROTOTYPE_SERVICE()
            ->addPrototype( STRINGIZE_STRING_LOCAL( "Resource" ), STRINGIZE_STRING_LOCAL( "ResourceMovie2" ), new FactorableUnique<ResourceMovie2PrototypeGenerator>( m_instance, archivator ) ) == false )
        {
            return false;
        }

        RenderVisitorPtr movie2RenderVisitor = new FactorableUnique<Movie2DebugRenderVisitor>();

        DEBUGRENDER_SERVICE()
            ->addDebugNodeRenderVisitor( movie2RenderVisitor );

        m_movie2RenderVisitor = movie2RenderVisitor;

        VisitorPtr movie2ValidateVisitor = new FactorableUnique<Movie2ResourceValidateVisitor>();

        RESOURCEVALIDATE_SERVICE()
            ->addResourceValidateVisitor( movie2ValidateVisitor );

        m_movie2ValidateVisitor = movie2ValidateVisitor;

        LOADER_SERVICE()
            ->addLoader( STRINGIZE_STRING_LOCAL( "ResourceMovie2" ), new FactorableUnique<LoaderResourceMovie2>() );

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
            ->removePrototype( STRINGIZE_STRING_LOCAL( "Node" ), STRINGIZE_STRING_LOCAL( "Movie2SubComposition" ) );

        PROTOTYPE_SERVICE()
            ->removePrototype( STRINGIZE_STRING_LOCAL( "Resource" ), STRINGIZE_STRING_LOCAL( "ResourceMovie2" ) );

        DEBUGRENDER_SERVICE()
            ->removeDebugNodeRenderVisitor( m_movie2RenderVisitor );
        m_movie2RenderVisitor = nullptr;

        RESOURCEVALIDATE_SERVICE()
            ->removeResourceValidateVisitor( m_movie2ValidateVisitor );
        m_movie2ValidateVisitor = nullptr;
    }
}
