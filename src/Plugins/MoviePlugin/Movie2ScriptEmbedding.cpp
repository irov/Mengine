#include "Movie2ScriptEmbedding.h"

#include "Interface/ScriptServiceInterface.h"
#include "Interface/VocabularyServiceInterface.h"
#include "Interface/StringizeServiceInterface.h"

#include "Environment/Python/PythonAnimatableEventReceiver.h"
#include "Environment/Python/PythonScriptWrapper.h"

#include "Movie2.h"

#include "Kernel/Logger.h"
#include "Kernel/ScriptWrapperInterface.h"

#include "pybind/pybind.hpp"
#include "pybind/stl/stl_type_cast.hpp"

namespace Mengine
{
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
            MENGINE_UNUSED( _args );

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
                MENGINE_UNUSED( _index );

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
                MENGINE_UNUSED( _index );

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
            MENGINE_UNUSED( _args );

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
    Movie2ScriptEmbedding::Movie2ScriptEmbedding()
    {
    }
    //////////////////////////////////////////////////////////////////////////
    Movie2ScriptEmbedding::~Movie2ScriptEmbedding()
    {
    }
    //////////////////////////////////////////////////////////////////////////
    bool Movie2ScriptEmbedding::embedding( pybind::kernel_interface * _kernel )
    {
        pybind::interface_<Movie2, pybind::bases<Node, Animatable> >( _kernel, "Movie2", false )
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
            .def( "hasMovieLayers", &Movie2::hasMovieLayers )
            .def( "setEnableMovieLayers", &Movie2::setEnableMovieLayers )
            ;

        pybind::interface_<Movie2Slot, pybind::bases<Node> >( _kernel, "Movie2Slot", false )
            ;

        pybind::interface_<Movie2SubComposition, pybind::bases<Eventable, Animatable, Scriptable, Identity> >( _kernel, "Movie2SubComposition", false )
            .def( "setEnable", &Movie2SubComposition::setEnable )
            .def( "getEnable", &Movie2SubComposition::getEnable )
            .def_static_native_kernel( "setEventListener", &Detail::s_Movie2SubComposition_setEventListener )
            ;

        pybind::interface_<ResourceMovie2, pybind::bases<Resource> >( _kernel, "ResourceMovie2", false )
            .def( "hasComposition", &ResourceMovie2::hasComposition )
            .def( "hasCompositionLayer", &ResourceMovie2::hasCompositionLayer )
            .def( "getCompositionDuration", &ResourceMovie2::getCompositionDuration )
            .def( "getCompositionFrameDuration", &ResourceMovie2::getCompositionFrameDuration )
            ;

        VOCABULARY_SET( ScriptWrapperInterface, STRINGIZE_STRING_LOCAL( "ClassWrapping" ), STRINGIZE_STRING_LOCAL( "Movie2" ), Helper::makeFactorableUnique<PythonScriptWrapper<Movie2> >( _kernel ) );
        VOCABULARY_SET( ScriptWrapperInterface, STRINGIZE_STRING_LOCAL( "ClassWrapping" ), STRINGIZE_STRING_LOCAL( "Movie2Slot" ), Helper::makeFactorableUnique<PythonScriptWrapper<Movie2Slot> >( _kernel ) );
        VOCABULARY_SET( ScriptWrapperInterface, STRINGIZE_STRING_LOCAL( "ClassWrapping" ), STRINGIZE_STRING_LOCAL( "Movie2SubComposition" ), Helper::makeFactorableUnique<PythonScriptWrapper<Movie2SubComposition> >( _kernel ) );
        VOCABULARY_SET( ScriptWrapperInterface, STRINGIZE_STRING_LOCAL( "ClassWrapping" ), STRINGIZE_STRING_LOCAL( "ResourceMovie2" ), Helper::makeFactorableUnique<PythonScriptWrapper<ResourceMovie2> >( _kernel ) );

        return true;
    }
    //////////////////////////////////////////////////////////////////////////
    void Movie2ScriptEmbedding::ejecting( pybind::kernel_interface * _kernel )
    {
        _kernel->remove_from_module( "Movie2", nullptr );
        _kernel->remove_from_module( "Movie2Slot", nullptr );
        _kernel->remove_from_module( "Movie2SubComposition", nullptr );
        _kernel->remove_from_module( "ResourceMovie2", nullptr );

        VOCABULARY_REMOVE( STRINGIZE_STRING_LOCAL( "ClassWrapping" ), STRINGIZE_STRING_LOCAL( "Movie2" ) );
        VOCABULARY_REMOVE( STRINGIZE_STRING_LOCAL( "ClassWrapping" ), STRINGIZE_STRING_LOCAL( "Movie2Slot" ) );
        VOCABULARY_REMOVE( STRINGIZE_STRING_LOCAL( "ClassWrapping" ), STRINGIZE_STRING_LOCAL( "Movie2SubComposition" ) );
        VOCABULARY_REMOVE( STRINGIZE_STRING_LOCAL( "ClassWrapping" ), STRINGIZE_STRING_LOCAL( "ResourceMovie2" ) );
    }
}

