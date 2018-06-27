#include "PythonScriptWrapper.h"

#include "Config/Blobject.h"

#include "Interface/ApplicationInterface.h"
#include "Interface/TimelineInterface.h"
#include "Interface/StringizeInterface.h"
#include "Interface/RenderSystemInterface.h"

#include "Interface/InputSystemInterface.h"
#include "Interface/NodeInterface.h"
#include "Interface/MemoryInterface.h"
#include "Interface/HttpSystemInterface.h"
#include "Interface/PrefetcherInterface.h"
#include "Interface/PlatformInterface.h"
#include "Interface/PackageInterface.h"

#include "Kernel/ThreadTask.h"
#include "Kernel/Scene.h"
#include "Kernel/Arrow.h"

#include "Interface/ScriptSystemInterface.h"
#include "Interface/ScheduleManagerInterface.h"

#include "Interface/TextInterface.h"
#include "Interface/AccountInterface.h"

#include "Interface/UnicodeInterface.h"

#include "Interface/ResourceInterface.h"

#include "Kernel/ResourceImageData.h"

#include "Engine/ResourceFile.h"
#include "Engine/ResourceMovie.h"
#include "Engine/ResourceAnimation.h"
#include "Engine/ResourceModel3D.h"
#include "Engine/ResourceVideo.h"
#include "Engine/ResourceSound.h"
#include "Engine/ResourceImageSolid.h"

#include "Engine/ResourceImageDefault.h"
#include "Engine/ResourceTestPick.h"
#include "Engine/ResourceHIT.h"
#include "Engine/ResourceShape.h"
#include "Engine/ResourceInternalObject.h"

#include "Engine/ResourceImageSubstractRGBAndAlpha.h"
#include "Engine/ResourceImageSubstract.h"

#include "Plugin/MoviePlugin/ResourceMovie2.h"

#include "Interface/ApplicationInterface.h"
#include "Interface/MousePickerSystemInterface.h"

#include "Engine/MovieSlot.h"
#include "Engine/MovieInternalObject.h"
#include "Engine/MovieEvent.h"
#include "Engine/ParticleEmitter2.h"
#include "Engine/Model3D.h"
#include "Engine/HotSpot.h"
#include "Engine/HotSpotPolygon.h"
#include "Engine/HotSpotCircle.h"
#include "Engine/HotSpotBubbles.h"
#include "Engine/HotSpotImage.h"
#include "Engine/HotSpotShape.h"
#include "Engine/Landscape2D.h"
#include "Engine/Grid2D.h"

#include "Engine/ShapeQuadFixed.h"
#include "Engine/ShapeQuadFlex.h"

//#include "Light2D.h"
#include "Engine/Gyroscope.h"
#include "Engine/TextField.h"
#include "Engine/SoundEmitter.h"
#include "Engine/Point.h"
#include "Engine/Line.h"
#include "Kernel/RenderCameraProjection.h"
//#include "RigidBody3D.h"
//#include "CapsuleController.h"
//#include "Skeleton.h"

#include "ScriptHolder.h"

#include "Engine/SurfaceVideo.h"
#include "Engine/SurfaceSound.h"
#include "Engine/SurfaceImage.h"
#include "Engine/SurfaceImageSequence.h"
#include "Engine/SurfaceSolidColor.h"


#include "Kernel/Isometric.h"
#include "Kernel/Parallax.h"
#include "Kernel/RenderViewport.h"
#include "Kernel/RenderScissor.h"
#include "Kernel/RenderCameraOrthogonal.h"
#include "Kernel/RenderCameraProjection.h"
#include "Kernel/RenderCameraOrthogonalTarget.h"

#include "Kernel/ResourceImage.h"

#include "Engine/Layer2D.h"


#include "Engine/Movie.h"
#include "Engine/Meshget.h"

#include "Engine/Window.h"


#include "Kernel/Shape.h"
#include "Kernel/Entity.h"

//#include "DiscreteEntity.h"

//#include "SoundEngine.h"
#include "Logger/Logger.h"

#include "Interface/RenderSystemInterface.h"
#include "Interface/PhysicSystemInterface.h"
#include "Interface/ThreadSystemInterface.h"

#include "Kernel/Identity.h"
#include "Kernel/Affector.h"
#include "Kernel/AffectorHelper.h"
#include "Kernel/ThreadTask.h"
#include "Kernel/ScriptEventReceiver.h"
#include "Kernel/ScriptWrapper.h"

#include "PythonAnimatableEventReceiver.h"
#include "ScriptableAffectorCallback.h"

#include "PythonScheduleTimer.h"
#include "PythonSchedulePipe.h"
#include "PythonScheduleEvent.h"
#include "DelaySchedulePipe.h"

#include "Core/Polygon.h"
#include "Core/MemoryHelper.h"
#include "Core/ValueFollower.h"

#include "math/angle.h"
#include "math/vec4.h"
#include "math/mat3.h"
#include "math/mat4.h"
#include "math/quat.h"
#include "math/utils.h"

#include "Core/Rect.h"
#include "Core/String.h"
#include "Core/Polygon.h"
#include "Core/ValueFollower.h"

#include "Factory/FactoryPool.h"

#include "stdex/xml_sax_parser.h"
#include "utf8.h"

#include <sstream>

#include <math.h>

namespace Mengine
{
    //////////////////////////////////////////////////////////////////////////
    class NodeScriptMethod
    {
    public:
        NodeScriptMethod()
        {
            m_factoryPythonScheduleEvent = new FactoryPool<PythonScheduleEvent, 16>();
            m_factoryDelaySchedulePipe = new FactoryPool<DelaySchedulePipe, 16>();
            m_factoryPythonScheduleTimer = new FactoryPool<PythonScheduleTimer, 16>();
            m_factoryPythonSchedulePipe = new FactoryPool<PythonSchedulePipe, 16>();
            m_factoryNodeAffectorCallback = new FactoryPool<ScriptableAffectorCallback, 4>();
        }

        ~NodeScriptMethod()
        {
            MENGINE_ASSERTION_FACTORY_EMPTY( m_factoryPythonScheduleEvent );
            MENGINE_ASSERTION_FACTORY_EMPTY( m_factoryDelaySchedulePipe );
            MENGINE_ASSERTION_FACTORY_EMPTY( m_factoryPythonScheduleTimer );
            MENGINE_ASSERTION_FACTORY_EMPTY( m_factoryPythonSchedulePipe );
            MENGINE_ASSERTION_FACTORY_EMPTY( m_factoryNodeAffectorCallback );

            m_factoryPythonScheduleEvent = nullptr;
            m_factoryDelaySchedulePipe = nullptr;
            m_factoryPythonScheduleTimer = nullptr;
            m_factoryPythonSchedulePipe = nullptr;
            m_factoryNodeAffectorCallback = nullptr;
        }

    public:
        //////////////////////////////////////////////////////////////////////////
        PyObject * s_TextField_setTextFormatArgs( pybind::kernel_interface * _kernel, TextField * _textField, PyObject * _args, PyObject * _kwds )
        {
            (void)_kwds;

            size_t args_count = pybind::tuple_size( _args );

            TVectorString cs_args;
            cs_args.reserve( args_count );

            for( uint32_t it = 0; it != args_count; ++it )
            {
                PyObject * py_string = pybind::tuple_getitem( _args, it );

                if( pybind::string_check( py_string ) == true )
                {
                    String key;
                    if( pybind::extract_value( _kernel, py_string, key, false ) == false )
                    {
                        LOGGER_ERROR( "textfield_setTextFormatArgs %s invalid get str %s"
                            , pybind::object_repr( py_string )
                        );

                        return pybind::ret_false();
                    }

                    cs_args.emplace_back( key );
                }
                else if( pybind::unicode_check( py_string ) == true )
                {
                    WString key;
                    if( pybind::extract_value( _kernel, py_string, key, false ) == false )
                    {
                        LOGGER_ERROR( "textfield_setTextFormatArgs %s invalid get unicode %s"
                            , pybind::object_repr( py_string )
                        );

                        return pybind::ret_false();
                    }

                    String utf8_arg;
                    Helper::unicodeToUtf8( key, utf8_arg );

                    cs_args.emplace_back( utf8_arg );
                }
                else
                {
                    const char * value = pybind::object_str( py_string );

                    if( value == nullptr )
                    {
                        LOGGER_ERROR( "textfield_setTextFormatArgs %s not suport arg %s"
                            , pybind::object_repr( py_string )
                        );

                        return pybind::ret_false();
                    }

                    cs_args.emplace_back( String( value ) );
                }
            }

            _textField->setTextFormatArgs( cs_args );

            return pybind::ret_true();
        }
        //////////////////////////////////////////////////////////////////////////
        TVectorWString s_TextField_getTextFormatArgs( TextField * _textField )
        {
            TVectorWString ws_args;

            const TVectorString & str_args = _textField->getTextFormatArgs();

            size_t args_count = str_args.size();
            ws_args.reserve( args_count );

            for( const String & str_arg : str_args )
            {
                WString unicode;
                Helper::utf8ToUnicode( str_arg, unicode );

                ws_args.emplace_back( unicode );
            }

            return ws_args;
        }
        //////////////////////////////////////////////////////////////////////////
        //WString textfield_getText( TextField * _textField )
        //{
        //	const U32String & utf8 = _textField->getText();

        //	WString unicode;
        //	Helper::utf8ToUnicode( utf8, unicode );

        //	return unicode;
        //}
        //////////////////////////////////////////////////////////////////////////
        NodePtr movie_getMovieNode( Movie * _movie, const ConstString & _name )
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
        bool movie_hasMovieNode( Movie * _movie, const ConstString & _name )
        {
            NodePtr node;
            MoviePtr submovie;
            bool successful = _movie->hasMovieNode( _name, ConstString::none(), &node, &submovie );

            return successful;
        }
        //////////////////////////////////////////////////////////////////////////
        mt::vec3f movie_getWorldAnchorPoint( Movie * _movie )
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
        PyObject * movie_getEnableMovieLayer( pybind::kernel_interface * _kernel, Movie * _movie, const ConstString & _name )
        {
            bool enable;
            if( _movie->getEnableMovieLayer( _name, enable ) == false )
            {
                return pybind::ret_none();
            }

            return pybind::ptr( _kernel, enable );
        }
        //////////////////////////////////////////////////////////////////////////
        NodePtr movie_getMovieSlot( Movie * _movie, const ConstString & _name )
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
        RenderScissorPtr movie_getMovieScissor( Movie * _movie, const ConstString & _name )
        {
            RenderScissorPtr scissor;
            if( _movie->getMovieScissor( _name, &scissor ) == false )
            {
                return nullptr;
            }

            return scissor;
        }
        //////////////////////////////////////////////////////////////////////////
        bool movie_hasMovieScissor( Movie * _movie, const ConstString & _name )
        {
            bool result = _movie->hasMovieScissor( _name );

            return result;
        }
        //////////////////////////////////////////////////////////////////////////
        bool movie_hasMovieSlot( Movie * _movie, const ConstString & _name )
        {
            NodePtr node;
            MoviePtr submovie;
            bool successful = _movie->hasMovieNode( _name, STRINGIZE_STRING_LOCAL( "MovieSlot" ), &node, &submovie );

            return successful;
        }
        //////////////////////////////////////////////////////////////////////////
        NodePtr movie_getMovieText( Movie * _movie, const ConstString & _name )
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
        bool movie_hasMovieText( Movie * _movie, const ConstString & _name )
        {
            NodePtr node;
            MoviePtr submovie;

            bool successful = _movie->hasMovieNode( _name, STRINGIZE_STRING_LOCAL( "MovieText" ), &node, &submovie );

            return successful;
        }
        //////////////////////////////////////////////////////////////////////////
        NodePtr movie_getSubMovie( Movie * _movie, const ConstString & _name )
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
        bool movie_hasSubMovie( Movie * _movie, const ConstString & _name )
        {
            NodePtr node;
            MoviePtr submovie;
            bool successful = _movie->hasMovieNode( _name, STRINGIZE_STRING_LOCAL( "SubMovie" ), &node, &submovie );

            return successful;
        }
        //////////////////////////////////////////////////////////////////////////
        NodePtr movie_getSocket( Movie * _movie, const ConstString & _name )
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
        bool movie_hasSocket( Movie * _movie, const ConstString & _name )
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
        bool movie_setMovieEvent( Movie * _movie, const ConstString & _name, const pybind::object & _cb, const pybind::args & _args )
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
        bool movie_removeMovieEvent( Movie * _movie, const ConstString & _name )
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
        bool movie_hasMovieEvent( Movie * _movie, const ConstString & _name )
        {
            NodePtr node;
            MoviePtr submovie;
            bool successful = _movie->hasMovieNode( _name, STRINGIZE_STRING_LOCAL( "MovieEvent" ), &node, &submovie );

            return successful;
        }
        //////////////////////////////////////////////////////////////////////////
        class PythonVisitorMovieSlot
            : public VisitorMovieNode
        {
        public:
            PythonVisitorMovieSlot( pybind::kernel_interface * _kernel, pybind::list & _list )
                : m_kernel( _kernel )
                , m_list( _list )
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
            void operator = ( const PythonVisitorMovieSlot & );
        };
        //////////////////////////////////////////////////////////////////////////
        pybind::list movie_getSlots( pybind::kernel_interface * _kernel, Movie * _movie )
        {
            pybind::list py_list( _kernel );

            PythonVisitorMovieSlot visitor( _kernel, py_list );
            _movie->visitMovieLayer( STRINGIZE_STRING_LOCAL( "MovieSlot" ), &visitor );

            return py_list;
        }
        //////////////////////////////////////////////////////////////////////////
        class PythonVisitorMovieSocket
            : public VisitorMovieNode
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
        pybind::list movie_getSockets( pybind::kernel_interface * _kernel, Movie * _movie )
        {
            pybind::list py_list( _kernel );

            PythonVisitorMovieSocket visitor( _kernel, py_list );
            _movie->visitMovieLayer( STRINGIZE_STRING_LOCAL( "MovieSocketImage" ), &visitor );
            _movie->visitMovieLayer( STRINGIZE_STRING_LOCAL( "MovieSocketShape" ), &visitor );

            return py_list;
        }
        //////////////////////////////////////////////////////////////////////////
        class PythonVisitorMovieSubMovie
            : public VisitorMovieNode
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
        pybind::list movie_getSubMovies( pybind::kernel_interface * _kernel, Movie * _movie )
        {
            pybind::list py_list( _kernel );

            PythonVisitorMovieSubMovie visitor( _kernel, py_list );
            _movie->visitMovieLayer( STRINGIZE_STRING_LOCAL( "SubMovie" ), &visitor );

            return py_list;
        }
        //////////////////////////////////////////////////////////////////////////
        class PythonVisitorMovieLayer
            : public VisitorMovieNode
        {
        public:
            PythonVisitorMovieLayer( pybind::kernel_interface * _kernel, const pybind::list & _list )
                : m_kernel( _kernel )
                , m_list( _list )
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
        pybind::list movie_filterLayers( pybind::kernel_interface * _kernel, Movie * _movie, const ConstString & _type )
        {
            pybind::list py_list( _kernel );

            PythonVisitorMovieLayer visitor( _kernel, py_list );
            _movie->visitMovieLayer( _type, &visitor );

            return py_list;
        }
        //////////////////////////////////////////////////////////////////////////
        float movie_getFrameDuration( Movie * _movie )
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
        float movie_getDuration( Movie * _movie )
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
        uint32_t movie_getFrameCount( Movie * _movie )
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
        const mt::vec2f & movie_getSize( Movie * _movie )
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
        float movie_getLayerPathLength( Movie * _movie, const ConstString & _name )
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
        pybind::list movie_getLayerPath( pybind::kernel_interface * _kernel, Movie * _movie, const ConstString & _name )
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
        pybind::list movie_getLayerPath2( pybind::kernel_interface * _kernel, Movie * _movie, const ConstString & _name )
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
        pybind::list movie_getLayerPath3( pybind::kernel_interface * _kernel, Movie * _movie, const ConstString & _name )
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
        pybind::list movie_getLayerPath4( pybind::kernel_interface * _kernel, Movie * _movie, const ConstString & _name )
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
        mt::vec3f movie_getMovieSlotWorldPosition( Movie * _movie, const ConstString & _slotName )
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
        mt::vec3f movie_getMovieSlotOffsetPosition( Movie * _movie, const ConstString & _slotName )
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
        bool movie_attachMovieSlotNode( Movie * _movie, const ConstString & _slotName, Node * _node )
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
        bool movie_removeAllMovieSlotNode( Movie * _movie, const ConstString & _slotName )
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

            node->removeChildren();

            return true;
        }
        //////////////////////////////////////////////////////////////////////////
        void s_Transformation3D_setAngleDeg( Transformation3D * _transformation, float _angle )
        {
            float rad = _angle * mt::constant::deg2rad;

            _transformation->setOrientationX( rad );
        }
        //////////////////////////////////////////////////////////////////////////
        void s_Transformation3D_removeRelationTransformation( Transformation3D * _transformation )
        {
            _transformation->setRelationTransformation( nullptr );
        }
        //////////////////////////////////////////////////////////////////////////
        const ConstString & s_Resource_getCategory( Resource * _resource )
        {
            const FileGroupInterfacePtr & category = _resource->getCategory();

            const ConstString & categoryName = category->getName();

            return categoryName;
        }
        //////////////////////////////////////////////////////////////////////////
        void Transformation3D_coordinate( Transformation3D * _transformation, const mt::vec3f & _coordinate )
        {
            const mt::vec3f & origin = _transformation->getOrigin();
            _transformation->setOrigin( origin + _coordinate );
            _transformation->translate( _coordinate );
        }
        //////////////////////////////////////////////////////////////////////////
        mt::vec3f s_ResourceMovie_getLayerPosition( ResourceMovie * _movie, const ConstString & _name )
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
        float s_ResourceMovie_getLayerIn( ResourceMovie * _movie, const ConstString & _name )
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
        bool s_ResourceMovie_hasLayer( ResourceMovie * _movie, const ConstString & _name )
        {
            const MovieLayer * layer;
            bool successful = _movie->hasMovieLayer( _name, &layer );

            return successful;
        }
        //////////////////////////////////////////////////////////////////////////
        bool s_ResourceMovie_hasLayerType( ResourceMovie * _movie, const ConstString & _name, const ConstString & _type )
        {
            const MovieLayer * layer;
            bool successful = _movie->hasMovieLayerType( _name, _type, &layer );

            return successful;
        }
        //////////////////////////////////////////////////////////////////////////
        mt::vec2f s_HotSpotPolygon_getLocalPolygonCenter( HotSpotPolygon * _hs )
        {
            const Polygon & polygon = _hs->getPolygon();

            mt::box2f bb;
            polygon.to_box2f( bb );

            mt::vec2f c;
            mt::get_center_box( bb, c );

            return c;
        }
        //////////////////////////////////////////////////////////////////////////
        mt::vec2f s_HotSpotPolygon_getWorldPolygonCenter( HotSpotPolygon * _hs )
        {
            const Polygon & polygon = _hs->getPolygon();

            mt::box2f bb;
            mt::ident_box( bb );

            const mt::mat4f & wm = _hs->getWorldMatrix();

            size_t outer_count = polygon.outer_count();
            const mt::vec2f * outer_points = polygon.outer_points();

            for( const mt::vec2f
                *it = outer_points,
                *it_end = outer_points + outer_count;
                it != it_end;
                ++it )
            {
                const mt::vec2f & v = *it;

                mt::vec2f v_wm;
                mt::mul_v2_v2_m4( v_wm, v, wm );

                mt::add_internal_point( bb, v_wm );
            }

            mt::vec2f c;
            mt::get_center_box( bb, c );

            return c;
        }
        //////////////////////////////////////////////////////////////////////////
        mt::vec2f s_HotSpotPolygon_getScreenPolygonCenter( HotSpotPolygon * _hs )
        {
            const Resolution & contentResolution = APPLICATION_SERVICE()
                ->getContentResolution();

            const RenderCameraInterfacePtr & camera = _hs->getRenderCameraInheritance();
            const RenderViewportInterfacePtr & viewport = _hs->getRenderViewportInheritance();

            mt::box2f b1;
            _hs->getPolygonScreen( camera, viewport, contentResolution, &b1, nullptr );

            mt::vec2f c;
            mt::get_center_box( b1, c );

            return c;
        }
        //////////////////////////////////////////////////////////////////////////
        Polygon s_HotSpotPolygon_getWorldPolygon( HotSpotPolygon * _hs )
        {
            const Polygon & polygon = _hs->getPolygon();

            const mt::mat4f & wm = _hs->getWorldMatrix();

            Polygon pwm;
            polygon.mul_wm( pwm, wm );

            return pwm;
        }
        //////////////////////////////////////////////////////////////////////////
        const mt::vec2f & s_Shape_getSurfaceSize( Shape * _shape )
        {
            const SurfacePtr & surface = _shape->getSurface();

            if( surface == nullptr )
            {
                LOGGER_ERROR( "s_getSurfaceSize shape %s not setup surface"
                    , _shape->getName().c_str()
                );

                return mt::vec2f::identity();
            }

            const mt::vec2f & size = surface->getSize();

            return size;
        }
        //////////////////////////////////////////////////////////////////////////
        mt::vec2f s_Shape_getLocalImageCenter( Shape * _shape )
        {
            const SurfacePtr & surface = _shape->getSurface();

            if( surface == nullptr )
            {
                LOGGER_ERROR( "s_getLocalImageCenter shape %s not setup surface"
                    , _shape->getName().c_str()
                );

                return mt::vec2f::identity();
            }

            const mt::vec2f & offset = surface->getOffset();

            const mt::vec2f & size = surface->getSize();

            mt::vec2f center = offset + size * 0.5f;

            return center;
        }
        //////////////////////////////////////////////////////////////////////////
        mt::vec2f s_Shape_getWorldImageCenter( Shape * _shape )
        {
            mt::vec2f imageCenter = s_Shape_getLocalImageCenter( _shape );

            const mt::mat4f & wm = _shape->getWorldMatrix();

            mt::vec2f imageCenter_wm;
            mt::mul_v2_v2_m4( imageCenter_wm, imageCenter, wm );

            return imageCenter_wm;
        }
        //////////////////////////////////////////////////////////////////////////
        NodeAffectorCreator::NodeAffectorCreatorInterpolateLinear<mt::vec4f> m_nodeAffectorCreatorInterpolateLinearVec4;
        //////////////////////////////////////////////////////////////////////////
        FactoryPtr m_factoryNodeAffectorCallback;
        //////////////////////////////////////////////////////////////////////////
        ScriptableAffectorCallbackPtr createNodeAffectorCallback( const ScriptablePtr & _scriptable, const pybind::object & _cb, const pybind::args & _args )
        {
            ScriptableAffectorCallbackPtr callback = m_factoryNodeAffectorCallback->createObject();

            callback->initialize( _scriptable, _cb, _args );

            return callback;
        }
        //////////////////////////////////////////////////////////////////////////
        uint32_t s_ShapeQuadFlex_setPercentVisibilityTo( const ShapeQuadFlexPtr & _shape, float _time, const mt::vec4f& _percent, const pybind::object & _cb, const pybind::args & _args )
        {
            if( _shape->isActivate() == false )
            {
                return 0;
            }

            ScriptableAffectorCallbackPtr callback = createNodeAffectorCallback( _shape, _cb, _args );

            AffectorPtr affector = m_nodeAffectorCreatorInterpolateLinearVec4.create( ETA_VISIBILITY
                , callback
                , [_shape]( const mt::vec4f & _v ) { _shape->setPercentVisibility( _v ); }
                , _shape->getPercentVisibility(), _percent, _time
            );

            if( affector == nullptr )
            {
                return 0;
            }

            _shape->stopAffectors( ETA_VISIBILITY );

            if( _shape->isActivate() == false )
            {
                return 0;
            }

            AFFECTOR_ID id = _shape->addAffector( affector );

            return id;
        }
        //////////////////////////////////////////////////////////////////////////
        void s_ShapeQuadFlex_setPercentVisibilityStop( const ShapeQuadFlexPtr & _shape )
        {
            _shape->stopAffectors( ETA_VISIBILITY );
        }
        //////////////////////////////////////////////////////////////////////////
        FactoryPtr m_factoryPythonScheduleEvent;
        //////////////////////////////////////////////////////////////////////////
        uint32_t ScheduleManagerInterface_schedule( ScheduleManagerInterface * _scheduleManager, float _timing, const pybind::object & _script, const pybind::args & _args )
        {
            PythonScheduleEventPtr sl = m_factoryPythonScheduleEvent->createObject();

            sl->initialize( _script, _args );

            uint32_t id = _scheduleManager->event( _timing, sl );

            return id;
        }
        //////////////////////////////////////////////////////////////////////////
        FactoryPtr m_factoryDelaySchedulePipe;
        FactoryPtr m_factoryPythonScheduleTimer;
        //////////////////////////////////////////////////////////////////////////
        uint32_t ScheduleManagerInterface_timing( ScheduleManagerInterface * _scheduleManager, float _delay, const pybind::object & _listener, const pybind::args & _args )
        {
            DelaySchedulePipePtr pipe = m_factoryDelaySchedulePipe->createObject();

            pipe->initialize( _delay );

            PythonScheduleTimerPtr tl = m_factoryPythonScheduleTimer->createObject();

            tl->initialize( _listener, _args );

            uint32_t id = _scheduleManager->timing( pipe, tl );

            return id;
        }
        //////////////////////////////////////////////////////////////////////////
        FactoryPtr m_factoryPythonSchedulePipe;
        //////////////////////////////////////////////////////////////////////////
        uint32_t ScheduleManagerInterface_pipe( ScheduleManagerInterface * _scheduleManager, const pybind::object & _pipe, const pybind::object & _listener, const pybind::args & _args )
        {
            PythonSchedulePipePtr pipe = m_factoryPythonSchedulePipe->createObject();

            pipe->initialize( _pipe, _args );

            PythonScheduleTimerPtr tl = m_factoryPythonScheduleTimer->createObject();

            tl->initialize( _listener, _args );

            uint32_t id = _scheduleManager->timing( pipe, tl );

            return id;
        }
        //////////////////////////////////////////////////////////////////////////
        class PythonSceneChangeCallback
            : public SceneChangeCallbackInterface
        {
        public:
            void initialize( const pybind::object & _cb, const pybind::args & _args )
            {
                m_cb = _cb;
                m_args = _args;
            }

        public:
            void onSceneChange( const ScenePtr & _scene, bool _enable, bool _remove ) override
            {
                if( _remove == false )
                {
                    if( _scene == nullptr )
                    {
                        m_cb.call_args( pybind::ret_none(), _enable, m_args );
                    }
                    else
                    {
                        const pybind::object & py_scene = _scene->getScriptObject();

                        m_cb.call_args( py_scene, _enable, m_args );
                    }
                }
                else
                {
                    m_cb.call_args( m_args );
                }
            }

        public:
            pybind::object m_cb;
            pybind::args m_args;
        };
        //////////////////////////////////////////////////////////////////////////
        typedef IntrusivePtr<PythonSceneChangeCallback> PythonSceneChangeCallbackPtr;
        //////////////////////////////////////////////////////////////////////////
        FactoryPtr m_factoryPythonSceneChangeCallback;
        //////////////////////////////////////////////////////////////////////////
        uint32_t s_Animatable_play( Animatable * _animatable )
        {
            float time = TIMELINE_SERVICE()
                ->getTime();

            uint32_t id = _animatable->play( time );

            return id;
        }
        //////////////////////////////////////////////////////////////////////////
        PyObject * s_SurfaceVideo_setEventListener( pybind::kernel_interface * _kernel, SurfaceVideo * _surface, PyObject * _args, PyObject * _kwds )
        {
            (void)_args;

            if( _kwds == nullptr )
            {
                return pybind::ret_none();
            }

            pybind::dict py_kwds( _kernel, _kwds );
            Helper::registerAnimatableEventReceiver<>( py_kwds, _surface );

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

                    LOGGER_ERROR( "SurfaceVideo::setEventListener invalid kwds '%s'\n"
                        , k.c_str()
                    );
                }

                throw;
            }
#endif

            return pybind::ret_none();
        }
        //////////////////////////////////////////////////////////////////////////
        PyObject * s_SurfaceImageSequence_setEventListener( pybind::kernel_interface * _kernel, SurfaceImageSequence * _surface, PyObject * _args, PyObject * _kwds )
        {
            (void)_args;

            if( _kwds == nullptr )
            {
                return pybind::ret_none();
            }

            pybind::dict py_kwds( _kernel, _kwds );
            Helper::registerAnimatableEventReceiver<>( py_kwds, _surface );

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

                    LOGGER_ERROR( "SurfaceImageSequence::setEventListener invalid kwds '%s'\n"
                        , k.c_str()
                    );
                }

                throw;
            }
#endif

            return pybind::ret_none();
        }
        //////////////////////////////////////////////////////////////////////////
        PyObject * s_SurfaceSound_setEventListener( pybind::kernel_interface * _kernel, SurfaceSound * _surface, PyObject * _args, PyObject * _kwds )
        {
            (void)_args;

            if( _kwds == nullptr )
            {
                return pybind::ret_none();
            }

            pybind::dict py_kwds( _kernel, _kwds );
            Helper::registerAnimatableEventReceiver<>( py_kwds, _surface );

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

                    LOGGER_ERROR( "SurfaceSound::setEventListener invalid kwds '%s'\n"
                        , k.c_str()
                    );
                }

                throw;
            }
#endif

            return pybind::ret_none();
        }
        //////////////////////////////////////////////////////////////////////////
        class PythonMeshEventReceiver
            : public ScriptEventReceiver
            , public MeshgetEventReceiver
        {
        public:
            void onMeshgetUpdate( float _current, float _timing ) override
            {
                m_cb.call( _current, _timing );
            }
        };
        //////////////////////////////////////////////////////////////////////////
        PyObject * s_Meshget_setEventListener( pybind::kernel_interface * _kernel, Meshget * _node, PyObject * _args, PyObject * _kwds )
        {
            (void)_args;

            if( _kwds == nullptr )
            {
                return pybind::ret_none();
            }

            pybind::dict py_kwds( _kernel, _kwds );
            Helper::registerScriptEventReceiver<PythonMeshEventReceiver>( py_kwds, _node, "onMeshgetUpdate", EVENT_MESHGET_UPDATE );

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

                    LOGGER_ERROR( "Meshget::setEventListener invalid kwds '%s'\n"
                        , k.c_str()
                    );
                }

                throw;
            }
#endif

            return pybind::ret_none();
        }
        ////////////////////////////////////////////////////////////////////////////        
        PyObject * s_ParticleEmitter2_setEventListener( pybind::kernel_interface * _kernel, ParticleEmitter2 * _node, PyObject * _args, PyObject * _kwds )
        {
            (void)_args;

            if( _kwds == nullptr )
            {
                return pybind::ret_none();
            }

            pybind::dict py_kwds( _kernel, _kwds );
            Helper::registerAnimatableEventReceiver<>( py_kwds, _node );

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

                    LOGGER_ERROR( "ParticleEmitter2::setEventListener invalid kwds '%s'\n"
                        , k.c_str()
                    );
                }

                throw;
            }
#endif

            return pybind::ret_none();
        }
        //////////////////////////////////////////////////////////////////////////
        class PythonScriptHolderEventReceiver
            : public ScriptEventReceiver
            , public ScriptHolderEventReceiver
        {
        public:
            pybind::object onScriptHolderKeepScript() override
            {
                return m_cb.call();
            }

            void onScriptHolderReleaseScript( const pybind::object & _script ) override
            {
                m_cb.call( _script );
            }
        };
        //////////////////////////////////////////////////////////////////////////
        PyObject * s_ScriptHolder_setEventListener( pybind::kernel_interface * _kernel, ScriptHolder * _node, PyObject * _args, PyObject * _kwds )
        {
            (void)_args;

            if( _kwds == nullptr )
            {
                return pybind::ret_none();
            }

            pybind::dict py_kwds( _kernel, _kwds );
            Helper::registerScriptEventReceiver<PythonScriptHolderEventReceiver>( py_kwds, _node, "onKeepScript", EVENT_KEEP_SCRIPT );
            Helper::registerScriptEventReceiver<PythonScriptHolderEventReceiver>( py_kwds, _node, "onReleaseScript", EVENT_RELEASE_SCRIPT );

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

                    LOGGER_ERROR( "ScriptHolder::setEventListener invalid kwds '%s'\n"
                        , k.c_str()
                    );
                }

                throw;
            }
#endif

            return pybind::ret_none();
        }
        //////////////////////////////////////////////////////////////////////////
        class PythonHotSpotEventReceiver
            : public ScriptEventReceiver
            , public HotSpotEventReceiver
        {
        public:
            void onHotSpotActivate() override
            {
                m_cb.call();
            }

            void onHotSpotDeactivate() override
            {
                m_cb.call();
            }

            void onHotSpotMouseLeave() override
            {
                m_cb.call();
            }

            bool onHotSpotMouseEnter( float _x, float _y ) override
            {
                return m_cb.call( _x, _y );
            }

            bool onHotSpotKey( const InputKeyEvent & _event ) override
            {
                return m_cb.call( _event.x, _event.y, (uint32_t)_event.code, _event.isDown, _event.isRepeat );
            }

            bool onHotSpotText( const InputTextEvent & _event ) override
            {
                return m_cb.call( _event.x, _event.y, _event.key );
            }

            bool onHotSpotMouseButton( const InputMouseButtonEvent & _event ) override
            {
                return m_cb.call( _event.touchId, _event.x, _event.y, _event.button, _event.pressure, _event.isDown, _event.isPressed );
            }

            bool onHotSpotMouseButtonBegin( const InputMouseButtonEvent & _event ) override
            {
                return m_cb.call( _event.touchId, _event.x, _event.y, _event.button, _event.pressure, _event.isDown, _event.isPressed );
            }

            bool onHotSpotMouseButtonEnd( const InputMouseButtonEvent & _event ) override
            {
                return m_cb.call( _event.touchId, _event.x, _event.y, _event.button, _event.pressure, _event.isDown, _event.isPressed );
            }

            bool onHotSpotMouseMove( const InputMouseMoveEvent & _event ) override
            {
                return m_cb.call( _event.touchId, _event.x, _event.y, _event.dx, _event.dy, _event.pressure );
            }

            bool onHotSpotMouseWheel( const InputMouseWheelEvent & _event ) override
            {
                return m_cb.call( _event.x, _event.y, _event.button, _event.wheel );
            }

            void onHotSpotMouseOverDestroy() override
            {
                m_cb.call();
            }

            void onHotSpotMouseButtonBegin( uint32_t _enumerator, bool _isEnd ) override
            {
                m_cb.call( _enumerator, _isEnd );
            }

            void onHotSpotMouseButtonEnd( uint32_t _enumerator, bool _isEnd ) override
            {
                m_cb.call( _enumerator, _isEnd );
            }
        };
        //////////////////////////////////////////////////////////////////////////
        PyObject * s_HotSpot_setEventListener( pybind::kernel_interface * _kernel, HotSpot * _node, PyObject * _args, PyObject * _kwds )
        {
            (void)_args;

            if( _kwds == nullptr )
            {
                return pybind::ret_none();
            }

            pybind::dict py_kwds( _kernel, _kwds );

            Helper::registerScriptEventReceiver<PythonHotSpotEventReceiver>( py_kwds, _node, "onHandleKeyEvent", EVENT_KEY );
            Helper::registerScriptEventReceiver<PythonHotSpotEventReceiver>( py_kwds, _node, "onHandleMouseButtonEvent", EVENT_MOUSE_BUTTON );
            Helper::registerScriptEventReceiver<PythonHotSpotEventReceiver>( py_kwds, _node, "onHandleMouseButtonEventBegin", EVENT_MOUSE_BUTTON_BEGIN );
            Helper::registerScriptEventReceiver<PythonHotSpotEventReceiver>( py_kwds, _node, "onHandleMouseButtonEventEnd", EVENT_MOUSE_BUTTON_END );
            Helper::registerScriptEventReceiver<PythonHotSpotEventReceiver>( py_kwds, _node, "onHandleMouseMove", EVENT_MOUSE_MOVE );
            Helper::registerScriptEventReceiver<PythonHotSpotEventReceiver>( py_kwds, _node, "onHandleMouseWheel", EVENT_MOUSE_WHEEL );
            Helper::registerScriptEventReceiver<PythonHotSpotEventReceiver>( py_kwds, _node, "onHandleMouseEnter", EVENT_MOUSE_ENTER );
            Helper::registerScriptEventReceiver<PythonHotSpotEventReceiver>( py_kwds, _node, "onHandleMouseLeave", EVENT_MOUSE_LEAVE );
            Helper::registerScriptEventReceiver<PythonHotSpotEventReceiver>( py_kwds, _node, "onHandleMouseOverDestroy", EVENT_MOUSE_OVER_DESTROY );
            Helper::registerScriptEventReceiver<PythonHotSpotEventReceiver>( py_kwds, _node, "onActivate", EVENT_ACTIVATE );
            Helper::registerScriptEventReceiver<PythonHotSpotEventReceiver>( py_kwds, _node, "onDeactivate", EVENT_DEACTIVATE );

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
                    LOGGER_ERROR( "HotSpot::setEventListener %s kwds %s"
                        , _node->getName().c_str()
                        , k.c_str()
                    );
                }

                throw;
            }
#endif

            return pybind::ret_none();
        }
        //////////////////////////////////////////////////////////////////////////
        PyObject * s_HotSpot_removeEventListener( HotSpot * _node, PyObject * _args, PyObject * _kwds )
        {
            (void)_args;
            (void)_kwds;

            _node->removeEvents();

            return pybind::ret_none();
        }
        //////////////////////////////////////////////////////////////////////////
        class PythonMovieEventReceiver
            : public PythonAnimatableEventReceiver<MovieEventReceiver>
        {
        public:
            pybind::object onMovieGetInternal( const ConstString & _group, const ConstString & _name ) override
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
        PyObject * s_Movie_setEventListener( pybind::kernel_interface * _kernel, Movie * _node, PyObject * _args, PyObject * _kwds )
        {
            (void)_args;

            if( _kwds == nullptr )
            {
                return pybind::ret_none();
            }

            pybind::dict py_kwds( _kernel, _kwds );
            Helper::registerAnimatableEventReceiver<PythonMovieEventReceiver>( py_kwds, _node );

            Helper::registerScriptEventReceiver<PythonMovieEventReceiver>( py_kwds, _node, "onMovieGetInternal", EVENT_MOVIE_GET_INTERNAL );
            Helper::registerScriptEventReceiver<PythonMovieEventReceiver>( py_kwds, _node, "onMovieActivateInternal", EVENT_MOVIE_ACTIVATE_INTERNAL );
            Helper::registerScriptEventReceiver<PythonMovieEventReceiver>( py_kwds, _node, "onMovieDeactivateInternal", EVENT_MOVIE_DEACTIVATE_INTERNAL );

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

                    LOGGER_ERROR( "Movie::setEventListener invalid kwds '%s'\n"
                        , k.c_str()
                    );
                }

                throw;
            }
#endif

            return pybind::ret_none();
        }
        //////////////////////////////////////////////////////////////////////////
        void s_Animatable_resume( Animatable * _animatable )
        {
            float time = TIMELINE_SERVICE()
                ->getTime();

            _animatable->resume( time );
        }
        //////////////////////////////////////////////////////////////////////////
        mt::vec3f s_Node_getWorldOffsetPosition( Node * _node, const mt::vec3f & _position )
        {
            const mt::vec3f & wp = _node->getWorldPosition();

            mt::vec3f offset = _position - wp;

            return offset;
        }
        //////////////////////////////////////////////////////////////////////////
        float s_Node_getLengthTo( Node * _node, const mt::vec3f & _position )
        {
            const mt::vec3f & wp = _node->getWorldPosition();

            float length = mt::length_v3_v3( wp, _position );

            return length;
        }
        //////////////////////////////////////////////////////////////////////////
        NodePtr s_Node_createChildren( Node * _node, const ConstString & _type )
        {
            NodePtr newNode = NODE_SERVICE()
                ->createNode( _type );

            if( newNode == nullptr )
            {
                return nullptr;
            }

            newNode->disable();

            _node->addChild( newNode );

            return newNode;
        }
        //////////////////////////////////////////////////////////////////////////
        pybind::list s_Node_getAllChildren( pybind::kernel_interface * _kernel, Node * _node )
        {
            TListNodeChild & children = _node->getChildren();

            TListNodeChild::size_type size = children.size();

            pybind::list py_children( _kernel, size );

            uint32_t index = 0;

            for( TSlugChild it( children ); it.eof() == false; )
            {
                Node * child = *it;

                it.next_shuffle();

                py_children[index++] = child;
            }

            return py_children;
        }
        //////////////////////////////////////////////////////////////////////////
        void s_Node_moveStop( Node * _node )
        {
            _node->stopAffectors( ETA_POSITION );
            _node->setLinearSpeed( mt::vec3f( 0.f, 0.f, 0.f ) );
        }
        //////////////////////////////////////////////////////////////////////////
        NodeAffectorCreator::NodeAffectorCreatorAccumulateLinear<mt::vec3f> m_nodeAffectorCreatorAccumulateLinear;
        //////////////////////////////////////////////////////////////////////////
        uint32_t s_Node_velocityTo( Node * _node, float _speed, const mt::vec3f& _dir, const pybind::object & _cb, const pybind::args & _args )
        {
            if( _node->isActivate() == false )
            {
                return 0;
            }

            if( _node->isAfterActive() == false )
            {
                return 0;
            }

            ScriptableAffectorCallbackPtr callback = createNodeAffectorCallback( _node, _cb, _args );

            AffectorPtr affector = m_nodeAffectorCreatorAccumulateLinear.create( ETA_POSITION
                , callback
                , [_node]( const mt::vec3f & _v ) { _node->setLocalPosition( _v ); }
                , _node->getLocalPosition(), _dir, _speed
            );

            if( affector == nullptr )
            {
                return 0;
            }

            s_Node_moveStop( _node );

            if( _node->isActivate() == false )
            {
                return 0;
            }

            mt::vec3f linearSpeed = _dir * _speed;
            _node->setLinearSpeed( linearSpeed );

            AFFECTOR_ID id = _node->addAffector( affector );

            return id;
        }
        //////////////////////////////////////////////////////////////////////////
        class AffectorVelocity2
            : public BaseAffector
        {
        public:
            AffectorVelocity2()
                : m_node( nullptr )
                , m_velocity( 0.f, 0.f, 0.f )
                , m_time( 0.f )
            {
            }

        public:
            void setNode( const NodePtr & _node )
            {
                m_node = _node;
            }

        public:
            void initialize( const mt::vec3f & _velocity, float _time )
            {
                m_velocity = _velocity;
                m_time = _time;
            }

        protected:
            bool _affect( float _current, float _timing ) override
            {
                (void)_current;

                m_time -= _timing;

                if( m_time > 0.f )
                {
                    m_node->translate( m_velocity * _timing );

                    return false;
                }

                m_node->translate( m_velocity * (m_time + _timing) );

                return true;
            }

            void stop() override
            {
                this->end_( false );
            }

        protected:
            NodePtr m_node;

            mt::vec3f m_velocity;
            float m_time;
        };
        //////////////////////////////////////////////////////////////////////////
        typedef IntrusivePtr<AffectorVelocity2> AffectorVelocity2Ptr;
        //////////////////////////////////////////////////////////////////////////
        class FactoryAffectorVelocity2
        {
        public:
            FactoryAffectorVelocity2()
            {
                m_factory = new FactoryPool<AffectorVelocity2, 4>();
            }

        public:
            AffectorPtr create( EAffectorType _type, const AffectorCallbackInterfacePtr & _cb
                , const NodePtr & _node, const mt::vec3f & _velocity, float _time )
            {
                AffectorVelocity2Ptr affector = m_factory->createObject();

                affector->setAffectorType( _type );

                affector->setCallback( _cb );

                affector->setNode( _node );

                affector->initialize( _velocity, _time );

                return affector;
            }

        protected:
            FactoryPtr m_factory;
        };
        //////////////////////////////////////////////////////////////////////////
        FactoryAffectorVelocity2 m_factoryAffectorVelocity2;
        //////////////////////////////////////////////////////////////////////////
        uint32_t s_Node_velocityTo2( Node * _node, const mt::vec3f & _velocity, float _time, const pybind::object & _cb, const pybind::args & _args )
        {
            if( _node->isActivate() == false )
            {
                return 0;
            }

            if( _node->isAfterActive() == false )
            {
                return 0;
            }

            ScriptableAffectorCallbackPtr callback = createNodeAffectorCallback( _node, _cb, _args );

            AffectorPtr affector = m_factoryAffectorVelocity2.create( ETA_POSITION
                , callback, _node, _velocity, _time
            );

            if( affector == nullptr )
            {
                return 0;
            }

            s_Node_moveStop( _node );

            if( _node->isActivate() == false )
            {
                return 0;
            }

            _node->setLinearSpeed( _velocity );

            AFFECTOR_ID id = _node->addAffector( affector );

            return id;
        }
        //////////////////////////////////////////////////////////////////////////
        NodeAffectorCreator::NodeAffectorCreatorInterpolateLinear<mt::vec3f> m_nodeAffectorCreatorInterpolateLinear;
        //////////////////////////////////////////////////////////////////////////
        uint32_t s_Node_moveTo( Node * _node, float _time, const mt::vec3f& _point, const pybind::object & _cb, const pybind::args & _args )
        {
            if( _node->isActivate() == false )
            {
                return 0;
            }

            if( _node->isAfterActive() == false )
            {
                return 0;
            }

            ScriptableAffectorCallbackPtr callback = createNodeAffectorCallback( _node, _cb, _args );

            AffectorPtr affector =
                m_nodeAffectorCreatorInterpolateLinear.create( ETA_POSITION
                    , callback
                    , [_node]( const mt::vec3f & _v ) { _node->setLocalPosition( _v ); }
                    , _node->getLocalPosition(), _point, _time
                );

            if( affector == nullptr )
            {
                return 0;
            }

            s_Node_moveStop( _node );

            if( _node->isActivate() == false )
            {
                return 0;
            }

            float invTime = 1.0f / _time;
            const mt::vec3f & pos = _node->getLocalPosition();
            mt::vec3f linearSpeed = (_point - pos) * invTime;

            _node->setLinearSpeed( linearSpeed );

            AFFECTOR_ID id = _node->addAffector( affector );

            return id;
        }
        //////////////////////////////////////////////////////////////////////////
        NodeAffectorCreator::NodeAffectorCreatorInterpolateQuadratic<mt::vec3f> m_nodeAffectorCreatorInterpolateQuadratic;
        //////////////////////////////////////////////////////////////////////////
        uint32_t s_Node_accMoveTo( Node * _node, float _time, const mt::vec3f& _point, const pybind::object & _cb, const pybind::args & _args )
        {
            if( _node->isActivate() == false )
            {
                return 0;
            }

            if( _node->isAfterActive() == false )
            {
                return 0;
            }

            const mt::vec3f & linearSpeed = _node->getLinearSpeed();

            ScriptableAffectorCallbackPtr callback = createNodeAffectorCallback( _node, _cb, _args );

            AffectorPtr affector = m_nodeAffectorCreatorInterpolateQuadratic.create( ETA_POSITION
                , callback, [_node]( const mt::vec3f & _v ) { _node->setLocalPosition( _v ); }
                , _node->getLocalPosition(), _point, linearSpeed, _time
            );

            if( affector == nullptr )
            {
                return 0;
            }

            s_Node_moveStop( _node );

            if( _node->isActivate() == false )
            {
                return 0;
            }

            AFFECTOR_ID id = _node->addAffector( affector );

            return id;
        }
        //////////////////////////////////////////////////////////////////////////
        NodeAffectorCreator::NodeAffectorCreatorInterpolateBezier<mt::vec3f, 1> m_nodeAffectorCreatorInterpolateQuadraticBezier;
        //////////////////////////////////////////////////////////////////////////
        uint32_t s_Node_bezier2To( Node * _node
            , float _time
            , const mt::vec3f& _to
            , const mt::vec3f& _v0
            , const pybind::object & _cb
            , const pybind::args & _args )
        {
            if( _node->isActivate() == false )
            {
                return 0;
            }

            if( _node->isAfterActive() == false )
            {
                return 0;
            }

            ScriptableAffectorCallbackPtr callback = createNodeAffectorCallback( _node, _cb, _args );

            mt::vec3f v[] = { _v0 };

            AffectorPtr affector = m_nodeAffectorCreatorInterpolateQuadraticBezier.create( ETA_POSITION
                , callback
                , [_node]( const mt::vec3f & _v ) { _node->setLocalPosition( _v ); }
                , _node->getLocalPosition(), _to, v, _time
            );

            if( affector == nullptr )
            {
                return 0;
            }

            s_Node_moveStop( _node );

            if( _node->isActivate() == false )
            {
                return 0;
            }

            AFFECTOR_ID id = _node->addAffector( affector );

            return id;
        }
        //////////////////////////////////////////////////////////////////////////
        NodeAffectorCreator::NodeAffectorCreatorInterpolateBezier<mt::vec3f, 2> m_nodeAffectorCreatorInterpolateCubicBezier;
        //////////////////////////////////////////////////////////////////////////
        uint32_t s_Node_bezier3To( Node * _node
            , float _time
            , const mt::vec3f& _to
            , const mt::vec3f& _v0
            , const mt::vec3f& _v1
            , const pybind::object & _cb
            , const pybind::args & _args )
        {
            if( _node->isActivate() == false )
            {
                return 0;
            }

            if( _node->isAfterActive() == false )
            {
                return 0;
            }

            ScriptableAffectorCallbackPtr callback = createNodeAffectorCallback( _node, _cb, _args );

            mt::vec3f v[] = { _v0, _v1 };

            AffectorPtr affector =
                m_nodeAffectorCreatorInterpolateCubicBezier.create( ETA_POSITION
                    , callback
                    , [_node]( const mt::vec3f & _v ) { _node->setLocalPosition( _v ); }
                    , _node->getLocalPosition(), _to, v, _time
                );

            if( affector == nullptr )
            {
                return 0;
            }

            s_Node_moveStop( _node );

            if( _node->isActivate() == false )
            {
                return 0;
            }

            AFFECTOR_ID id = _node->addAffector( affector );

            return id;
        }
        //////////////////////////////////////////////////////////////////////////
        NodeAffectorCreator::NodeAffectorCreatorInterpolateBezier<mt::vec3f, 3> m_nodeAffectorCreatorInterpolateQuarticBezier;
        //////////////////////////////////////////////////////////////////////////
        uint32_t s_Node_bezier4To( Node * _node
            , float _time
            , const mt::vec3f& _to
            , const mt::vec3f& _v0
            , const mt::vec3f& _v1
            , const mt::vec3f& _v2
            , const pybind::object & _cb
            , const pybind::args & _args )
        {
            if( _node->isActivate() == false )
            {
                return 0;
            }

            if( _node->isAfterActive() == false )
            {
                return 0;
            }

            ScriptableAffectorCallbackPtr callback = createNodeAffectorCallback( _node, _cb, _args );

            mt::vec3f v[] = { _v0, _v1, _v2 };

            AffectorPtr affector =
                m_nodeAffectorCreatorInterpolateQuarticBezier.create( ETA_POSITION
                    , callback
                    , [_node]( const mt::vec3f & _v ) { _node->setLocalPosition( _v ); }
                    , _node->getLocalPosition(), _to, v, _time
                );

            if( affector == nullptr )
            {
                return 0;
            }

            s_Node_moveStop( _node );

            if( _node->isActivate() == false )
            {
                return 0;
            }

            AFFECTOR_ID id = _node->addAffector( affector );

            return id;
        }
        //////////////////////////////////////////////////////////////////////////
        class AffectorCreatorInterpolateParabolic
            : public BaseAffector
        {
        public:
            AffectorCreatorInterpolateParabolic()
                : m_speed( 5.f )
            {
            }

        public:
            void setNode( const NodePtr & _node )
            {
                m_node = _node;
            }

        public:
            void initialize( const mt::vec3f & _end, const mt::vec3f & _v0, float _time )
            {
                const mt::vec3f & start_position = m_node->getLocalPosition();

                m_interpolator.start( start_position, _end, _v0, _time );

                mt::vec3f next_position;
                m_interpolator.step( 100.f, &next_position );

                mt::vec3f dir;
                mt::dir_v3_v3( dir, start_position, next_position );

                m_prevDir = dir;
                m_currentDir = dir;
                m_targetDir = dir;

                m_node->setBillboard( dir, mt::vec3f( 0.f, 1.f, 1.f ) );
            }

        protected:
            bool _affect( float _current, float _timing ) override
            {
                mt::vec3f position;
                bool finish = m_interpolator.update( _current, _timing, &position );

                this->updateDirection_( _current, _timing, position );
                this->updatePosition_( position );

                if( finish == false )
                {
                    return false;
                }

                return true;
            }

            void stop() override
            {
                m_interpolator.stop();

                this->end_( false );
            }

        protected:
            void updateDirection_( float _current, float _timing, const mt::vec3f & _position )
            {
                (void)_current;

                const mt::vec3f & prev_position = m_node->getLocalPosition();

                if( mt::sqrlength_v3_v3( prev_position, _position ) > mt::constant::eps )
                {
                    mt::dir_v3_v3( m_targetDir, prev_position, _position );
                }

                float length = mt::length_v3_v3( m_targetDir, m_currentDir );

                if( length < mt::constant::eps )
                {
                    return;
                }

                float t = length / _timing * m_speed;

                if( t > 1.f )
                {
                    m_currentDir = m_targetDir;

                    m_node->setBillboard( m_currentDir, mt::vec3f( 0.f, 1.f, 1.f ) );
                }
                else
                {
                    m_currentDir = m_currentDir + (m_targetDir - m_currentDir) * t;

                    m_node->setBillboard( m_currentDir, mt::vec3f( 0.f, 1.f, 1.f ) );
                }
            }

            void updatePosition_( const mt::vec3f & _position )
            {
                const mt::vec3f & prev_position = m_node->getLocalPosition();

                if( mt::sqrlength_v3_v3( prev_position, _position ) < mt::constant::eps )
                {
                    return;
                }

                m_node->setLocalPosition( _position );
            }

        protected:
            NodePtr m_node;

            mt::vec3f m_prevDir;
            mt::vec3f m_currentDir;
            mt::vec3f m_targetDir;
            float m_speed;

            ValueInterpolatorParabolic<mt::vec3f> m_interpolator;
        };
        //////////////////////////////////////////////////////////////////////////
        typedef IntrusivePtr<AffectorCreatorInterpolateParabolic> AffectorCreatorInterpolateParabolicPtr;
        //////////////////////////////////////////////////////////////////////////
        class FactoryAffectorInterpolateParabolic
        {
        public:
            FactoryAffectorInterpolateParabolic()
            {
                m_factory = new FactoryPool<AffectorCreatorInterpolateParabolic, 4>();
            }

        public:
            AffectorPtr create( EAffectorType _type, const AffectorCallbackInterfacePtr & _cb
                , const NodePtr & _node, const mt::vec3f & _end, const mt::vec3f & _v0, float _time )
            {
                AffectorCreatorInterpolateParabolicPtr affector = m_factory->createObject();

                affector->setAffectorType( _type );

                affector->setCallback( _cb );

                affector->setNode( _node );

                affector->initialize( _end, _v0, _time );

                return affector;
            }

        protected:
            FactoryPtr m_factory;
        };
        //////////////////////////////////////////////////////////////////////////
        FactoryAffectorInterpolateParabolic m_nodeAffectorCreatorInterpolateParabolic;
        //////////////////////////////////////////////////////////////////////////
        uint32_t s_Node_parabolaTo( Node * _node
            , float _time
            , const mt::vec3f& _end
            , const mt::vec3f& _v0
            , const pybind::object & _cb
            , const pybind::args & _args )
        {
            if( _node->isActivate() == false )
            {
                return 0;
            }

            if( _node->isAfterActive() == false )
            {
                return 0;
            }

            ScriptableAffectorCallbackPtr callback = createNodeAffectorCallback( _node, _cb, _args );

            AffectorPtr affector =
                m_nodeAffectorCreatorInterpolateParabolic.create( ETA_POSITION
                    , callback, _node, _end, _v0, _time
                );

            if( affector == nullptr )
            {
                return 0;
            }

            s_Node_moveStop( _node );

            if( _node->isActivate() == false )
            {
                return 0;
            }

            AFFECTOR_ID id = _node->addAffector( affector );

            return id;
        }
        //////////////////////////////////////////////////////////////////////////
        class AffectorCreatorFollowTo
            : public BaseAffector
        {
        public:
            AffectorCreatorFollowTo()
                : m_offset( 0.f, 0.f, 0.f )
                , m_distance( 0.f )
                , m_moveSpeed( 0.f )
                , m_moveAcceleration( 0.f )
                , m_moveLimit( 0.f )
                , m_rotate( false )
                , m_rotationSpeed( 0.f )
                , m_rotationAcceleration( 0.f )
                , m_rotationLimit( 0.f )
            {
            }

        public:
            bool initialize( const NodePtr & _node, const NodePtr & _target, const mt::vec3f & _offset, float _distance, float _moveSpeed, float _moveAcceleration, float _moveLimit, bool _rotate, float _rotationSpeed, float _rotationAcceleration, float _rotationLimit )
            {
                if( _node == nullptr )
                {
                    return false;
                }

                if( _target == nullptr )
                {
                    return false;
                }

                m_node = _node;
                m_target = _target;
                m_offset = _offset;
                m_distance = _distance;
                m_moveSpeed = _moveSpeed;
                m_moveAcceleration = _moveAcceleration;
                m_moveLimit = _moveLimit;
                m_rotate = _rotate;
                m_rotationSpeed = _rotationSpeed;
                m_rotationAcceleration = _rotationAcceleration;
                m_rotationLimit = _rotationLimit;

                return true;
            }

        protected:
            bool _affect( float _current, float _timing ) override
            {
                (void)_current;

                mt::vec3f node_position = m_node->getLocalPosition();
                mt::vec3f follow_position = m_target->getLocalPosition();

                mt::vec3f current_direction;

                if( m_rotate == true )
                {
                    mt::vec3f direction = follow_position - node_position;

                    mt::mat4f mr;
                    mt::make_rotate_m4_direction( mr, direction, mt::vec3f( 0.f, 0.f, 1.f ) );

                    mt::vec3f target_orientation;
                    mt::make_euler_angles( target_orientation, mr );

                    const mt::vec3f & node_orientation = m_node->getOrientation();

                    mt::vec3f correct_rotate_from;
                    mt::vec3f correct_rotate_to;
                    mt::angle_correct_interpolate_from_to( node_orientation.x, target_orientation.x, correct_rotate_from.x, correct_rotate_to.x );
                    mt::angle_correct_interpolate_from_to( node_orientation.y, target_orientation.y, correct_rotate_from.y, correct_rotate_to.y );
                    mt::angle_correct_interpolate_from_to( node_orientation.z, target_orientation.z, correct_rotate_from.z, correct_rotate_to.z );

                    float roatationSpeedStep = m_rotationAcceleration * _timing;

                    if( m_rotationSpeed + roatationSpeedStep > m_rotationLimit )
                    {
                        m_rotationSpeed = m_rotationLimit;
                    }
                    else
                    {
                        m_rotationSpeed += m_rotationAcceleration * _timing;
                    }

                    mt::vec3f new_orientation;
                    mt::follow_v3( new_orientation, correct_rotate_from, correct_rotate_to, m_rotationSpeed * _timing );

                    m_node->setOrientation( new_orientation );

                    current_direction = m_node->getAxisDirection();
                }
                else
                {
                    mt::dir_v3_v3( current_direction, node_position, follow_position );
                }

                float directionSpeedStep = m_moveAcceleration * _timing;

                if( m_moveSpeed + directionSpeedStep > m_moveLimit )
                {
                    m_moveSpeed = m_moveLimit;
                }
                else
                {
                    m_moveSpeed += m_moveAcceleration * _timing;
                }

                float step = m_moveSpeed * _timing;

                float length = mt::length_v3_v3( node_position, follow_position );

                if( m_distance > 0.0 )
                {
                    if( length - step < m_distance )
                    {
                        mt::vec3f distance_position = follow_position + mt::norm_v3( node_position - follow_position ) * m_distance;

                        m_node->setLocalPosition( distance_position );

                        return true;
                    }
                }
                else
                {
                    if( length - step < 0.f )
                    {
                        m_node->setLocalPosition( follow_position );

                        return false;
                    }
                }

                mt::vec3f new_position = node_position + current_direction * step;

                m_node->setLocalPosition( new_position );

                return false;
            }

            void stop() override
            {
                this->end_( false );
            }

        protected:
            NodePtr m_node;
            NodePtr m_target;

            mt::vec3f m_offset;
            float m_distance;
            float m_moveSpeed;
            float m_moveAcceleration;
            float m_moveLimit;
            bool m_rotate;
            float m_rotationSpeed;
            float m_rotationAcceleration;
            float m_rotationLimit;
        };
        //////////////////////////////////////////////////////////////////////////
        typedef IntrusivePtr<AffectorCreatorFollowTo> AffectorCreatorFollowToPtr;
        //////////////////////////////////////////////////////////////////////////
        class FactoryAffectorFollowTo
        {
        public:
            FactoryAffectorFollowTo()
            {
                m_factory = new FactoryPool<AffectorCreatorFollowTo, 4>();
            }

        public:
            AffectorPtr create( EAffectorType _type, const AffectorCallbackInterfacePtr & _cb
                , const NodePtr & _node, const NodePtr & _target, const mt::vec3f & _offset, float _distance, float _moveSpeed, float _moveAcceleration, float _moveLimit
                , bool _rotate
                , float _rotationSpeed, float _rotationAcceleration, float _rotationLimit )
            {
                AffectorCreatorFollowToPtr affector = m_factory->createObject();

                affector->setAffectorType( _type );

                affector->setCallback( _cb );

                if( affector->initialize( _node, _target, _offset, _distance, _moveSpeed, _moveAcceleration, _moveLimit, _rotate, _rotationSpeed, _rotationAcceleration, _rotationLimit ) == false )
                {
                    return nullptr;
                }

                return affector;
            }

        protected:
            FactoryPtr m_factory;
        };
        //////////////////////////////////////////////////////////////////////////
        FactoryAffectorFollowTo m_nodeAffectorCreatorFollowTo;
        //////////////////////////////////////////////////////////////////////////		
        uint32_t s_Node_followTo( Node * _node
            , const NodePtr & _target
            , const mt::vec3f & _offset
            , float _distance
            , float _moveSpeed
            , float _moveAcceleration
            , float _moveLimit
            , bool _rotate
            , float _rotationSpeed
            , float _rotationAcceleration
            , float _rotationLimit
            , const pybind::object & _cb
            , const pybind::args & _args )
        {
            if( _node->isActivate() == false )
            {
                return 0;
            }

            if( _node->isAfterActive() == false )
            {
                return 0;
            }

            ScriptableAffectorCallbackPtr callback = createNodeAffectorCallback( _node, _cb, _args );

            AffectorPtr affector =
                m_nodeAffectorCreatorFollowTo.create( ETA_POSITION
                    , callback, _node, _target, _offset, _distance
                    , _moveSpeed, _moveAcceleration, _moveLimit
                    , _rotate
                    , _rotationSpeed, _rotationAcceleration, _rotationLimit
                );

            if( affector == nullptr )
            {
                return 0;
            }

            s_Node_moveStop( _node );

            if( _node->isActivate() == false )
            {
                return 0;
            }

            AFFECTOR_ID id = _node->addAffector( affector );

            return id;
        }
        //////////////////////////////////////////////////////////////////////////
        class AffectorCreatorFollowToW
            : public BaseAffector
        {
        public:
            AffectorCreatorFollowToW()
                : m_offset( 0.f, 0.f, 0.f )
                , m_distance( 0.f )
                , m_moveSpeed( 0.f )
                , m_moveAcceleration( 0.f )
                , m_moveLimit( 0.f )
            {
            }

        public:
            bool initialize( const NodePtr & _node, const NodePtr & _target, const mt::vec3f & _offset, float _distance, float _moveSpeed, float _moveAcceleration, float _moveLimit )
            {
                if( _node == nullptr )
                {
                    return false;
                }

                if( _target == nullptr )
                {
                    return false;
                }

                m_node = _node;
                m_target = _target;
                m_offset = _offset;
                m_distance = _distance;
                m_moveSpeed = _moveSpeed;
                m_moveAcceleration = _moveAcceleration;
                m_moveLimit = _moveLimit;

                return true;
            }

        protected:
            bool _affect( float _current, float _timing ) override
            {
                (void)_current;

                mt::vec3f node_position = m_node->getWorldPosition();
                mt::vec3f follow_position = m_target->getWorldPosition();

                mt::vec3f current_direction;

                mt::dir_v3_v3( current_direction, node_position, follow_position );

                float directionSpeedStep = m_moveAcceleration * _timing;

                if( m_moveSpeed + directionSpeedStep > m_moveLimit )
                {
                    m_moveSpeed = m_moveLimit;
                }
                else
                {
                    m_moveSpeed += m_moveAcceleration * _timing;
                }

                float step = m_moveSpeed * _timing;

                float length = mt::length_v3_v3( node_position, follow_position );

                if( m_distance > 0.0 )
                {
                    if( length - step < m_distance )
                    {
                        mt::vec3f distance_position = follow_position + mt::norm_v3( node_position - follow_position ) * m_distance;

                        m_node->setWorldPosition( distance_position );

                        return true;
                    }
                }
                else
                {
                    if( length - step < 0.f )
                    {
                        m_node->setWorldPosition( follow_position );

                        return false;
                    }
                }

                mt::vec3f new_position = node_position + current_direction * step;

                m_node->setWorldPosition( new_position );

                return false;
            }

            void stop() override
            {
                this->end_( false );
            }

        protected:
            NodePtr m_node;
            NodePtr m_target;

            mt::vec3f m_offset;
            float m_distance;
            float m_moveSpeed;
            float m_moveAcceleration;
            float m_moveLimit;
        };
        //////////////////////////////////////////////////////////////////////////
        typedef IntrusivePtr<AffectorCreatorFollowToW> AffectorCreatorFollowToWPtr;
        //////////////////////////////////////////////////////////////////////////
        class FactoryAffectorFollowToW
        {
        public:
            FactoryAffectorFollowToW()
            {
                m_factory = new FactoryPool<AffectorCreatorFollowToW, 4>();
            }

        public:
            AffectorPtr create( EAffectorType _type, const AffectorCallbackInterfacePtr & _cb
                , const NodePtr & _node, const NodePtr & _target, const mt::vec3f & _offset, float _distance, float _moveSpeed, float _moveAcceleration, float _moveLimit )
            {
                AffectorCreatorFollowToWPtr affector = m_factory->createObject();

                affector->setAffectorType( _type );

                affector->setCallback( _cb );

                if( affector->initialize( _node, _target, _offset, _distance, _moveSpeed, _moveAcceleration, _moveLimit ) == false )
                {
                    return nullptr;
                }

                return affector;
            }

        protected:
            FactoryPtr m_factory;
        };
        //////////////////////////////////////////////////////////////////////////
        FactoryAffectorFollowToW m_nodeAffectorCreatorFollowToW;
        //////////////////////////////////////////////////////////////////////////		
        uint32_t s_Node_followToW( Node * _node
            , const NodePtr & _target
            , const mt::vec3f & _offset
            , float _distance
            , float _moveSpeed
            , float _moveAcceleration
            , float _moveLimit
            , const pybind::object & _cb
            , const pybind::args & _args )
        {
            if( _node->isActivate() == false )
            {
                return 0;
            }

            if( _node->isAfterActive() == false )
            {
                return 0;
            }

            ScriptableAffectorCallbackPtr callback = createNodeAffectorCallback( _node, _cb, _args );

            AffectorPtr affector =
                m_nodeAffectorCreatorFollowToW.create( ETA_POSITION
                    , callback, _node, _target, _offset, _distance
                    , _moveSpeed, _moveAcceleration, _moveLimit
                );

            if( affector == nullptr )
            {
                return 0;
            }

            s_Node_moveStop( _node );

            if( _node->isActivate() == false )
            {
                return 0;
            }

            AFFECTOR_ID id = _node->addAffector( affector );

            return id;
        }
        //////////////////////////////////////////////////////////////////////////
        void s_Node_angleStop( const NodePtr & _node )
        {
            _node->stopAffectors( ETA_ANGLE );
            _node->setAngularSpeed( 0.f );
        }
        //////////////////////////////////////////////////////////////////////////
        NodeAffectorCreator::NodeAffectorCreatorInterpolateLinear<float> m_nodeAffectorCreatorInterpolateLinearFloat;
        //////////////////////////////////////////////////////////////////////////
        uint32_t s_Node_angleTo( Node * _node, float _time, float _angle, const pybind::object & _cb, const pybind::args & _args )
        {
            if( _node->isActivate() == false )
            {
                return 0;
            }

            if( _node->isAfterActive() == false )
            {
                return 0;
            }

            float angle = _node->getOrientationX();

            float correct_angle_from = angle;
            float correct_angle_to = _angle;
            //mt::angle_correct_interpolate_from_to( angle, _angle, correct_angle_from, correct_angle_to );

            ScriptableAffectorCallbackPtr callback = createNodeAffectorCallback( _node, _cb, _args );

            AffectorPtr affector = m_nodeAffectorCreatorInterpolateLinearFloat.create( ETA_ANGLE
                , callback
                , [_node]( float _v ) {_node->setOrientationX( _v ); }
                , correct_angle_from, correct_angle_to, _time
            );

            if( affector == nullptr )
            {
                return 0;
            }

            s_Node_angleStop( _node );

            if( _node->isActivate() == false )
            {
                return 0;
            }

            float invTime = 1.0f / _time;
            float angularSpeed = fabsf( correct_angle_from - correct_angle_to ) * invTime;

            _node->setAngularSpeed( angularSpeed );
            AFFECTOR_ID id = _node->addAffector( affector );

            return id;
        }
        //////////////////////////////////////////////////////////////////////////
        NodeAffectorCreator::NodeAffectorCreatorInterpolateQuadratic<float> m_nodeAffectorCreatorInterpolateQuadraticFloat;
        //////////////////////////////////////////////////////////////////////////
        uint32_t s_Node_accAngleTo( Node * _node, float _time, float _angle, const pybind::object & _cb, const pybind::args & _args )
        {
            if( _node->isActivate() == false )
            {
                return 0;
            }

            if( _node->isAfterActive() == false )
            {
                return 0;
            }

            float angularSpeed = _node->getAngularSpeed();

            float angle = _node->getOrientationX();

            float correct_angle_from = angle;
            float correct_angle_to = _angle;
            //mt::angle_correct_interpolate_from_to( angle, _angle, correct_angle_from, correct_angle_to );

            ScriptableAffectorCallbackPtr callback = createNodeAffectorCallback( _node, _cb, _args );

            AffectorPtr affector =
                m_nodeAffectorCreatorInterpolateQuadraticFloat.create( ETA_ANGLE
                    , callback
                    , [_node]( float _v ) {_node->setOrientationX( _v ); }
                    , correct_angle_from, correct_angle_to, angularSpeed, _time
                );

            if( affector == nullptr )
            {
                return 0;
            }

            s_Node_angleStop( _node );

            if( _node->isActivate() == false )
            {
                return 0;
            }

            AFFECTOR_ID id = _node->addAffector( affector );

            return id;
        }
        //////////////////////////////////////////////////////////////////////////
        void s_Node_scaleStop( Node * _node )
        {
            _node->stopAffectors( ETA_SCALE );
        }
        //////////////////////////////////////////////////////////////////////////
        uint32_t s_Node_scaleTo( Node * _node, float _time, const mt::vec3f& _scale, const pybind::object & _cb, const pybind::args & _args )
        {
            if( _node->isActivate() == false )
            {
                return 0;
            }

            if( _node->isAfterActive() == false )
            {
                return 0;
            }

            ScriptableAffectorCallbackPtr callback = createNodeAffectorCallback( _node, _cb, _args );

            AffectorPtr affector = m_nodeAffectorCreatorInterpolateLinear.create( ETA_SCALE
                , callback
                , [_node]( const mt::vec3f & _v ) { _node->setScale( _v ); }
                , _node->getScale(), _scale, _time
            );

            if( affector == nullptr )
            {
                return 0;
            }

            s_Node_scaleStop( _node );

            if( _node->isActivate() == false )
            {
                return 0;
            }

            AFFECTOR_ID id = _node->addAffector( affector );

            return id;
        }
        //////////////////////////////////////////////////////////////////////////
        void s_Node_colorStop( Node * _node )
        {
            _node->stopAffectors( ETA_COLOR );
        }
        //////////////////////////////////////////////////////////////////////////
        NodeAffectorCreator::NodeAffectorCreatorInterpolateLinear<ColourValue> m_nodeAffectorCreatorInterpolateLinearColour;
        //////////////////////////////////////////////////////////////////////////
        uint32_t s_Node_colorTo( Node * _node, float _time, const ColourValue& _color, const pybind::object & _cb, const pybind::args & _args )
        {
            if( _node->isActivate() == false )
            {
                return 0;
            }

            if( _node->isAfterActive() == false )
            {
                return 0;
            }

            ScriptableAffectorCallbackPtr callback = createNodeAffectorCallback( _node, _cb, _args );

            AffectorPtr affector =
                m_nodeAffectorCreatorInterpolateLinearColour.create( ETA_COLOR
                    , callback
                    , [_node]( const ColourValue & _v ) { _node->setLocalColor( _v ); }
                    , _node->getLocalColor(), _color, _time
                );

            if( affector == nullptr )
            {
                return 0;
            }

            s_Node_colorStop( _node );

            if( _node->isActivate() == false )
            {
                return 0;
            }

            AFFECTOR_ID id = _node->addAffector( affector );

            return id;
        }
        //////////////////////////////////////////////////////////////////////////
        uint32_t s_Node_alphaTo( Node * _node, float _time, float _alpha, const pybind::object & _cb, const pybind::args & _args )
        {
            if( _node->isActivate() == false )
            {
                return 0;
            }

            if( _node->isAfterActive() == false )
            {
                return 0;
            }

            ColourValue color = _node->getLocalColor();
            color.setA( _alpha );

            uint32_t id = s_Node_colorTo( _node, _time, color, _cb, _args );

            return id;
        }
    };
    //////////////////////////////////////////////////////////////////////////
    static void classWrapping()
    {
# define SCRIPT_CLASS_WRAPPING( Class )\
    SCRIPT_SERVICE()->setWrapper( Helper::stringizeString(#Class), new ScriptWrapper<Class>() )

        SCRIPT_CLASS_WRAPPING( Node );
        SCRIPT_CLASS_WRAPPING( Layer );
        SCRIPT_CLASS_WRAPPING( Layer2D );
        SCRIPT_CLASS_WRAPPING( HotSpot );
        SCRIPT_CLASS_WRAPPING( HotSpotPolygon );
        SCRIPT_CLASS_WRAPPING( HotSpotCircle );
        SCRIPT_CLASS_WRAPPING( HotSpotBubbles );
        SCRIPT_CLASS_WRAPPING( HotSpotImage );
        SCRIPT_CLASS_WRAPPING( HotSpotShape );

        SCRIPT_CLASS_WRAPPING( ScriptHolder );

        //SCRIPT_CLASS_WRAPPING( Light2D );
        //SCRIPT_CLASS_WRAPPING( ShadowCaster2D );
        SCRIPT_CLASS_WRAPPING( Gyroscope );
        SCRIPT_CLASS_WRAPPING( Isometric );
        SCRIPT_CLASS_WRAPPING( Arrow );
        SCRIPT_CLASS_WRAPPING( TextField );
        SCRIPT_CLASS_WRAPPING( SoundEmitter );
        SCRIPT_CLASS_WRAPPING( ParticleEmitter2 );
        SCRIPT_CLASS_WRAPPING( Movie );
        SCRIPT_CLASS_WRAPPING( MovieSlot );
        SCRIPT_CLASS_WRAPPING( MovieInternalObject );
        SCRIPT_CLASS_WRAPPING( Meshget );
        SCRIPT_CLASS_WRAPPING( Model3D );
        SCRIPT_CLASS_WRAPPING( Point );
        SCRIPT_CLASS_WRAPPING( Line );
        SCRIPT_CLASS_WRAPPING( Landscape2D );
        SCRIPT_CLASS_WRAPPING( Grid2D );

        SCRIPT_CLASS_WRAPPING( ShapeQuadFixed );
        SCRIPT_CLASS_WRAPPING( ShapeQuadFlex );

        //SCRIPT_CLASS_WRAPPING( FFCamera3D );
        //SCRIPT_CLASS_WRAPPING( DiscreteEntity );
        //SCRIPT_CLASS_WRAPPING( RigidBody3D );
        //SCRIPT_CLASS_WRAPPING( Layer3D );
        //SCRIPT_CLASS_WRAPPING( RigidBody2D );
        //SCRIPT_CLASS_WRAPPING( PhysicalBody2D );
        //SCRIPT_CLASS_WRAPPING( CapsuleController );
        //SCRIPT_CLASS_WRAPPING( SceneNode3D );
        //SCRIPT_CLASS_WRAPPING( Camera3D );
        //SCRIPT_CLASS_WRAPPING( RenderMesh );
        SCRIPT_CLASS_WRAPPING( Window );

        SCRIPT_CLASS_WRAPPING( Parallax );
        SCRIPT_CLASS_WRAPPING( RenderViewport );
        SCRIPT_CLASS_WRAPPING( RenderCameraOrthogonal );
        SCRIPT_CLASS_WRAPPING( RenderCameraProjection );
        SCRIPT_CLASS_WRAPPING( RenderCameraOrthogonalTarget );
        //SCRIPT_CLASS_WRAPPING( Layer2DTexture );

        SCRIPT_CLASS_WRAPPING( Resource );
        SCRIPT_CLASS_WRAPPING( ResourceImage );
        SCRIPT_CLASS_WRAPPING( ResourceImageData );
        SCRIPT_CLASS_WRAPPING( ResourceImageDefault );
        SCRIPT_CLASS_WRAPPING( ResourceAnimation );
        SCRIPT_CLASS_WRAPPING( ResourceMovie );
        SCRIPT_CLASS_WRAPPING( ResourceModel3D );
        SCRIPT_CLASS_WRAPPING( ResourceVideo );
        SCRIPT_CLASS_WRAPPING( ResourceSound );

        SCRIPT_CLASS_WRAPPING( ResourceImageSolid );
        SCRIPT_CLASS_WRAPPING( ResourceShape );
        SCRIPT_CLASS_WRAPPING( ResourceWindow );
        SCRIPT_CLASS_WRAPPING( ResourceImageSubstractRGBAndAlpha );
        SCRIPT_CLASS_WRAPPING( ResourceImageSubstract );
        SCRIPT_CLASS_WRAPPING( ResourceInternalObject );
        SCRIPT_CLASS_WRAPPING( ResourceHIT );

        SCRIPT_CLASS_WRAPPING( Surface );
        SCRIPT_CLASS_WRAPPING( SurfaceVideo );
        SCRIPT_CLASS_WRAPPING( SurfaceSound );
        SCRIPT_CLASS_WRAPPING( SurfaceImage );
        SCRIPT_CLASS_WRAPPING( SurfaceImageSequence );
        SCRIPT_CLASS_WRAPPING( SurfaceSolidColor );

# undef SCRIPT_CLASS_WRAPPING
    }

    void PythonScriptWrapper::nodeWrap()
    {
        NodeScriptMethod * nodeScriptMethod = new NodeScriptMethod();

        pybind::kernel_interface * kernel = pybind::get_kernel();

        classWrapping();

        pybind::interface_<Mixin>( kernel, "Mixin", true )
            .def_smart_pointer()
            ;

        pybind::interface_<Affector, pybind::bases<Mixin> >( kernel, "Affector", true )
            .def( "stop", &Affector::stop )
            .def( "getId", &Affector::getId )
            .def( "setFreeze", &Affector::setFreeze )
            .def( "getFreeze", &Affector::getFreeze )
            .def( "setSpeedFactor", &Affector::setSpeedFactor )
            .def( "getSpeedFactor", &Affector::getSpeedFactor )
            ;

        pybind::interface_<Scriptable, pybind::bases<Mixin> >( kernel, "Scriptable" )
            .def_bindable()
            ;

        pybind::interface_<Identity, pybind::bases<Mixin> >( kernel, "Identity" )
            .def( "setName", &Identity::setName )
            .def( "getName", &Identity::getName )
            .def( "getType", &Identity::getType )
            ;

        pybind::interface_<BoundingBox, pybind::bases<Mixin> >( kernel, "BoundingBox" )
            .def( "getBoundingBox", &BoundingBox::getBoundingBox )
            ;

        pybind::interface_<Transformation3D, pybind::bases<Mixin> >( kernel, "Transformation3D" )
            .def( "setLocalPosition", &Transformation3D::setLocalPosition )
            .def( "getLocalPosition", &Transformation3D::getLocalPosition )
            .def( "setLocalPositionX", &Transformation3D::setLocalPositionX )
            .def( "getLocalPositionX", &Transformation3D::getLocalPositionX )
            .def( "setLocalPositionY", &Transformation3D::setLocalPositionY )
            .def( "getLocalPositionY", &Transformation3D::getLocalPositionY )
            .def( "setLocalPositionZ", &Transformation3D::setLocalPositionZ )
            .def( "getLocalPositionZ", &Transformation3D::getLocalPositionZ )
            //.def( "getLocalDirection", &Transformation3D::getLocalDirection )
            //.def( "setLocalDirection", &Transformation3D::setLocalDirection )
            .def( "setOrigin", &Transformation3D::setOrigin )
            .def( "getOrigin", &Transformation3D::getOrigin )
            .def( "setScale", &Transformation3D::setScale )
            .def( "getScale", &Transformation3D::getScale )
            .def( "setSkew", &Transformation3D::setSkew )
            .def( "getSkew", &Transformation3D::getSkew )
            .def( "setOrientationX", &Transformation3D::setOrientationX )
            .def( "getOrientationX", &Transformation3D::getOrientationX )
            .def( "setOrientationY", &Transformation3D::setOrientationY )
            .def( "getOrientationY", &Transformation3D::getOrientationY )
            .def( "setOrientationZ", &Transformation3D::setOrientationZ )
            .def( "getOrientationZ", &Transformation3D::getOrientationZ )
            .def( "setOrientation", &Transformation3D::setOrientation )
            .def( "getOrientation", &Transformation3D::getOrientation )

            .def( "setAngle", &Transformation3D::setOrientationX )
            .def( "getAngle", &Transformation3D::getOrientationX )
            .def_proxy_static( "setAngleDeg", nodeScriptMethod, &NodeScriptMethod::s_Transformation3D_setAngleDeg )

            .def( "setDirection", &Transformation3D::setDirection )
            .def( "setBillboard", &Transformation3D::setBillboard )
            .def( "setAxes", &Transformation3D::setAxes )

            .def( "billboardAt", &Transformation3D::billboardAt )
            .def( "lookAt", &Transformation3D::lookAt )

            .def( "getAxisDirection", &Transformation3D::getAxisDirection )
            .def( "getAxisLeft", &Transformation3D::getAxisLeft )
            .def( "getAxisUp", &Transformation3D::getAxisUp )

            .def( "translate", &Transformation3D::translate )
            .def( "rotate", &Transformation3D::rotate )
            .def( "coordinate", &Transformation3D::coordinate )

            .def( "resetTransformation", &Transformation3D::resetTransformation )
            .def( "setRelationTransformation", &Transformation3D::setRelationTransformation )
            .def_proxy_static( "removeRelationTransformation", nodeScriptMethod, &NodeScriptMethod::s_Transformation3D_removeRelationTransformation )
            ;

        pybind::interface_<Compilable, pybind::bases<Mixin> >( kernel, "Compilable" )
            .def( "compile", &Compilable::compile )
            .def( "release", &Compilable::release )
            .def( "isCompile", &Compilable::isCompile )
            ;

        pybind::interface_<Reference, pybind::bases<Mixin> >( kernel, "Reference" )
            .def( "incrementReference", &Reference::incrementReference )
            .def( "decrementReference", &Reference::decrementReference )
            .def( "countReference", &Reference::countReference )
            ;

        pybind::interface_<Resource, pybind::bases<Scriptable, Compilable, Identity, Reference> >( kernel, "ResourceReference", false )
            .def_proxy_static( "getCategory", nodeScriptMethod, &NodeScriptMethod::s_Resource_getCategory )
            .def( "getGroup", &Resource::getGroupName )
            .def( "cache", &Resource::cache )
            .def( "uncache", &Resource::uncache )
            ;

        pybind::interface_<ResourceImage, pybind::bases<Resource> >( kernel, "ResourceImage", false )
            .def( "setMaxSize", &ResourceImage::setMaxSize )
            .def( "getMaxSize", &ResourceImage::getMaxSize )
            .def( "setSize", &ResourceImage::setSize )
            .def( "getSize", &ResourceImage::getSize )
            .def( "setOffset", &ResourceImage::setOffset )
            .def( "getOffset", &ResourceImage::getOffset )
            .def( "setUVImage", &ResourceImage::setUVImage )
            .def( "getUVImage", &ResourceImage::getUVImage )
            .def( "setUVAlpha", &ResourceImage::setUVAlpha )
            .def( "getUVAlpha", &ResourceImage::getUVAlpha )
            .def( "setAlpha", &ResourceImage::setAlpha )
            .def( "isAlpha", &ResourceImage::hasAlpha )
            .def( "setPremultiply", &ResourceImage::setPremultiply )
            .def( "isPremultiply", &ResourceImage::isPremultiply )
            .def( "setPow2", &ResourceImage::setPow2 )
            .def( "isPow2", &ResourceImage::isPow2 )
            .def( "setColor", &ResourceImage::setColor )
            .def( "getColor", &ResourceImage::getColor )
            ;

        pybind::interface_<ResourceImageData, pybind::bases<Resource> >( kernel, "ResourceImageData", false )
            .def( "getImagePath", &ResourceImageData::getImagePath )
            .def( "getCodecType", &ResourceImageData::getCodecType )
            .def( "getImageMaxSize", &ResourceImageData::getImageMaxSize )
            ;

        pybind::interface_<ResourceImageDefault, pybind::bases<ResourceImage> >( kernel, "ResourceImageDefault", false )
            .def( "getFilePath", &ResourceImageDefault::getFilePath )
            .def( "getCodecType", &ResourceImageDefault::getCodecType )
            ;

        pybind::interface_<ResourceImageSubstractRGBAndAlpha, pybind::bases<ResourceImage> >( kernel, "ResourceImageSubstractRGBAndAlpha", false )
            ;

        pybind::interface_<ResourceImageSubstract, pybind::bases<ResourceImage> >( kernel, "ResourceImageSubstract", false )
            ;

        pybind::interface_<ResourceImageSolid, pybind::bases<ResourceImage> >( kernel, "ResourceImageSolid", false )
            ;

        pybind::interface_<ResourceMovie, pybind::bases<Resource> >( kernel, "ResourceMovie", false )
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
            .def_proxy_static( "hasLayer", nodeScriptMethod, &NodeScriptMethod::s_ResourceMovie_hasLayer )
            .def_proxy_static( "hasLayerType", nodeScriptMethod, &NodeScriptMethod::s_ResourceMovie_hasLayerType )
            .def_proxy_static( "getLayerPosition", nodeScriptMethod, &NodeScriptMethod::s_ResourceMovie_getLayerPosition )
            .def_proxy_static( "getLayerIn", nodeScriptMethod, &NodeScriptMethod::s_ResourceMovie_getLayerIn )
            ;

        pybind::interface_<ResourceAnimation, pybind::bases<Resource> >( kernel, "ResourceAnimation", false )
            ;

        pybind::interface_<ResourceModel3D, pybind::bases<Resource> >( kernel, "ResourceModel3D", false )
            ;

        pybind::interface_<ResourceVideo, pybind::bases<Resource> >( kernel, "ResourceVideo", false )
            ;

        pybind::interface_<ResourceSound, pybind::bases<Resource> >( kernel, "ResourceSound", false )
            ;

        pybind::interface_<ResourceInternalObject, pybind::bases<Resource> >( kernel, "ResourceInternalObject", false )
            ;

        pybind::interface_<ResourceShape, pybind::bases<Resource> >( kernel, "ResourceShape", false )
            .def( "getPolygon", &ResourceShape::getPolygon )
            ;

        pybind::interface_<ResourceWindow, pybind::bases<Resource> >( kernel, "ResourceWindow", false )
            ;

        pybind::interface_<ResourceHIT, pybind::bases<Resource> >( kernel, "ResourceHIT", false )
            .def( "getWidth", &ResourceHIT::getWidth )
            .def( "getHeight", &ResourceHIT::getHeight )
            ;

        pybind::interface_<Renderable, pybind::bases<Mixin> >( kernel, "Renderable" )
            .def( "hide", &Renderable::setHide )
            .def( "isHide", &Renderable::getHide )
            .def( "localHide", &Renderable::setLocalHide )
            .def( "isLocalHide", &Renderable::getLocalHide )
            ;

        pybind::interface_<Colorable, pybind::bases<Mixin> >( kernel, "Colorable" )
            .def( "setLocalColor", &Colorable::setLocalColor )
            .def( "getLocalColor", &Colorable::getLocalColor )
            .def( "setLocalColorR", &Colorable::setLocalColorR )
            .def( "getLocalColorR", &Colorable::getLocalColorR )
            .def( "setLocalColorG", &Colorable::setLocalColorG )
            .def( "getLocalColorG", &Colorable::getLocalColorG )
            .def( "setLocalColorB", &Colorable::setLocalColorB )
            .def( "getLocalColorB", &Colorable::getLocalColorB )
            .def( "setLocalColorRGB", &Colorable::setLocalColorRGB )
            .def( "setLocalAlpha", &Colorable::setLocalAlpha )
            .def( "getLocalAlpha", &Colorable::getLocalAlpha )
            .def( "setPersonalColor", &Colorable::setPersonalColor )
            .def( "getPersonalColor", &Colorable::getPersonalColor )
            .def( "setPersonalAlpha", &Colorable::setPersonalAlpha )
            .def( "getPersonalAlpha", &Colorable::getPersonalAlpha )
            ;

        pybind::interface_<Animatable, pybind::bases<Mixin> >( kernel, "Animatable" )
            .def_proxy_static( "play", nodeScriptMethod, &NodeScriptMethod::s_Animatable_play )
            .def( "stop", &Animatable::stop )
            .def( "pause", &Animatable::pause )
            .def_proxy_static( "resume", nodeScriptMethod, &NodeScriptMethod::s_Animatable_resume )
            .def( "interrupt", &Animatable::interrupt )
            .def( "isInterrupt", &Animatable::isInterrupt )
            .def( "isPlay", &Animatable::isPlay )
            .def( "getPlayId", &Animatable::getPlayId )
            .def( "setAnimationSpeedFactor", &Animatable::setAnimationSpeedFactor )
            .def( "getAnimationSpeedFactor", &Animatable::getAnimationSpeedFactor )
            .def( "setFirstFrame", &Animatable::setFirstFrame )
            .def( "setLastFrame", &Animatable::setLastFrame )
            .def( "setTiming", &Animatable::setTiming )
            .def( "getTiming", &Animatable::getTiming )
            .def( "setPlayCount", &Animatable::setPlayCount )
            .def( "getPlayCount", &Animatable::getPlayCount )
            .def( "getPlayIterator", &Animatable::getPlayIterator )
            .def( "setAutoPlay", &Animatable::setAutoPlay )
            .def( "getAutoPlay", &Animatable::getAutoPlay )
            .def( "setLoop", &Animatable::setLoop )
            .def( "getLoop", &Animatable::getLoop )
            .def( "setInterval", &Animatable::setInterval )
            .def( "getIntervalBegin", &Animatable::getIntervalBegin )
            .def( "getIntervalEnd", &Animatable::getIntervalEnd )
            .def( "setIntervalStart", &Animatable::setIntervalStart )
            .def( "getIntervalStart", &Animatable::getIntervalStart )
            ;

        pybind::interface_<Eventable, pybind::bases<Mixin> >( kernel, "Eventable" )
            ;

        pybind::interface_<Soundable, pybind::bases<Mixin> >( kernel, "Soundable" )
            .def( "setVolume", &Soundable::setVolume )
            .def( "getVolume", &Soundable::getVolume )
            ;

        pybind::interface_<Affectorable, pybind::bases<Mixin> >( kernel, "Affectorable" )
            .def( "addAffector", &Affectorable::addAffector )
            .def( "stopAffector", &Affectorable::stopAffector )
            .def( "stopAllAffectors", &Affectorable::stopAllAffectors )
            .def( "setLinearSpeed", &Affectorable::setLinearSpeed )
            .def( "getLinearSpeed", &Affectorable::getLinearSpeed )
            ;

        pybind::interface_<Materialable, pybind::bases<Mixin> >( kernel, "Materialable", false )
            .def( "setMaterialName", &Materialable::setMaterialName )
            .def( "getMaterialName", &Materialable::getMaterialName )
            .def( "setDisableTextureColor", &Materialable::setDisableTextureColor )
            .def( "getDisableTextureColor", &Materialable::getDisableTextureColor )
            .def( "setBlendMode", &Materialable::setBlendMode )
            .def( "getBlendMode", &Materialable::getBlendMode )
            ;

        pybind::interface_<Node, pybind::bases<Scriptable, Identity, Transformation3D, BoundingBox, Colorable, Compilable, Renderable, Affectorable> >( kernel, "Node", false )
            .def( "enable", &Node::enable )
            .def( "disable", &Node::disable )
            .def( "isEnable", &Node::isEnable )
            .def( "render", &Node::render )
            .def( "isActivate", &Node::isActivate )
            .def( "freeze", &Node::freeze )
            .def( "isFreeze", &Node::isFreeze )
            .def( "setSpeedFactor", &Node::setSpeedFactor )
            .def( "getSpeedFactor", &Node::getSpeedFactor )
            .def( "isRenderable", &Node::isRenderable )
            .def( "addChildren", &Node::addChild )
            .def( "addChildrenFront", &Node::addChildFront )
            .def( "addChildrenAfter", &Node::addChildAfter )
            .def( "removeChildren", &Node::removeChild )
            .def( "removeAllChild", &Node::removeChildren )
            .def( "removeFromParent", &Node::removeFromParent )
            .def( "destroyAllChild", &Node::destroyAllChild )
            .def( "isHomeless", &Node::isHomeless )
            //.def_static( "getChild", &ScriptMethod::s_getChild )
            .def( "findChildren", &Node::findChild )
            .def( "getSiblingPrev", &Node::getSiblingPrev )
            .def( "getSiblingNext", &Node::getSiblingNext )
            .def( "emptyChild", &Node::emptyChildren )
            .def( "hasChildren", &Node::hasChild )
            .def( "update", &Node::update )
            .def( "getParent", &Node::getParent )
            .def( "hasParent", &Node::hasParent )

            .def( "getWorldPosition", &Node::getWorldPosition )
            .def( "setWorldPosition", &Node::setWorldPosition )
            //.def( "getWorldDirection", &Node::getWorldDirection )
            .def_proxy_static( "getWorldOffsetPosition", nodeScriptMethod, &NodeScriptMethod::s_Node_getWorldOffsetPosition )
            .def_proxy_static( "getLengthTo", nodeScriptMethod, &NodeScriptMethod::s_Node_getLengthTo )

            .def( "getWorldColor", &Node::getWorldColor )

            .def( "setRenderViewport", &Node::setRenderViewport )
            .def( "getRenderViewport", &Node::getRenderViewport )
            .def( "getRenderViewportInheritance", &Node::getRenderViewportInheritance )

            .def( "setRenderCamera", &Node::setRenderCamera )
            .def( "getRenderCamera", &Node::getRenderCamera )
            .def( "getRenderCameraInheritance", &Node::getRenderCameraInheritance )

            .def( "setRenderScissor", &Node::setRenderScissor )
            .def( "getRenderScissor", &Node::getRenderScissor )
            .def( "getRenderScissorInheritance", &Node::getRenderScissorInheritance )

            .def( "setRenderTarget", &Node::setRenderTarget )
            .def( "getRenderTarget", &Node::getRenderTarget )
            .def( "getRenderTargetInheritance", &Node::getRenderTargetInheritance )

            .def_proxy_static( "createChildren", nodeScriptMethod, &NodeScriptMethod::s_Node_createChildren )
            .def_proxy_static_kernel( "getAllChildren", nodeScriptMethod, &NodeScriptMethod::s_Node_getAllChildren )

            .def_proxy_args_static( "colorTo", nodeScriptMethod, &NodeScriptMethod::s_Node_colorTo )
            .def_proxy_args_static( "alphaTo", nodeScriptMethod, &NodeScriptMethod::s_Node_alphaTo )
            .def_proxy_static( "colorStop", nodeScriptMethod, &NodeScriptMethod::s_Node_colorStop )

            .def_proxy_args_static( "velocityTo", nodeScriptMethod, &NodeScriptMethod::s_Node_velocityTo )
            .def_proxy_args_static( "velocityTo2", nodeScriptMethod, &NodeScriptMethod::s_Node_velocityTo2 )

            .def_proxy_args_static( "moveTo", nodeScriptMethod, &NodeScriptMethod::s_Node_moveTo )
            .def_proxy_args_static( "bezier2To", nodeScriptMethod, &NodeScriptMethod::s_Node_bezier2To )
            .def_proxy_args_static( "bezier3To", nodeScriptMethod, &NodeScriptMethod::s_Node_bezier3To )
            .def_proxy_args_static( "bezier4To", nodeScriptMethod, &NodeScriptMethod::s_Node_bezier4To )
            .def_proxy_args_static( "parabolaTo", nodeScriptMethod, &NodeScriptMethod::s_Node_parabolaTo )
            .def_proxy_args_static( "followTo", nodeScriptMethod, &NodeScriptMethod::s_Node_followTo )
            .def_proxy_args_static( "followToW", nodeScriptMethod, &NodeScriptMethod::s_Node_followToW )
            .def_proxy_static( "moveStop", nodeScriptMethod, &NodeScriptMethod::s_Node_moveStop )

            .def_proxy_args_static( "angleTo", nodeScriptMethod, &NodeScriptMethod::s_Node_angleTo )
            .def_proxy_static( "angleStop", nodeScriptMethod, &NodeScriptMethod::s_Node_angleStop )
            .def_proxy_args_static( "scaleTo", nodeScriptMethod, &NodeScriptMethod::s_Node_scaleTo )
            .def_proxy_static( "scaleStop", nodeScriptMethod, &NodeScriptMethod::s_Node_scaleStop )

            .def_proxy_args_static( "accMoveTo", nodeScriptMethod, &NodeScriptMethod::s_Node_accMoveTo )
            .def_proxy_args_static( "accAngleTo", nodeScriptMethod, &NodeScriptMethod::s_Node_accAngleTo )
            ;

        pybind::interface_<Surface, pybind::bases<Scriptable, Identity, Materialable, Compilable> >( kernel, "Surface", false )
            .def( "getMaxSize", &Surface::getMaxSize )
            .def( "getSize", &Surface::getSize )
            .def( "getOffset", &Surface::getOffset )
            .def( "getUVCount", &Surface::getUVCount )
            .def( "getUV", &Surface::getUV )
            .def( "getColour", &Surface::getColor )
            ;

        pybind::interface_<SurfaceVideo, pybind::bases<Surface, Eventable, Animatable> >( kernel, "SurfaceVideo", false )
            .def( "setResourceVideo", &SurfaceVideo::setResourceVideo )
            .def( "getResourceVideo", &SurfaceVideo::getResourceVideo )
            .def( "getWidth", &SurfaceVideo::getWidth )
            .def( "getHeight", &SurfaceVideo::getHeight )
            .def( "getDuration", &SurfaceVideo::getDuration )
            .def_proxy_native_kernel( "setEventListener", nodeScriptMethod, &NodeScriptMethod::s_SurfaceVideo_setEventListener )
            ;

        pybind::interface_<SurfaceSound, pybind::bases<Surface, Eventable, Animatable, Soundable> >( kernel, "SurfaceSound", false )
            .def( "setResourceSound", &SurfaceSound::setResourceSound )
            .def( "getResourceSound", &SurfaceSound::getResourceSound )
            .def( "getDuration", &SurfaceSound::getDuration )
            .def_proxy_native_kernel( "setEventListener", nodeScriptMethod, &NodeScriptMethod::s_SurfaceSound_setEventListener )
            ;

        pybind::interface_<SurfaceImage, pybind::bases<Surface> >( kernel, "SurfaceImage", false )
            .def( "setResourceImage", &SurfaceImage::setResourceImage )
            .def( "getResourceImage", &SurfaceImage::getResourceImage )
            ;

        pybind::interface_<SurfaceImageSequence, pybind::bases<Surface, Eventable, Animatable> >( kernel, "SurfaceImageSequence", false )
            .def( "setResourceAnimation", &SurfaceImageSequence::setResourceAnimation )
            .def( "getResourceAnimation", &SurfaceImageSequence::getResourceAnimation )
            .def( "getFrameCount", &SurfaceImageSequence::getFrameCount )
            .def( "getFrameDelay", &SurfaceImageSequence::getFrameDelay )
            .def( "setCurrentFrame", &SurfaceImageSequence::setCurrentFrame )
            .def( "getCurrentFrame", &SurfaceImageSequence::getCurrentFrame )
            .def_proxy_native_kernel( "setEventListener", nodeScriptMethod, &NodeScriptMethod::s_SurfaceImageSequence_setEventListener )
            ;

        pybind::interface_<SurfaceSolidColor, pybind::bases<Surface> >( kernel, "SurfaceSolidColor", false )
            .def( "setSolidColor", &SurfaceSolidColor::setSolidColor )
            .def( "getSolidColor", &SurfaceSolidColor::getSolidColor )
            .def( "setSolidSize", &SurfaceSolidColor::setSolidSize )
            .def( "getSolidSize", &SurfaceSolidColor::getSolidSize )
            ;


        pybind::interface_<ThreadTask, pybind::bases<Mixin> >( kernel, "Task" )
            ;

        pybind::interface_<RenderViewportInterface, pybind::bases<Mixin> >( kernel, "RenderViewportInterface" )
            .def( "getViewport", &RenderViewportInterface::getViewport )
            ;

        pybind::interface_<RenderViewport, pybind::bases<Node, RenderViewportInterface> >( kernel, "RenderViewport", false )
            .def( "setFixedViewport", &RenderViewport::setFixedViewport )
            .def( "getFixedViewport", &RenderViewport::getFixedViewport )
            .def( "setViewport", &RenderViewport::setViewport )
            ;

        pybind::interface_<RenderCameraInterface, pybind::bases<Mixin> >( kernel, "RenderCameraInterface" )
            ;

        pybind::interface_<RenderCameraOrthogonal, pybind::bases<Node, RenderCameraInterface> >( kernel, "RenderCameraOrthogonal", false )
            .def( "setCameraPosition", &RenderCameraOrthogonal::setCameraPosition )
            .def( "setCameraDirection", &RenderCameraOrthogonal::setCameraDirection )
            .def( "setCameraUp", &RenderCameraOrthogonal::setCameraUp )
            .def( "setCameraRightSign", &RenderCameraOrthogonal::setCameraRightSign )
            .def( "setCameraNear", &RenderCameraOrthogonal::setCameraNear )
            .def( "setCameraFar", &RenderCameraOrthogonal::setCameraFar )
            .def( "setOrthogonalViewport", &RenderCameraOrthogonal::setOrthogonalViewport )
            .def( "getOrthogonalViewport", &RenderCameraOrthogonal::getOrthogonalViewport )
            .def( "setFixedOrthogonalViewport", &RenderCameraOrthogonal::setFixedOrthogonalViewport )
            .def( "getFixedOrthogonalViewport", &RenderCameraOrthogonal::getFixedOrthogonalViewport )
            .def( "setProxyViewMatrix", &RenderCameraOrthogonal::setProxyViewMatrix )
            .def( "getProxyViewMatrix", &RenderCameraOrthogonal::getProxyViewMatrix )
            ;

        pybind::interface_<RenderCameraProjection, pybind::bases<Node, RenderCameraInterface> >( kernel, "RenderCameraProjection", false )
            .def( "setCameraPosition", &RenderCameraProjection::setCameraPosition )
            .def( "setCameraDirection", &RenderCameraProjection::setCameraDirection )
            .def( "setCameraUp", &RenderCameraProjection::setCameraUp )
            .def( "setCameraRightSign", &RenderCameraProjection::setCameraRightSign )
            .def( "setCameraFOV", &RenderCameraProjection::setCameraFOV )
            .def( "setCameraAspect", &RenderCameraProjection::setCameraAspect )
            .def( "setCameraNear", &RenderCameraProjection::setCameraNear )
            .def( "setCameraFar", &RenderCameraProjection::setCameraFar )
            ;

        pybind::interface_<RenderCameraOrthogonalTarget, pybind::bases<Node> >( kernel, "RenderCameraOrthogonalTarget", false )
            .def( "setRenderCameraOrthogonal", &RenderCameraOrthogonalTarget::setRenderCameraOrthogonal )
            .def( "getRenderCameraOrthogonal", &RenderCameraOrthogonalTarget::getRenderCameraOrthogonal )
            .def( "setSpeed", &RenderCameraOrthogonalTarget::setSpeed )
            .def( "getSpeed", &RenderCameraOrthogonalTarget::getSpeed )
            .def( "setFixedHorizont", &RenderCameraOrthogonalTarget::setFixedHorizont )
            .def( "getFixedHorizont", &RenderCameraOrthogonalTarget::getFixedHorizont )
            .def( "isFixedHorizont", &RenderCameraOrthogonalTarget::isFixedHorizont )
            ;


        pybind::interface_<RenderScissorInterface, pybind::bases<Mixin> >( kernel, "RenderScissorInterface", false )
            .def( "getScissorRect", &RenderScissorInterface::getScissorViewport )
            ;

        pybind::interface_<RenderScissor, pybind::bases<Node, RenderScissorInterface> >( kernel, "RenderScissorInterface", false )
            ;

        pybind::interface_<RenderTargetInterface, pybind::bases<Mixin> >( kernel, "RenderTargetInterface", false )
            .def( "getWidth", &RenderTargetInterface::getWidth )
            .def( "getHeight", &RenderTargetInterface::getHeight )
            ;

        {
            pybind::interface_<ParticleEmitter2, pybind::bases<Node, Eventable, Animatable> >( kernel, "ParticleEmitter2", false )
                .def( "setResourceParticle", &ParticleEmitter2::setResourceParticle )
                .def( "getResourceParticle", &ParticleEmitter2::getResourceParticle )

                .def( "setEmitterTranslateWithParticle", &ParticleEmitter2::setEmitterTranslateWithParticle )
                .def( "setEmitterPositionRelative", &ParticleEmitter2::setEmitterPositionRelative )
                .def( "setEmitterCameraRelative", &ParticleEmitter2::setEmitterCameraRelative )
                .def( "setEmitterPositionProviderOriginOffset", &ParticleEmitter2::setEmitterPositionProviderOriginOffset )

                .def( "changeEmitterImage", &ParticleEmitter2::changeEmitterImage )
                .def( "removeEmitterImage", &ParticleEmitter2::removeEmitterImage )
                .def( "changeEmitterPolygon", &ParticleEmitter2::changeEmitterPolygon )
                .def( "removeEmitterPolygon", &ParticleEmitter2::removeEmitterPolygon )

                .def( "getDuration", &ParticleEmitter2::getDuration )

                .def( "setEmitterRandomMode", &ParticleEmitter2::setEmitterRandomMode )
                .def( "getEmitterRandomMode", &ParticleEmitter2::getEmitterRandomMode )
                .def_proxy_native_kernel( "setEventListener", nodeScriptMethod, &NodeScriptMethod::s_ParticleEmitter2_setEventListener )
                ;

            pybind::interface_<SoundEmitter, pybind::bases<Node> >( kernel, "SoundEmitter", false )
                .def( "setSurfaceSound", &SoundEmitter::setSurfaceSound )
                .def( "getSurfaceSound", &SoundEmitter::getSurfaceSound )
                ;

            pybind::interface_<TextField, pybind::bases<Node> >( kernel, "TextField", false )
                .def_deprecated( "setTextByKey", &TextField::setTextID, "use setTextID" )
                .def( "setTextID", &TextField::setTextID )
                .def( "removeTextID", &TextField::removeTextID )
                .def_proxy_native_kernel( "setTextFormatArgs", nodeScriptMethod, &NodeScriptMethod::s_TextField_setTextFormatArgs )
                .def_proxy_static( "getTextFormatArgs", nodeScriptMethod, &NodeScriptMethod::s_TextField_getTextFormatArgs )
                .def( "removeTextFormatArgs", &TextField::removeTextFormatArgs )
                .def_deprecated( "getTextKey", &TextField::getTextID, "use getTextID" )
                .def( "getTextID", &TextField::getTextID )
                .def( "getTextExpectedArgument", &TextField::getTextExpectedArgument )

                .def_deprecated( "getHeight", &TextField::getFontHeight, "use getFontHeight" )
                .def_deprecated( "getAlphaHeight", &TextField::getFontHeight, "use getFontHeight" )
                .def( "getFontHeight", &TextField::getFontHeight )
                .def( "setFontName", &TextField::setFontName )
                .def( "getFontName", &TextField::getFontName )

                .def( "setWrap", &TextField::setWrap )
                .def( "getWrap", &TextField::getWrap )

                .def( "setFontColor", &TextField::setFontColor )
                .def( "getFontColor", &TextField::getFontColor )

                .def( "setLineOffset", &TextField::setLineOffset )
                .def( "getLineOffset", &TextField::getLineOffset )

                .def( "setCharOffset", &TextField::setCharOffset )
                .def( "getCharOffset", &TextField::getCharOffset )

                .def_deprecated( "getLength", &TextField::getTextSize, "use getTextSize" )
                .def( "getTextSize", &TextField::getTextSize )
                .def_deprecated( "setMaxLen", &TextField::setMaxLength, "use setMaxLength" )
                .def( "setMaxLength", &TextField::setMaxLength )
                .def( "getMaxLength", &TextField::getMaxLength )

                .def_deprecated( "setCenterAlign", &TextField::setHorizontalCenterAlign, "use setHorizontalCenterAlign" )
                .def_deprecated( "isCenterAlign", &TextField::isHorizontalCenterAlign, "use isHorizontalCenterAlign" )
                .def_deprecated( "setRightAlign", &TextField::setHorizontalRightAlign, "use setHorizontalRightAlign" )
                .def_deprecated( "isRightAlign", &TextField::isHorizontalRightAlign, "use isHorizontalRightAlign" )
                .def_deprecated( "setLeftAlign", &TextField::setHorizontalLeftAlign, "use setHorizontalLeftAlign" )
                .def_deprecated( "isLeftAlign", &TextField::isHorizontalLeftAlign, "use isHorizontalLeftAlign" )

                .def( "setHorizontalCenterAlign", &TextField::setHorizontalCenterAlign )
                .def( "isHorizontalCenterAlign", &TextField::isHorizontalCenterAlign )
                .def( "setHorizontalRightAlign", &TextField::setHorizontalRightAlign )
                .def( "isHorizontalRightAlign", &TextField::isHorizontalRightAlign )
                .def( "setHorizontalLeftAlign", &TextField::setHorizontalLeftAlign )
                .def( "isHorizontalLeftAlign", &TextField::isHorizontalLeftAlign )

                .def( "setVerticalBottomAlign", &TextField::setVerticalBottomAlign )
                .def( "isVerticalBottomAlign", &TextField::isVerticalBottomAlign )
                .def( "setVerticalCenterAlign", &TextField::setVerticalCenterAlign )
                .def( "isVerticalCenterAlign", &TextField::isVerticalCenterAlign )

                .def( "setPixelsnap", &TextField::setPixelsnap )
                .def( "getPixelsnap", &TextField::getPixelsnap )

                .def( "setMaxCharCount", &TextField::setMaxCharCount )
                .def( "getMaxCharCount", &TextField::getMaxCharCount )
                .def( "getCharCount", &TextField::getCharCount )
                ;

            pybind::interface_<ScriptHolder, pybind::bases<Node, Eventable> >( kernel, "ScriptHolder", false )
                .def_proxy_native_kernel( "setEventListener", nodeScriptMethod, &NodeScriptMethod::s_ScriptHolder_setEventListener )
                ;

            pybind::interface_<Point, pybind::bases<Node> >( kernel, "Point", false )
                .def( "setLinkedPoint", &Point::setLinkedPoint )
                .def( "removeLinkedPoint", &Point::removeLinkedPoint )
                .def( "getLinkedPoint", &Point::getLinkedPoint )
                .def( "setWidth", &Point::setWidth )
                .def( "getWidth", &Point::getWidth )
                ;

            pybind::interface_<Line, pybind::bases<Node> >( kernel, "Line", false )
                .def( "setFrom", &Line::setFrom )
                .def( "getFrom", &Line::getFrom )
                .def( "setTo", &Line::setTo )
                .def( "getTo", &Line::getTo )
                .def( "setWidth", &Line::setWidth )
                .def( "getWidth", &Line::getWidth )
                ;

            pybind::interface_<Layer, pybind::bases<Node> >( kernel, "Layer", false )
                ;

            pybind::interface_<Layer2D, pybind::bases<Layer> >( kernel, "Layer2D", false )
                .def( "setSize", &Layer2D::setSize )
                .def( "getSize", &Layer2D::getSize )
                .def( "setViewport", &Layer2D::setViewport )
                .def( "removeViewport", &Layer2D::removeViewport )
                .def( "setImageMask", &Layer2D::setImageMask )
                .def( "removeImageMask", &Layer2D::removeImageMask )
                ;

            pybind::interface_<Parallax, pybind::bases<Node> >( kernel, "Parallax", false )
                .def( "setParallaxFactor", &Parallax::setParallaxFactor )
                .def( "getParallaxFactor", &Parallax::getParallaxFactor )
                ;

            pybind::interface_<MousePickerTrapInterface, pybind::bases<Mixin> >( kernel, "MousePickerTrap", false )
                .def( "pick", &MousePickerTrapInterface::pick )
                ;

            pybind::interface_<HotSpot, pybind::bases<Node, Eventable> >( kernel, "HotSpot", false )
                .def( "testPoint", &HotSpot::testPoint )
                .def( "setOutward", &HotSpot::setOutward )
                .def( "getOutward", &HotSpot::getOutward )
                .def( "setGlobal", &HotSpotPolygon::setGlobal )
                .def( "getGlobal", &HotSpotPolygon::getGlobal )
                .def( "setDefaultHandle", &HotSpot::setDefaultHandle )
                .def( "getDefaultHandle", &HotSpot::getDefaultHandle )
                .def( "isMousePickerOver", &HotSpot::isMousePickerOver )
                .def_proxy_native_kernel( "setEventListener", nodeScriptMethod, &NodeScriptMethod::s_HotSpot_setEventListener )
                .def_proxy_native( "removeEventListener", nodeScriptMethod, &NodeScriptMethod::s_HotSpot_removeEventListener )
                ;

            pybind::interface_<HotSpotPolygon, pybind::bases<HotSpot> >( kernel, "HotSpotPolygon", false )
                .def( "setPolygon", &HotSpotPolygon::setPolygon )
                .def( "getPolygon", &HotSpotPolygon::getPolygon )
                .def( "clearPoints", &HotSpotPolygon::clearPoints )
                .def_proxy_static( "getLocalPolygonCenter", nodeScriptMethod, &NodeScriptMethod::s_HotSpotPolygon_getLocalPolygonCenter )
                .def_proxy_static( "getWorldPolygonCenter", nodeScriptMethod, &NodeScriptMethod::s_HotSpotPolygon_getWorldPolygonCenter )
                .def_proxy_static( "getScreenPolygonCenter", nodeScriptMethod, &NodeScriptMethod::s_HotSpotPolygon_getScreenPolygonCenter )
                .def_proxy_static( "getWorldPolygon", nodeScriptMethod, &NodeScriptMethod::s_HotSpotPolygon_getWorldPolygon )
                ;

            pybind::interface_<HotSpotShape, pybind::bases<HotSpotPolygon> >( kernel, "HotSpotShape", false )
                .def( "setResourceShape", &HotSpotShape::setResourceShape )
                .def( "getResourceShape", &HotSpotShape::getResourceShape )
                ;

            pybind::interface_<HotSpotImage, pybind::bases<HotSpot> >( kernel, "HotSpotImage", false )
                .def( "setResourceHIT", &HotSpotImage::setResourceHIT )
                .def( "getResourceHIT", &HotSpotImage::getResourceHIT )
                .def( "setAlphaTest", &HotSpotImage::setAlphaTest )
                .def( "getAlphaTest", &HotSpotImage::getAlphaTest )
                .def( "getWidth", &HotSpotImage::getWidth )
                .def( "getHeight", &HotSpotImage::getHeight )
                ;

            pybind::interface_<HotSpotCircle, pybind::bases<HotSpot> >( kernel, "HotSpotCircle", false )
                .def( "setRadius", &HotSpotCircle::setRadius )
                .def( "getRadius", &HotSpotCircle::getRadius )
                .def( "setEllipse", &HotSpotCircle::setEllipse )
                .def( "getEllipse", &HotSpotCircle::getEllipse )
                ;

            pybind::interface_<HotSpotBubbles, pybind::bases<HotSpot> >( kernel, "HotSpotBubbles", false )
                .def( "addBubble", &HotSpotBubbles::addBubble )
                ;

            pybind::interface_<Shape, pybind::bases<Node> >( kernel, "Shape", false )
                .def( "setSurface", &Shape::setSurface )
                .def( "getSurface", &Shape::getSurface )

                .def_proxy_static( "getSurfaceSize", nodeScriptMethod, &NodeScriptMethod::s_Shape_getSurfaceSize )
                .def_proxy_static( "getLocalImageCenter", nodeScriptMethod, &NodeScriptMethod::s_Shape_getLocalImageCenter )
                .def_proxy_static( "getWorldImageCenter", nodeScriptMethod, &NodeScriptMethod::s_Shape_getWorldImageCenter )
                ;

            pybind::interface_<ShapeQuad, pybind::bases<Shape> >( kernel, "ShapeQuad", false )
                ;

            pybind::interface_<ShapeQuadFixed, pybind::bases<ShapeQuad> >( kernel, "ShapeQuadFixed", false )
                ;

            pybind::interface_<ShapeQuadFlex, pybind::bases<ShapeQuad> >( kernel, "ShapeQuadFlex", false )
                .def( "setCenterAlign", &ShapeQuadFlex::setCenterAlign )
                .def( "getCenterAlign", &ShapeQuadFlex::getCenterAlign )
                .def( "setFlipX", &ShapeQuadFlex::setFlipX )
                .def( "getFlipX", &ShapeQuadFlex::getFlipX )
                .def( "setFlipY", &ShapeQuadFlex::setFlipY )
                .def( "getFlipY", &ShapeQuadFlex::getFlipY )
                .def( "setPercentVisibility", &ShapeQuadFlex::setPercentVisibility )
                .def( "getPercentVisibility", &ShapeQuadFlex::getPercentVisibility )
                .def( "setCustomSize", &ShapeQuadFlex::setCustomSize )
                .def( "getCustomSize", &ShapeQuadFlex::getCustomSize )
                .def( "removeCustomSize", &ShapeQuadFlex::removeCustomSize )
                .def( "hasCustomSize", &ShapeQuadFlex::hasCustomSize )
                .def_proxy_static( "setPercentVisibilityTo", nodeScriptMethod, &NodeScriptMethod::s_ShapeQuadFlex_setPercentVisibilityTo )
                .def_proxy_static( "setPercentVisibilityStop", nodeScriptMethod, &NodeScriptMethod::s_ShapeQuadFlex_setPercentVisibilityStop )
                .def( "setTextureUVOffset", &ShapeQuadFlex::setTextureUVOffset )
                .def( "getTextureUVOffset", &ShapeQuadFlex::getTextureUVOffset )
                .def( "setTextureUVScale", &ShapeQuadFlex::setTextureUVScale )
                .def( "getTextureUVScale", &ShapeQuadFlex::getTextureUVScale )
                ;

            pybind::interface_<Landscape2D, pybind::bases<Node, Materialable> >( kernel, "Landscape2D", false )
                .def( "setBackParts", &Landscape2D::setBackParts )
                ;

            pybind::interface_<Grid2D, pybind::bases<Node, Materialable> >( kernel, "Grid2D", false )
                .def( "setResourceImage", &Grid2D::setResourceImage )
                .def( "getResourceImage", &Grid2D::getResourceImage )
                .def( "setWidth", &Grid2D::setWidth )
                .def( "getWidth", &Grid2D::getWidth )
                .def( "setHeight", &Grid2D::setHeight )
                .def( "getHeight", &Grid2D::getHeight )
                .def( "setCountX", &Grid2D::setCountX )
                .def( "getCountX", &Grid2D::getCountX )
                .def( "setCountY", &Grid2D::setCountY )
                .def( "getCountY", &Grid2D::getCountY )
                .def( "setAngle", &Grid2D::setAngle )
                ;

            pybind::interface_<Gyroscope, pybind::bases<Node> >( kernel, "Gyroscope", false )
                ;

            pybind::interface_<Isometric, pybind::bases<Node> >( kernel, "Isometric", false )
                ;


            {
                pybind::interface_<Model3D, pybind::bases<Node, Animatable, Materialable> >( kernel, "Model3D", false )
                    .def( "setResourceModel3D", &Model3D::setResourceModel3D )
                    .def( "getResourceModel3D", &Model3D::getResourceModel3D )
                    ;
            }

            pybind::interface_<Movie, pybind::bases<Node, Eventable, Animatable> >( kernel, "Movie", false )
                .def( "setResourceMovie", &Movie::setResourceMovie )
                .def( "getResourceMovie", &Movie::getResourceMovie )
                .def( "hasMovieLayer", &Movie::hasMovieLayer )
                .def( "setEnableMovieLayer", &Movie::setEnableMovieLayer )
                .def( "setEnableMovieLayers", &Movie::setEnableMovieLayers )
                .def_proxy_static( "getWorldAnchorPoint", nodeScriptMethod, &NodeScriptMethod::movie_getWorldAnchorPoint )
                .def_proxy_static_kernel( "getEnableMovieLayer", nodeScriptMethod, &NodeScriptMethod::movie_getEnableMovieLayer )
                .def_proxy_static( "getMovieSlot", nodeScriptMethod, &NodeScriptMethod::movie_getMovieSlot )
                .def_proxy_static( "hasMovieSlot", nodeScriptMethod, &NodeScriptMethod::movie_hasMovieSlot )
                .def_proxy_static( "getMovieText", nodeScriptMethod, &NodeScriptMethod::movie_getMovieText )
                .def_proxy_static( "hasMovieText", nodeScriptMethod, &NodeScriptMethod::movie_hasMovieText )
                .def_proxy_static( "getMovieScissor", nodeScriptMethod, &NodeScriptMethod::movie_getMovieScissor )
                .def_proxy_static( "hasMovieScissor", nodeScriptMethod, &NodeScriptMethod::movie_hasMovieScissor )
                .def_proxy_static( "getSubMovie", nodeScriptMethod, &NodeScriptMethod::movie_getSubMovie )
                .def_proxy_static( "hasSubMovie", nodeScriptMethod, &NodeScriptMethod::movie_hasSubMovie )
                .def_proxy_static( "getSocket", nodeScriptMethod, &NodeScriptMethod::movie_getSocket )
                .def_proxy_static( "hasSocket", nodeScriptMethod, &NodeScriptMethod::movie_hasSocket )
                .def_proxy_args_static( "setMovieEvent", nodeScriptMethod, &NodeScriptMethod::movie_setMovieEvent )
                .def_proxy_static( "removeMovieEvent", nodeScriptMethod, &NodeScriptMethod::movie_removeMovieEvent )
                .def_proxy_static( "hasMovieEvent", nodeScriptMethod, &NodeScriptMethod::movie_hasMovieEvent )
                .def_proxy_static_kernel( "getSockets", nodeScriptMethod, &NodeScriptMethod::movie_getSockets )
                .def_proxy_static_kernel( "getSlots", nodeScriptMethod, &NodeScriptMethod::movie_getSlots )
                .def_proxy_static_kernel( "getSubMovies", nodeScriptMethod, &NodeScriptMethod::movie_getSubMovies )
                .def_proxy_static( "getMovieNode", nodeScriptMethod, &NodeScriptMethod::movie_getMovieNode )
                .def_proxy_static( "hasMovieNode", nodeScriptMethod, &NodeScriptMethod::movie_hasMovieNode )
                .def_proxy_static_kernel( "filterLayers", nodeScriptMethod, &NodeScriptMethod::movie_filterLayers )
                .def_proxy_static( "getFrameDuration", nodeScriptMethod, &NodeScriptMethod::movie_getFrameDuration )
                .def_proxy_static( "getDuration", nodeScriptMethod, &NodeScriptMethod::movie_getDuration )
                .def_proxy_static( "getFrameCount", nodeScriptMethod, &NodeScriptMethod::movie_getFrameCount )
                .def_proxy_static( "getSize", nodeScriptMethod, &NodeScriptMethod::movie_getSize )
                .def_proxy_static( "getLayerPathLength", nodeScriptMethod, &NodeScriptMethod::movie_getLayerPathLength )
                .def_proxy_static_kernel( "getLayerPath", nodeScriptMethod, &NodeScriptMethod::movie_getLayerPath )
                .def_proxy_static_kernel( "getLayerPath2", nodeScriptMethod, &NodeScriptMethod::movie_getLayerPath2 )
                .def_proxy_static_kernel( "getLayerPath3", nodeScriptMethod, &NodeScriptMethod::movie_getLayerPath3 )
                .def_proxy_static_kernel( "getLayerPath4", nodeScriptMethod, &NodeScriptMethod::movie_getLayerPath4 )
                .def_proxy_static( "getMovieSlotWorldPosition", nodeScriptMethod, &NodeScriptMethod::movie_getMovieSlotWorldPosition )
                .def_proxy_static( "getMovieSlotOffsetPosition", nodeScriptMethod, &NodeScriptMethod::movie_getMovieSlotOffsetPosition )
                .def_proxy_static( "attachMovieSlotNode", nodeScriptMethod, &NodeScriptMethod::movie_attachMovieSlotNode )
                .def_proxy_static( "removeAllMovieSlotNode", nodeScriptMethod, &NodeScriptMethod::movie_removeAllMovieSlotNode )
                .def_proxy_native_kernel( "setEventListener", nodeScriptMethod, &NodeScriptMethod::s_Movie_setEventListener )
                ;

            pybind::interface_<Meshget, pybind::bases<Node, Eventable> >( kernel, "Meshget", false )
                .def( "setSurface", &Meshget::setSurface )
                .def( "getSurface", &Meshget::getSurface )
                .def( "setVertices", &Meshget::setVertices )
                .def_proxy_native_kernel( "setEventListener", nodeScriptMethod, &NodeScriptMethod::s_Meshget_setEventListener )
                ;

            pybind::interface_<MovieSlot, pybind::bases<Node> >( kernel, "MovieSlot", false )
                ;

            pybind::interface_<MovieInternalObject, pybind::bases<Node> >( kernel, "MovieInternalObject", false )
                ;

            pybind::interface_<Window, pybind::bases<Node> >( kernel, "Window", false )
                .def( "setResourceWindow", &Window::setResourceWindow )
                .def( "getResourceWindow", &Window::getResourceWindow )
                .def( "setClientSize", &Window::setClientSize )
                .def( "setClientSizeClip", &Window::setClientSizeClip )
                .def( "setClientSizeInTiles", &Window::setClientSizeInTiles )
                .def( "getClientSize", &Window::getClientSize )
                .def( "getWindowSize", &Window::getWindowSize )
                .def( "getTileSize", &Window::getTileSize )
                ;
        }

        pybind::interface_<ScheduleManagerInterface, pybind::bases<Mixin> >( kernel, "ScheduleManagerInterface", true )
            .def_proxy_args_static( "timing", nodeScriptMethod, &NodeScriptMethod::ScheduleManagerInterface_timing )
            .def_proxy_args_static( "schedule", nodeScriptMethod, &NodeScriptMethod::ScheduleManagerInterface_schedule )
            .def_proxy_args_static( "pipe", nodeScriptMethod, &NodeScriptMethod::ScheduleManagerInterface_pipe )
            .def( "refresh", &ScheduleManagerInterface::refresh )
            .def( "exist", &ScheduleManagerInterface::exist )
            .def( "remove", &ScheduleManagerInterface::remove )
            .def( "removeAll", &ScheduleManagerInterface::removeAll )
            .def( "freeze", &ScheduleManagerInterface::freeze )
            .def( "freezeAll", &ScheduleManagerInterface::freezeAll )
            .def( "isFreeze", &ScheduleManagerInterface::isFreeze )
            .def( "time", &ScheduleManagerInterface::time )
            .def( "left", &ScheduleManagerInterface::left )
            .def( "setSpeedFactor", &ScheduleManagerInterface::setSpeedFactor )
            .def( "getSpeedFactor", &ScheduleManagerInterface::getSpeedFactor )
            .def( "getTiming", &ScheduleManagerInterface::getTiming )
            ;
    }
}
