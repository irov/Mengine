#include "MovieScriptEmbedding.h"

#include "Interface/ResourceServiceInterface.h"
#include "Interface/ApplicationInterface.h"
#include "Interface/VocabularyServiceInterface.h"

#include "Environment/Python/PythonAnimatableEventReceiver.h"
#include "Environment/Python/PythonScriptWrapper.h"
#include "Environment/Python/PythonDocumentTraceback.h"

#include "Movie.h"
#include "MovieSlot.h"
#include "MovieEvent.h"
#include "MovieMesh2D.h"

#include "Engine/HotSpot.h"
#include "Engine/HotSpotResourceShape.h"
#include "Engine/HotSpotSurface.h"

#include "Kernel/Logger.h"
#include "Kernel/PolygonHelper.h"
#include "Kernel/NodeRenderHierarchy.h"
#include "Kernel/AssertionNotImplemented.h"
#include "Kernel/AssertionMemoryPanic.h"
#include "Kernel/ConstStringHelper.h"
#include "Kernel/ArrayString.h"
#include "Kernel/RenderContextHelper.h"

#include "pybind/pybind.hpp"

namespace Mengine
{
    //////////////////////////////////////////////////////////////////////////
    namespace Detail
    {
        //////////////////////////////////////////////////////////////////////////
        static NodePtr Movie_getMovieNode( Movie * _movie, const ConstString & _name )
        {
            NodePtr node;
            MoviePtr submovie;
            if( _movie->getMovieNode( _name, ConstString::none(), &node, &submovie ) == false )
            {
                return nullptr;
            }

            return node;
        }
        //////////////////////////////////////////////////////////////////////////
        static bool Movie_hasMovieNode( Movie * _movie, const ConstString & _name )
        {
            NodePtr node;
            MoviePtr submovie;
            bool successful = _movie->hasMovieNode( _name, ConstString::none(), &node, &submovie );

            return successful;
        }
        //////////////////////////////////////////////////////////////////////////
        static mt::vec3f Movie_getWorldAnchorPoint( Movie * _movie )
        {
            const ResourceMoviePtr & resourceMovie = _movie->getResourceMovie();

            MENGINE_ASSERTION_MEMORY_PANIC( resourceMovie, "'%s' invalid setup resource"
                , _movie->getName().c_str()
            );

            const mt::vec3f & ap = resourceMovie->getAnchorPoint();

            const TransformationInterface * transformation = _movie->getTransformation();

            const mt::mat4f & wm = transformation->getWorldMatrix();

            mt::vec3f wap;
            mt::mul_v3_v3_m4( wap, ap, wm );

            return wap;
        }
        //////////////////////////////////////////////////////////////////////////
        static PyObject * Movie_getEnableMovieLayer( pybind::kernel_interface * _kernel, Movie * _movie, const ConstString & _name )
        {
            bool enable;
            if( _movie->getEnableMovieLayer( _name, &enable ) == false )
            {
                return _kernel->ret_none();
            }

            return pybind::ptr( _kernel, enable );
        }
        //////////////////////////////////////////////////////////////////////////
        static NodePtr Movie_getMovieSlot( Movie * _movie, const ConstString & _name )
        {
            NodePtr node;
            MoviePtr submovie;
            if( _movie->getMovieNode( _name, STRINGIZE_STRING_LOCAL( "MovieSlot" ), &node, &submovie ) == false )
            {
                return nullptr;
            }

            return node;
        }
        //////////////////////////////////////////////////////////////////////////
        static RenderScissorPtr Movie_getMovieScissor( Movie * _movie, const ConstString & _name )
        {
            RenderScissorPtr scissor;
            if( _movie->getMovieScissor( _name, &scissor ) == false )
            {
                return nullptr;
            }

            return scissor;
        }
        //////////////////////////////////////////////////////////////////////////
        static bool Movie_hasMovieScissor( Movie * _movie, const ConstString & _name )
        {
            bool result = _movie->hasMovieScissor( _name );

            return result;
        }
        //////////////////////////////////////////////////////////////////////////
        static bool Movie_hasMovieSlot( Movie * _movie, const ConstString & _name )
        {
            NodePtr node;
            MoviePtr submovie;
            bool successful = _movie->hasMovieNode( _name, STRINGIZE_STRING_LOCAL( "MovieSlot" ), &node, &submovie );

            return successful;
        }
        //////////////////////////////////////////////////////////////////////////
        static NodePtr Movie_getMovieText( Movie * _movie, const ConstString & _name )
        {
            NodePtr node;
            MoviePtr submovie;

            if( _movie->getMovieNode( _name, STRINGIZE_STRING_LOCAL( "MovieText" ), &node, &submovie ) == false )
            {
                return nullptr;
            }

            return node;
        }
        //////////////////////////////////////////////////////////////////////////
        static bool Movie_hasMovieText( Movie * _movie, const ConstString & _name )
        {
            NodePtr node;
            MoviePtr submovie;

            bool successful = _movie->hasMovieNode( _name, STRINGIZE_STRING_LOCAL( "MovieText" ), &node, &submovie );

            return successful;
        }
        //////////////////////////////////////////////////////////////////////////
        static NodePtr Movie_getSubMovie( Movie * _movie, const ConstString & _name )
        {
            NodePtr node;
            MoviePtr submovie;
            if( _movie->getMovieNode( _name, STRINGIZE_STRING_LOCAL( "SubMovie" ), &node, &submovie ) == false )
            {
                return nullptr;
            }

            return node;
        }
        //////////////////////////////////////////////////////////////////////////
        static bool Movie_hasSubMovie( Movie * _movie, const ConstString & _name )
        {
            NodePtr node;
            MoviePtr submovie;
            bool successful = _movie->hasMovieNode( _name, STRINGIZE_STRING_LOCAL( "SubMovie" ), &node, &submovie );

            return successful;
        }
        //////////////////////////////////////////////////////////////////////////
        static NodePtr Movie_getSocket( Movie * _movie, const ConstString & _name )
        {
            NodePtr node;
            MoviePtr submovie;

            if( _movie->hasMovieNode( _name, STRINGIZE_STRING_LOCAL( "MovieSocketImage" ), &node, &submovie ) == true )
            {
                return node;
            }

            if( _movie->hasMovieNode( _name, STRINGIZE_STRING_LOCAL( "MovieSocketShape" ), &node, &submovie ) == true )
            {
                return node;
            }

            LOGGER_ERROR( "movie '%s' resource '%s' not found '%s'"
                , _movie->getName().c_str()
                , _movie->getResourceMovieName().c_str()
                , _name.c_str()
            );

            return nullptr;
        }
        //////////////////////////////////////////////////////////////////////////
        static bool Movie_hasSocket( Movie * _movie, const ConstString & _name )
        {
            NodePtr node;
            MoviePtr submovie;

            if( _movie->hasMovieNode( _name, STRINGIZE_STRING_LOCAL( "MovieSocketImage" ), &node, &submovie ) == true )
            {
                return true;
            }

            if( _movie->hasMovieNode( _name, STRINGIZE_STRING_LOCAL( "MovieSocketShape" ), &node, &submovie ) == true )
            {
                return true;
            }

            return false;
        }
        //////////////////////////////////////////////////////////////////////////
        static bool Movie_setMovieEvent( Movie * _movie, const ConstString & _name, const pybind::object & _cb, const pybind::args & _args )
        {
            NodePtr node;
            MoviePtr submovie;

            if( _movie->getMovieNode( _name, STRINGIZE_STRING_LOCAL( "MovieEvent" ), &node, &submovie ) == false )
            {
                return false;
            }

            MovieEventPtr ev = stdex::intrusive_static_cast<MovieEventPtr>(node);

            ev->setEvent( _cb, _args );

            return true;
        }
        //////////////////////////////////////////////////////////////////////////
        static bool Movie_removeMovieEvent( Movie * _movie, const ConstString & _name )
        {
            NodePtr node;
            MoviePtr submovie;

            if( _movie->getMovieNode( _name, STRINGIZE_STRING_LOCAL( "MovieEvent" ), &node, &submovie ) == false )
            {
                return false;
            }

            MovieEventPtr ev = stdex::intrusive_static_cast<MovieEventPtr>(node);

            ev->removeEvent();

            return true;
        }
        //////////////////////////////////////////////////////////////////////////
        static bool Movie_hasMovieEvent( Movie * _movie, const ConstString & _name )
        {
            NodePtr node;
            MoviePtr submovie;
            bool successful = _movie->hasMovieNode( _name, STRINGIZE_STRING_LOCAL( "MovieEvent" ), &node, &submovie );

            return successful;
        }
        //////////////////////////////////////////////////////////////////////////
        class PythonVisitorMovieSlot
            : public VisitorMovieNodeInterface
            , public Factorable
        {
        public:
            PythonVisitorMovieSlot( pybind::kernel_interface * _kernel, pybind::list * _list )
                : m_kernel( _kernel )
                , m_list( _list )
            {
            }

            ~PythonVisitorMovieSlot() override
            {
            }

        protected:
            void visitMovieNode( Movie * _movie, const NodePtr & _node ) override
            {
                MovieSlotPtr slot = stdex::intrusive_static_cast<MovieSlotPtr>(_node);

                if( slot == nullptr )
                {
                    return;
                }

                const ConstString & name = slot->getName();

                pybind::tuple py_value = pybind::make_tuple_t( m_kernel, _movie, name, slot );
                m_list->append( py_value );
            }

        protected:
            pybind::kernel_interface * m_kernel;
            pybind::list * m_list;

        private:
            PythonVisitorMovieSlot( const PythonVisitorMovieSlot & );
            void operator = ( const PythonVisitorMovieSlot & );
        };
        //////////////////////////////////////////////////////////////////////////
        static pybind::list Movie_getSlots( pybind::kernel_interface * _kernel, Movie * _movie )
        {
            pybind::list py_list( _kernel );

            VisitorMovieNodeInterfacePtr visitor = Helper::makeFactorableUnique<PythonVisitorMovieSlot>( MENGINE_DOCUMENT_PYBIND, _kernel, &py_list );
            _movie->visitMovieLayer( STRINGIZE_STRING_LOCAL( "MovieSlot" ), visitor );

            return py_list;
        }
        //////////////////////////////////////////////////////////////////////////
        class PythonVisitorMovieSocket
            : public VisitorMovieNodeInterface
            , public Factorable
        {
        public:
            PythonVisitorMovieSocket( pybind::kernel_interface * _kernel, const pybind::list & _list )
                : m_kernel( _kernel )
                , m_list( _list )
            {
            }

            ~PythonVisitorMovieSocket() override
            {
            }

        protected:
            void visitMovieNode( Movie * _movie, const NodePtr & _node ) override
            {
                HotSpotPtr hotspot = stdex::intrusive_static_cast<HotSpotPtr>(_node);

                if( hotspot == nullptr )
                {
                    return;
                }

                const ConstString & name = hotspot->getName();

                m_list.append( pybind::make_tuple_t( m_kernel, _movie, name, hotspot ) );
            }

        protected:
            pybind::kernel_interface * m_kernel;
            pybind::list m_list;
        };
        //////////////////////////////////////////////////////////////////////////
        static pybind::list Movie_getSockets( pybind::kernel_interface * _kernel, Movie * _movie )
        {
            pybind::list py_list( _kernel );

            VisitorMovieNodeInterfacePtr visitor = Helper::makeFactorableUnique<PythonVisitorMovieSocket>( MENGINE_DOCUMENT_PYBIND, _kernel, py_list );
            _movie->visitMovieLayer( STRINGIZE_STRING_LOCAL( "MovieSocketImage" ), visitor );
            _movie->visitMovieLayer( STRINGIZE_STRING_LOCAL( "MovieSocketShape" ), visitor );

            return py_list;
        }
        //////////////////////////////////////////////////////////////////////////
        class PythonVisitorMovieSubMovie
            : public VisitorMovieNodeInterface
            , public Factorable
        {
        public:
            PythonVisitorMovieSubMovie( pybind::kernel_interface * _kernel, const pybind::list & _list )
                : m_kernel( _kernel )
                , m_list( _list )
            {
            }

            ~PythonVisitorMovieSubMovie() override
            {
            }

        protected:
            void visitMovieNode( Movie * _movie, const NodePtr & _node ) override
            {
                MoviePtr subMovie = stdex::intrusive_static_cast<MoviePtr>(_node);

                if( subMovie == nullptr )
                {
                    return;
                }

                const ConstString & name = subMovie->getName();

                m_list.append( pybind::make_tuple_t( m_kernel, _movie, name, subMovie ) );
            }

        protected:
            pybind::kernel_interface * m_kernel;
            pybind::list m_list;
        };
        //////////////////////////////////////////////////////////////////////////
        static pybind::list Movie_getSubMovies( pybind::kernel_interface * _kernel, Movie * _movie )
        {
            pybind::list py_list( _kernel );

            VisitorMovieNodeInterfacePtr visitor = Helper::makeFactorableUnique<PythonVisitorMovieSubMovie>( MENGINE_DOCUMENT_PYBIND, _kernel, py_list );
            _movie->visitMovieLayer( STRINGIZE_STRING_LOCAL( "SubMovie" ), visitor );

            return py_list;
        }
        //////////////////////////////////////////////////////////////////////////
        class PyVisitorMovieLayer
            : public VisitorMovieNodeInterface
            , public Factorable
        {
        public:
            PyVisitorMovieLayer( pybind::kernel_interface * _kernel, const pybind::list & _list )
                : m_kernel( _kernel )
                , m_list( _list )
            {
            }

            ~PyVisitorMovieLayer() override
            {
            }

        protected:
            void visitMovieNode( Movie * _movie, const NodePtr & _node ) override
            {
                m_list.append( pybind::make_tuple_t( m_kernel, _movie, _node ) );
            }

        protected:
            pybind::kernel_interface * m_kernel;
            pybind::list m_list;
        };
        //////////////////////////////////////////////////////////////////////////
        static pybind::list Movie_filterLayers( pybind::kernel_interface * _kernel, Movie * _movie, const ConstString & _type )
        {
            pybind::list py_list( _kernel );

            VisitorMovieNodeInterfacePtr visitor = Helper::makeFactorableUnique<PyVisitorMovieLayer>( MENGINE_DOCUMENT_PYBIND, _kernel, py_list );
            _movie->visitMovieLayer( _type, visitor );

            return py_list;
        }
        //////////////////////////////////////////////////////////////////////////
        static float Movie_getFrameDuration( Movie * _movie )
        {
            const ResourceMoviePtr & resourceMovie = _movie->getResourceMovie();

            MENGINE_ASSERTION_MEMORY_PANIC( resourceMovie, "'%s' not activate"
                , _movie->getName().c_str()
            );

            float frameDuration = resourceMovie->getFrameDuration();

            return frameDuration;
        }
        //////////////////////////////////////////////////////////////////////////
        static float Movie_getDuration( Movie * _movie )
        {
            const ResourceMoviePtr & resourceMovie = _movie->getResourceMovie();

            MENGINE_ASSERTION_MEMORY_PANIC( resourceMovie, "'%s' not activate"
                , _movie->getName().c_str()
            );

            float duration = resourceMovie->getDuration();

            return duration;
        }
        //////////////////////////////////////////////////////////////////////////
        static uint32_t Movie_getFrameCount( Movie * _movie )
        {
            const ResourceMoviePtr & resourceMovie = _movie->getResourceMovie();

            MENGINE_ASSERTION_MEMORY_PANIC( resourceMovie, "'%s' not activate"
                , _movie->getName().c_str()
            );

            uint32_t count = resourceMovie->getFrameCount();

            return count;
        }
        //////////////////////////////////////////////////////////////////////////
        static const mt::vec2f & Movie_getSize( Movie * _movie )
        {
            const ResourceMoviePtr & resourceMovie = _movie->getResourceMovie();

            MENGINE_ASSERTION_MEMORY_PANIC( resourceMovie, "'%s' not activate"
                , _movie->getName().c_str()
            );

            const mt::vec2f & size = resourceMovie->getSize();

            return size;
        }
        //////////////////////////////////////////////////////////////////////////
        static float Movie_getLayerPathLength( Movie * _movie, const ConstString & _name )
        {
            const MovieLayer * layer;
            MoviePtr sub_movie;
            if( _movie->getMovieLayer( _name, &layer, &sub_movie ) == false )
            {
                LOGGER_ERROR( "'%s' not found layer '%s'"
                    , _movie->getName().c_str()
                    , _name.c_str()
                );

                return 0.f;
            }

            const ResourceMoviePtr & resourceMovie = sub_movie->getResourceMovie();

            float frameDuration = resourceMovie->getFrameDuration();
            uint32_t indexIn = (uint32_t)((layer->in / frameDuration) + 0.5f);
            uint32_t indexOut = (uint32_t)((layer->out / frameDuration) + 0.5f);
            uint32_t indexCount = indexOut - indexIn;

            const MovieFramePackInterfacePtr & framePack = resourceMovie->getFramePack();

            MovieFrameSource start_frame;
            if( framePack->getLayerFrame( layer->index, 0, &start_frame ) == false )
            {
                LOGGER_ERROR( "movie '%s' invalid get layer '%s' frame [%u]"
                    , _movie->getName().c_str()
                    , _name.c_str()
                    , layer->index
                );

                return 0.f;
            }

            mt::vec3f pos = start_frame.position;
            float len = 0.f;

            for( uint32_t i = 1; i != indexCount; ++i )
            {
                MovieFrameSource frame;
                framePack->getLayerFrame( layer->index, i, &frame );

                len += mt::length_v3_v3( pos, frame.position );
                pos = frame.position;
            }

            return len;
        }
        //////////////////////////////////////////////////////////////////////////
        static pybind::list Movie_getLayerPath( pybind::kernel_interface * _kernel, Movie * _movie, const ConstString & _name )
        {
            const MovieLayer * layer;
            MoviePtr sub_movie;
            if( _movie->getMovieLayer( _name, &layer, &sub_movie ) == false )
            {
                LOGGER_ERROR( "'%s' not found layer '%s'"
                    , _movie->getName().c_str()
                    , _name.c_str()
                );

                return pybind::make_invalid_list_t();
            }

            const ResourceMoviePtr & resourceMovie = sub_movie->getResourceMovie();

            float frameDuration = resourceMovie->getFrameDuration();
            uint32_t indexIn = (uint32_t)((layer->in / frameDuration) + 0.5f);
            uint32_t indexOut = (uint32_t)((layer->out / frameDuration) + 0.5f);
            uint32_t indexCount = indexOut - indexIn;

            bool isCompile = resourceMovie->isCompile();

            if( isCompile == false )
            {
                resourceMovie->compile();
            }

            const MovieFramePackInterfacePtr & framePack = resourceMovie->getFramePack();

            MENGINE_ASSERTION_MEMORY_PANIC( framePack, "sub_movie '%s' not found layer '%s' frame pack is nullptr"
                , sub_movie->getName().c_str()
                , _name.c_str()
            );

            pybind::list py_path( _kernel, indexCount );

            for( uint32_t i = 0; i != indexCount; ++i )
            {
                MovieFrameSource frame;
                framePack->getLayerFrame( layer->index, i, &frame );

                py_path[i] = frame.position;
            }

            if( isCompile == false )
            {
                resourceMovie->release();
            }

            return py_path;
        }
        //////////////////////////////////////////////////////////////////////////
        static pybind::list Movie_getLayerPath2( pybind::kernel_interface * _kernel, Movie * _movie, const ConstString & _name )
        {
            const MovieLayer * layer;
            MoviePtr sub_movie;
            if( _movie->getMovieLayer( _name, &layer, &sub_movie ) == false )
            {
                LOGGER_ERROR( "'%s' not found layer '%s'"
                    , _movie->getName().c_str()
                    , _name.c_str()
                );

                return pybind::make_invalid_list_t();
            }

            const ResourceMoviePtr & resourceMovie = sub_movie->getResourceMovie();

            float frameDuration = resourceMovie->getFrameDuration();
            uint32_t indexIn = (uint32_t)((layer->in / frameDuration) + 0.5f);
            uint32_t indexOut = (uint32_t)((layer->out / frameDuration) + 0.5f);
            uint32_t indexCount = indexOut - indexIn;

            bool isCompile = resourceMovie->isCompile();

            if( isCompile == false )
            {
                resourceMovie->compile();
            }

            const MovieFramePackInterfacePtr & framePack = resourceMovie->getFramePack();

            pybind::list py_path( _kernel, indexCount );

            for( uint32_t i = 0; i != indexCount; ++i )
            {
                MovieFrameSource frame;
                framePack->getLayerFrame( layer->index, i, &frame );

                mt::vec2f pos;
                pos.x = frame.position.x;
                pos.y = frame.position.y;

                py_path[i] = pos;
            }

            if( isCompile == false )
            {
                resourceMovie->release();
            }

            return py_path;
        }
        //////////////////////////////////////////////////////////////////////////
        static pybind::list Movie_getLayerPath3( pybind::kernel_interface * _kernel, Movie * _movie, const ConstString & _name )
        {
            const MovieLayer * layer;
            MoviePtr sub_movie;
            if( _movie->getMovieLayer( _name, &layer, &sub_movie ) == false )
            {
                LOGGER_ERROR( "'%s' not found layer '%s'"
                    , _movie->getName().c_str()
                    , _name.c_str()
                );

                return pybind::make_invalid_list_t();
            }

            const ResourceMoviePtr & resourceMovie = sub_movie->getResourceMovie();

            float frameDuration = resourceMovie->getFrameDuration();
            uint32_t indexIn = (uint32_t)((layer->in / frameDuration) + 0.5f);
            uint32_t indexOut = (uint32_t)((layer->out / frameDuration) + 0.5f);
            uint32_t indexCount = indexOut - indexIn;

            bool isCompile = resourceMovie->isCompile();

            if( isCompile == false )
            {
                if( resourceMovie->compile() == false )
                {
                    LOGGER_ERROR( "'%s' invalid compile"
                        , _movie->getName().c_str()
                    );

                    return pybind::make_invalid_list_t();
                }
            }

            const MovieFramePackInterfacePtr & framePack = resourceMovie->getFramePack();

            const TransformationInterface * transformation = _movie->getTransformation();

            const mt::mat4f & wm = transformation->getWorldMatrix();

            pybind::list py_path( _kernel, indexCount );

            for( uint32_t i = 0; i != indexCount; ++i )
            {
                MovieFrameSource frame;
                framePack->getLayerFrame( layer->index, i, &frame );

                mt::vec3f pos;
                mt::mul_v3_v3_m4( pos, frame.position, wm );

                pos.y *= mt::constant::sqrt2;

                py_path[i] = pos;
            }

            if( isCompile == false )
            {
                resourceMovie->release();
            }

            return py_path;
        }
        //////////////////////////////////////////////////////////////////////////
        static pybind::list Movie_getLayerPath4( pybind::kernel_interface * _kernel, Movie * _movie, const ConstString & _name )
        {
            const MovieLayer * layer;
            MoviePtr sub_movie;
            if( _movie->getMovieLayer( _name, &layer, &sub_movie ) == false )
            {
                LOGGER_ERROR( "'%s' not found layer '%s'"
                    , _movie->getName().c_str()
                    , _name.c_str()
                );

                return pybind::make_invalid_list_t();
            }

            const ResourceMoviePtr & resourceMovie = sub_movie->getResourceMovie();

            float frameDuration = resourceMovie->getFrameDuration();
            uint32_t indexIn = (uint32_t)((layer->in / frameDuration) + 0.5f);
            uint32_t indexOut = (uint32_t)((layer->out / frameDuration) + 0.5f);
            uint32_t indexCount = indexOut - indexIn;

            bool isCompile = resourceMovie->isCompile();

            if( isCompile == false )
            {
                if( resourceMovie->compile() == false )
                {
                    LOGGER_ERROR( "'%s' invalid compile"
                        , _movie->getName().c_str()
                    );

                    return pybind::make_invalid_list_t();
                }
            }

            const MovieFramePackInterfacePtr & framePack = resourceMovie->getFramePack();

            const TransformationInterface * transformation = _movie->getTransformation();

            const mt::mat4f & wm = transformation->getWorldMatrix();

            pybind::list py_path( _kernel, indexCount );

            for( uint32_t i = 0; i != indexCount; ++i )
            {
                MovieFrameSource frame;
                framePack->getLayerFrame( layer->index, i, &frame );

                mt::vec3f pos;
                mt::mul_v3_v3_m4( pos, frame.position, wm );

                py_path[i] = pos;
            }

            if( isCompile == false )
            {
                resourceMovie->release();
            }

            return py_path;
        }
        //////////////////////////////////////////////////////////////////////////
        static mt::vec3f Movie_getMovieSlotWorldPosition( Movie * _movie, const ConstString & _slotName )
        {
            NodePtr node;
            MoviePtr submovie;
            if( _movie->getMovieNode( _slotName, STRINGIZE_STRING_LOCAL( "MovieSlot" ), &node, &submovie ) == false )
            {
                LOGGER_ERROR( "'%s' not found slot '%s'"
                    , _movie->getName().c_str()
                    , _slotName.c_str()
                );

                return mt::vec3f( 0.f, 0.f, 0.f );
            }

            const TransformationInterface * transformation = node->getTransformation();

            const mt::vec3f & wp = transformation->getWorldPosition();

            return wp;
        }
        //////////////////////////////////////////////////////////////////////////
        static mt::vec3f Movie_getMovieSlotOffsetPosition( Movie * _movie, const ConstString & _slotName )
        {
            NodePtr node;
            MoviePtr submovie;
            if( _movie->getMovieNode( _slotName, STRINGIZE_STRING_LOCAL( "MovieSlot" ), &node, &submovie ) == false )
            {
                LOGGER_ERROR( "'%s' not found slot '%s'"
                    , _movie->getName().c_str()
                    , _slotName.c_str()
                );

                return mt::vec3f( 0.f, 0.f, 0.f );
            }

            const ResourceMoviePtr & resourceMovie = _movie->getResourceMovie();

            MENGINE_ASSERTION_MEMORY_PANIC( resourceMovie, "'%s' invalid setup resource"
                , _movie->getName().c_str()
            );

            const mt::vec3f & ap = resourceMovie->getAnchorPoint();

            const TransformationInterface * movieTransformation = _movie->getTransformation();

            const mt::mat4f & wm = movieTransformation->getWorldMatrix();

            mt::vec3f wap;
            mt::mul_v3_v3_m4( wap, ap, wm );

            const TransformationInterface * nodeTransformation = node->getTransformation();

            const mt::vec3f & wp = nodeTransformation->getWorldPosition();

            mt::vec3f sop = wp - wap;

            return sop;
        }
        //////////////////////////////////////////////////////////////////////////
        static bool Movie_attachMovieSlotNode( Movie * _movie, const ConstString & _slotName, const NodePtr & _node )
        {
            NodePtr node;
            MoviePtr submovie;
            if( _movie->getMovieNode( _slotName, STRINGIZE_STRING_LOCAL( "MovieSlot" ), &node, &submovie ) == false )
            {
                LOGGER_ERROR( "'%s' not found slot '%s'"
                    , _movie->getName().c_str()
                    , _slotName.c_str()
                );

                return false;
            }

            node->addChild( _node );

            return true;
        }
        //////////////////////////////////////////////////////////////////////////
        static bool Movie_removeAllMovieSlotNode( Movie * _movie, const ConstString & _slotName )
        {
            NodePtr node;
            MoviePtr submovie;
            if( _movie->getMovieNode( _slotName, STRINGIZE_STRING_LOCAL( "MovieSlot" ), &node, &submovie ) == false )
            {
                LOGGER_ERROR( "'%s' not found slot '%s'"
                    , _movie->getName().c_str()
                    , _slotName.c_str()
                );

                return false;
            }

            node->removeChildren( []( const NodePtr & )
            {} );

            return true;
        }
        //////////////////////////////////////////////////////////////////////////
        class PythonMovieEventReceiver
            : public PythonAnimatableEventReceiver<MovieEventReceiverInterface>
        {
        public:
            PythonMovieEventReceiver()
            {
            }

            ~PythonMovieEventReceiver() override
            {
            }

        public:
            pybind::object onMovieGetInternal( const ConstString & _group, const ConstString & _name ) const override
            {
                return m_cb.call( _group, _name );
            }

            NodePtr onMovieActivateInternal( const pybind::object & _internal ) override
            {
                return m_cb.call( _internal );
            }

            void onMovieDeactivateInternal( const pybind::object & _internal ) override
            {
                m_cb.call( _internal );
            }
        };
        //////////////////////////////////////////////////////////////////////////
        static PyObject * Movie_setEventListener( pybind::kernel_interface * _kernel, Movie * _node, PyObject * _args, PyObject * _kwds )
        {
            MENGINE_UNUSED( _args );

            MENGINE_ASSERTION_MEMORY_PANIC( _kwds );

            pybind::dict py_kwds( _kernel, _kwds );
            Helper::registerAnimatableEventReceiver<PythonMovieEventReceiver>( _kernel, py_kwds, _node, MENGINE_DOCUMENT_PYBIND );

            Helper::registerPythonEventReceiver<PythonMovieEventReceiver>( _kernel, py_kwds, _node, "onMovieGetInternal", EVENT_MOVIE_GET_INTERNAL, MENGINE_DOCUMENT_PYBIND );
            Helper::registerPythonEventReceiver<PythonMovieEventReceiver>( _kernel, py_kwds, _node, "onMovieActivateInternal", EVENT_MOVIE_ACTIVATE_INTERNAL, MENGINE_DOCUMENT_PYBIND );
            Helper::registerPythonEventReceiver<PythonMovieEventReceiver>( _kernel, py_kwds, _node, "onMovieDeactivateInternal", EVENT_MOVIE_DEACTIVATE_INTERNAL, MENGINE_DOCUMENT_PYBIND );

            MENGINE_ASSERTION_PYTHON_EVENT_RECEIVER( _node, py_kwds );

            return _kernel->ret_none();
        }
        //////////////////////////////////////////////////////////////////////////
        static bool ResourceMovie_hasLayer( ResourceMovie * _movie, const ConstString & _name )
        {
            const MovieLayer * layer;
            bool successful = _movie->hasMovieLayer( _name, &layer );

            return successful;
        }
        //////////////////////////////////////////////////////////////////////////
        static bool ResourceMovie_hasLayerType( ResourceMovie * _movie, const ConstString & _name, const ConstString & _type )
        {
            const MovieLayer * layer;
            bool successful = _movie->hasMovieLayerType( _name, _type, &layer );

            return successful;
        }
        //////////////////////////////////////////////////////////////////////////
        static mt::vec3f ResourceMovie_getLayerPosition( ResourceMovie * _movie, const ConstString & _name )
        {
            const MovieLayer * layer;
            bool successful = _movie->hasMovieLayer( _name, &layer );

            if( successful == false )
            {
                LOGGER_ERROR( "'%s' not found layer '%s'"
                    , _movie->getName().c_str()
                    , _name.c_str()
                );

                return mt::vec3f( 0.f, 0.f, 0.f );
            }

            return layer->position;
        }
        //////////////////////////////////////////////////////////////////////////
        static float ResourceMovie_getLayerIn( ResourceMovie * _movie, const ConstString & _name )
        {
            const MovieLayer * layer;
            bool successful = _movie->hasMovieLayer( _name, &layer );

            if( successful == false )
            {
                LOGGER_ERROR( "'%s' not found layer '%s'"
                    , _movie->getName().c_str()
                    , _name.c_str()
                );

                return 0.f;
            }

            return layer->in;
        }
        //////////////////////////////////////////////////////////////////////////
        static bool s_intersectsMoviesHotspot( Movie * _movie1, const ConstString & _socket1, Movie * _movie2, const ConstString & _socket2 )
        {
            MENGINE_ASSERTION_MEMORY_PANIC( _movie1, "movie1 is nullptr" );
            MENGINE_ASSERTION_MEMORY_PANIC( _movie2, "movie2 is nullptr" );

            if( _movie1->enable() == false )
            {
                LOGGER_ERROR( "movie1 invalid compile"
                );

                return false;
            }

            if( _movie2->enable() == false )
            {
                LOGGER_ERROR( "movie2 invalid compile"
                );

                return false;
            }

            NodePtr node1;
            MoviePtr submovie1;

            if( _movie1->hasMovieNode( _socket1, STRINGIZE_STRING_LOCAL( "MovieSocketShape" ), &node1, &submovie1 ) == false )
            {
                LOGGER_ERROR( "movie1 '%s' not found socket shape '%s'"
                    , _movie1->getName().c_str()
                    , _socket1.c_str()
                );

                return false;
            }

            NodePtr node2;
            MoviePtr submovie2;

            if( _movie2->hasMovieNode( _socket2, STRINGIZE_STRING_LOCAL( "MovieSocketShape" ), &node2, &submovie2 ) == false )
            {
                LOGGER_ERROR( "movie2 '%s' not found socket shape '%s'"
                    , _movie2->getName().c_str()
                    , _socket2.c_str()
                );

                return false;
            }

            HotSpotResourceShapePtr shape1 = stdex::intrusive_static_cast<HotSpotResourceShapePtr>(node1);

            if( shape1->enable() == false )
            {
                LOGGER_ERROR( "movie1 '%s' socket shape '%s' invalid compile"
                    , _movie1->getName().c_str()
                    , _socket1.c_str()
                );

                return false;
            }

            HotSpotResourceShapePtr shape2 = stdex::intrusive_static_cast<HotSpotResourceShapePtr>(node2);

            if( shape2->enable() == false )
            {
                LOGGER_ERROR( "movie2 '%s' socket shape '%s' invalid compile"
                    , _movie2->getName().c_str()
                    , _socket2.c_str()
                );

                return false;
            }

            const Resolution & contentResolution = APPLICATION_SERVICE()
                ->getContentResolution();

            const RenderCameraInterfacePtr & shape1_camera = Helper::getNodeRenderCameraInheritance( shape1.get() );
            const RenderViewportInterfacePtr & shape1_viewport = Helper::getNodeRenderViewportInheritance( shape1.get() );

            RenderContext shape1_context;
            Helper::clearRenderContext( &shape1_context );

            shape1_context.camera = shape1_camera.get();
            shape1_context.viewport = shape1_viewport.get();

            mt::box2f b1;
            Polygon p1;
            shape1->getScreenPolygon( &shape1_context, contentResolution, &b1, &p1 );

            const RenderCameraInterfacePtr & shape2_camera = Helper::getNodeRenderCameraInheritance( shape2.get() );
            const RenderViewportInterfacePtr & shape2_viewport = Helper::getNodeRenderViewportInheritance( shape2.get() );

            RenderContext shape2_context;
            Helper::clearRenderContext( &shape2_context );

            shape2_context.camera = shape2_camera.get();
            shape2_context.viewport = shape2_viewport.get();

            mt::box2f b2;
            Polygon p2;
            shape2->getScreenPolygon( &shape2_context, contentResolution, &b2, &p2 );

            if( mt::is_intersect( b1, b2 ) == false )
            {
                return false;
            }

            bool intersect = Helper::intersects( p1, p2 );

            return intersect;
        }
        //////////////////////////////////////////////////////////////////////////
        static bool s_intersectMoviesHotspotVsPolygon( const MoviePtr & _movie, const ConstString & _socket, const Polygon & _polygon )
        {
            MENGINE_ASSERTION_MEMORY_PANIC( _movie, "movie is nullptr" );

            if( _movie->enable() == false )
            {
                LOGGER_ERROR( "movie invalid compile"
                );

                return false;
            }

            NodePtr node;
            MoviePtr submovie;

            if( _movie->hasMovieNode( _socket, STRINGIZE_STRING_LOCAL( "MovieSocketShape" ), &node, &submovie ) == false )
            {
                LOGGER_ERROR( "movie '%s' not found socket shape '%s'"
                    , _movie->getName().c_str()
                    , _socket.c_str()
                );

                return false;
            }

            HotSpotResourceShapePtr shape = stdex::intrusive_static_cast<HotSpotResourceShapePtr>(node);

            if( shape->enable() == false )
            {
                LOGGER_ERROR( "movie '%s' socket shape '%s' invalid compile"
                    , _movie->getName().c_str()
                    , _socket.c_str()
                );

                return false;
            }

            const Resolution & contentResolution = APPLICATION_SERVICE()
                ->getContentResolution();

            const RenderCameraInterfacePtr & shape_camera = Helper::getNodeRenderCameraInheritance( shape.get() );
            const RenderViewportInterfacePtr & shape_viewport = Helper::getNodeRenderViewportInheritance( shape.get() );

            RenderContext shape_context;
            Helper::clearRenderContext( &shape_context );

            shape_context.camera = shape_camera.get();
            shape_context.viewport = shape_viewport.get();

            Polygon p1;
            shape->getScreenPolygon( &shape_context, contentResolution, nullptr, &p1 );

            bool intersect = Helper::intersects( p1, _polygon );

            return intersect;
        }
        //////////////////////////////////////////////////////////////////////////
        static pybind::object s_getMovieSlotsPosition( pybind::kernel_interface * _kernel, const ConstString & _groupName, const ConstString & _movieName )
        {
            ArrayString<128> buffer;
            buffer.append( "Movie" );
            buffer.append( _groupName );
            buffer.append( "_" );
            buffer.append( _movieName );

            ConstString resourceMovieName = Helper::stringizeStringSize( buffer.c_str(), buffer.size() );

            ResourceMoviePtr resourceMovie;

            if( RESOURCE_SERVICE()
                ->hasResourceT<ResourceMoviePtr>( _groupName, resourceMovieName, false, &resourceMovie ) == false )
            {
                LOGGER_ERROR( "not found resource movie '%s'"
                    , resourceMovieName.c_str()
                );

                return pybind::make_none_t( _kernel );
            }

            pybind::list py_list( _kernel );

            const VectorMovieLayers & layers = resourceMovie->getLayers();

            for( const MovieLayer & layer : layers )
            {
                if( layer.type != STRINGIZE_STRING_LOCAL( "MovieSlot" ) )
                {
                    continue;
                }

                py_list.append( pybind::make_tuple_t( _kernel, layer.name, layer.position ) );
            }

            return py_list;
        }
        //////////////////////////////////////////////////////////////////////////
        static PyObject * s_getMovieSlotPosition( pybind::kernel_interface * _kernel, const ConstString & _groupName, const ConstString & _movieName, const ConstString & _slotName )
        {
            ArrayString<128> buffer;
            buffer.append( "Movie" );
            buffer.append( _groupName );
            buffer.append( "_" );
            buffer.append( _movieName );

            ConstString resourceMovieName = Helper::stringizeStringSize( buffer.c_str(), buffer.size() );

            ResourceMoviePtr resourceMovie;

            if( RESOURCE_SERVICE()
                ->hasResourceT<ResourceMoviePtr>( _groupName, resourceMovieName, false, &resourceMovie ) == false )
            {
                LOGGER_ERROR( "not found resource movie '%s'"
                    , resourceMovieName.c_str()
                );

                return _kernel->ret_none();
            }

            const MovieLayer * layer;
            if( resourceMovie->hasMovieLayer( _slotName, &layer ) == false )
            {
                LOGGER_ERROR( "movie '%s' not found slot '%s'"
                    , resourceMovieName.c_str()
                    , _slotName.c_str()
                );

                return _kernel->ret_none();
            }

            PyObject * py_position = pybind::ptr( _kernel, layer->position );

            return py_position;
        }
        //////////////////////////////////////////////////////////////////////////
        static float s_getMovieDuration( const ConstString & _resourceName )
        {
            const ResourceMoviePtr & resourceMovie = RESOURCE_SERVICE()
                ->getResourceReference( ConstString::none(), _resourceName );

            MENGINE_ASSERTION_MEMORY_PANIC( resourceMovie, "invalid movie resource '%s'"
                , _resourceName.c_str()
            );

            if( resourceMovie->compile() == false )
            {
                LOGGER_ERROR( "resource '%s' type '%s' invalid compile"
                    , resourceMovie->getName().c_str()
                    , resourceMovie->getType().c_str()
                );

                return 0.f;
            }

            float duration = resourceMovie->getDuration();

            resourceMovie->release();

            return duration;
        }
        //////////////////////////////////////////////////////////////////////////
        static PyObject * s_getNullObjectsFromResourceMovie( pybind::kernel_interface * _kernel, ResourceMovie * _resource )
        {
            MENGINE_ASSERTION_MEMORY_PANIC( _resource );

            pybind::dict dictResult( _kernel );

            float frameDuration = _resource->getFrameDuration();

            _resource->foreachResourceMovie( [_kernel, &dictResult, frameDuration]( const MovieFramePackInterfacePtr & _framePack, const MovieLayer & _layer )
            {
                if( _layer.source != "MovieSlot" )
                {
                    return;
                }

                if( _framePack->hasLayer( _layer.index ) == false )
                {
                    return;
                }

                pybind::list py_list_frames( _kernel );

                const MovieLayerFrame & frames = _framePack->getLayer( _layer.index );

                for( uint32_t i = 0; i != frames.count; ++i )
                {
                    MovieFrameSource frame_source;
                    _framePack->getLayerFrame( _layer.index, i, &frame_source );

                    pybind::dict py_dict_frame( _kernel );

                    py_dict_frame["position"] = frame_source.position;

                    float frameTime = _layer.in + i * frameDuration;

                    py_dict_frame["time"] = frameTime;

                    py_list_frames.append( py_dict_frame );
                }

                dictResult[_layer.name] = py_list_frames;
            } );

            return dictResult.ret();
        }
        //////////////////////////////////////////////////////////////////////////
        static bool s_hasMovieElement2( const ResourceMoviePtr & _resource, const ConstString & _slotName, const ConstString & _typeName );
        //////////////////////////////////////////////////////////////////////////
        static bool s_hasMovieElement( const ConstString & _resourceName, const ConstString & _slotName, const ConstString & _typeName )
        {
            ResourceMoviePtr resource = RESOURCE_SERVICE()
                ->getResourceReference( ConstString::none(), _resourceName );

            MENGINE_ASSERTION_MEMORY_PANIC( resource );

            if( resource->compile() == false )
            {
                LOGGER_ERROR( "resource '%s' type '%s' invalid compile"
                    , resource->getName().c_str()
                    , resource->getType().c_str()
                );

                return false;
            }

            bool result = s_hasMovieElement2( resource, _slotName, _typeName );

            resource->release();

            return result;
        }
        //////////////////////////////////////////////////////////////////////////
        static bool s_hasMovieElement2( const ResourceMoviePtr & _resource, const ConstString & _slotName, const ConstString & _typeName )
        {
            const VectorMovieLayers & layers = _resource->getLayers();

            for( const MovieLayer & layer : layers )
            {
                if( layer.type == STRINGIZE_STRING_LOCAL( "Movie" ) )
                {
                    if( s_hasMovieElement( layer.source, _slotName, _typeName ) == true )
                    {
                        return true;
                    }
                }

                if( layer.type != _typeName )
                {
                    continue;
                }

                if( layer.name != _slotName )
                {
                    continue;
                }

                return true;
            }

            return false;
        }
        //////////////////////////////////////////////////////////////////////////
        static bool s_hasMovieSlot( const ConstString & _resourceName, const ConstString & _slotName )
        {
            bool result = s_hasMovieElement( _resourceName, _slotName, STRINGIZE_STRING_LOCAL( "MovieSlot" ) );

            return result;
        }
        //////////////////////////////////////////////////////////////////////////
        static bool s_hasMovieSubMovie( const ConstString & _resourceName, const ConstString & _subMovieName )
        {
            bool result = s_hasMovieElement( _resourceName, _subMovieName, STRINGIZE_STRING_LOCAL( "SubMovie" ) );

            return result;
        }
        //////////////////////////////////////////////////////////////////////////
        static bool s_hasMovieSocket( const ConstString & _resourceName, const ConstString & _socketName )
        {
            if( s_hasMovieElement( _resourceName, _socketName, STRINGIZE_STRING_LOCAL( "MovieSocketImage" ) ) == true )
            {
                return true;
            }

            if( s_hasMovieElement( _resourceName, _socketName, STRINGIZE_STRING_LOCAL( "MovieSocketShape" ) ) == true )
            {
                return true;
            }

            return false;
        }
        //////////////////////////////////////////////////////////////////////////
        static bool s_hasMovieEvent( const ConstString & _resourceName, const ConstString & _eventName )
        {
            bool result = s_hasMovieElement( _resourceName, _eventName, STRINGIZE_STRING_LOCAL( "MovieEvent" ) );

            return result;
        }
    }
    //////////////////////////////////////////////////////////////////////////
    MovieScriptEmbedding::MovieScriptEmbedding()
    {
    }
    //////////////////////////////////////////////////////////////////////////
    MovieScriptEmbedding::~MovieScriptEmbedding()
    {
    }
    //////////////////////////////////////////////////////////////////////////
    bool MovieScriptEmbedding::embedding( pybind::kernel_interface * _kernel )
    {
        pybind::interface_<Movie, pybind::bases<Node>>( _kernel, "Movie", false )
            .def( "setResourceMovie", &Movie::setResourceMovie )
            .def( "getResourceMovie", &Movie::getResourceMovie )
            .def( "hasMovieLayer", &Movie::hasMovieLayer )
            .def( "setEnableMovieLayer", &Movie::setEnableMovieLayer )
            .def( "setEnableMovieLayers", &Movie::setEnableMovieLayers )
            .def_static( "getWorldAnchorPoint", &Detail::Movie_getWorldAnchorPoint )
            .def_static_kernel( "getEnableMovieLayer", &Detail::Movie_getEnableMovieLayer )
            .def_static( "getMovieSlot", &Detail::Movie_getMovieSlot )
            .def_static( "hasMovieSlot", &Detail::Movie_hasMovieSlot )
            .def_static( "getMovieText", &Detail::Movie_getMovieText )
            .def_static( "hasMovieText", &Detail::Movie_hasMovieText )
            .def_static( "getMovieScissor", &Detail::Movie_getMovieScissor )
            .def_static( "hasMovieScissor", &Detail::Movie_hasMovieScissor )
            .def_static( "getSubMovie", &Detail::Movie_getSubMovie )
            .def_static( "hasSubMovie", &Detail::Movie_hasSubMovie )
            .def_static( "getSocket", &Detail::Movie_getSocket )
            .def_static( "hasSocket", &Detail::Movie_hasSocket )
            .def_static_args( "setMovieEvent", &Detail::Movie_setMovieEvent )
            .def_static( "removeMovieEvent", &Detail::Movie_removeMovieEvent )
            .def_static( "hasMovieEvent", &Detail::Movie_hasMovieEvent )
            .def_static_kernel( "getSockets", &Detail::Movie_getSockets )
            .def_static_kernel( "getSlots", &Detail::Movie_getSlots )
            .def_static_kernel( "getSubMovies", &Detail::Movie_getSubMovies )
            .def_static( "getMovieNode", &Detail::Movie_getMovieNode )
            .def_static( "hasMovieNode", &Detail::Movie_hasMovieNode )
            .def_static_kernel( "filterLayers", &Detail::Movie_filterLayers )
            .def_static( "getFrameDuration", &Detail::Movie_getFrameDuration )
            .def_static( "getDuration", &Detail::Movie_getDuration )
            .def_static( "getFrameCount", &Detail::Movie_getFrameCount )
            .def_static( "getSize", &Detail::Movie_getSize )
            .def_static( "getLayerPathLength", &Detail::Movie_getLayerPathLength )
            .def_static_kernel( "getLayerPath", &Detail::Movie_getLayerPath )
            .def_static_kernel( "getLayerPath2", &Detail::Movie_getLayerPath2 )
            .def_static_kernel( "getLayerPath3", &Detail::Movie_getLayerPath3 )
            .def_static_kernel( "getLayerPath4", &Detail::Movie_getLayerPath4 )
            .def_static( "getMovieSlotWorldPosition", &Detail::Movie_getMovieSlotWorldPosition )
            .def_static( "getMovieSlotOffsetPosition", &Detail::Movie_getMovieSlotOffsetPosition )
            .def_static( "attachMovieSlotNode", &Detail::Movie_attachMovieSlotNode )
            .def_static( "removeAllMovieSlotNode", &Detail::Movie_removeAllMovieSlotNode )
            .def_static_native_kernel( "setEventListener", &Detail::Movie_setEventListener )
            ;

        pybind::interface_<MovieSlot, pybind::bases<Node>>( _kernel, "MovieSlot", false )
            ;

        pybind::interface_<MovieSceneEffect, pybind::bases<Node>>( _kernel, "MovieSceneEffect", false )
            ;

        pybind::interface_<MovieInternalObject, pybind::bases<Node>>( _kernel, "MovieInternalObject", false )
            ;

        pybind::interface_<MovieMesh2D, pybind::bases<Node, Materialable>>( _kernel, "MovieMesh2D", false )
            .def( "setResourceImage", &MovieMesh2D::setResourceImage )
            .def( "getResourceImage", &MovieMesh2D::getResourceImage )
            ;

        pybind::interface_<MovieEvent, pybind::bases<Node>>( _kernel, "MovieEvent", false )
            ;

        pybind::interface_<ResourceMovie, pybind::bases<Resource>>( _kernel, "ResourceMovie", false )
            .def( "getSize", &ResourceMovie::getSize )
            .def( "getLoopSegment", &ResourceMovie::getLoopSegment )
            .def( "getFrameCount", &ResourceMovie::getFrameCount )
            .def( "getFrameDuration", &ResourceMovie::getFrameDuration )
            .def( "getDuration", &ResourceMovie::getDuration )
            .def( "getSocketResourceShape", &ResourceMovie::getSocketResourceShape )
            .def( "hasAnchorPoint", &ResourceMovie::hasAnchorPoint )
            .def( "getAnchorPoint", &ResourceMovie::getAnchorPoint )
            .def( "hasBoundBox", &ResourceMovie::hasBoundBox )
            .def( "getBoundBox", &ResourceMovie::getBoundBox )
            .def_static( "hasLayer", &Detail::ResourceMovie_hasLayer )
            .def_static( "hasLayerType", &Detail::ResourceMovie_hasLayerType )
            .def_static( "getLayerPosition", &Detail::ResourceMovie_getLayerPosition )
            .def_static( "getLayerIn", &Detail::ResourceMovie_getLayerIn )
            ;

        pybind::interface_<ResourceInternalObject, pybind::bases<Resource>>( _kernel, "ResourceInternalObject", false )
            ;

        pybind::def_function( _kernel, "intersectsMoviesHotspot", &Detail::s_intersectsMoviesHotspot );
        pybind::def_function( _kernel, "intersectMoviesHotspotVsPolygon", &Detail::s_intersectMoviesHotspotVsPolygon );

        pybind::def_function_kernel( _kernel, "getMovieSlotsPosition", &Detail::s_getMovieSlotsPosition );
        pybind::def_function_kernel( _kernel, "getMovieSlotPosition", &Detail::s_getMovieSlotPosition );

        pybind::def_function( _kernel, "getMovieDuration", &Detail::s_getMovieDuration );

        pybind::def_function_kernel( _kernel, "getNullObjectsFromResourceMovie", &Detail::s_getNullObjectsFromResourceMovie );

        pybind::def_function( _kernel, "hasMovieSlot", &Detail::s_hasMovieSlot );
        pybind::def_function( _kernel, "hasMovieSubMovie", &Detail::s_hasMovieSubMovie );
        pybind::def_function( _kernel, "hasMovieSocket", &Detail::s_hasMovieSocket );
        pybind::def_function( _kernel, "hasMovieEvent", &Detail::s_hasMovieEvent );

        pybind::def_function_kernel( _kernel, "getMovieSlotsPosition", &Detail::s_getMovieSlotsPosition );
        pybind::def_function_kernel( _kernel, "getMovieSlotPosition", &Detail::s_getMovieSlotPosition );

        Helper::registerScriptWrapping<Movie>( _kernel, STRINGIZE_STRING_LOCAL( "Movie" ), MENGINE_DOCUMENT_FACTORABLE );
        Helper::registerScriptWrapping<MovieSlot>( _kernel, STRINGIZE_STRING_LOCAL( "MovieSlot" ), MENGINE_DOCUMENT_FACTORABLE );
        Helper::registerScriptWrapping<MovieSceneEffect>( _kernel, STRINGIZE_STRING_LOCAL( "MovieSceneEffect" ), MENGINE_DOCUMENT_FACTORABLE );
        Helper::registerScriptWrapping<MovieInternalObject>( _kernel, STRINGIZE_STRING_LOCAL( "MovieInternalObject" ), MENGINE_DOCUMENT_FACTORABLE );
        Helper::registerScriptWrapping<MovieMesh2D>( _kernel, STRINGIZE_STRING_LOCAL( "MovieMesh2D" ), MENGINE_DOCUMENT_FACTORABLE );
        Helper::registerScriptWrapping<MovieEvent>( _kernel, STRINGIZE_STRING_LOCAL( "MovieEvent" ), MENGINE_DOCUMENT_FACTORABLE );
        Helper::registerScriptWrapping<ResourceMovie>( _kernel, STRINGIZE_STRING_LOCAL( "ResourceMovie" ), MENGINE_DOCUMENT_FACTORABLE );
        Helper::registerScriptWrapping<ResourceInternalObject>( _kernel, STRINGIZE_STRING_LOCAL( "ResourceInternalObject" ), MENGINE_DOCUMENT_FACTORABLE );

        return true;
    }
    //////////////////////////////////////////////////////////////////////////
    void MovieScriptEmbedding::ejecting( pybind::kernel_interface * _kernel )
    {
        _kernel->remove_scope<Movie>();
        _kernel->remove_scope<MovieSlot>();
        _kernel->remove_scope<MovieInternalObject>();
        _kernel->remove_scope<MovieMesh2D>();
        _kernel->remove_scope<ResourceMovie>();
        _kernel->remove_scope<ResourceInternalObject>();

        _kernel->remove_from_module( "intersectsMoviesHotspot", nullptr );
        _kernel->remove_from_module( "intersectMoviesHotspotVsPolygon", nullptr );
        _kernel->remove_from_module( "getMovieSlotsPosition", nullptr );
        _kernel->remove_from_module( "getMovieSlotPosition", nullptr );
        _kernel->remove_from_module( "getMovieDuration", nullptr );
        _kernel->remove_from_module( "getNullObjectsFromResourceMovie", nullptr );
        _kernel->remove_from_module( "hasMovieSlot", nullptr );
        _kernel->remove_from_module( "hasMovieSubMovie", nullptr );
        _kernel->remove_from_module( "hasMovieSocket", nullptr );
        _kernel->remove_from_module( "hasMovieEvent", nullptr );
        _kernel->remove_from_module( "getMovieSlotsPosition", nullptr );
        _kernel->remove_from_module( "getMovieSlotPosition", nullptr );

        Helper::unregisterScriptWrapping( STRINGIZE_STRING_LOCAL( "Movie" ) );
        Helper::unregisterScriptWrapping( STRINGIZE_STRING_LOCAL( "MovieSlot" ) );
        Helper::unregisterScriptWrapping( STRINGIZE_STRING_LOCAL( "MovieSceneEffect" ) );
        Helper::unregisterScriptWrapping( STRINGIZE_STRING_LOCAL( "MovieInternalObject" ) );
        Helper::unregisterScriptWrapping( STRINGIZE_STRING_LOCAL( "MovieMesh2D" ) );
        Helper::unregisterScriptWrapping( STRINGIZE_STRING_LOCAL( "MovieEvent" ) );
        Helper::unregisterScriptWrapping( STRINGIZE_STRING_LOCAL( "ResourceMovie" ) );
        Helper::unregisterScriptWrapping( STRINGIZE_STRING_LOCAL( "ResourceInternalObject" ) );
    }
    //////////////////////////////////////////////////////////////////////////
}

