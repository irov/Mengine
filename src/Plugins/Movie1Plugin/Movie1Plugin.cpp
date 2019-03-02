#include "Movie1Plugin.h"

#include "Interface/PrototypeServiceInterface.h"
#include "Interface/ApplicationInterface.h"
#include "Interface/StringizeServiceInterface.h"
#include "Interface/ScriptServiceInterface.h"
#include "Interface/ResourceServiceInterface.h"
#include "Interface/ConfigServiceInterface.h"
#include "Interface/DataServiceInterface.h"
#include "Interface/CodecServiceInterface.h"
#include "Interface/LoaderServiceInterface.h"
#include "Interface/VocabularyServiceInterface.h"

#include "Plugins/ResourcePrefetcherPlugin/ResourcePrefetcherServiceInterface.h"
#include "Plugins/ResourceValidatePlugin/ResourceValidateServiceInterface.h"

#include "Environment/Python/PythonScriptWrapper.h"
#include "Environment/Python/PythonAnimatableEventReceiver.h"

#include "Engine/HotSpot.h"
#include "Engine/HotSpotShape.h"
#include "Engine/ResourceShape.h"

#include "Kernel/RenderScissor.h"
#include "Kernel/DefaultPrototypeGenerator.h"
#include "Kernel/NodePrototypeGenerator.h"
#include "Kernel/ResourcePrototypeGenerator.h"
#include "Kernel/NodeRenderHelper.h"
#include "Kernel/PolygonHelper.h"

#include "Kernel/Logger.h"

#include "Movie.h"
#include "MovieSlot.h"
#include "MovieSceneEffect.h"
#include "MovieEvent.h"
#include "MovieInternalObject.h"
#include "MovieMesh2D.h"

#include "ResourceMovie.h"
#include "ResourceInternalObject.h"

#include "LoaderResourceMovie.h"

#include "DataflowAEK.h"

#include "ResourceMovieValidator.h"

#include "pybind/pybind.hpp"

//////////////////////////////////////////////////////////////////////////
PLUGIN_FACTORY( Movie1, Mengine::Movie1Plugin )
//////////////////////////////////////////////////////////////////////////
namespace Mengine
{
    //////////////////////////////////////////////////////////////////////////
    Movie1Plugin::Movie1Plugin()
    {
    }
    //////////////////////////////////////////////////////////////////////////
    Movie1Plugin::~Movie1Plugin()
    {
    }
    //////////////////////////////////////////////////////////////////////////
    bool Movie1Plugin::_availablePlugin() const
    {
        bool available = CONFIG_VALUE( "Engine", "Movie1PluginAvailable", true );

        return available;
    }
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

            if( resourceMovie == nullptr )
            {
                LOGGER_ERROR( "Movie::getWorldAnchorPoint %s invalid setup resource"
                    , _movie->getName().c_str()
                );

                return mt::vec3f( 0.f, 0.f, 0.f );
            }

            const mt::vec3f & ap = resourceMovie->getAnchorPoint();

            const mt::mat4f & wm = _movie->getWorldMatrix();

            mt::vec3f wap;
            mt::mul_v3_v3_m4( wap, ap, wm );

            return wap;
        }
        //////////////////////////////////////////////////////////////////////////
        static PyObject * Movie_getEnableMovieLayer( pybind::kernel_interface * _kernel, Movie * _movie, const ConstString & _name )
        {
            bool enable;
            if( _movie->getEnableMovieLayer( _name, enable ) == false )
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

            LOGGER_ERROR( "Movie::getSocket: movie %s resource %s not found %s"
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
            PythonVisitorMovieSlot( pybind::kernel_interface * _kernel, pybind::list & _list )
                : m_kernel( _kernel )
                , m_list( _list )
            {
            }

            ~PythonVisitorMovieSlot() override
            {
            }

        protected:
            void visitMovieNode( const MoviePtr & _movie, const NodePtr & _node ) override
            {
                MovieSlotPtr slot = stdex::intrusive_static_cast<MovieSlotPtr>(_node);

                if( slot == nullptr )
                {
                    return;
                }

                const ConstString & name = slot->getName();

                pybind::tuple py_value = pybind::make_tuple_t( m_kernel, _movie, name, slot );
                m_list.append( py_value );
            }

        protected:
            pybind::kernel_interface * m_kernel;
            pybind::list & m_list;

        private:
            PythonVisitorMovieSlot( const PythonVisitorMovieSlot & );
            void operator = ( const PythonVisitorMovieSlot & );
        };
        //////////////////////////////////////////////////////////////////////////
        static pybind::list Movie_getSlots( pybind::kernel_interface * _kernel, Movie * _movie )
        {
            pybind::list py_list( _kernel );

            VisitorMovieNodeInterfacePtr visitor = Helper::makeFactorableUnique<PythonVisitorMovieSlot>( _kernel, py_list );
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

        protected:
            void visitMovieNode( const MoviePtr & _movie, const NodePtr & _node ) override
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

            VisitorMovieNodeInterfacePtr visitor = Helper::makeFactorableUnique<PythonVisitorMovieSocket>( _kernel, py_list );
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

        protected:
            void visitMovieNode( const MoviePtr & _movie, const NodePtr & _node ) override
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

            VisitorMovieNodeInterfacePtr visitor = Helper::makeFactorableUnique<PythonVisitorMovieSubMovie>( _kernel, py_list );
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

            ~PyVisitorMovieLayer()
            {
            }

        protected:
            void visitMovieNode( const MoviePtr & _movie, const NodePtr & _node ) override
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

            VisitorMovieNodeInterfacePtr visitor = Helper::makeFactorableUnique<PyVisitorMovieLayer>( _kernel, py_list );
            _movie->visitMovieLayer( _type, visitor );

            return py_list;
        }
        //////////////////////////////////////////////////////////////////////////
        static float Movie_getFrameDuration( Movie * _movie )
        {
            const ResourceMoviePtr & resourceMovie = _movie->getResourceMovie();

            if( resourceMovie == nullptr )
            {
                LOGGER_ERROR( "Movie.getFrameDuration: '%s' not activate"
                    , _movie->getName().c_str()
                );

                return 0.f;
            }

            float frameDuration = resourceMovie->getFrameDuration();

            return frameDuration;
        }
        //////////////////////////////////////////////////////////////////////////
        static float Movie_getDuration( Movie * _movie )
        {
            const ResourceMoviePtr & resourceMovie = _movie->getResourceMovie();

            if( resourceMovie == nullptr )
            {
                LOGGER_ERROR( "Movie.getDuration: '%s' not activate"
                    , _movie->getName().c_str()
                );

                return 0.f;
            }

            float duration = resourceMovie->getDuration();

            return duration;
        }
        //////////////////////////////////////////////////////////////////////////
        static uint32_t Movie_getFrameCount( Movie * _movie )
        {
            const ResourceMoviePtr & resourceMovie = _movie->getResourceMovie();

            if( resourceMovie == nullptr )
            {
                LOGGER_ERROR( "Movie.getFrameCount: '%s' not activate"
                    , _movie->getName().c_str()
                );

                return 0;
            }

            uint32_t count = resourceMovie->getFrameCount();

            return count;
        }
        //////////////////////////////////////////////////////////////////////////
        static const mt::vec2f & Movie_getSize( Movie * _movie )
        {
            const ResourceMoviePtr & resourceMovie = _movie->getResourceMovie();

            if( resourceMovie == nullptr )
            {
                LOGGER_ERROR( "Movie.getSize: '%s' not activate"
                    , _movie->getName().c_str()
                );

                static mt::vec2f zero( 0.f, 0.f );

                return zero;
            }

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
                LOGGER_ERROR( "Movie::getLayerPathLength: '%s' not found layer '%s'"
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
            if( framePack->getLayerFrame( layer->index, 0, start_frame ) == false )
            {
                LOGGER_ERROR( "Movie::getLayerPathLength: '%s' invalid get layer '%s' frame %d"
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
                framePack->getLayerFrame( layer->index, i, frame );

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
                LOGGER_ERROR( "Movie::getLayerPathLength: '%s' not found layer '%s'"
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

            if( framePack == nullptr )
            {
                LOGGER_ERROR( "Movie::getLayerPathLength: sub_movie '%s' not found layer '%s' frame pack is null"
                    , sub_movie->getName().c_str()
                    , _name.c_str()
                );

                return pybind::make_invalid_list_t();
            }

            pybind::list py_path( _kernel, indexCount );

            for( uint32_t i = 0; i != indexCount; ++i )
            {
                MovieFrameSource frame;
                framePack->getLayerFrame( layer->index, i, frame );

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
                LOGGER_ERROR( "Movie::getLayerPathLength: '%s' not found layer '%s'"
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
                framePack->getLayerFrame( layer->index, i, frame );

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
                LOGGER_ERROR( "Movie::getLayerPathLength: '%s' not found layer '%s'"
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
                    LOGGER_ERROR( "Movie::getLayerPathLength: '%s' invalid compile"
                        , _movie->getName().c_str()
                    );

                    return pybind::make_invalid_list_t();
                }
            }

            const MovieFramePackInterfacePtr & framePack = resourceMovie->getFramePack();

            const mt::mat4f & wm = _movie->getWorldMatrix();

            pybind::list py_path( _kernel, indexCount );

            for( uint32_t i = 0; i != indexCount; ++i )
            {
                MovieFrameSource frame;
                framePack->getLayerFrame( layer->index, i, frame );

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
                LOGGER_ERROR( "Movie::getLayerPathLength: '%s' not found layer '%s'"
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
                    LOGGER_ERROR( "Movie::getLayerPathLength: '%s' invalid compile"
                        , _movie->getName().c_str()
                    );

                    return pybind::make_invalid_list_t();
                }
            }

            const MovieFramePackInterfacePtr & framePack = resourceMovie->getFramePack();

            const mt::mat4f & wm = _movie->getWorldMatrix();

            pybind::list py_path( _kernel, indexCount );

            for( uint32_t i = 0; i != indexCount; ++i )
            {
                MovieFrameSource frame;
                framePack->getLayerFrame( layer->index, i, frame );

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
                LOGGER_ERROR( "Movie::getMovieSlotWorldPosition %s not found slot '%s"
                    , _movie->getName().c_str()
                    , _slotName.c_str()
                );

                return mt::vec3f( 0.f, 0.f, 0.f );
            }

            const mt::vec3f & wp = node->getWorldPosition();

            return wp;
        }
        //////////////////////////////////////////////////////////////////////////
        static mt::vec3f Movie_getMovieSlotOffsetPosition( Movie * _movie, const ConstString & _slotName )
        {
            NodePtr node;
            MoviePtr submovie;
            if( _movie->getMovieNode( _slotName, STRINGIZE_STRING_LOCAL( "MovieSlot" ), &node, &submovie ) == false )
            {
                LOGGER_ERROR( "Movie::getMovieSlotOffsetPosition %s not found slot '%s"
                    , _movie->getName().c_str()
                    , _slotName.c_str()
                );

                return mt::vec3f( 0.f, 0.f, 0.f );
            }

            const ResourceMoviePtr & resourceMovie = _movie->getResourceMovie();

            if( resourceMovie == nullptr )
            {
                LOGGER_ERROR( "Movie::getMovieSlotOffsetPosition %s invalid setup resource"
                    , _movie->getName().c_str()
                );

                return mt::vec3f( 0.f, 0.f, 0.f );
            }

            const mt::vec3f & ap = resourceMovie->getAnchorPoint();

            const mt::mat4f & wm = _movie->getWorldMatrix();

            mt::vec3f wap;
            mt::mul_v3_v3_m4( wap, ap, wm );

            const mt::vec3f & wp = node->getWorldPosition();

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
                LOGGER_ERROR( "Movie::attachMovieSlotNode %s not found slot '%s"
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
                LOGGER_ERROR( "Movie::removeAllMovieSlotNode %s not found slot '%s"
                    , _movie->getName().c_str()
                    , _slotName.c_str()
                );

                return false;
            }

            node->removeChildren( []( const NodePtr & ) {} );

            return true;
        }
        //////////////////////////////////////////////////////////////////////////
        class PythonMovieEventReceiver
            : public PythonAnimatableEventReceiver<MovieEventReceiver>
        {
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
            (void)_args;

            if( _kwds == nullptr )
            {
                return _kernel->ret_none();
            }

            pybind::dict py_kwds( _kernel, _kwds );
            Helper::registerAnimatableEventReceiver<PythonMovieEventReceiver>( _kernel, py_kwds, _node );

            Helper::registerPythonEventReceiver<PythonMovieEventReceiver>( _kernel, py_kwds, _node, "onMovieGetInternal", EVENT_MOVIE_GET_INTERNAL );
            Helper::registerPythonEventReceiver<PythonMovieEventReceiver>( _kernel, py_kwds, _node, "onMovieActivateInternal", EVENT_MOVIE_ACTIVATE_INTERNAL );
            Helper::registerPythonEventReceiver<PythonMovieEventReceiver>( _kernel, py_kwds, _node, "onMovieDeactivateInternal", EVENT_MOVIE_DEACTIVATE_INTERNAL );

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
                LOGGER_ERROR( "ResourceMovie::getLayerPosition %s not found layer '%s'"
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
                LOGGER_ERROR( "ResourceMovie::getLayerIn %s not found layer '%s'"
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
            if( _movie1 == nullptr )
            {
                LOGGER_ERROR( "s_intersectsMoviesHotspot movie1 is NULL"
                );

                return false;
            }

            if( _movie2 == nullptr )
            {
                LOGGER_ERROR( "s_intersectsMoviesHotspot movie2 is NULL"
                );

                return false;
            }

            if( _movie1->compile() == false )
            {
                LOGGER_ERROR( "s_intersectsMoviesHotspot movie1 invalid compile"
                );

                return false;
            }

            if( _movie2->compile() == false )
            {
                LOGGER_ERROR( "s_intersectsMoviesHotspot movie2 invalid compile"
                );

                return false;
            }

            NodePtr node1;
            MoviePtr submovie1;

            if( _movie1->hasMovieNode( _socket1, STRINGIZE_STRING_LOCAL( "MovieSocketShape" ), &node1, &submovie1 ) == false )
            {
                LOGGER_ERROR( "s_intersectsMoviesHotspot movie1 %s not found socket shape %s"
                    , _movie1->getName().c_str()
                    , _socket1.c_str()
                );

                return false;
            }

            NodePtr node2;
            MoviePtr submovie2;

            if( _movie2->hasMovieNode( _socket2, STRINGIZE_STRING_LOCAL( "MovieSocketShape" ), &node2, &submovie2 ) == false )
            {
                LOGGER_ERROR( "s_intersectsMoviesHotspot movie2 %s not found socket shape %s"
                    , _movie2->getName().c_str()
                    , _socket2.c_str()
                );

                return false;
            }

            HotSpotShapePtr shape1 = stdex::intrusive_static_cast<HotSpotShapePtr>(node1);

            if( shape1->compile() == false )
            {
                LOGGER_ERROR( "s_intersectsMoviesHotspot movie1 %s socket shape %s invalid compile"
                    , _movie1->getName().c_str()
                    , _socket1.c_str()
                );

                return false;
            }

            HotSpotShapePtr shape2 = stdex::intrusive_static_cast<HotSpotShapePtr>(node2);

            if( shape2->compile() == false )
            {
                LOGGER_ERROR( "s_intersectsMoviesHotspot movie2 %s socket shape %s invalid compile"
                    , _movie2->getName().c_str()
                    , _socket2.c_str()
                );

                return false;
            }

            const Resolution & contentResolution = APPLICATION_SERVICE()
                ->getContentResolution();

            const RenderCameraInterfacePtr & shape1_camera = Helper::getNodeRenderCameraInheritance( shape1.get() );
            const RenderViewportInterfacePtr & shape1_viewport = Helper::getNodeRenderViewportInheritance( shape1.get() );

            mt::box2f b1;
            Polygon p1;
            shape1->getScreenPolygon( shape1_camera, shape1_viewport, contentResolution, &b1, &p1 );

            const RenderCameraInterfacePtr & shape2_camera = Helper::getNodeRenderCameraInheritance( shape2.get() );
            const RenderViewportInterfacePtr & shape2_viewport = Helper::getNodeRenderViewportInheritance( shape2.get() );

            mt::box2f b2;
            Polygon p2;
            shape2->getScreenPolygon( shape2_camera, shape2_viewport, contentResolution, &b2, &p2 );

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
            if( _movie == nullptr )
            {
                LOGGER_ERROR( "s_intersectMoviesHotspotVsPolygon movie is NULL"
                );

                return false;
            }

            if( _movie->compile() == false )
            {
                LOGGER_ERROR( "s_intersectMoviesHotspotVsPolygon movie invalid compile"
                );

                return false;
            }

            NodePtr node;
            MoviePtr submovie;

            if( _movie->hasMovieNode( _socket, STRINGIZE_STRING_LOCAL( "MovieSocketShape" ), &node, &submovie ) == false )
            {
                LOGGER_ERROR( "s_intersectMoviesHotspotVsPolygon movie %s not found socket shape %s"
                    , _movie->getName().c_str()
                    , _socket.c_str()
                );

                return false;
            }

            HotSpotShapePtr shape = stdex::intrusive_static_cast<HotSpotShapePtr>(node);

            if( shape->compile() == false )
            {
                LOGGER_ERROR( "s_intersectMoviesHotspotVsPolygon movie %s socket shape %s invalid compile"
                    , _movie->getName().c_str()
                    , _socket.c_str()
                );

                return false;
            }

            const Resolution & contentResolution = APPLICATION_SERVICE()
                ->getContentResolution();

            const RenderCameraInterfacePtr & shape_camera = Helper::getNodeRenderCameraInheritance( shape.get() );
            const RenderViewportInterfacePtr & shape_viewport = Helper::getNodeRenderViewportInheritance( shape.get() );

            Polygon p1;
            shape->getScreenPolygon( shape_camera, shape_viewport, contentResolution, nullptr, &p1 );

            bool intersect = Helper::intersects( p1, _polygon );

            return intersect;
        }
        //////////////////////////////////////////////////////////////////////////
        static pybind::object s_getMovieSlotsPosition( pybind::kernel_interface * _kernel, const ConstString & _groupName, const ConstString & _movieName )
        {
            stdex::array_string<128> buffer;
            buffer.append( "Movie" );
            buffer.append( _groupName );
            buffer.append( "_" );
            buffer.append( _movieName );

            ConstString resourceMovieName = Helper::stringizeStringSize( buffer.c_str(), buffer.size() );

            ResourceMoviePtr resourceMovie;

            if( RESOURCE_SERVICE()
                ->hasResourceT<ResourceMoviePtr>( resourceMovieName, &resourceMovie ) == false )
            {
                LOGGER_ERROR( "s_getMovieSlotsPosition: not found resource movie %s"
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
            stdex::array_string<128> buffer;
            buffer.append( "Movie" );
            buffer.append( _groupName );
            buffer.append( "_" );
            buffer.append( _movieName );

            ConstString resourceMovieName = Helper::stringizeStringSize( buffer.c_str(), buffer.size() );

            ResourceMoviePtr resourceMovie;

            if( RESOURCE_SERVICE()
                ->hasResourceT<ResourceMoviePtr>( resourceMovieName, &resourceMovie ) == false )
            {
                LOGGER_ERROR( "getMovieSlotPosition: not found resource movie %s"
                    , resourceMovieName.c_str()
                );

                return _kernel->ret_none();
            }

            const MovieLayer * layer;
            if( resourceMovie->hasMovieLayer( _slotName, &layer ) == false )
            {
                LOGGER_ERROR( "getMovieSlotPosition: movie %s not found slot %s"
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
                ->getResource( _resourceName );

            if( resourceMovie == nullptr )
            {
                LOGGER_ERROR( "getMovieDuration invalid movie resource '%s'"
                    , _resourceName.c_str()
                );

                return 0.f;
            }

            float duration = resourceMovie->getDuration();

            resourceMovie->decrementReference();

            return duration;
        }
        //////////////////////////////////////////////////////////////////////////
        class ResourceMovieVisitorNullLayers
            : public VisitorResourceMovie
        {
        public:
            ResourceMovieVisitorNullLayers( pybind::kernel_interface * _kernel, const pybind::dict & _dictResult, float _frameDuration )
                : m_kernel( _kernel )
                , m_dictResult( _dictResult )
                , m_frameDuration( _frameDuration )
            {
            }

        protected:
            void visitLayer( const MovieFramePackInterfacePtr & _framePack, const MovieLayer & _layer ) override
            {
                if( _layer.source != "MovieSlot" )
                {
                    return;
                }

                if( _framePack->hasLayer( _layer.index ) == false )
                {
                    return;
                }

                pybind::list py_list_frames( m_kernel );

                const MovieLayerFrame & frames = _framePack->getLayer( _layer.index );

                for( uint32_t i = 0; i != frames.count; ++i )
                {
                    MovieFrameSource frame_source;
                    _framePack->getLayerFrame( _layer.index, i, frame_source );

                    pybind::dict py_dict_frame( m_kernel );

                    py_dict_frame["position"] = frame_source.position;

                    float frameTime = _layer.in + i * m_frameDuration;

                    py_dict_frame["time"] = frameTime;

                    py_list_frames.append( py_dict_frame );
                }

                m_dictResult[_layer.name] = py_list_frames;
            }

        protected:
            pybind::kernel_interface * m_kernel;
            pybind::dict m_dictResult;
            float m_frameDuration;
        };
        //////////////////////////////////////////////////////////////////////////
        static PyObject * s_getNullObjectsFromResourceMovie( pybind::kernel_interface * _kernel, ResourceMovie * _resource )
        {
            if( _resource == nullptr )
            {
                return _kernel->ret_none();
            }

            pybind::dict py_dict_result( _kernel );

            float frameTime = _resource->getFrameDuration();
            ResourceMovieVisitorNullLayers visitor( _kernel, py_dict_result, frameTime );

            _resource->visitResourceMovie( &visitor );

            return py_dict_result.ret();
        }
        //////////////////////////////////////////////////////////////////////////
        static bool s_hasMovieElement2( const ResourceMoviePtr & _resource, const ConstString & _slotName, const ConstString & _typeName );
        //////////////////////////////////////////////////////////////////////////
        static bool s_hasMovieElement( const ConstString & _resourceName, const ConstString & _slotName, const ConstString & _typeName )
        {
            const ResourceMoviePtr & resource = RESOURCE_SERVICE()
                ->getResource( _resourceName );

            if( resource == nullptr )
            {
                return false;
            }

            bool result = s_hasMovieElement2( resource, _slotName, _typeName );

            resource->decrementReference();

            return result;
        }
        //////////////////////////////////////////////////////////////////////////
        static bool s_hasMovieElement2( const ResourceMoviePtr & _resource, const ConstString & _slotName, const ConstString & _typeName )
        {
            const VectorMovieLayers & layers = _resource->getLayers();

            for( const MovieLayer & layer : layers )
            {
                if( layer.type == "Movie" )
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
    bool Movie1Plugin::_initializePlugin()
    {
        this->addDependencyService( "PrefetcherService" );

        if( SERVICE_EXIST( ScriptServiceInterface ) == true )
        {
            pybind::kernel_interface * kernel = SCRIPT_SERVICE()
                ->getKernel();

            pybind::set_kernel( kernel );

            pybind::interface_<Movie, pybind::bases<Node, Eventable, Animatable> >( kernel, "Movie", false )
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

            pybind::interface_<MovieSlot, pybind::bases<Node> >( kernel, "MovieSlot", false )
                ;

            pybind::interface_<MovieInternalObject, pybind::bases<Node> >( kernel, "MovieInternalObject", false )
                ;

            pybind::interface_<MovieMesh2D, pybind::bases<Node, Materialable> >( kernel, "MovieMesh2D", false )
                .def( "setResourceImage", &MovieMesh2D::setResourceImage )
                .def( "getResourceImage", &MovieMesh2D::getResourceImage )
                ;

            pybind::interface_<ResourceMovie, pybind::bases<Resource, Content> >( kernel, "ResourceMovie", false )
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

            pybind::interface_<ResourceInternalObject, pybind::bases<Resource> >( kernel, "ResourceInternalObject", false )
                ;

            pybind::def_function( kernel, "intersectsMoviesHotspot", &Detail::s_intersectsMoviesHotspot );
            pybind::def_function( kernel, "intersectMoviesHotspotVsPolygon", &Detail::s_intersectMoviesHotspotVsPolygon );

            pybind::def_function_kernel( kernel, "getMovieSlotsPosition", &Detail::s_getMovieSlotsPosition );
            pybind::def_function_kernel( kernel, "getMovieSlotPosition", &Detail::s_getMovieSlotPosition );

            pybind::def_function( kernel, "getMovieDuration", &Detail::s_getMovieDuration );

            pybind::def_function_kernel( kernel, "getNullObjectsFromResourceMovie", &Detail::s_getNullObjectsFromResourceMovie );

            pybind::def_function( kernel, "hasMovieSlot", &Detail::s_hasMovieSlot );
            pybind::def_function( kernel, "hasMovieSubMovie", &Detail::s_hasMovieSubMovie );
            pybind::def_function( kernel, "hasMovieSocket", &Detail::s_hasMovieSocket );
            pybind::def_function( kernel, "hasMovieEvent", &Detail::s_hasMovieEvent );

            pybind::def_function_kernel( kernel, "getMovieSlotsPosition", &Detail::s_getMovieSlotsPosition );
            pybind::def_function_kernel( kernel, "getMovieSlotPosition", &Detail::s_getMovieSlotPosition );

            VOCALUBARY_SET( ScriptWrapperInterface, STRINGIZE_STRING_LOCAL( "ClassWrapping" ), STRINGIZE_STRING_LOCAL( "Movie" ), Helper::makeFactorableUnique<PythonScriptWrapper<Movie> >( kernel ) );
            VOCALUBARY_SET( ScriptWrapperInterface, STRINGIZE_STRING_LOCAL( "ClassWrapping" ), STRINGIZE_STRING_LOCAL( "MovieSlot" ), Helper::makeFactorableUnique<PythonScriptWrapper<MovieSlot> >( kernel ) );
            VOCALUBARY_SET( ScriptWrapperInterface, STRINGIZE_STRING_LOCAL( "ClassWrapping" ), STRINGIZE_STRING_LOCAL( "MovieSceneEffect" ), Helper::makeFactorableUnique<PythonScriptWrapper<MovieSceneEffect> >( kernel ) );
            VOCALUBARY_SET( ScriptWrapperInterface, STRINGIZE_STRING_LOCAL( "ClassWrapping" ), STRINGIZE_STRING_LOCAL( "MovieInternalObject" ), Helper::makeFactorableUnique<PythonScriptWrapper<MovieInternalObject> >( kernel ) );
            VOCALUBARY_SET( ScriptWrapperInterface, STRINGIZE_STRING_LOCAL( "ClassWrapping" ), STRINGIZE_STRING_LOCAL( "MovieMesh2D" ), Helper::makeFactorableUnique<PythonScriptWrapper<MovieMesh2D> >( kernel ) );
            VOCALUBARY_SET( ScriptWrapperInterface, STRINGIZE_STRING_LOCAL( "ClassWrapping" ), STRINGIZE_STRING_LOCAL( "MovieEvent" ), Helper::makeFactorableUnique<PythonScriptWrapper<MovieEvent> >( kernel ) );
            VOCALUBARY_SET( ScriptWrapperInterface, STRINGIZE_STRING_LOCAL( "ClassWrapping" ), STRINGIZE_STRING_LOCAL( "ResourceMovie" ), Helper::makeFactorableUnique<PythonScriptWrapper<ResourceMovie> >( kernel ) );
            VOCALUBARY_SET( ScriptWrapperInterface, STRINGIZE_STRING_LOCAL( "ClassWrapping" ), STRINGIZE_STRING_LOCAL( "ResourceInternalObject" ), Helper::makeFactorableUnique<PythonScriptWrapper<ResourceInternalObject> >( kernel ) );
        }

        if( PROTOTYPE_SERVICE()
            ->addPrototype( STRINGIZE_STRING_LOCAL( "Node" ), STRINGIZE_STRING_LOCAL( "Movie" ), Helper::makeFactorableUnique<NodePrototypeGenerator<Movie, 128>>() ) == false )
        {
            return false;
        }

        if( PROTOTYPE_SERVICE()
            ->addPrototype( STRINGIZE_STRING_LOCAL( "Node" ), STRINGIZE_STRING_LOCAL( "MovieSlot" ), Helper::makeFactorableUnique<NodePrototypeGenerator<MovieSlot, 128>>() ) == false )
        {
            return false;
        }

        if( PROTOTYPE_SERVICE()
            ->addPrototype( STRINGIZE_STRING_LOCAL( "Node" ), STRINGIZE_STRING_LOCAL( "MovieSceneEffect" ), Helper::makeFactorableUnique<NodePrototypeGenerator<MovieSceneEffect, 128>>() ) == false )
        {
            return false;
        }

        if( PROTOTYPE_SERVICE()
            ->addPrototype( STRINGIZE_STRING_LOCAL( "Node" ), STRINGIZE_STRING_LOCAL( "MovieInternalObject" ), Helper::makeFactorableUnique<NodePrototypeGenerator<MovieInternalObject, 128>>() ) == false )
        {
            return false;
        }

        if( PROTOTYPE_SERVICE()
            ->addPrototype( STRINGIZE_STRING_LOCAL( "Node" ), STRINGIZE_STRING_LOCAL( "MovieEvent" ), Helper::makeFactorableUnique<NodePrototypeGenerator<MovieEvent, 128>>() ) == false )
        {
            return false;
        }

        if( PROTOTYPE_SERVICE()
            ->addPrototype( STRINGIZE_STRING_LOCAL( "Node" ), STRINGIZE_STRING_LOCAL( "MovieMesh2D" ), Helper::makeFactorableUnique<NodePrototypeGenerator<MovieMesh2D, 128>>() ) == false )
        {
            return false;
        }

        if( PROTOTYPE_SERVICE()
            ->addPrototype( STRINGIZE_STRING_LOCAL( "Resource" ), STRINGIZE_STRING_LOCAL( "ResourceMovie" ), Helper::makeFactorableUnique<ResourcePrototypeGenerator<ResourceMovie, 64>>() ) == false )
        {
            return false;
        }

        if( PROTOTYPE_SERVICE()
            ->addPrototype( STRINGIZE_STRING_LOCAL( "Resource" ), STRINGIZE_STRING_LOCAL( "ResourceInternalObject" ), Helper::makeFactorableUnique<ResourcePrototypeGenerator<ResourceInternalObject, 64> >() ) == false )
        {
            return false;
        }

        DataflowInterfacePtr dataflowAEK = Helper::makeFactorableUnique<DataflowAEK>();

        if( dataflowAEK->initialize() == false )
        {
            return false;
        }

        VOCALUBARY_SET( DataflowInterface, STRINGIZE_STRING_LOCAL( "Dataflow" ), STRINGIZE_STRING_LOCAL( "aekMovie" ), dataflowAEK );

        CODEC_SERVICE()
            ->registerCodecExt( "aek", STRINGIZE_STRING_LOCAL( "aekMovie" ) );

        if( SERVICE_EXIST( ResourcePrefetcherServiceInterface ) == true )
        {
            ResourcePrefetcherInterfacePtr resourcePrefetcher = VOCALUBARY_GET( STRINGIZE_STRING_LOCAL( "ResourcePrefetcher" ), STRINGIZE_STRING_LOCAL( "Dataflow" ) );

            RESOURCEPREFETCHER_SERVICE()
                ->addResourcePrefetcher( STRINGIZE_STRING_LOCAL( "ResourceMovie" ), resourcePrefetcher );
        }

        if( SERVICE_EXIST( ResourceValidateServiceInterface ) == true )
        {
            VOCALUBARY_SET( ResourceValidatorInterface, STRINGIZE_STRING_LOCAL( "Validator" ), STRINGIZE_STRING_LOCAL( "ResourceMovie" ), Helper::makeFactorableUnique<ResourceMovieValidator>() );
        }

        if( SERVICE_EXIST( LoaderServiceInterface ) == true )
        {
            VOCALUBARY_SET( LoaderInterface, STRINGIZE_STRING_LOCAL( "Loader" ), STRINGIZE_STRING_LOCAL( "ResourceMovie" ), Helper::makeFactorableUnique<LoaderResourceMovie>() );
        }

        return true;
    }
    //////////////////////////////////////////////////////////////////////////
    void Movie1Plugin::_finalizePlugin()
    {
        if( SERVICE_EXIST( ScriptServiceInterface ) == true )
        {
            VOCALUBARY_REMOVE( STRINGIZE_STRING_LOCAL( "ClassWrapping" ), STRINGIZE_STRING_LOCAL( "Movie" ) );
            VOCALUBARY_REMOVE( STRINGIZE_STRING_LOCAL( "ClassWrapping" ), STRINGIZE_STRING_LOCAL( "MovieSlot" ) );
            VOCALUBARY_REMOVE( STRINGIZE_STRING_LOCAL( "ClassWrapping" ), STRINGIZE_STRING_LOCAL( "MovieSceneEffect" ) );
            VOCALUBARY_REMOVE( STRINGIZE_STRING_LOCAL( "ClassWrapping" ), STRINGIZE_STRING_LOCAL( "MovieInternalObject" ) );
            VOCALUBARY_REMOVE( STRINGIZE_STRING_LOCAL( "ClassWrapping" ), STRINGIZE_STRING_LOCAL( "MovieMesh2D" ) );
            VOCALUBARY_REMOVE( STRINGIZE_STRING_LOCAL( "ClassWrapping" ), STRINGIZE_STRING_LOCAL( "MovieEvent" ) );
            VOCALUBARY_REMOVE( STRINGIZE_STRING_LOCAL( "ClassWrapping" ), STRINGIZE_STRING_LOCAL( "ResourceMovie" ) );
            VOCALUBARY_REMOVE( STRINGIZE_STRING_LOCAL( "ClassWrapping" ), STRINGIZE_STRING_LOCAL( "ResourceInternalObject" ) );
        }

        PROTOTYPE_SERVICE()
            ->removePrototype( STRINGIZE_STRING_LOCAL( "Node" ), STRINGIZE_STRING_LOCAL( "Movie" ) );

        PROTOTYPE_SERVICE()
            ->removePrototype( STRINGIZE_STRING_LOCAL( "Node" ), STRINGIZE_STRING_LOCAL( "MovieSlot" ) );

        PROTOTYPE_SERVICE()
            ->removePrototype( STRINGIZE_STRING_LOCAL( "Node" ), STRINGIZE_STRING_LOCAL( "MovieSceneEffect" ) );

        PROTOTYPE_SERVICE()
            ->removePrototype( STRINGIZE_STRING_LOCAL( "Node" ), STRINGIZE_STRING_LOCAL( "MovieInternalObject" ) );

        PROTOTYPE_SERVICE()
            ->removePrototype( STRINGIZE_STRING_LOCAL( "Node" ), STRINGIZE_STRING_LOCAL( "MovieEvent" ) );

        PROTOTYPE_SERVICE()
            ->removePrototype( STRINGIZE_STRING_LOCAL( "Node" ), STRINGIZE_STRING_LOCAL( "MovieMesh2D" ) );

        PROTOTYPE_SERVICE()
            ->removePrototype( STRINGIZE_STRING_LOCAL( "Resource" ), STRINGIZE_STRING_LOCAL( "ResourceMovie" ) );

        PROTOTYPE_SERVICE()
            ->removePrototype( STRINGIZE_STRING_LOCAL( "Resource" ), STRINGIZE_STRING_LOCAL( "ResourceInternalObject" ) );

        if( SERVICE_EXIST( ResourcePrefetcherServiceInterface ) == true )
        {
            RESOURCEPREFETCHER_SERVICE()
                ->removeResourcePrefetcher( STRINGIZE_STRING_LOCAL( "ResourceMovie" ) );
        }

        if( SERVICE_EXIST( ResourceValidateServiceInterface ) == true )
        {
            VOCALUBARY_REMOVE( STRINGIZE_STRING_LOCAL( "Validator" ), STRINGIZE_STRING_LOCAL( "ResourceMovie" ) );
        }

        DataflowInterfacePtr dataflow = VOCALUBARY_REMOVE( STRINGIZE_STRING_LOCAL( "Dataflow" ), STRINGIZE_STRING_LOCAL( "aekMovie" ) );
        dataflow->finalize();

        if( SERVICE_EXIST( LoaderServiceInterface ) == true )
        {
            VOCALUBARY_REMOVE( STRINGIZE_STRING_LOCAL( "Loader" ), STRINGIZE_STRING_LOCAL( "ResourceMovie" ) );
        }
    }
}
