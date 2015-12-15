#	include "ScriptWrapper.h"

#   include "Interface/ApplicationInterface.h"
#   include "Interface/FileSystemInterface.h"
#   include "Interface/TimelineInterface.h"

#   include "Interface/InputSystemInterface.h"
#   include "Interface/NodeInterface.h"
#   include "Interface/MemoryInterface.h"
#	include "Interface/HttpSystemInterface.h"
#	include "Interface/PrefetcherInterface.h"
#	include "Interface/PlatformInterface.h"
#	include "Interface/PackageInterface.h"

#	include "Kernel/ScriptClassWrapper.h"
#	include "Kernel/ThreadTask.h"
#	include "Kernel/Scene.h"

#	include "Game.h"

#	include "Consts.h"

#   include "Interface/ScriptSystemInterface.h"
#   include "Interface/ScheduleManagerInterface.h"

#   include "Interface/TextInterface.h"
#   include "Interface/AccountInterface.h"

#   include "Interface/UnicodeInterface.h"

#   include "Interface/ResourceInterface.h"

#	include "Kernel/ResourceImageData.h"
#	include "Kernel/ScriptHolder.h"

#	include "ResourceFile.h"
#	include "ResourceMovie.h"
#	include "ResourceAnimation.h"
#	include "ResourceModel3D.h"
#	include "ResourceVideo.h"
#	include "ResourceSound.h"
#   include "ResourceImageSolid.h"

#	include "ResourceImageDefault.h"
#	include "ResourceTestPick.h"
#	include "ResourceHIT.h"
#	include "ResourceShape.h"
#	include "ResourceInternalObject.h"

#   include "ResourceImageSubstractRGBAndAlpha.h"
#   include "ResourceImageSubstract.h"

#	include "Player.h"

#   include "Interface/ApplicationInterface.h"
#   include "Interface/MousePickerSystemInterface.h"

#	include "Sprite.h"
#   include "MovieSlot.h"
#	include "MovieInternalObject.h"
#   include "MovieEvent.h"
#	include "Animation.h"
#	include "ParticleEmitter2.h"
#	include "Model3D.h"
#	include "HotSpot.h"
#	include "HotSpotPolygon.h"
#	include "HotSpotBubbles.h"
#	include "HotSpotImage.h"
#   include "HotSpotShape.h"
#	include "Landscape2D.h"
#	include "Grid2D.h"
//#	include "Light2D.h"
#	include "ShadowCaster2D.h"
#	include "Gyroscope.h"
#	include "Arrow.h"
#	include "TextField.h"
#	include "SoundEmitter.h"
#	include "Point.h"
#	include "Line.h"
#	include "Kernel/Camera3D.h"
//#	include "RigidBody3D.h"
//#	include "CapsuleController.h"
//#	include "Skeleton.h"
#	include "Kernel/Isometric.h"
#	include "Kernel/Parallax.h"
#	include "Kernel/RenderViewport.h"
#	include "Kernel/Camera2D.h"
#	include "Kernel/CameraTarget2D.h"

#	include "Kernel/CameraIsometric.h"

#	include "Layer2D.h"
#	include "Layer2DParallax.h"
#	include "Layer2DIsometric.h"
#	include "Layer2DTexture.h"

#   include "Kernel/ResourceImage.h"

//#	include "RigidBody2D.h"

#	include "PhysicalBody2D.h"
#	include "TilePolygon.h"

#	include "Movie.h"
#	include "Video.h"

#	include "Window.h"


#	include "Kernel/Shape.h"
#	include "Kernel/Entity.h"

//#	include "DiscreteEntity.h"

//#	include "SoundEngine.h"
#	include "Logger/Logger.h"

#   include "Interface/RenderSystemInterface.h"
#   include "Interface/PhysicSystemInterface.h"

#	include "Kernel/Identity.h"

#	include "Kernel/Affector.h"

#	include "Kernel/Camera3D.h"

#   include "Kernel/ThreadTask.h"

#   include "Interface/ThreadSystemInterface.h"

#	include "Core/Polygon.h"
#	include "Core/MemoryHelper.h"

#	include "Utils/Math/angle.h"
#	include "Utils/Math/vec4.h"
#	include "Utils/Math/mat3.h"
#	include "Utils/Math/mat4.h"
#	include "Utils/Math/quat.h"
#	include "Utils/Math/utils.h"

#	include "Utils/Core/Rect.h"
#	include "Utils/Core/String.h"
#	include "Utils/Core/Polygon.h"

#	include "pybind/stl_type_cast.hpp"
#	include "stdex/xml_sax_parser.h"
#   include "utf8.h"

#	include <sstream>

#	include <math.h>

namespace Menge
{
    //////////////////////////////////////////////////////////////////////////
    class NodeScriptMethod
    {
    public:
        NodeScriptMethod( ServiceProviderInterface * _serviceProvider )
            : m_serviceProvider(_serviceProvider)
        {
        }

    public:
        //////////////////////////////////////////////////////////////////////////
        PyObject * textfield_setTextFormatArgs( TextField * _textField, PyObject * _args, PyObject * _kwds )
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
					if( pybind::extract_value( py_string, key, false ) == false )
					{
						LOGGER_ERROR(m_serviceProvider)("textfield_setTextFormatArgs %s invalid get str %s"
							, pybind::object_repr( py_string )
							);

						return pybind::ret_false();
					}

					cs_args.push_back( key );
				}
				else if( pybind::unicode_check( py_string ) == true )
				{
					WString key;
					if( pybind::extract_value( py_string, key, false ) == false )
					{
						LOGGER_ERROR(m_serviceProvider)("textfield_setTextFormatArgs %s invalid get unicode %s"
							, pybind::object_repr( py_string )
							);

						return pybind::ret_false();
					}

					String utf8_arg;
					Helper::unicodeToUtf8( m_serviceProvider, key, utf8_arg );

					cs_args.push_back( utf8_arg );
				}
				else
				{
					const char * value = pybind::object_str( py_string );

					if( value == nullptr )
					{
						LOGGER_ERROR(m_serviceProvider)("textfield_setTextFormatArgs %s not suport arg %s"
							, pybind::object_repr( py_string )
							);

						return pybind::ret_false();							 
					}

					cs_args.push_back( String(value) );
				}
			}
						
			_textField->setTextFormatArgs( cs_args );
			
			return pybind::ret_true();
        }
		//////////////////////////////////////////////////////////////////////////
		TVectorWString textfield_getTextFormatArgs( TextField * _textField )
		{
			TVectorWString ws_args;

			const TVectorString & str_args = _textField->getTextFormatArgs();

			size_t args_count = str_args.size();
			ws_args.reserve( args_count );

			for( TVectorString::const_iterator
				it = str_args.begin(),
				it_end = str_args.end();
			it != it_end;
			++it )
			{
				const String & str_arg = *it;

				WString unicode;
				Helper::utf8ToUnicode( m_serviceProvider, str_arg, unicode );
								
				ws_args.push_back( unicode );
			}

			return ws_args;
		}
        //////////////////////////////////////////////////////////////////////////
        WString textfield_getText( TextField * _textField )
        {
            const String & utf8 = _textField->getText();

            WString unicode;
            Helper::utf8ToUnicode( m_serviceProvider, utf8, unicode );

            return unicode;            
        }
		//////////////////////////////////////////////////////////////////////////
		Node * movie_getMovieNode( Movie * _movie, const ConstString & _name )
		{
			Node * node;
			Movie * submovie;
			if( _movie->getMovieNode( _name, ConstString::none(), &node, &submovie ) == false )
			{
				return nullptr;
			}
			
			return node;
		}
		//////////////////////////////////////////////////////////////////////////
		bool movie_hasMovieNode( Movie * _movie, const ConstString & _name )
		{
			Node * node;
			Movie * submovie;
			bool successful = _movie->hasMovieNode( _name, ConstString::none(), &node, &submovie );

			return successful;
		}
		//////////////////////////////////////////////////////////////////////////
		mt::vec3f movie_getWorldAnchorPoint( Movie * _movie )
		{
			const ResourceMoviePtr & resourceMovie = _movie->getResourceMovie();

			if( resourceMovie == nullptr )
			{
				LOGGER_ERROR( m_serviceProvider )("Movie::getWorldAnchorPoint %s invalid setup resource"
					, _movie->getName().c_str()
					);

				return mt::vec3f( 0.f, 0.f, 0.f );
			}

			const mt::vec3f & ap = resourceMovie->getAnchorPoint();

			const mt::mat4f & wm = _movie->getWorldMatrix();

			mt::vec3f wap;
			mt::mul_v3_m4( wap, ap, wm );

			return wap;
		}
		//////////////////////////////////////////////////////////////////////////
		PyObject * movie_getEnableMovieLayer( Movie * _movie, const ConstString & _name )
		{
			bool enable;
			if( _movie->getEnableMovieLayer( _name, enable ) == false )
			{
				return pybind::ret_none();
			}

			return pybind::ptr( enable );
		}
		//////////////////////////////////////////////////////////////////////////
		Node * movie_getMovieSlot( Movie * _movie, const ConstString & _name )
		{
			Node * node;
			Movie * submovie;
			if( _movie->getMovieNode( _name, CONST_STRING(m_serviceProvider, MovieSlot), &node, &submovie ) == false )
			{
				return nullptr;
			}			

			return node;
		}
		//////////////////////////////////////////////////////////////////////////
		bool movie_hasMovieSlot( Movie * _movie, const ConstString & _name )
		{
			Node * node;
			Movie * submovie;
			bool successful = _movie->hasMovieNode( _name, CONST_STRING(m_serviceProvider, MovieSlot), &node, &submovie );

			return successful;
		}
		//////////////////////////////////////////////////////////////////////////
		Node * movie_getMovieText( Movie * _movie, const ConstString & _name )
		{
			Node * node;
			Movie * submovie;

			if( _movie->getMovieNode( _name, CONST_STRING(m_serviceProvider, MovieText), &node, &submovie ) == false )
			{
				return nullptr;
			}

			return node;
		}
		//////////////////////////////////////////////////////////////////////////
		bool movie_hasMovieText( Movie * _movie, const ConstString & _name )
		{
			Node * node;
			Movie * submovie;

			bool successful = _movie->hasMovieNode( _name, CONST_STRING(m_serviceProvider, MovieText), &node, &submovie );

			return successful;
		}
		//////////////////////////////////////////////////////////////////////////
		Node * movie_getSubMovie( Movie * _movie, const ConstString & _name )
		{
			Node * node;
			Movie * submovie;
			if( _movie->getMovieNode( _name, CONST_STRING(m_serviceProvider, SubMovie), &node, &submovie ) == false )
			{
				return nullptr;
			}

			return node;
		}
		//////////////////////////////////////////////////////////////////////////
		bool movie_hasSubMovie( Movie * _movie, const ConstString & _name )
		{
			Node * node;
			Movie * submovie;
			bool successful = _movie->hasMovieNode( _name, CONST_STRING(m_serviceProvider, SubMovie), &node, &submovie );

			return successful;
		}
		//////////////////////////////////////////////////////////////////////////
		Node * movie_getSocket( Movie * _movie, const ConstString & _name )
		{
			Node * node;
			Movie * submovie;

			if( _movie->hasMovieNode( _name, CONST_STRING(m_serviceProvider, MovieSocketImage), &node, &submovie ) == true )
			{				
				return node;
			}

			if( _movie->hasMovieNode( _name, CONST_STRING(m_serviceProvider, MovieSocketShape), &node, &submovie ) == true )
			{
				return node;
			}

			LOGGER_ERROR(m_serviceProvider)("Movie::getSocket: movie %s resource %s not found %s"
				, _movie->getName().c_str()
				, _movie->getResourceMovieName().c_str()
				, _name.c_str()
				);

			return nullptr;
		}
		//////////////////////////////////////////////////////////////////////////
		bool movie_hasSocket( Movie * _movie, const ConstString & _name )
		{
			Node * node;
			Movie * submovie;

			if( _movie->hasMovieNode( _name, CONST_STRING(m_serviceProvider, MovieSocketImage), &node, &submovie ) == true )
			{
				return true;
			}

			if( _movie->hasMovieNode( _name, CONST_STRING(m_serviceProvider, MovieSocketShape), &node, &submovie ) == true )
			{
				return true;
			}

			return false;
		}
		//////////////////////////////////////////////////////////////////////////
		bool movie_setMovieEvent( Movie * _movie, const ConstString & _name, const pybind::object & _cb )
		{
			Node * node;
			Movie * submovie;

			if( _movie->getMovieNode( _name, CONST_STRING(m_serviceProvider, MovieEvent), &node, &submovie ) == false )
			{
				return false;
			}

			MovieEvent * ev = static_node_cast<MovieEvent>(node);

			ev->setEvent( _cb );

			return true;
		}
		//////////////////////////////////////////////////////////////////////////
		bool movie_hasMovieEvent( Movie * _movie, const ConstString & _name )
		{
			Node * node;
			Movie * submovie;
			bool successful = _movie->hasMovieNode( _name, CONST_STRING(m_serviceProvider, MovieEvent), &node, &submovie );

			return successful;
		}
		//////////////////////////////////////////////////////////////////////////
		class PythonVisitorMovieSlot
			: public VisitorMovieNode
		{
		public:
			PythonVisitorMovieSlot( pybind::list & _list )
				: m_list(_list)
			{
			}

		protected:
			void visitMovieNode( Movie * _movie, Node * _node ) override
			{
				MovieSlot * slot = static_node_cast<MovieSlot>(_node);

				if( slot == nullptr )
				{
					return;
				}

				const ConstString & name = slot->getName();

				pybind::tuple py_value = pybind::make_tuple_t( _movie, name, slot );
				m_list.append( py_value );
								
				//pybind::list_appendtuple_t( m_list, _movie, name, slot );
			}

		protected:
			pybind::list & m_list;

		private:
			void operator = (const PythonVisitorMovieSlot &);
		};
		//////////////////////////////////////////////////////////////////////////
		pybind::list movie_getSlots( Movie * _movie )
		{
			pybind::list py_list;
			
			PythonVisitorMovieSlot visitor( py_list );
			_movie->visitMovieLayer( CONST_STRING(m_serviceProvider, MovieSlot), &visitor );

			return py_list;
		}
        //////////////////////////////////////////////////////////////////////////
        class PythonVisitorMovieSocket
            : public VisitorMovieNode
        {
        public:
			PythonVisitorMovieSocket( const pybind::list & _list )
                : m_list(_list)
            {
            }

        protected:
            void visitMovieNode( Movie * _movie, Node * _node ) override
            {
				HotSpot * hotspot = static_node_cast<HotSpot>(_node);

				if( hotspot == nullptr )
				{
					return;
				}

				const ConstString & name = hotspot->getName();

				m_list.append( pybind::make_tuple_t( _movie, name, hotspot ) );
            }

        protected:
            pybind::list m_list;
        };
        //////////////////////////////////////////////////////////////////////////
		pybind::list movie_getSockets( Movie * _movie )
        {
			pybind::list py_list;

			PythonVisitorMovieSocket visitor(py_list);
			_movie->visitMovieLayer( CONST_STRING(m_serviceProvider, MovieSocketImage), &visitor );
			_movie->visitMovieLayer( CONST_STRING(m_serviceProvider, MovieSocketShape), &visitor );

            return py_list;
        }
		//////////////////////////////////////////////////////////////////////////
		class PythonVisitorMovieSubMovie
			: public VisitorMovieNode
		{
		public:
			PythonVisitorMovieSubMovie( const pybind::list & _list )
				: m_list(_list)
			{
			}

		protected:
			void visitMovieNode( Movie * _movie, Node * _node ) override
			{
				Movie * subMovie = static_node_cast<Movie>(_node);

				if( subMovie == nullptr )
				{
					return;
				}

				const ConstString & name = subMovie->getName();

				m_list.append( pybind::make_tuple_t( _movie, name, subMovie ) );
			}

		protected:
			pybind::list m_list;
		};
		//////////////////////////////////////////////////////////////////////////
		pybind::list movie_getSubMovies( Movie * _movie )
		{
			pybind::list py_list;

			PythonVisitorMovieSubMovie visitor(py_list);
			_movie->visitMovieLayer( CONST_STRING(m_serviceProvider, SubMovie), &visitor );

			return py_list;
		}
		//////////////////////////////////////////////////////////////////////////
		class PythonVisitorMovieLayer
			: public VisitorMovieNode
		{
		public:
			PythonVisitorMovieLayer( const pybind::list & _list )
				: m_list(_list)
			{
			}

		protected:
			void visitMovieNode( Movie * _movie, Node * _layer ) override
			{
				m_list.append( pybind::make_tuple_t( _movie, _layer ) );
			}

		protected:
			pybind::list m_list;
		};
		//////////////////////////////////////////////////////////////////////////
		pybind::list movie_filterLayers( Movie * _movie, const ConstString & _type )
		{
			pybind::list py_list;

			PythonVisitorMovieLayer visitor(py_list);
			_movie->visitMovieLayer( _type, &visitor );

			return py_list;
		}
		//////////////////////////////////////////////////////////////////////////
        float movie_getFrameDuration( Movie * _movie )
        {
            const ResourceMoviePtr & resourceMovie = _movie->getResourceMovie();

            if( resourceMovie == nullptr )
            {
                LOGGER_ERROR(m_serviceProvider)("Movie.getFrameDuration: '%s' not activate"
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
                LOGGER_ERROR(m_serviceProvider)("Movie.getDuration: '%s' not activate"
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
                LOGGER_ERROR(m_serviceProvider)("Movie.getFrameCount: '%s' not activate"
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
                LOGGER_ERROR(m_serviceProvider)("Movie.getSize: '%s' not activate"
                    , _movie->getName().c_str()
                    );

				static mt::vec2f zero(0.f, 0.f);

                return zero;
            }

            const mt::vec2f & size = resourceMovie->getSize();

            return size; 
        }
		//////////////////////////////////////////////////////////////////////////
		float movie_getLayerPathLength( Movie * _movie, const ConstString & _name )
		{
			const MovieLayer * layer;
			Movie * sub_movie;						
			if( _movie->getMovieLayer( _name, &layer, &sub_movie ) == false )
			{
				LOGGER_ERROR(m_serviceProvider)("Movie::getLayerPathLength: '%s' not found layer '%s'"
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
				LOGGER_ERROR( m_serviceProvider )("Movie::getLayerPathLength: '%s' invalid get layer '%s' frame %d"
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
		pybind::list movie_getLayerPath( Movie * _movie, const ConstString & _name )
		{
			const MovieLayer * layer;
			Movie * sub_movie;
			if( _movie->getMovieLayer( _name, &layer, &sub_movie ) == false )
			{
				LOGGER_ERROR(m_serviceProvider)("Movie::getLayerPathLength: '%s' not found layer '%s'"
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
				LOGGER_ERROR(m_serviceProvider)("Movie::getLayerPathLength: sub_movie '%s' not found layer '%s' frame pack is null"
					, sub_movie->getName().c_str()
					, _name.c_str()
					);

				return pybind::make_invalid_list_t();
			}

			pybind::list py_path( indexCount );

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
		pybind::list movie_getLayerPath2( Movie * _movie, const ConstString & _name )
		{
			const MovieLayer * layer;
			Movie * sub_movie;
			if( _movie->getMovieLayer( _name, &layer, &sub_movie ) == false )
			{
				LOGGER_ERROR(m_serviceProvider)("Movie::getLayerPathLength: '%s' not found layer '%s'"
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

			const MovieFramePackInterfacePtr & framePack = resourceMovie->getFramePack();

			pybind::list py_path( indexCount );

			for( uint32_t i = 0; i != indexCount; ++i )
			{
				MovieFrameSource frame;
				framePack->getLayerFrame( layer->index, i, frame );

				mt::vec2f pos;
				pos.x = frame.position.x;
				pos.y = frame.position.y;
				
				py_path[i] = pos;
			}

			return py_path;
		}
		//////////////////////////////////////////////////////////////////////////
		mt::vec3f movie_getMovieSlotWorldPosition( Movie * _movie, const ConstString & _slotName )
		{
			Node * node;
			Movie * submovie;
			if( _movie->getMovieNode( _slotName, CONST_STRING( m_serviceProvider, MovieSlot ), &node, &submovie ) == false )
			{
				LOGGER_ERROR( m_serviceProvider )("Movie::getMovieSlotWorldPosition %s not found slot '%s"
					, _movie->getName().c_str()
					, _slotName.c_str()
					);

				return mt::vec3f(0.f, 0.f, 0.f);
			}

			const mt::vec3f & wp = node->getWorldPosition();

			return wp;
		}
		//////////////////////////////////////////////////////////////////////////
		mt::vec3f movie_getMovieSlotOffsetPosition( Movie * _movie, const ConstString & _slotName )
		{
			Node * node;
			Movie * submovie;
			if( _movie->getMovieNode( _slotName, CONST_STRING( m_serviceProvider, MovieSlot ), &node, &submovie ) == false )
			{
				LOGGER_ERROR( m_serviceProvider )("Movie::getMovieSlotOffsetPosition %s not found slot '%s"
					, _movie->getName().c_str()
					, _slotName.c_str()
					);

				return mt::vec3f( 0.f, 0.f, 0.f );
			}

			const ResourceMoviePtr & resourceMovie = _movie->getResourceMovie();

			if( resourceMovie == nullptr )
			{
				LOGGER_ERROR( m_serviceProvider )("Movie::getMovieSlotOffsetPosition %s invalid setup resource"
					, _movie->getName().c_str()
					);

				return mt::vec3f( 0.f, 0.f, 0.f );
			}

			const mt::vec3f & ap = resourceMovie->getAnchorPoint();

			const mt::mat4f & wm = _movie->getWorldMatrix();

			mt::vec3f wap;
			mt::mul_v3_m4( wap, ap, wm );

			const mt::vec3f & wp = node->getWorldPosition();

			mt::vec3f sop = wp - wap;
			
			return sop;
		}
		//////////////////////////////////////////////////////////////////////////
		bool movie_attachMovieSlotNode( Movie * _movie, const ConstString & _slotName, Node * _node )
		{
			Node * node;
			Movie * submovie;
			if( _movie->getMovieNode( _slotName, CONST_STRING( m_serviceProvider, MovieSlot ), &node, &submovie ) == false )
			{
				LOGGER_ERROR( m_serviceProvider )("Movie::attachMovieSlotNode %s not found slot '%s"
					, _movie->getName().c_str()
					, _slotName.c_str()
					);

				return false;
			}

			node->addChild( _node );

			return true;
		}
        //////////////////////////////////////////////////////////////////////////
        void Transformation3D_removeRelationTransformation( Transformation3D * _transformation )
        {
            _transformation->setRelationTransformation( nullptr );
        }
        //////////////////////////////////////////////////////////////////////////
        void Transformation3D_setScale( Transformation3D * _transformation, const mt::vec3f & _scale )
        {
			//if( mt::equal_f_z( _scale.x ) == true || mt::equal_f_z( _scale.y ) == true || mt::equal_f_z( _scale.z ) == true )
			//{
			//	pybind::throw_exception("Transformation3D::setScale scale xyz not zero! (%f %f %f)"
			//		, _scale.x
			//		, _scale.y
			//		, _scale.z
			//		);

			//	return;
			//}
            
            _transformation->setScale( _scale );
        }
		//////////////////////////////////////////////////////////////////////////
		mt::vec3f ResourceMovie_getLayerPosition( ResourceMovie * _movie, const ConstString & _name )
		{
			const MovieLayer * layer;
			bool successful = _movie->hasMovieLayer( _name, &layer );

			if( successful == false )
			{
				LOGGER_ERROR(m_serviceProvider)("ResourceMovie::getLayerPosition %s not found layer '%s'"
					, _movie->getName().c_str()
					, _name.c_str()
					);

				return mt::vec3f(0.f, 0.f, 0.f);
			}

			return layer->position;
		}
		//////////////////////////////////////////////////////////////////////////
		bool ResourceMovie_hasLayer( ResourceMovie * _movie, const ConstString & _name )
		{
			const MovieLayer * layer;
			bool successful = _movie->hasMovieLayer( _name, &layer );

			return successful;
		}
		//////////////////////////////////////////////////////////////////////////
		bool ResourceMovie_hasLayerType( ResourceMovie * _movie, const ConstString & _name, const ConstString & _type )
		{
			const MovieLayer * layer;
			bool successful = _movie->hasMovieLayerType( _name, _type, &layer );

			return successful;
		}
        //////////////////////////////////////////////////////////////////////////
        mt::vec2f s_getLocalPolygonCenter( HotSpotPolygon * _hs )
        {
			const Polygon & polygon = _hs->getPolygon();

			mt::box2f bb;
			polygon.to_box2f( bb );

			mt::vec2f c;
			mt::get_center_box( bb, c );

			return c;
        }
        //////////////////////////////////////////////////////////////////////////
        mt::vec2f s_getWorldPolygonCenter( HotSpotPolygon * _hs )
        {
			const Polygon & polygon = _hs->getPolygonWM();

			mt::box2f bb;
			polygon.to_box2f( bb );

			mt::vec2f c;
			mt::get_center_box( bb, c );

			return c;
        }
        //////////////////////////////////////////////////////////////////////////
        Polygon s_getWorldPolygon( HotSpotPolygon * _hs )
        {
            const Polygon & polygon = _hs->getPolygon();

            const mt::mat4f & wm = _hs->getWorldMatrix();

            Polygon pwm;
			polygon.mul_wm( pwm, wm );

            return pwm;
        }
        //////////////////////////////////////////////////////////////////////////
        Polygon s_polygon_wm( Node * _node, const Polygon & _polygon )
        {
			const mt::mat4f & wm = _node->getWorldMatrix();

            Polygon polygon;
			_polygon.mul_wm( polygon, wm );

            return polygon;
        }
		//////////////////////////////////////////////////////////////////////////
		Polygon s_polygon_anchor( const Polygon & _polygon, const mt::vec2f & _anchor )
		{
			Polygon polygon;
			_polygon.transpose( polygon, _anchor );

			return polygon;
		}
        //////////////////////////////////////////////////////////////////////////
        bool s_testHotspot( HotSpotPolygon * _left, HotSpotPolygon * _right )
        {
            const Polygon & left_poligon = _left->getPolygon();
            const mt::mat4f & left_wm = _left->getWorldMatrix();

            const Polygon & right_poligon = _right->getPolygon();
            const mt::mat4f & right_wm = _right->getWorldMatrix();

            Polygon left_polygon_wm;

			left_poligon.mul_wm( left_polygon_wm, left_wm );

            Polygon right_polygon_wm;

			right_poligon.mul_wm( right_polygon_wm, right_wm );

            bool result = left_polygon_wm.intersects( right_polygon_wm );

            return result;

            //return _left->testPolygon( left_wm, right_poligon, right_wm );
        }
        //////////////////////////////////////////////////////////////////////////
        bool s_loadPlugin( const WString & _pluginName )
        {
            bool successful = PLUGIN_SERVICE(m_serviceProvider)
                ->loadPlugin( _pluginName );

			return successful;
        }
        //////////////////////////////////////////////////////////////////////////
        void s_setMousePickerBlockInput( bool _value )
        {
            MousePickerSystemInterface * mousePickerSystem = PLAYER_SERVICE(m_serviceProvider)
                ->getMousePickerSystem();

            mousePickerSystem->setBlock( _value );
        }
        //////////////////////////////////////////////////////////////////////////
        void s_setMousePickerHandleValue( bool _value )
        {
            MousePickerSystemInterface * mousePickerSystem = PLAYER_SERVICE(m_serviceProvider)
                ->getMousePickerSystem();

            mousePickerSystem->setHandleValue( _value );
        }
        //////////////////////////////////////////////////////////////////////////
        void s_setInputMouseButtonEventBlock( bool _value )
        {
            APPLICATION_SERVICE(m_serviceProvider)
                ->setInputMouseButtonEventBlock( _value );
        }
        //////////////////////////////////////////////////////////////////////////
        bool s_getInputMouseButtonEventBlock()
        {
            return APPLICATION_SERVICE(m_serviceProvider)
                ->getInputMouseButtonEventBlock();
        }
		//////////////////////////////////////////////////////////////////////////
		ScheduleManagerInterface * createTiming()
		{
			ScheduleManagerInterface * sm = PLAYER_SERVICE( m_serviceProvider )
				->createTimingManager();
			
			return sm;
		}
		//////////////////////////////////////////////////////////////////////////
		bool destroyTiming( ScheduleManagerInterface * _tm )
		{
			if( _tm == nullptr )
			{
				LOGGER_ERROR(m_serviceProvider)("Menge.destroyTiming destroy is NULL"
					);

				return false;
			}

			bool successful = PLAYER_SERVICE(m_serviceProvider)
				->destroyTimingManager( _tm );

			return successful;
		}
		//////////////////////////////////////////////////////////////////////////
		class PyScheduleTimerInterface
			: public ScheduleTimerInterface
		{
		public:
			PyScheduleTimerInterface()
				: m_serviceProvider(nullptr)
			{
			}

			~PyScheduleTimerInterface()
			{
			}

		public:
			void initialize( ServiceProviderInterface * _serviceProvider, const pybind::object & _script )
			{
				m_serviceProvider = _serviceProvider;
				m_script = _script;
			}

		protected:
			void onScheduleUpdate( uint32_t _id, uint32_t _iterate, float _delay ) override
			{
				m_script( _id, _iterate, _delay, false );
			}

			void onScheduleStop( uint32_t _id ) override
			{
				m_script( _id, 0, 0.f, true );
			}

		protected:
			ServiceProviderInterface * m_serviceProvider;
			pybind::object m_script;
		};
		//////////////////////////////////////////////////////////////////////////
		typedef FactoryPoolStore<PyScheduleTimerInterface, 8> TFactoryPyObjectTimingListener;
		TFactoryPyObjectTimingListener m_factoryPyObjectTimingListener;
		//////////////////////////////////////////////////////////////////////////
		class PySchedulePipeInterface
			: public SchedulePipeInterface
		{
		public:
			PySchedulePipeInterface()
				: m_serviceProvider( nullptr )
			{
			}

			~PySchedulePipeInterface()
			{
			}

		public:
			void initialize( ServiceProviderInterface * _serviceProvider, const pybind::object & _cb )
			{
				m_serviceProvider = _serviceProvider;
				m_cb = _cb;
			}

		protected:
			float onSchedulePipe( uint32_t _id, uint32_t _index ) override
			{
				float delay = m_cb( _id, _index );

				return delay;
			}

		protected:
			ServiceProviderInterface * m_serviceProvider;
			pybind::object m_cb;
		};
		//////////////////////////////////////////////////////////////////////////
		typedef FactoryPoolStore<PySchedulePipeInterface, 8> TFactoryPySchedulePipeInterface;
		TFactoryPySchedulePipeInterface m_factoryPySchedulePipeInterface;
		//////////////////////////////////////////////////////////////////////////
		class DelaySchedulePipeInterface
			: public SchedulePipeInterface
		{
		public:
			DelaySchedulePipeInterface()
				: m_serviceProvider( nullptr )
				, m_delay(0.f)
			{
			}

			~DelaySchedulePipeInterface()
			{
			}

		public:
			void initialize( ServiceProviderInterface * _serviceProvider, float _delay )
			{
				m_serviceProvider = _serviceProvider;

				m_delay = _delay;
			}

		protected:
			float onSchedulePipe( uint32_t _id, uint32_t _index ) override
			{
				(void)_id;
				(void)_index;

				return m_delay;
			}

		protected:
			ServiceProviderInterface * m_serviceProvider;

			float m_delay;
		};
		//////////////////////////////////////////////////////////////////////////
		typedef FactoryPoolStore<DelaySchedulePipeInterface, 8> TFactoryDelaySchedulePipeInterface;
		TFactoryDelaySchedulePipeInterface m_factoryDelaySchedulePipeInterface;
		//////////////////////////////////////////////////////////////////////////
		class PyScheduleEventInterface
			: public ScheduleEventInterface
		{
		public:
			PyScheduleEventInterface()
				: m_serviceProvider( nullptr )
			{
			}

			~PyScheduleEventInterface()
			{
			}

		public:
			void initialize( ServiceProviderInterface * _serviceProvider, const pybind::object & _cb )
			{
				m_serviceProvider = _serviceProvider;
				m_cb = _cb;
			}

		protected:
			void onScheduleComplete( uint32_t _id ) override
			{
				m_cb( _id, false );
			}

			void onScheduleStop( uint32_t _id ) override
			{
				m_cb( _id, true );
			}

		protected:
			ServiceProviderInterface * m_serviceProvider;
			pybind::object m_cb;
		};
		//////////////////////////////////////////////////////////////////////////
		typedef FactoryPoolStore<PyScheduleEventInterface, 8> TFactoryPyObjectScheduleListener;
		TFactoryPyObjectScheduleListener m_factoryPyObjectScheduleListener;
        //////////////////////////////////////////////////////////////////////////
		uint32_t timing( float _delay, const pybind::object & _listener )
        {
			ScheduleManagerInterface * tm = PLAYER_SERVICE( m_serviceProvider )
                ->getTimingManager();

			DelaySchedulePipeInterface * pipe = m_factoryDelaySchedulePipeInterface.createObject();

			pipe->initialize( m_serviceProvider, _delay );

            PyScheduleTimerInterface * listener = m_factoryPyObjectTimingListener.createObject();
			
			listener->initialize( m_serviceProvider, _listener );

			uint32_t id = tm->timing( pipe, listener );

            return id;
        }
        //////////////////////////////////////////////////////////////////////////
        bool timingRemove( uint32_t _id )
        {
			ScheduleManagerInterface * tm = PLAYER_SERVICE( m_serviceProvider )
                ->getTimingManager();

            bool successful = tm->remove( _id );

			return successful;
        }
		//////////////////////////////////////////////////////////////////////////
		ScheduleManagerInterface * createScheduler()
		{
			ScheduleManagerInterface * sm = PLAYER_SERVICE(m_serviceProvider)
				->createSchedulerManager();
			
			return sm;
		}
		//////////////////////////////////////////////////////////////////////////
		bool destroyScheduler( ScheduleManagerInterface * _sm )
		{
			if( _sm == nullptr )
			{
				LOGGER_ERROR(m_serviceProvider)("Menge.destroyScheduler destroy scheduler is NULL"
					);

				return false;
			}

			bool successful = PLAYER_SERVICE(m_serviceProvider)
				->destroySchedulerManager( _sm );

			return successful;
		}
        //////////////////////////////////////////////////////////////////////////
        uint32_t schedule( float _timing, const pybind::object & _script )
        {
            ScheduleManagerInterface * sm = PLAYER_SERVICE(m_serviceProvider)
                ->getScheduleManager();

            PyScheduleEventInterface * sl = m_factoryPyObjectScheduleListener.createObject();

			sl->initialize( m_serviceProvider, _script );

            uint32_t id = sm->event( _timing, sl );

            return id;
        }
		//////////////////////////////////////////////////////////////////////////
		uint32_t ScheduleManagerInterface_schedule( ScheduleManagerInterface * _scheduleManager, float _timing, const pybind::object & _script )
		{
			PyScheduleEventInterface * sl = m_factoryPyObjectScheduleListener.createObject();

			sl->initialize( m_serviceProvider, _script );

			uint32_t id = _scheduleManager->event( _timing, sl );

			return id;
		}
		//////////////////////////////////////////////////////////////////////////
		uint32_t ScheduleManagerInterface_timing( ScheduleManagerInterface * _scheduleManager, float _delay, const pybind::object & _listener )
		{
			DelaySchedulePipeInterface * pipe = m_factoryDelaySchedulePipeInterface.createObject();

			pipe->initialize( m_serviceProvider, _delay );

			PyScheduleTimerInterface * tl = m_factoryPyObjectTimingListener.createObject();

			tl->initialize( m_serviceProvider, _listener );

			uint32_t id = _scheduleManager->timing( pipe, tl );

			return id;
		}
		//////////////////////////////////////////////////////////////////////////
		uint32_t ScheduleManagerInterface_pipe( ScheduleManagerInterface * _scheduleManager, const pybind::object & _pipe, const pybind::object & _listener )
		{
			PySchedulePipeInterface * pipe = m_factoryPySchedulePipeInterface.createObject();

			pipe->initialize( m_serviceProvider, _pipe );

			PyScheduleTimerInterface * tl = m_factoryPyObjectTimingListener.createObject();

			tl->initialize( m_serviceProvider, _listener );

			uint32_t id = _scheduleManager->timing( pipe, tl );

			return id;
		}
        //////////////////////////////////////////////////////////////////////////
        bool scheduleRemove( uint32_t _id )
        {
            ScheduleManagerInterface * sm = PLAYER_SERVICE(m_serviceProvider)
                ->getScheduleManager();

			if( sm == nullptr )
			{
				return false;
			}

            bool successful = sm->remove( _id );

			return successful;
        }
        //////////////////////////////////////////////////////////////////////////
        void scheduleRemoveAll()
        {
            ScheduleManagerInterface * sm = PLAYER_SERVICE(m_serviceProvider)
                ->getScheduleManager();

			if( sm == nullptr )
			{
				return;
			}

            sm->removeAll();
        }	
        //////////////////////////////////////////////////////////////////////////
        bool s_scheduleFreeze( uint32_t _id, bool _freeze )
        {
            ScheduleManagerInterface * sm = PLAYER_SERVICE(m_serviceProvider)
                ->getScheduleManager();

			if( sm == nullptr )
			{
				return false;
			}

            bool successful = sm->freeze( _id, _freeze );

			return successful;
        }
        //////////////////////////////////////////////////////////////////////////
        void s_scheduleFreezeAll()
        {
            ScheduleManagerInterface * sm = PLAYER_SERVICE(m_serviceProvider)
                ->getScheduleManager();

			if( sm == nullptr )
			{
				return;
			}

            sm->freezeAll( true );
        }
		//////////////////////////////////////////////////////////////////////////
        void scheduleResumeAll()
        {
            ScheduleManagerInterface * sm = PLAYER_SERVICE(m_serviceProvider)
                ->getScheduleManager();

			if( sm == nullptr )
			{
				return;
			}

            sm->freezeAll( false );
        }
        //////////////////////////////////////////////////////////////////////////
        bool s_scheduleIsFreeze( uint32_t _id )
        {
            ScheduleManagerInterface * sm = PLAYER_SERVICE(m_serviceProvider)
                ->getScheduleManager();

			if( sm == nullptr )
			{
				return false;
			}

            return sm->isFreeze( _id );
        }
        //////////////////////////////////////////////////////////////////////////
        float s_scheduleTime( uint32_t _id )
        {
            ScheduleManagerInterface * sm = PLAYER_SERVICE(m_serviceProvider)
                ->getScheduleManager();

			if( sm == nullptr )
			{
				return 0.f;
			}

            float time = sm->time( _id );

            return time;
        }
        //////////////////////////////////////////////////////////////////////////
		uint32_t s_scheduleGlobal( float _timing, const pybind::object & _script )
        {
            ScheduleManagerInterface * sm = PLAYER_SERVICE(m_serviceProvider)
                ->getScheduleManagerGlobal();

			if( sm == nullptr )
			{
				return 0;
			}

			PyScheduleEventInterface * sl = m_factoryPyObjectScheduleListener.createObject();

			sl->initialize( m_serviceProvider, _script );

            uint32_t id = sm->event( _timing, sl );

            return id;
        }
        //////////////////////////////////////////////////////////////////////////
        bool s_scheduleGlobalRemove( uint32_t _id )
        {
            ScheduleManagerInterface * sm = PLAYER_SERVICE(m_serviceProvider)
                ->getScheduleManagerGlobal();

			if( sm == nullptr )
			{
				return false;
			}

            bool successful = sm->remove( _id );

			return successful;
        }
        //////////////////////////////////////////////////////////////////////////
        void s_scheduleGlobalRemoveAll()
        {
            ScheduleManagerInterface * sm = PLAYER_SERVICE(m_serviceProvider)
                ->getScheduleManagerGlobal();

			if( sm == nullptr )
			{
				return;
			}

            sm->removeAll();
        }	
        //////////////////////////////////////////////////////////////////////////
        bool s_scheduleGlobalFreeze( uint32_t _id, bool _freeze )
        {
            ScheduleManagerInterface * sm = PLAYER_SERVICE(m_serviceProvider)
                ->getScheduleManagerGlobal();

			if( sm == nullptr )
			{
				return false;
			}

			bool successful = sm->freeze( _id, _freeze );

			return successful;
        }
        //////////////////////////////////////////////////////////////////////////
        void s_scheduleGlobalFreezeAll()
        {
            ScheduleManagerInterface* sm = PLAYER_SERVICE(m_serviceProvider)
                ->getScheduleManagerGlobal();

			if( sm == nullptr )
			{
				return;
			}

            sm->freezeAll( true );
        }
        //////////////////////////////////////////////////////////////////////////
        void s_scheduleGlobalResumeAll()
        {
            ScheduleManagerInterface * sm = PLAYER_SERVICE(m_serviceProvider)
                ->getScheduleManagerGlobal();

			if( sm == nullptr )
			{
				return;
			}

            sm->freezeAll( false );
        }
        //////////////////////////////////////////////////////////////////////////
        bool s_scheduleGlobalIsFreeze( uint32_t _id )
        {
            ScheduleManagerInterface* sm = PLAYER_SERVICE(m_serviceProvider)
                ->getScheduleManagerGlobal();

			if( sm == nullptr )
			{
				return false;
			}

            bool freeze = sm->isFreeze( _id );

            return freeze;
        }
        //////////////////////////////////////////////////////////////////////////
        float s_scheduleGlobalTime( uint32_t _id )
        {
            ScheduleManagerInterface* sm = PLAYER_SERVICE(m_serviceProvider)
                ->getScheduleManagerGlobal();

			if( sm == nullptr )
			{
				return 0.f;
			}

            float time = sm->time( _id );

            return time;
        }
        //////////////////////////////////////////////////////////////////////////
        void s_setTimingFactor( float _factor )
        {
            GAME_SERVICE(m_serviceProvider)
				->setTimingFactor( _factor );
        }
        //////////////////////////////////////////////////////////////////////////
        void s_addHomeless( Node * _node )
        {
            NODE_SERVICE(m_serviceProvider)
                ->addHomeless( _node );

			_node->release();
        }
		//////////////////////////////////////////////////////////////////////////
		bool s_isHomeless( Node * _node )
		{
			bool is = NODE_SERVICE(m_serviceProvider)
				->isHomeless( _node );

			return is;
		}
        //////////////////////////////////////////////////////////////////////////
        float s_getTimingFactor( )
        {
            float factor = GAME_SERVICE(m_serviceProvider)
                ->getTimingFactor();

            return factor;
        }
		//////////////////////////////////////////////////////////////////////////
		class PythonSceneChangeCallback
			: public SceneChangeCallbackInterface
		{
		public:
			void initialize( const pybind::object & _cb )				
			{
				m_cb = _cb;
			}
			
		public:
			void onSceneChange( Scene * _scene, bool _enable, bool _remove ) override
			{
				if( _remove == false )
				{
					if( _scene == nullptr )
					{
						m_cb( pybind::ret_none(), _enable );
					}
					else
					{
						const pybind::object & py_scene = _scene->getScriptObject();

						m_cb( py_scene, _enable );
					}
				}
				else
				{
					m_cb();
				}
			}

		public:
			pybind::object m_cb;
		};
		//////////////////////////////////////////////////////////////////////////
		typedef stdex::intrusive_ptr<PythonSceneChangeCallback> PythonSceneChangeCallbackPtr;
		//////////////////////////////////////////////////////////////////////////
		typedef FactoryPoolStore<PythonSceneChangeCallback, 8> TFactoryPythonSceneChangeCallback;
		TFactoryPythonSceneChangeCallback m_factoryPythonSceneChangeCallback;
        //////////////////////////////////////////////////////////////////////////
        bool setCurrentScene( const ConstString & _prototype, const ConstString & _name, bool _destroyOld, const pybind::object & _cb )
        {
            LOGGER_INFO(m_serviceProvider)( "set current scene '%s'"
                , _name.c_str()
                );

			if( PLAYER_SERVICE( m_serviceProvider )
				->isChangedScene() == true )
			{
				return false;
			}

			PythonSceneChangeCallbackPtr py_cb = m_factoryPythonSceneChangeCallback.createObject();

			py_cb->initialize( _cb );

			Scene * scene = PLAYER_SERVICE( m_serviceProvider )
				->getCurrentScene();

			if( scene != nullptr && scene->getName() == _name )
			{
				if( PLAYER_SERVICE( m_serviceProvider )
					->restartCurrentScene( py_cb ) == false )
				{
					return false;
				}
			}
			else
			{
				Scene * scene = PROTOTYPE_SERVICE( m_serviceProvider )
					->generatePrototypeT<Scene>( CONST_STRING( m_serviceProvider, Scene ), _prototype );

				if( scene == nullptr )
				{
					return false;
				}

				scene->setName( _name );

				if( PLAYER_SERVICE( m_serviceProvider )
					->setCurrentScene( scene, _destroyOld, py_cb ) == false )
				{
					scene->destroy();

					return false;
				}
			}

			return true;
        }
        //////////////////////////////////////////////////////////////////////////
        Scene * getCurrentScene()
        {
            Scene * scene = PLAYER_SERVICE(m_serviceProvider)
                ->getCurrentScene();

            return scene;
        }
        //////////////////////////////////////////////////////////////////////////
        Scene * s_createScene( const ConstString & _prototype, const ConstString & _name )
        {
            Scene * scene = PROTOTYPE_SERVICE(m_serviceProvider)
				->generatePrototypeT<Scene>( CONST_STRING( m_serviceProvider, Scene ), _prototype );

			scene->setName( _name );

            return scene;
        }
        //////////////////////////////////////////////////////////////////////////
        void s_setArrow( const ConstString & _prototype )
        {
            Arrow * arrow = PROTOTYPE_SERVICE(m_serviceProvider)
                ->generatePrototypeT<Arrow>( CONST_STRING(m_serviceProvider, Arrow), _prototype );

            if( arrow == nullptr )
            {
                LOGGER_ERROR(m_serviceProvider)("Error: can't setup arrow '%s'"
                    , _prototype.c_str()
                    );

                return;
            }

            PLAYER_SERVICE(m_serviceProvider)
                ->setArrow( arrow );
        }
        //////////////////////////////////////////////////////////////////////////
        const pybind::object & s_getArrow()
        {
            Arrow * arrow = PLAYER_SERVICE(m_serviceProvider)
                ->getArrow();

			if( arrow == nullptr )
			{
				return pybind::object::get_invalid();
			}

			const pybind::object & py_arrow = arrow->getScriptObject();

			return py_arrow;
        }
        //////////////////////////////////////////////////////////////////////////
        const Resolution & s_getCurrentResolution()
        {
            return APPLICATION_SERVICE(m_serviceProvider)
                ->getCurrentResolution();
        }
        //////////////////////////////////////////////////////////////////////////
        const Resolution & s_getContentResolution()
        {
            return APPLICATION_SERVICE(m_serviceProvider)
                ->getContentResolution();
        }
		//////////////////////////////////////////////////////////////////////////
		void s_setNopause( bool _value )
		{
			return APPLICATION_SERVICE( m_serviceProvider )
				->setNopause( _value );
		}
		//////////////////////////////////////////////////////////////////////////
		bool s_getNopause()
		{
			return APPLICATION_SERVICE( m_serviceProvider )
				->getNopause();
		}
        //////////////////////////////////////////////////////////////////////////
        bool s_setArrowLayer( Layer * _layer )
        {
			if( _layer == nullptr )
			{
				return false;
			}

            Arrow * arrow = PLAYER_SERVICE(m_serviceProvider)
                ->getArrow();

			if( arrow == nullptr )
			{
				return false;
			}

            _layer->addChild( arrow );

			return true;
        }
        //////////////////////////////////////////////////////////////////////////
        mt::vec2f s_getHotSpotImageSize( HotSpotImage * _hotspotImage )
        {
            if( _hotspotImage == 0 || _hotspotImage->isCompile() == false )
            {
                return mt::vec2f(0.f,0.f);
            }

            const ResourceHITPtr & resourceHIT = _hotspotImage->getResourceHIT();

            mt::vec2f size;
            size.x = (float)resourceHIT->getWidth();
            size.y = (float)resourceHIT->getHeight();

            return size;
        }
        //////////////////////////////////////////////////////////////////////////
        const mt::vec2f& s_getCamera2DPosition()
        {
			const RenderViewportInterface * rv = PLAYER_SERVICE(m_serviceProvider)
				->getRenderViewport();

			const Viewport & vp = rv->getViewport();

            return vp.begin;
        }
        //////////////////////////////////////////////////////////////////////////
        uint32_t s_Animatable_play( Animatable * _animatable )
        {
            float time = TIMELINE_SERVICE(m_serviceProvider)
				->getTime();

            uint32_t id = _animatable->play( time );

            return id;
        }
		//////////////////////////////////////////////////////////////////////////
		void s_Animatable_resume( Animatable * _animatable )
		{
			float time = TIMELINE_SERVICE( m_serviceProvider )
				->getTime();

			_animatable->resume( time );
		}
        //////////////////////////////////////////////////////////////////////////
        void destroyNode( Node * _node )
        {
			if( _node == nullptr)
			{
				LOGGER_ERROR(m_serviceProvider)("Menge.destroyNode invalid take None object"
					);

				return;
			}

			_node->removeFromParent();
            _node->destroy();
        }
        //////////////////////////////////////////////////////////////////////////
        Node * createNode( const ConstString & _type )
        {
			Node * node = NODE_SERVICE( m_serviceProvider )
				->createNode( _type );

            if( node == nullptr )
            {
                return nullptr;
            }

            NODE_SERVICE(m_serviceProvider)
                ->addHomeless( node );
        
            return node;
        }
        //////////////////////////////////////////////////////////////////////////
        void quitApplication()
        {
            APPLICATION_SERVICE(m_serviceProvider)
				->quit();
        }
        //////////////////////////////////////////////////////////////////////////
        bool directResourceCompile( const ConstString & _nameResource )
        {
            ResourceReferencePtr resource;				
			if( RESOURCE_SERVICE(m_serviceProvider)
                ->hasResource( _nameResource, &resource ) == false )
			{
				LOGGER_ERROR(m_serviceProvider)("directResourceCompile: not found resource '%s'"
					, _nameResource.c_str()
					);

				return false;
			}

			if( resource->incrementReference() == false )
			{
				LOGGER_ERROR(m_serviceProvider)("directResourceCompile: resource '%s' type '%s' invalid compile"
					, _nameResource.c_str()
					, resource->getType().c_str()
					);

				return false;
			}

            return true;
        }
		//////////////////////////////////////////////////////////////////////////
		bool directResourceRelease( const ConstString & _nameResource )
		{
			ResourceReferencePtr resource;				
			if( RESOURCE_SERVICE(m_serviceProvider)
				->hasResource( _nameResource, &resource ) == false )
			{
				LOGGER_ERROR(m_serviceProvider)("directResourceRelease: not found resource '%s'"
					, _nameResource.c_str()
					);
				
				return false;
			}

			if( resource->decrementReference() == false )
			{
				LOGGER_ERROR(m_serviceProvider)("directResourceCompile: resource '%s' type '%s' invalid release"
					, _nameResource.c_str()
					, resource->getType().c_str()
					);

				return false;
			}

			return true;
		}
		//////////////////////////////////////////////////////////////////////////
		bool directFontCompile( const ConstString & _fontName )
		{
			bool successful = TEXT_SERVICE( m_serviceProvider )
				->directFontCompile( _fontName );

			return successful;
		}
		//////////////////////////////////////////////////////////////////////////
		bool directFontRelease( const ConstString & _fontName )
		{
			bool successful = TEXT_SERVICE( m_serviceProvider )
				->directFontRelease( _fontName );

			return successful;
		}
        //////////////////////////////////////////////////////////////////////////
		ResourceReferencePtr s_getResourceReference( const ConstString & _nameResource )
        {
            const ResourceReferencePtr & resource = RESOURCE_SERVICE(m_serviceProvider)
                ->getResourceReference( _nameResource );

            if( resource == nullptr )
            {
                LOGGER_ERROR(m_serviceProvider)("Menge.getResourceReference: not exist resource %s"
                    , _nameResource.c_str() 
                    );

                return nullptr;
            }

            return resource;
        }
        //////////////////////////////////////////////////////////////////////////
        void s_cancelTask( ThreadTask * _task )
        {
            _task->cancel();
        }
        //////////////////////////////////////////////////////////////////////////
        bool s_joinTask( ThreadTask * _task )
        {
            return THREAD_SERVICE(m_serviceProvider)
                ->joinTask( _task );
        }
        //////////////////////////////////////////////////////////////////////////
        void s_setFullscreenMode( bool _fullscreen )
        {
            APPLICATION_SERVICE(m_serviceProvider)
                ->setFullscreenMode( _fullscreen );
        }
        //////////////////////////////////////////////////////////////////////////
        bool s_getFullscreenMode()
        {
            return APPLICATION_SERVICE(m_serviceProvider)
                ->getFullscreenMode();
        }
        //////////////////////////////////////////////////////////////////////////
        void s_setFixedContentResolution( bool _fullscreen )
        {
            APPLICATION_SERVICE(m_serviceProvider)
                ->setFixedContentResolution( _fullscreen );
        }
        //////////////////////////////////////////////////////////////////////////
        bool s_getFixedContentResolution()
        {
            return APPLICATION_SERVICE(m_serviceProvider)
                ->getFixedContentResolution();
        }
        //////////////////////////////////////////////////////////////////////////
        void renderOneFrame()
        {
            RENDER_SERVICE(m_serviceProvider)
                ->beginScene();

            GAME_SERVICE(m_serviceProvider)
                ->render();

            RENDER_SERVICE(m_serviceProvider)
                ->endScene();
        }
        //////////////////////////////////////////////////////////////////////////
        void writeImageToFile( const ConstString& _resource, const FilePath& _fileName )
        {
            ResourceImagePtr resource = RESOURCE_SERVICE(m_serviceProvider)
                ->getResourceT<ResourceImagePtr>( _resource );

            if( resource == nullptr )
            {
                LOGGER_ERROR(m_serviceProvider)("Error: Image resource not getting '%s'"
                    , _resource.c_str() 
                    );

                return;
            }

            const RenderTextureInterfacePtr & texture = resource->getTexture();

            RENDERTEXTURE_SERVICE(m_serviceProvider)
                ->saveImage( texture, CONST_STRING(m_serviceProvider, user), STRINGIZE_STRING_LOCAL(m_serviceProvider, "pngImage"), _fileName );
        }
        //////////////////////////////////////////////////////////////////////////
        void setParticlesEnabled( bool _enabled )
        {
            APPLICATION_SERVICE(m_serviceProvider)
                ->setParticleEnable( _enabled );
        }
        //////////////////////////////////////////////////////////////////////////
		ResourceImageDefaultPtr s_createImageResource( const ConstString & _resourceName, const ConstString& _pakName, const FilePath& _fileName, const mt::vec2f & _maxSize )
        {
			mt::vec2f maxSize;

			if( _maxSize.x < 0.f || _maxSize.y < 0.f )
			{
				InputStreamInterfacePtr stream = FILE_SERVICE( m_serviceProvider )
					->openInputFile( _pakName, _fileName, false );

				if( stream == nullptr )
				{
					return nullptr;
				}

				ConstString codecType = CODEC_SERVICE( m_serviceProvider )
					->findCodecType( _fileName );

				ImageDecoderInterfacePtr imageDecoder = CODEC_SERVICE( m_serviceProvider )
					->createDecoderT<ImageDecoderInterfacePtr>( codecType );

				if( imageDecoder == nullptr )
				{
					return nullptr;
				}

				if( imageDecoder->prepareData( stream ) == false )
				{
					return nullptr;
				}

				const ImageCodecDataInfo * dataInfo = imageDecoder->getCodecDataInfo();

				maxSize.x = (float)dataInfo->width;
				maxSize.y = (float)dataInfo->height;
			}
			else
			{
				maxSize = _maxSize;
			}

			ResourceImageDefaultPtr resource = RESOURCE_SERVICE( m_serviceProvider )
				->generateResourceT<ResourceImageDefaultPtr>( ConstString::none(), ConstString::none(), ConstString::none(), _resourceName, CONST_STRING( m_serviceProvider, ResourceImageDefault ) );

			if( resource == nullptr )
			{
				return nullptr;
			}

			mt::uv4f uv_image;
			mt::uv4f uv_alpha;
			
			resource->setup( _fileName, ConstString::none(), uv_image, uv_alpha, maxSize );
			
			return resource;
        }
		//////////////////////////////////////////////////////////////////////////
		ResourceImageSolidPtr s_createImageSolidResource( const ConstString & _resourceName, const ColourValue & _colour, const mt::vec2f & _maxSize )
		{
			ResourceImageSolidPtr resource = RESOURCE_SERVICE( m_serviceProvider )
				->generateResourceT<ResourceImageSolidPtr>( ConstString::none(), ConstString::none(), ConstString::none(), _resourceName, CONST_STRING( m_serviceProvider, ResourceImageSolid ) );

			if( resource == nullptr )
			{
				return nullptr;
			}
						
			resource->setColor( _colour );

			resource->setMaxSize( _maxSize );
			resource->setSize( _maxSize );			

			return resource;
		}
        //////////////////////////////////////////////////////////////////////////
        void minimizeWindow()
        {
            APPLICATION_SERVICE(m_serviceProvider)
                ->minimizeWindow();
        }
		//////////////////////////////////////////////////////////////////////////
		bool s_calcMouseScreenPosition( const mt::vec2f & _pos, mt::vec2f & _screen )
		{
			Arrow * arrow = PLAYER_SERVICE(m_serviceProvider)
				->getArrow();

			if( arrow == nullptr )
			{
				return false;
			}

			const RenderCameraInterface * renderCamera = arrow->getRenderCamera();
			const RenderViewportInterface * renderViewport = arrow->getRenderViewport();

			arrow->calcMouseScreenPosition( renderCamera, renderViewport, _pos, _screen );

			return true;
		}
        //////////////////////////////////////////////////////////////////////////
        void s_pushMouseMove( uint32_t _touchId, const mt::vec2f & _pos )
        {
            const mt::vec2f & cp = INPUT_SERVICE(m_serviceProvider)
				->getCursorPosition( _touchId );

			mt::vec2f pos_screen;
			this->s_calcMouseScreenPosition( _pos, pos_screen );

            mt::vec2f mp = pos_screen - cp;
			
            INPUT_SERVICE(m_serviceProvider)
				->pushMouseMoveEvent( _touchId, cp.x, cp.y, mp.x, mp.y, 0.f );
        }
        //////////////////////////////////////////////////////////////////////////
        void s_pushMouseButtonEvent( uint32_t _touchId, const mt::vec2f & _pos, unsigned int _button, bool _isDown )
        {
			mt::vec2f pos_screen;
			this->s_calcMouseScreenPosition( _pos, pos_screen );

            INPUT_SERVICE(m_serviceProvider)
				->pushMouseButtonEvent( _touchId, pos_screen.x, pos_screen.y, _button, 0.f, _isDown );
        }
        //////////////////////////////////////////////////////////////////////////
        void s_platformEvent( const ConstString & _event, const TMapParams & _params )
        {
            PLATFORM_SERVICE(m_serviceProvider)
                ->onEvent( _event, _params );
        }
		//////////////////////////////////////////////////////////////////////////
		const ConstString & s_getProjectCodename()
		{
			const ConstString & codename = APPLICATION_SERVICE(m_serviceProvider)
				->getProjectCodename();

			return codename;
		}
		//////////////////////////////////////////////////////////////////////////
		void s_sleep( uint32_t _time )
		{
			THREAD_SYSTEM(m_serviceProvider)
				->sleep( _time );
		}
		//////////////////////////////////////////////////////////////////////////
		class PyHttpDownloadAssetReceiver
			: public HttpDownloadAssetReceiver
		{
		public:
			PyHttpDownloadAssetReceiver( const pybind::object & _cb )
				: m_cb(_cb)
			{
			}

			~PyHttpDownloadAssetReceiver()
			{
			}

		protected:
			void onDownloadAssetComplete( uint32_t _id, bool _successful ) override
			{
				m_cb( _id, _successful );
			}

		protected:
			pybind::object m_cb;
		};
		//////////////////////////////////////////////////////////////////////////
		uint32_t s_downloadAsset( const String & _url, const ConstString & _category, const FilePath & _filepath, const pybind::object & _cb )
		{
			uint32_t id = HTTP_SYSTEM(m_serviceProvider)
				->downloadAsset( _url, _category, _filepath, new PyHttpDownloadAssetReceiver(_cb) );

			return id;
		}
		//////////////////////////////////////////////////////////////////////////
		void s_cancelDownloadAsset( uint32_t _id )
		{
			HTTP_SYSTEM(m_serviceProvider)
				->cancelAsset( _id );
		}
		//////////////////////////////////////////////////////////////////////////
		bool s_loadResourcePak( const ConstString & _fileGroup
			, const ConstString & _name
			, const ConstString & _type			
			, const ConstString & _path
			, const ConstString & _descriptionPath
			)
		{
			FileGroupInterfacePtr fileGroup;
			if( FILE_SERVICE(m_serviceProvider)
				->hasFileGroup( _fileGroup, &fileGroup ) == false )
			{
				LOGGER_ERROR(m_serviceProvider)("Menge.loadResourcePak invalid found file group %s"
					, _fileGroup.c_str()
					);

				return false;
			}

			PackageDesc desc;
			desc.dev = false;
			desc.immediately = true;
			desc.preload = false;

			desc.name = _name;
			desc.type = _type;

			desc.path = _path;
			desc.descriptionPath = _descriptionPath;
			
			bool result = PACKAGE_SERVICE(m_serviceProvider)
				->addPackage( desc );

			return result;
		}
		//////////////////////////////////////////////////////////////////////////
		bool s_existFile( const ConstString & _fileGroup, const FilePath & _path )
		{
			bool result = FILE_SERVICE(m_serviceProvider)
				->existFile( _fileGroup, _path, nullptr );

			return result;
		}
		//////////////////////////////////////////////////////////////////////////
		class PythonSaxCallback
		{
		public:
			PythonSaxCallback( ServiceProviderInterface * _serviceProvider, const pybind::object & _cb )
				: m_serviceProvider(_serviceProvider)
				, m_cb(_cb)
			{
			}

		protected:
			void operator = ( const PythonSaxCallback & )
			{
			}

		public:
			void callback_begin_node( const char * _node )
			{
				pybind::object py_cb_begin = m_cb.get_attr( "begin" );

				py_cb_begin( _node );
			}

			void callback_node_attributes( const char * _node, uint32_t _count, const char ** _keys, const char ** _values )
			{
				(void)_node;

				pybind::dict py_attr( _count );
				
				for( uint32_t i = 0; i != _count; ++i )
				{
					const char * key = _keys[i];
					const char * value = _values[i];
					
					py_attr[key] = value;					
				}

				pybind::object py_cb_attr = m_cb.get_attr( "attr" );

				py_cb_attr( py_attr );
			}

			void callback_end_node( const char * _node )
			{
				(void)_node;

				pybind::object py_cb_end = m_cb.get_attr( "end" );

				py_cb_end();
			}

		protected:
			ServiceProviderInterface * m_serviceProvider;
			pybind::object m_cb;
		};
		//////////////////////////////////////////////////////////////////////////
		bool s_parseXml( const ConstString & _fileGroup, const FilePath & _path, const pybind::object & _cb )
		{
			MemoryCacheBufferInterfacePtr binary_buffer = Helper::createMemoryCacheFileString( m_serviceProvider, _fileGroup, _path, false, "parseXml" );

			if( binary_buffer == nullptr )
			{
				return false;
			}

			char * memory = binary_buffer->getMemory();

			PythonSaxCallback pysc(m_serviceProvider, _cb);
			if( stdex::xml_sax_parse( memory, pysc ) == false )
			{
				return false;
			}

			return true;
		}
		//////////////////////////////////////////////////////////////////////////
		class MyVisitorTextFont
			: public VisitorTextFontInterface
		{
		public:
			MyVisitorTextFont( const pybind::object & _cb )
				: m_cb( _cb )
			{
			}

		protected:
			void onTextFont( const TextFontInterfacePtr & _font ) override
			{
				const ConstString & name = _font->getName();

				m_cb( name );
			}

		protected:
			pybind::object m_cb;

		private:
			void operator = (const MyVisitorTextFont &)
			{
			}
		};
		//////////////////////////////////////////////////////////////////////////
		void s_visitFonts( const pybind::object & _cb )
		{
			MyVisitorTextFont mvtf(_cb);

			TEXT_SERVICE(m_serviceProvider)
				->visitFonts( &mvtf );
		}
		//////////////////////////////////////////////////////////////////////////
		class MyVisitorCollectTextFont
			: public VisitorTextFontInterface
		{
		public:
			MyVisitorCollectTextFont( pybind::list & _l )
				: m_l( _l )
			{
			}

		protected:
			void onTextFont( const TextFontInterfacePtr & _font ) override
			{
				const ConstString & name = _font->getName();

				m_l.append( name );
			}

		protected:
			pybind::list & m_l;

		private:
			void operator = (const MyVisitorCollectTextFont &)
			{
			}
		};
		//////////////////////////////////////////////////////////////////////////
		pybind::list s_getFonts()
		{
			pybind::list l;
			MyVisitorCollectTextFont mvtf( l );

			TEXT_SERVICE( m_serviceProvider )
				->visitFonts( &mvtf );

			return l;
		}
		//////////////////////////////////////////////////////////////////////////
		bool s_hasFont( const ConstString & _fontName )
		{
			TextFontInterfacePtr font;
			bool has = TEXT_SERVICE(m_serviceProvider)
				->existFont( _fontName, font );

			return has;
		}
		//////////////////////////////////////////////////////////////////////////
		bool s_validateFont( const ConstString & _fontName, const String & _text, const pybind::object & _cb )
		{
			TextFontInterfacePtr font;
			if( TEXT_SERVICE(m_serviceProvider)
				->existFont( _fontName, font ) == false )
			{
				return false;
			}

			WString text_ws;

			if( Helper::utf8ToUnicode(m_serviceProvider, _text, text_ws) == false )
			{
				return false;
			}

			for( WString::const_iterator
				it = text_ws.begin(),
				it_end = text_ws.end();
			it != it_end;
			++it )
			{
				WChar ws_ch = *it;
				WString ws_str(&ws_ch, 1);

				Char text_utf8[8];
				size_t text_utf8_len = 0;
				if( UNICODE_SERVICE(m_serviceProvider)
					->unicodeToUtf8( ws_str.c_str(), ws_str.size(), text_utf8, 8, &text_utf8_len ) == false )
				{
					_cb( "invalid utf8 ", 0, ws_str );
					
					continue;
				}

				uint32_t code = 0;
				const char * test_text = text_utf8;
				utf8::internal::utf_error err = utf8::internal::validate_next( test_text, test_text + text_utf8_len, code );
				
				if( err != utf8::internal::UTF8_OK )
				{
					_cb( "validate utf8 ", code, ws_str );

					continue;
				}

				GlyphCode glyphChar;
				glyphChar.setCode( code );

				if( font->hasGlyph( glyphChar ) == false )
				{					
					_cb( "not found glyph ", code, ws_str );

					continue;
				}
			}

			return true;
		}
		//////////////////////////////////////////////////////////////////////////
		class PrefetchResourceVisitor
			: public Visitor
			, public ConcreteVisitor<ResourceImageDefault>
			, public ConcreteVisitor<ResourceSound>
			, public ConcreteVisitor<ResourceMovie>
		{
		public:
			PrefetchResourceVisitor( ServiceProviderInterface * _serviceProvider, const ConstString & _category )
				: m_serviceProvider( _serviceProvider )
				, m_category( _category )
			{
			}

			~PrefetchResourceVisitor()
			{
			}

		protected:
			void accept( ResourceImageDefault * _resource ) override
			{
				const FilePath & fileName = _resource->getFilePath();
				const ConstString & codecType = _resource->getCodecType();

				PREFETCHER_SERVICE( m_serviceProvider )
					->prefetchImageDecoder( m_category, fileName, codecType );
			}

			void accept( ResourceSound * _resource ) override
			{
				const FilePath & filePath = _resource->getFilePath();
				const ConstString & codecType = _resource->getCodecType();

				PREFETCHER_SERVICE( m_serviceProvider )
					->prefetchSoundDecoder( m_category, filePath, codecType );
			}

			void accept( ResourceMovie * _resource ) override
			{
				const FilePath & filePath = _resource->getFilePath();
				const ConstString & dataflowType = _resource->getDataflowType();

				PREFETCHER_SERVICE( m_serviceProvider )
					->prefetchData( m_category, filePath, dataflowType );
			}

		protected:
			ServiceProviderInterface * m_serviceProvider;
			ConstString m_category;
		};
		//////////////////////////////////////////////////////////////////////////
		void s_prefetchResources( const ConstString & _category, const ConstString & _groupName )
		{
			PrefetchResourceVisitor rv_gac( m_serviceProvider, _category );

			RESOURCE_SERVICE(m_serviceProvider)
				->visitGroupResources( _category, _groupName, &rv_gac );
		}
		//////////////////////////////////////////////////////////////////////////
		class UnfetchResourceVisitor
			: public Visitor
			, public ConcreteVisitor<ResourceImageDefault>
			, public ConcreteVisitor<ResourceSound>
			, public ConcreteVisitor<ResourceMovie>
		{
		public:
			UnfetchResourceVisitor( ServiceProviderInterface * _serviceProvider )
				: m_serviceProvider( _serviceProvider )
			{
			}

			~UnfetchResourceVisitor()
			{
			}

		protected:
			void accept( ResourceImageDefault * _resource ) override
			{
				const ConstString & category = _resource->getCategory();
				const FilePath & fileName = _resource->getFilePath();

				PREFETCHER_SERVICE( m_serviceProvider )
					->unfetch( category, fileName );
			}

			void accept( ResourceSound * _resource ) override
			{
				const ConstString & category = _resource->getCategory();
				const FilePath & filePath = _resource->getFilePath();

				PREFETCHER_SERVICE( m_serviceProvider )
					->unfetch( category, filePath );
			}

			void accept( ResourceMovie * _resource ) override
			{
				const ConstString & category = _resource->getCategory();
				const FilePath & fileName = _resource->getFilePath();

				PREFETCHER_SERVICE( m_serviceProvider )
					->unfetch( category, fileName );
			}

		protected:
			ServiceProviderInterface * m_serviceProvider;
		};
		//////////////////////////////////////////////////////////////////////////
		void s_unfetchResources( const ConstString & _category, const ConstString & _groupName )
		{
			UnfetchResourceVisitor rv_gac( m_serviceProvider );

			RESOURCE_SERVICE( m_serviceProvider )
				->visitGroupResources( _category, _groupName, &rv_gac );
		}
		//////////////////////////////////////////////////////////////////////////
		class CacheResourceVisitor
			: public Visitor
			, public ConcreteVisitor<ResourceReference>
		{
		public:
			CacheResourceVisitor( ServiceProviderInterface * _serviceProvider )
				: m_serviceProvider( _serviceProvider )
			{
			}

			~CacheResourceVisitor()
			{
			}

		protected:
			bool filterResource( ResourceReference * _resource ) const
			{
				if( _resource->isCompile() == false )
				{
					return false;
				}

				return true;
			}

		protected:
			void accept( ResourceReference * _resource ) override
			{
				if( this->filterResource( _resource ) == false )
				{
					return;
				}

				_resource->cache();
			}

		protected:
			ServiceProviderInterface * m_serviceProvider;
		};
		//////////////////////////////////////////////////////////////////////////
		void s_cacheResources( const ConstString & _category, const ConstString & _groupName )
		{
			CacheResourceVisitor rv_gac( m_serviceProvider );

			RESOURCE_SERVICE(m_serviceProvider)
				->visitGroupResources( _category, _groupName, &rv_gac );
		}
		//////////////////////////////////////////////////////////////////////////
		class UncacheResourceVisitor
			: public Visitor
			, public ConcreteVisitor<ResourceReference>
		{
		public:
			UncacheResourceVisitor( ServiceProviderInterface * _serviceProvider )
				: m_serviceProvider( _serviceProvider )
			{
			}

			~UncacheResourceVisitor()
			{
			}

		protected:
			bool filterResource( ResourceReference * _resource ) const
			{
				if( _resource->isCompile() == false )
				{
					return false;
				}

				if( _resource->isCache() == false )
				{
					return false;
				}

				return true;
			}

		protected:
			void accept( ResourceReference * _resource ) override
			{
				if( this->filterResource( _resource ) == false )
				{
					return;
				}

				_resource->uncache();
			}


		protected:
			ServiceProviderInterface * m_serviceProvider;
		};
		//////////////////////////////////////////////////////////////////////////
		void s_uncacheResources( const ConstString & _category, const ConstString & _groupName )
		{
			UncacheResourceVisitor rv_gac( m_serviceProvider );

			RESOURCE_SERVICE(m_serviceProvider)
				->visitGroupResources( _category, _groupName, &rv_gac );
		}
		//////////////////////////////////////////////////////////////////////////
		uint32_t s_rotateToTrimetric( const mt::vec2f & _dir, const mt::vec2f & _vx, const mt::vec2f & _vy )
		{
			float dir_angle = mt::signed_angle( _dir );

			float isometric_angle8[8] = {
				mt::signed_angle( +_vx ),
				mt::signed_angle( +_vx + _vy ),
				mt::signed_angle( +_vy ),
				mt::signed_angle( +_vy - _vx ),
				mt::signed_angle( -_vx ),
				mt::signed_angle( -_vx - _vy ),
				mt::signed_angle( -_vy ),
				mt::signed_angle( +_vx - _vy )
			};

			float min_angle = mt::m_two_pi;
			uint32_t isometric_index = 0;

			for( uint32_t i = 0; i != 8; ++i )
			{
				float isometric_angle = isometric_angle8[i];

				float angle_length = mt::angle_length( isometric_angle, dir_angle );

				float abs_angle_length = ::fabsf( angle_length );
				
				if( abs_angle_length < min_angle )
				{
					min_angle = abs_angle_length;
					isometric_index = i;
				}
			}

			return isometric_index;
		}
		//////////////////////////////////////////////////////////////////////////
		uint32_t s_rotateToTrimetric2( const mt::vec2f & _from, const mt::vec2f & _to, const mt::vec2f & _vx, const mt::vec2f & _vy )
		{
			uint32_t trimetric = s_rotateToTrimetric( _to - _from, _vx, _vy );

			return trimetric;
		}		
		//////////////////////////////////////////////////////////////////////////
		typedef stdex::vector<HotSpotPolygon *> TVectorHotSpotPolygon;
		//////////////////////////////////////////////////////////////////////////
		Polygon s_hotspotCorrect( HotSpotPolygon * _base, const TVectorHotSpotPolygon & _overlap )
		{
			(void)_base;
			//const Polygon & base_polygon = _base->getPolygon();

			Polygon correct_polygon;

			for( TVectorHotSpotPolygon::const_iterator
				it = _overlap.begin(),
				it_end = _overlap.end();
			it != it_end;
			++it )
			{
				const HotSpotPolygon * overlap_hotspot = *it;

				const Polygon & overlap_polygon = overlap_hotspot->getPolygon();

				TVectorPolygon output;
				correct_polygon.intersection( overlap_polygon, output );

				if( output.empty() == true )
				{
					return Polygon();
				}

				correct_polygon = output[0];
			}

			return correct_polygon;
		}
		//////////////////////////////////////////////////////////////////////////
		bool s_copyFile_( const ConstString & _resourceFileName, MemoryInterfacePtr & _blob )
		{
			ResourceFilePtr resourceFile = RESOURCE_SERVICE( m_serviceProvider )
				->getResourceT<ResourceFilePtr>( _resourceFileName );

			if( resourceFile == false )
			{
				return false;
			}

			const ConstString & category = resourceFile->getCategory();

			const FilePath & filePath = resourceFile->getFilePath();

			InputStreamInterfacePtr stream = FILE_SERVICE(m_serviceProvider)
				->openInputFile( category, filePath, false );

			if( stream == nullptr )
			{
				return false;
			}

			size_t size = stream->size();

			void * memory_buffer = _blob->newMemory( size );

			if( stream->read( memory_buffer, size ) != size )
			{
				return false;
			}

			resourceFile->decrementReference();

			return true;
		}
		//////////////////////////////////////////////////////////////////////////
		bool s_copyUserPicture( const ConstString & _resourceFileName, const String & _fileName )
		{
			MemoryInterfacePtr memory = MEMORY_SERVICE(m_serviceProvider)
				->createMemory();

			if( this->s_copyFile_( _resourceFileName, memory ) == false )
			{
				return false;
			}

			const WString & projectName = APPLICATION_SERVICE(m_serviceProvider)
				->getProjectName();

			WString wc_fileName;
			if( Helper::utf8ToUnicode( m_serviceProvider, _fileName, wc_fileName ) == false )
			{
				return false;
			}

			if( PLATFORM_SERVICE(m_serviceProvider)
				->createDirectoryUserPicture( projectName, wc_fileName, memory->getMemory(), memory->getSize() ) == false )
			{
				return false;
			}

			return true;
		}
		//////////////////////////////////////////////////////////////////////////
		bool s_copyUserMusic( const ConstString & _resourceFileName, const String & _fileName )
		{
			MemoryInterfacePtr memory = MEMORY_SERVICE( m_serviceProvider )
				->createMemory();

			if( this->s_copyFile_( _resourceFileName, memory ) == false )
			{
				return false;
			}

			const WString & projectName = APPLICATION_SERVICE(m_serviceProvider)
				->getProjectName();

			WString wc_fileName;
			if( Helper::utf8ToUnicode( m_serviceProvider, _fileName, wc_fileName ) == false )
			{
				return false;
			}

			if( PLATFORM_SERVICE(m_serviceProvider)
				->createDirectoryUserMusic( projectName, wc_fileName, memory->getMemory(), memory->getSize() ) == false )
			{
				return false;
			}

			return true;
		}
		//////////////////////////////////////////////////////////////////////////
		mt::vec2f s_screenToWorldPoint( Arrow * _arrow, const RenderCameraInterface * _camera, const RenderViewportInterface * _viewport, const mt::vec2f & _screenPoint )
		{
			if( _arrow == nullptr )
			{
				_arrow = PLAYER_SERVICE(m_serviceProvider)
					->getArrow();

				if( _arrow == nullptr )
				{
					return mt::vec2f( 0.f, 0.f );
				}
			}

			if( _camera == nullptr )
			{
				_camera = PLAYER_SERVICE(m_serviceProvider)
					->getRenderCamera();
			}

			if( _viewport == nullptr )
			{
				_viewport = PLAYER_SERVICE(m_serviceProvider)
					->getRenderViewport();
			}

			mt::vec2f wp;
			_arrow->calcMouseWorldPosition( _camera, _viewport, _screenPoint, wp );

			return wp;
		}
		//////////////////////////////////////////////////////////////////////////
		mt::vec2f s_screenToWorldClick( Arrow * _arrow, const RenderCameraInterface * _camera, const RenderViewportInterface * _viewport, const mt::vec2f & _screenPoint )
		{
			if( _arrow == nullptr )
			{
				_arrow = PLAYER_SERVICE(m_serviceProvider)
					->getArrow();

				if( _arrow == nullptr )
				{
					return mt::vec2f( 0.f, 0.f );
				}
			}

			if( _camera == nullptr )
			{
				_camera = PLAYER_SERVICE(m_serviceProvider)
					->getRenderCamera();
			}

			if( _viewport == nullptr )
			{
				_viewport = PLAYER_SERVICE(m_serviceProvider)
					->getRenderViewport();
			}

			mt::vec2f wp;
			_arrow->calcPointClick( _camera, _viewport, _screenPoint, wp );

			return wp;
		}
		//////////////////////////////////////////////////////////////////////////
		pybind::object s_getMovieSlotsPosition( const ConstString & _groupName, const ConstString & _movieName )
		{
			stdex::array_string<128> buffer;
			buffer.append( "Movie" );			
			buffer.append( _groupName );
			buffer.append( "_" );
			buffer.append( _movieName );

			ConstString resourceMovieName = Helper::stringizeStringSize( m_serviceProvider, buffer.c_str(), buffer.size() );

			ResourceMoviePtr resourceMovie;

			if( RESOURCE_SERVICE(m_serviceProvider)
				->hasResourceT<ResourceMoviePtr>( resourceMovieName, &resourceMovie ) == false )
			{
				LOGGER_ERROR(m_serviceProvider)("s_getMovieSlotsPosition: not found resource movie %s"
					, resourceMovieName.c_str()
					);

				return pybind::make_none_t();
			}

			pybind::list py_list;

			const TVectorMovieLayers & layers = resourceMovie->getLayers();

			for( TVectorMovieLayers::const_iterator
				it = layers.begin(),
				it_end = layers.end();
			it != it_end;
			++it )
			{
				const MovieLayer & layer = *it;

				if( layer.type != CONST_STRING(m_serviceProvider, MovieSlot) )
				{
					continue;
				}

				py_list.append( pybind::make_tuple_t( layer.name, layer.position ) );
			}

			return py_list;
		}
		//////////////////////////////////////////////////////////////////////////
		PyObject * s_getMovieSlotPosition( const ConstString & _groupName, const ConstString & _movieName, const ConstString & _slotName )
		{
			stdex::array_string<128> buffer;
			buffer.append( "Movie" );			
			buffer.append( _groupName );
			buffer.append( "_" );
			buffer.append( _movieName );

			ConstString resourceMovieName = Helper::stringizeStringSize( m_serviceProvider, buffer.c_str(), buffer.size() );

			ResourceMoviePtr resourceMovie;

			if( RESOURCE_SERVICE(m_serviceProvider)
				->hasResourceT<ResourceMoviePtr>( resourceMovieName, &resourceMovie ) == false )
			{
				LOGGER_ERROR(m_serviceProvider)("getMovieSlotPosition: not found resource movie %s"
					, resourceMovieName.c_str()
					);

				return pybind::ret_none();
			}

			const MovieLayer * layer;
			if( resourceMovie->hasMovieLayer( _slotName, &layer ) == false )
			{
				LOGGER_ERROR(m_serviceProvider)("getMovieSlotPosition: movie %s not found slot %s"
					, resourceMovieName.c_str()
					, _slotName.c_str()
					);

				return pybind::ret_none();
			}

			PyObject * py_position = pybind::ptr( layer->position );

			return py_position;
		}
		//////////////////////////////////////////////////////////////////////////
		class AffectorGridBurnTransparency
			: public Affector
		{
		public:
			AffectorGridBurnTransparency()
				: m_grid( nullptr )
				, m_pos( 0.f, 0.f )
				, m_time( 0.f )
				, m_radius( 0.f )
				, m_ellipse( 0.f )
				, m_penumbra( 0.f )
				, m_timing( 0.f )
			{
			}

		public:
			void initialize( Grid2D * _grid, const mt::vec2f & _pos, float _time, float _radius, float _ellipse, float _penumbra, const pybind::object & _cb )
			{
				m_grid = _grid;
				m_pos = _pos;
				m_time = _time;
				m_radius = _radius;
				m_ellipse = _ellipse;
				m_penumbra = _penumbra;
				m_cb = _cb;
			}

		protected:
			bool _affect( float _timing ) override
			{
				m_timing += _timing;

				bool complete = false;
				if( m_timing + _timing > m_time )
				{
					m_timing = m_time;
					complete = true;
				}

				float a1 = m_timing / m_time;

				float radius = m_radius * a1;
				float penumbra = m_penumbra * a1;

				float width = m_grid->getWidth();
				float height = m_grid->getHeight();

				uint32_t countX = m_grid->getCountX();
				uint32_t countY = m_grid->getCountY();

				float pos_step_x = width / float( countX - 1 );
				float pos_step_y = height / float( countY - 1 );

				for( uint32_t j = 0; j != countY; ++j )
				{
					for( uint32_t i = 0; i != countX; ++i )
					{
						mt::vec2f grid_pos( pos_step_x * i, pos_step_y * j );
						
						mt::vec2f v = m_pos - grid_pos;

						v.y /= m_ellipse;

						float pos_sqrdistance = v.sqrlength();

						if( pos_sqrdistance < radius * radius )
						{
							m_grid->setGridColor( i, j, ColourValue( 0x00FFFFFF ) );
						}
						else if( pos_sqrdistance < (radius + penumbra) * (radius + penumbra) )
						{
							ColourValue cv;
							m_grid->getGridColor( i, j, cv );

							float cv_a = cv.getA();

							float pos_distance = sqrtf( pos_sqrdistance );

							float a0 = (pos_distance - radius) / penumbra;
							
							if( cv_a < a0 )
							{
								continue;
							}

							m_grid->setGridColor( i, j, ColourValue( a0, 1.f, 1.f, 1.f ) );
						}
					}
				}

				return complete;
			}

			void complete() override
			{
				if( m_cb.is_callable() == true )
				{
					m_cb( true );
				}
			}

			void stop() override
			{
				if( m_cb.is_callable() == true )
				{
					m_cb( false );
				}
			}

		protected:
			Grid2D * m_grid;

			mt::vec2f m_pos;
			float m_time;
			float m_radius;
			float m_ellipse;
			float m_penumbra;

			pybind::object m_cb;

			float m_timing;			
		};
		//////////////////////////////////////////////////////////////////////////
		FactoryPoolStore<AffectorGridBurnTransparency, 4> m_factoryAffectorGridBurnTransparency;
		//////////////////////////////////////////////////////////////////////////
		uint32_t s_gridBurnTransparency( Grid2D * _grid, const mt::vec2f & _pos, float _time, float _radius, float _ellipse, float _penumbra, const pybind::object & _cb )
		{
			AffectorGridBurnTransparency * affector = m_factoryAffectorGridBurnTransparency.createObject();

			affector->setServiceProvider( m_serviceProvider );
			affector->setAffectorType( ETA_USER );

			affector->initialize( _grid, _pos, _time, _radius, _ellipse, _penumbra, _cb );

			uint32_t id = _grid->addAffector( affector );

			return id;
		}
		//////////////////////////////////////////////////////////////////////////
		void s_setLocale( const ConstString & _locale )
		{
			APPLICATION_SERVICE( m_serviceProvider )
				->setLocale( _locale );
		}
        //////////////////////////////////////////////////////////////////////////
        mt::vec2f s_getCursorPosition()
        {
			mt::vec2f wp = this->s_getTouchPosition( 0 );

            return wp;
        }
		//////////////////////////////////////////////////////////////////////////
		mt::vec2f s_getTouchPosition( uint32_t _touchId )
		{
			const mt::vec2f & pos = INPUT_SERVICE( m_serviceProvider )
				->getCursorPosition( _touchId );

			mt::vec2f wp;
			PLAYER_SERVICE( m_serviceProvider )
				->calcGlobalMouseWorldPosition( pos, wp );

			return wp;
		}
        //////////////////////////////////////////////////////////////////////////
        bool s_isInViewport( const mt::vec2f & _pos )
        {
			const RenderViewportInterface * renderViewport = PLAYER_SERVICE(m_serviceProvider)
				->getRenderViewport();

			const Viewport & vp = renderViewport->getViewport();

			bool test = vp.testPoint( _pos );

            return test;
        }
        //////////////////////////////////////////////////////////////////////////
        void s_enableTextureFiltering( bool _enable )
        {
            (void)_enable;
        }
        //////////////////////////////////////////////////////////////////////////
        bool s_isTextureFilteringEnabled()
        {
            return true;
        }
        //////////////////////////////////////////////////////////////////////////
        bool s_existText( const ConstString & _key )
        {
            bool exist = TEXT_SERVICE(m_serviceProvider)
				->existText( _key, nullptr );

            return exist;
        }
        //////////////////////////////////////////////////////////////////////////
		pybind::list s_pickHotspot( const mt::vec2f & _point )
        {           
            MousePickerSystemInterface * mousePickerSystem = PLAYER_SERVICE(m_serviceProvider)
                ->getMousePickerSystem();
            
            TVectorPickerTraps traps;
            mousePickerSystem->pickTrap( _point, traps );

            pybind::list pyret;

            bool onFocus = APPLICATION_SERVICE(m_serviceProvider)
                ->isFocus();

            if( onFocus == false )
            {
                return pyret;
            }

            for( TVectorPickerTraps::iterator
                it = traps.begin(),
                it_end = traps.end();
            it != it_end;
            ++it )
            {
                MousePickerTrapInterface * mousePickerTrap = (*it);

				Scriptable * scriptable = mousePickerTrap->propagatePickerScriptable();

				pyret.append( scriptable );
            }

            return pyret;
        }
        //////////////////////////////////////////////////////////////////////////
        const ConstString & s_getPlatformName()
        {
            const ConstString & platformName = PLATFORM_SERVICE(m_serviceProvider)
                ->getPlatformName();

            return platformName;
        }
        //////////////////////////////////////////////////////////////////////////
        mt::vec2f s_getImageSize( Sprite * _sprite )
        {
            const ResourceImagePtr & resourceImage = _sprite->getResourceImage();

            if( resourceImage == nullptr )
            {
                LOGGER_ERROR(m_serviceProvider)("s_getImageSize sprite %s not setup resource"
                    , _sprite->getName().c_str()
                    );
				
				return  mt::vec2f(0.f, 0.f);
            }
            
			const mt::vec2f & size = resourceImage->getSize();

			return size;
        }
        //////////////////////////////////////////////////////////////////////////
        mt::vec2f s_getLocalImageCenter( Sprite * _sprite )
        {
            const mt::vec2f & imageSize = s_getImageSize( _sprite );

            mt::vec2f imageCenter(imageSize.x * 0.5f, imageSize.y * 0.5f);

            return imageCenter;
        }
        //////////////////////////////////////////////////////////////////////////
        mt::vec2f s_getWorldImageCenter( Sprite * _sprite )
        {
            mt::vec2f imageCenter = s_getLocalImageCenter( _sprite );

            const mt::mat4f & wm = _sprite->getWorldMatrix();

            mt::vec2f imageCenter_wm;
            mt::mul_v2_m4( imageCenter_wm, imageCenter, wm );

            return imageCenter_wm;
        }
        //////////////////////////////////////////////////////////////////////////
        Node * createChildren( Node * _node, const ConstString & _type )
        {
			if( _node == nullptr )
			{
				return nullptr;
			}

			Node * newNode = NODE_SERVICE( m_serviceProvider )
				->createNode( _type );

			if( newNode == nullptr )
			{
				return nullptr;
			}

            _node->addChild( newNode );

            return newNode;
        }
		//////////////////////////////////////////////////////////////////////////
		pybind::list getAllChildren( Node * _node )
		{
			TListNodeChild & children = _node->getChildren();

			size_t size = children.size();

			pybind::list py_children( size );

			size_t index = 0;

			for( TSlugChild it(children); it.eof() == false; )
			{
				Node * child = *it;

				it.next_shuffle();
				
				py_children[index++] = child;
			}

			return py_children;
		}
        //////////////////////////////////////////////////////////////////////////
        void moveStop( Node * _node )
        {
            _node->stopAffectors( ETA_POSITION );
			_node->setLinearSpeed( mt::vec3f(0.f, 0.f, 0.f) );
        }
		//////////////////////////////////////////////////////////////////////////
		class ScriptableAffectorCallback
			: public AffectorCallback
		{
		public:
			ScriptableAffectorCallback()
				: m_serviceProvider(nullptr)
				, m_scriptable(nullptr)
			{
			}

			~ScriptableAffectorCallback()
			{
			}

		public:
			void initialize( ServiceProviderInterface * _serviceProvider, Scriptable * _scriptable, const pybind::object & _cb )
			{
				m_serviceProvider = _serviceProvider;
				m_scriptable = _scriptable;
				m_cb = _cb;
			}

		protected:
			void onAffectorEnd( uint32_t _id, bool _isEnd ) override
			{
				if( m_cb.is_invalid() == true )
				{
					return;
				}

				if( m_cb.is_none() == true )
				{
					return;
				}

				m_cb( m_scriptable, _id, _isEnd );
			}

		protected:
			ServiceProviderInterface * m_serviceProvider;
			Scriptable * m_scriptable;

			pybind::object m_cb;
		};
		//////////////////////////////////////////////////////////////////////////
		FactoryPoolStore<ScriptableAffectorCallback, 4> m_factoryNodeAffectorCallback;
		//////////////////////////////////////////////////////////////////////////
		ScriptableAffectorCallback * createNodeAffectorCallback( Scriptable * _scriptable, const pybind::object & _cb )
		{
			ScriptableAffectorCallback * callback = m_factoryNodeAffectorCallback.createObject();

			callback->initialize( m_serviceProvider, _scriptable, _cb );

			return callback; 
		}
        //////////////////////////////////////////////////////////////////////////
        NodeAffectorCreator::NodeAffectorCreatorAccumulateLinear<Node, void (Node::*)( const mt::vec3f & ), mt::vec3f> m_nodeAffectorCreatorAccumulateLinear;
        //////////////////////////////////////////////////////////////////////////
        uint32_t velocityTo( Node * _node, float _speed, const mt::vec3f& _dir, const pybind::object & _cb )
        {
            if( _node->isActivate() == false )
            {
                return 0;
            }

			if( _node->isAfterActive() == false )
			{
				return 0;
			}

            moveStop( _node );

			ScriptableAffectorCallback * callback = createNodeAffectorCallback( _node, _cb );			

            Affector * affector = m_nodeAffectorCreatorAccumulateLinear.create( m_serviceProvider
                , ETA_POSITION, callback, _node, &Node::setLocalPosition
				, _node->getLocalPosition(), _dir, _speed, &mt::sqrlength_v3
                );

            mt::vec3f linearSpeed = _dir * _speed;
            _node->setLinearSpeed( linearSpeed );

            AFFECTOR_ID id = _node->addAffector( affector );

            return id;
        }
        NodeAffectorCreator::NodeAffectorCreatorInterpolateLinear<Node, void (Node::*)( const mt::vec3f & ), mt::vec3f> m_nodeAffectorCreatorInterpolateLinear;
        //////////////////////////////////////////////////////////////////////////
		uint32_t moveTo( Node * _node, float _time, const mt::vec3f& _point, const pybind::object & _cb )
        {
            if( _node->isActivate() == false )
            {
                return 0;
            }

			if( _node->isAfterActive() == false )
			{
				return 0;
			}

            moveStop( _node );

			ScriptableAffectorCallback * callback = createNodeAffectorCallback( _node, _cb );

            Affector* affector = 
                m_nodeAffectorCreatorInterpolateLinear.create( m_serviceProvider
                , ETA_POSITION, callback, _node, &Node::setLocalPosition
                , _node->getLocalPosition(), _point, _time
				, &mt::sqrlength_v3
                );

            float invTime = 1.0f / _time;
            const mt::vec3f & pos = _node->getLocalPosition();
            mt::vec3f linearSpeed = ( _point - pos ) * invTime;

            _node->setLinearSpeed( linearSpeed );

            AFFECTOR_ID id = _node->addAffector( affector );

            return id;
        }
        //////////////////////////////////////////////////////////////////////////
        NodeAffectorCreator::NodeAffectorCreatorInterpolateQuadratic<Node, void (Node::*)( const mt::vec3f & ), mt::vec3f> m_nodeAffectorCreatorInterpolateQuadratic;
        //////////////////////////////////////////////////////////////////////////
		uint32_t accMoveTo( Node * _node, float _time, const mt::vec3f& _point, const pybind::object & _cb )
        {
            if( _node->isActivate() == false )
            {
                return 0;
            }

			if( _node->isAfterActive() == false )
			{
				return 0;
			}

            mt::vec3f linearSpeed = _node->getLinearSpeed();

            moveStop( _node );

			ScriptableAffectorCallback * callback = createNodeAffectorCallback( _node, _cb );

            Affector* affector = m_nodeAffectorCreatorInterpolateQuadratic.create( m_serviceProvider
                , ETA_POSITION, callback, _node, &Node::setLocalPosition
                , _node->getLocalPosition(), _point, linearSpeed, _time
				, &mt::sqrlength_v3
                );

            AFFECTOR_ID id = _node->addAffector( affector );

            return id;
        }
        //////////////////////////////////////////////////////////////////////////
        NodeAffectorCreator::NodeAffectorCreatorInterpolateQuadraticBezier<Node, void (Node::*)( const mt::vec3f &), mt::vec3f> m_nodeAffectorCreatorInterpolateQuadraticBezier;
        //////////////////////////////////////////////////////////////////////////
        uint32_t bezier2To( Node * _node
            , float _time
            , const mt::vec3f& _point1
            , const mt::vec3f& _point2
			, const pybind::object & _cb )
        {
            if( _node->isActivate() == false )
            {
                return 0;
            }

			if( _node->isAfterActive() == false )
			{
				return 0;
			}

            moveStop( _node );

			ScriptableAffectorCallback * callback = createNodeAffectorCallback( _node, _cb );

            Affector* affector = m_nodeAffectorCreatorInterpolateQuadraticBezier.create( m_serviceProvider
                , ETA_POSITION, callback, _node, &Node::setLocalPosition
                , _node->getLocalPosition(), _point1, _point2, _time
				, &mt::sqrlength_v3
                );

            AFFECTOR_ID id = _node->addAffector( affector );

            return id;
        }
        //////////////////////////////////////////////////////////////////////////
        NodeAffectorCreator::NodeAffectorCreatorInterpolateCubicBezier<Node, void (Node::*)( const mt::vec3f &), mt::vec3f> m_nodeAffectorCreatorInterpolateCubicBezier;
        //////////////////////////////////////////////////////////////////////////
        uint32_t bezier3To( Node * _node
            , float _time
            , const mt::vec3f& _point1
            , const mt::vec3f& _point2
            , const mt::vec3f& _point3
			, const pybind::object & _cb )
        {
            if( _node->isActivate() == false )
            {
                return 0;
            }

			if( _node->isAfterActive() == false )
			{
				return 0;
			}

            moveStop( _node );

			ScriptableAffectorCallback * callback = createNodeAffectorCallback( _node, _cb );

            Affector* affector = 
                m_nodeAffectorCreatorInterpolateCubicBezier.create( m_serviceProvider
                , ETA_POSITION, callback, _node, &Node::setLocalPosition
                , _node->getLocalPosition(), _point1, _point2, _point3, _time
                , &mt::length_v3
                );

            AFFECTOR_ID id = _node->addAffector( affector );

            return id;
        }
		//////////////////////////////////////////////////////////////////////////
		class FactoryAffectorInterpolateParabolic
		{
		public:
			class AffectorCreatorInterpolateParabolic
				: public CallbackAffector
			{
			public:
				AffectorCreatorInterpolateParabolic()
					: m_node( nullptr )
					, m_speed(2.f)
				{ 
				}

			public:
				void setNode( Node * _node )
				{
					m_node = _node;
				}

			public:
				void initialize( const mt::vec3f & _end, const mt::vec3f & _v0, float _time )
				{
					const mt::vec3f & start_position = m_node->getLocalPosition();

					m_interpolator.start( start_position, _end, _v0, _time, mt::sqrlength_v3 );

					mt::vec3f next_position;
					m_interpolator.step( 100.f, &next_position );

					mt::vec3f dir;
					mt::dir_v3_v3( dir, next_position, start_position );

					m_prevDir = dir;
					m_currentDir = dir;
					m_targetDir = dir;

					m_node->setDirection( dir, mt::vec3f( 0.f, 0.f, 1.f ) );
				}

			protected:
				bool _affect( float _timing ) override
				{
					mt::vec3f position;
					bool finish = m_interpolator.update( _timing, &position );

					this->updateDirection_( _timing, position );
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
				void updateDirection_( float _timing, const mt::vec3f & _position )
				{
					const mt::vec3f & prev_position = m_node->getLocalPosition();

					if( mt::sqrlength_v3_v3( prev_position, _position ) > mt::m_eps )
					{
						mt::dir_v3_v3( m_targetDir, _position, prev_position );
					}
					
					float length = mt::length_v3_v3( m_targetDir, m_currentDir );

					if( length < mt::m_eps )
					{
						return;
					}

					float t = length / _timing * m_speed;

					if( t > 1.f )
					{
						m_currentDir = m_targetDir;

						m_node->setDirection( m_currentDir, mt::vec3f( 0.f, 0.f, 1.f ) );
					}
					else
					{
						m_currentDir = m_currentDir + (m_targetDir - m_currentDir) * t;

						m_node->setDirection( m_currentDir, mt::vec3f( 0.f, 0.f, 1.f ) );
					}
				}

				void updatePosition_( const mt::vec3f & _position )
				{
					const mt::vec3f & prev_position = m_node->getLocalPosition();

					if( mt::sqrlength_v3_v3( prev_position, _position ) < mt::m_eps )
					{
						return;
					}

					m_node->setLocalPosition( _position );
				}

			protected:
				Node * m_node;

				mt::vec3f m_prevDir;
				mt::vec3f m_currentDir;
				mt::vec3f m_targetDir;
				float m_speed;

				ValueInterpolatorParabolic<mt::vec3f> m_interpolator;
			};

		public:
			Affector * create( ServiceProviderInterface * _serviceProvider, EAffectorType _type, const AffectorCallbackPtr & _cb
				, Node * _node, const mt::vec3f & _end, const mt::vec3f & _v0, float _time )
			{
				AffectorCreatorInterpolateParabolic * affector = m_factory.createObject();

				affector->setServiceProvider( _serviceProvider );
				affector->setAffectorType( _type );

				affector->setCallback( _cb );

				affector->setNode( _node );
												
				affector->initialize( _end, _v0, _time );

				return affector;
			}

		protected:
			typedef FactoryPoolStore<AffectorCreatorInterpolateParabolic, 4> TFactoryAffector;
			TFactoryAffector m_factory;
		};
		//////////////////////////////////////////////////////////////////////////
		FactoryAffectorInterpolateParabolic m_nodeAffectorCreatorInterpolateParabolic;
		//////////////////////////////////////////////////////////////////////////
		uint32_t parabolaTo( Node * _node
			, float _time
			, const mt::vec3f& _end
			, const mt::vec3f& _v0
			, const pybind::object & _cb )
		{
			if( _node->isActivate() == false )
			{
				return 0;
			}

			if( _node->isAfterActive() == false )
			{
				return 0;
			}

			moveStop( _node );

			ScriptableAffectorCallback * callback = createNodeAffectorCallback( _node, _cb );

			Affector* affector =
				m_nodeAffectorCreatorInterpolateParabolic.create( m_serviceProvider
				, ETA_POSITION, callback, _node, _end, _v0, _time				
				);

			AFFECTOR_ID id = _node->addAffector( affector );

			return id;
		}
        //////////////////////////////////////////////////////////////////////////
        void angleStop( Node * _node )
        {
            _node->stopAffectors( ETA_ANGLE );
            _node->setAngularSpeed(0.f);
        }
        //////////////////////////////////////////////////////////////////////////
        NodeAffectorCreator::NodeAffectorCreatorInterpolateLinear<Node, void (Node::*)( float ), float> m_odeAffectorCreatorInterpolateLinear;
        //////////////////////////////////////////////////////////////////////////
		uint32_t angleTo( Node * _node, float _time, float _angle, const pybind::object & _cb )
        {
            if( _node->isActivate() == false )
            {
                return 0;
            }

			if( _node->isAfterActive() == false )
			{
				return 0;
			}

            angleStop( _node );

            float angle = _node->getOrientationZ();

            float correct_angle_from = angle;
            float correct_angle_to = _angle;
            //mt::angle_correct_interpolate_from_to( angle, _angle, correct_angle_from, correct_angle_to );

			ScriptableAffectorCallback * callback = createNodeAffectorCallback( _node, _cb );

            Affector* affector = m_odeAffectorCreatorInterpolateLinear.create( m_serviceProvider
                , ETA_ANGLE, callback, _node, &Node::setOrientationX
                , correct_angle_from, correct_angle_to, _time
                , &fabsf 
                );				

            float invTime = 1.0f / _time;
            float angularSpeed = fabsf( correct_angle_from - correct_angle_to ) * invTime;

            _node->setAngularSpeed( angularSpeed );				
            AFFECTOR_ID id = _node->addAffector( affector );

            return id;
        }
        //////////////////////////////////////////////////////////////////////////
        NodeAffectorCreator::NodeAffectorCreatorInterpolateQuadratic<Node, void (Node::*)( float ), float> m_nodeAffectorCreatorInterpolateQuadraticFloat;
        //////////////////////////////////////////////////////////////////////////
		uint32_t accAngleTo( Node * _node, float _time, float _angle, const pybind::object & _cb )
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

            angleStop( _node );

            float angle = _node->getOrientationZ();

            float correct_angle_from = angle;
            float correct_angle_to = _angle;
            //mt::angle_correct_interpolate_from_to( angle, _angle, correct_angle_from, correct_angle_to );

			ScriptableAffectorCallback * callback = createNodeAffectorCallback( _node, _cb );

            Affector* affector = 
                m_nodeAffectorCreatorInterpolateQuadraticFloat.create( m_serviceProvider
                , ETA_ANGLE, callback, _node, &Node::setOrientationX
                , correct_angle_from, correct_angle_to, angularSpeed, _time
                , &fabsf
                );				

			AFFECTOR_ID id = _node->addAffector( affector );

            return id;
        }
        //////////////////////////////////////////////////////////////////////////
        void scaleStop( Node * _node )
        {
            _node->stopAffectors( ETA_SCALE );
        }	
        //////////////////////////////////////////////////////////////////////////
		uint32_t scaleTo( Node * _node, float _time, const mt::vec3f& _scale, const pybind::object & _cb )
        {
            if( _node->isActivate() == false )
            {
                return 0;
            }

			if( _node->isAfterActive() == false )
			{
				return 0;
			}

            //if( mt::equal_f_z( _scale.x ) == true || mt::equal_f_z( _scale.y ) == true || mt::equal_f_z( _scale.z ) == true )
            //{
            //    LOGGER_ERROR(m_serviceProvider)("Node::scaleTo %s scale xyz not zero! (%f %f %f)"
            //        , _node->getName().c_str()
            //        , _scale.x
            //        , _scale.y
            //        , _scale.z
            //        );

            //    return 0;
            //}

            scaleStop( _node );

			ScriptableAffectorCallback * callback = createNodeAffectorCallback( _node, _cb );

            Affector* affector = m_nodeAffectorCreatorInterpolateLinear.create( m_serviceProvider
                , ETA_SCALE, callback, _node, &Node::setScale
                , _node->getScale(), _scale, _time
                , &mt::length_v3
                );

            AFFECTOR_ID id = _node->addAffector( affector );

            return id;
        }
        //////////////////////////////////////////////////////////////////////////
        void colorStop( Node * _node )
        {
            _node->stopAffectors( ETA_COLOR );
        }
        //////////////////////////////////////////////////////////////////////////
        NodeAffectorCreator::NodeAffectorCreatorInterpolateLinear<Node, void (Node::*)( const ColourValue &), ColourValue> m_nodeAffectorCreatorInterpolateLinearColour;
		//////////////////////////////////////////////////////////////////////////		
		inline static float s_length_color( const ColourValue & _rColor )
		{
			(void)_rColor;

			return 1.0f;
		}
        //////////////////////////////////////////////////////////////////////////
		uint32_t colorTo( Node * _node, float _time, const ColourValue& _color, const pybind::object & _cb )
        {
            if( _node->isActivate() == false )
            {
                return 0;
            }

			if( _node->isAfterActive() == false )
			{
				return 0;
			}

            colorStop( _node );

			ScriptableAffectorCallback * callback = createNodeAffectorCallback( _node, _cb );

            Affector* affector = 
                m_nodeAffectorCreatorInterpolateLinearColour.create( m_serviceProvider
                , ETA_COLOR, callback, _node, &Colorable::setLocalColor
                , _node->getLocalColor(), _color, _time
                , &s_length_color
                );

			AFFECTOR_ID id = _node->addAffector( affector );

            return id;
        }
        //////////////////////////////////////////////////////////////////////////
        uint32_t alphaTo( Node * _node, float _time, float _alpha, const pybind::object & _cb )
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

            uint32_t id = colorTo( _node, _time, color, _cb );

            return id;
        }
        //////////////////////////////////////////////////////////////////////////
        NodeAffectorCreator::NodeAffectorCreatorInterpolateLinear<Shape, void (Shape::*)( const mt::vec4f & ), mt::vec4f> m_NodeAffectorCreatorInterpolateLinearVec4;
        //////////////////////////////////////////////////////////////////////////
		uint32_t setPercentVisibilityTo( Shape * _shape, float _time, const mt::vec4f& _percent, const pybind::object & _cb )
        {
            if( _shape->isActivate() == false )
            {
                return 0;
            }

            _shape->stopAffectors( ETA_VISIBILITY );

			ScriptableAffectorCallback * callback = createNodeAffectorCallback( _shape, _cb );

            Affector* affector = m_NodeAffectorCreatorInterpolateLinearVec4.create( m_serviceProvider
                , ETA_VISIBILITY, callback, _shape, &Shape::setPercentVisibility
                , _shape->getPercentVisibility(), _percent, _time
                , &mt::length_v4 
                );

            AFFECTOR_ID id = _shape->addAffector( affector );

            return id;
        }
        //////////////////////////////////////////////////////////////////////////
        void setPercentVisibilityStop( Shape * _shape )
        {
            _shape->stopAffectors( ETA_VISIBILITY );
        }
        //////////////////////////////////////////////////////////////////////////
        mt::vec2f s_getCameraPosition( Node * _node, const RenderCameraInterface * _renderCamera )
        {
            mt::vec2f position;
            _node->getScreenPosition( _renderCamera, position );
            
            return position;
        }
		//////////////////////////////////////////////////////////////////////////
		mt::vec3f s_getWorldOffsetPosition( Node * _node, const mt::vec3f & _position )
		{
			const mt::vec3f & wp = _node->getWorldPosition();

			mt::vec3f offset = _position - wp;

			return offset;
		}
		//////////////////////////////////////////////////////////////////////////
		float s_getLengthTo( Node * _node, const mt::vec3f & _position )
		{
			const mt::vec3f & wp = _node->getWorldPosition();

			float length = mt::length_v3_v3( wp, _position );

			return length;
		}
        //////////////////////////////////////////////////////////////////////////
        const RenderCameraInterface * s_getRenderCamera2D()
        {
            const RenderCameraInterface * renderCamera = PLAYER_SERVICE(m_serviceProvider)
                ->getRenderCamera();

            return renderCamera;
        }
        //////////////////////////////////////////////////////////////////////////
        void showKeyboard()
        {
            APPLICATION_SERVICE(m_serviceProvider)
                ->showKeyboard();
        }
        //////////////////////////////////////////////////////////////////////////
        void hideKeyboard()
        {
            APPLICATION_SERVICE(m_serviceProvider)
                ->hideKeyboard();
        }
        //////////////////////////////////////////////////////////////////////////
        bool hasResource( const ConstString & _name )
        {
            return RESOURCE_SERVICE(m_serviceProvider)
                ->hasResource( _name, nullptr );
        }
        //////////////////////////////////////////////////////////////////////////
        void removeCurrentScene( const pybind::object & _cb )
        {
			PythonSceneChangeCallbackPtr py_cb = m_factoryPythonSceneChangeCallback.createObject();

			py_cb->initialize( _cb );

            PLAYER_SERVICE(m_serviceProvider)
				->removeCurrentScene( py_cb );
        }
		//////////////////////////////////////////////////////////////////////////
		class PyGlobalBaseHandler
			: public InputSystemHandler
			, public Factorable
		{ 
		public:
			PyGlobalBaseHandler()
				: m_serviceProvider( nullptr )
			{
			}

			~PyGlobalBaseHandler()
			{
			}

		public:
			void initialize( ServiceProviderInterface * _serviceProvider, const pybind::object & _cb )
			{
				m_serviceProvider = _serviceProvider;
				m_cb = _cb;
			}

			void finalize()
			{
			}

		protected:
			bool handleKeyEvent( const InputKeyEvent & _event ) override
			{
				(void)_event;

				return false;
			}

		protected:
			bool handleMouseButtonEvent( const InputMouseButtonEvent & _event ) override
			{ 
				(void)_event;

				return false;
			}
			
			bool handleMouseButtonEventBegin( const InputMouseButtonEvent & _event ) override
			{
				(void)_event;

				return false;
			}

			bool handleMouseButtonEventEnd( const InputMouseButtonEvent & _event ) override
			{
				(void)_event;

				return false;
			}

			bool handleMouseMove( const InputMouseMoveEvent & _event )
			{
				(void)_event;

				return false;
			}

			bool handleMouseWheel( const InputMouseWheelEvent & _event )
			{
				(void)_event;

				return false;
			}

		protected:
			ServiceProviderInterface * m_serviceProvider;
			pybind::object m_cb;
		};
        //////////////////////////////////////////////////////////////////////////
        class PyGlobalMouseMoveHandler
			: public PyGlobalBaseHandler
        {
        protected:
			bool handleMouseMove( const InputMouseMoveEvent & _event ) override
            {
				pybind::object py_result = m_cb( _event.touchId, _event.x, _event.y, _event.dx, _event.dy );

				if( py_result.is_none() == false )
                {
                    LOGGER_ERROR(m_serviceProvider)("PyGlobalMouseMoveHandler %s return value %s not None"
                        , m_cb.repr()
                        , py_result.repr()
                        );
                }

				return false;
            }
        };
		//////////////////////////////////////////////////////////////////////////
		typedef FactoryPoolStore<PyGlobalMouseMoveHandler, 32> TFactoryPyGlobalMouseMoveHandlers;
		TFactoryPyGlobalMouseMoveHandlers m_factoryPyGlobalMouseMoveHandlers;
        //////////////////////////////////////////////////////////////////////////
        uint32_t s_addMouseMoveHandler( const pybind::object & _cb )
        {
            GlobalHandleSystemInterface * globalHandleSystem = PLAYER_SERVICE(m_serviceProvider)
                ->getGlobalHandleSystem();

			PyGlobalMouseMoveHandler * handler = m_factoryPyGlobalMouseMoveHandlers.createObject();

			handler->initialize( m_serviceProvider, _cb );
            
            uint32_t id = globalHandleSystem->addGlobalHandler( handler );

            return id;
        }
        //////////////////////////////////////////////////////////////////////////
        class PyGlobalMouseHandlerButton
			: public PyGlobalBaseHandler
        {
        protected:
			bool handleMouseButtonEvent( const InputMouseButtonEvent & _event ) override
            {
				pybind::object py_result = m_cb( _event.touchId, _event.x, _event.y, _event.button, _event.isDown );

				if( py_result.is_none() == false )
                {
                    LOGGER_ERROR(m_serviceProvider)("PyGlobalMouseHandlerButton %s return value %s not None"
						, m_cb.repr()
                        , py_result.repr()
                        );
                }

				return false;
            }
        };
		//////////////////////////////////////////////////////////////////////////
		typedef FactoryPoolStore<PyGlobalMouseHandlerButton, 32> TFactoryPyGlobalMouseHandlerButtons;
		TFactoryPyGlobalMouseHandlerButtons m_factoryPyGlobalMouseHandlerButtons;
        //////////////////////////////////////////////////////////////////////////
        uint32_t s_addMouseButtonHandler( const pybind::object & _cb )
        {
            GlobalHandleSystemInterface * globalHandleSystem = PLAYER_SERVICE(m_serviceProvider)
                ->getGlobalHandleSystem();

            PyGlobalMouseHandlerButton * handler = m_factoryPyGlobalMouseHandlerButtons.createObject();
			
			handler->initialize( m_serviceProvider, _cb );

            uint32_t id = globalHandleSystem->addGlobalHandler( handler );

            return id;
        }
        //////////////////////////////////////////////////////////////////////////
        class PyGlobalMouseHandlerButtonEnd
			: public PyGlobalBaseHandler
        {
        protected:
            //////////////////////////////////////////////////////////////////////////
			bool handleMouseButtonEventEnd( const InputMouseButtonEvent & _event ) override
            {
				pybind::object py_result = m_cb( _event.touchId, _event.x, _event.y, _event.button, _event.isDown );

				if( py_result.is_none() == false )
				{
					LOGGER_ERROR( m_serviceProvider )("PyGlobalMouseHandlerButtonEnd %s return value %s not None"
						, m_cb.repr()
						, py_result.repr()
						);
				}

				return false;
            }
        };
		//////////////////////////////////////////////////////////////////////////
		typedef FactoryPoolStore<PyGlobalMouseHandlerButtonEnd, 32> TFactoryPyGlobalMouseHandlerButtonEnds;
		TFactoryPyGlobalMouseHandlerButtonEnds m_factoryPyGlobalMouseHandlerButtonEnds;
        //////////////////////////////////////////////////////////////////////////
		uint32_t s_addMouseButtonHandlerEnd( const pybind::object & _cb )
        {
            GlobalHandleSystemInterface * globalHandleSystem = PLAYER_SERVICE(m_serviceProvider)
                ->getGlobalHandleSystem();

            PyGlobalMouseHandlerButtonEnd * handler = m_factoryPyGlobalMouseHandlerButtonEnds.createObject();

			handler->initialize( m_serviceProvider, _cb );

            uint32_t id = globalHandleSystem->addGlobalHandler( handler );

            return id;
        }
		//////////////////////////////////////////////////////////////////////////
		class PyGlobalMouseHandlerWheel
			: public PyGlobalBaseHandler
		{
		protected:
			//////////////////////////////////////////////////////////////////////////
			bool handleMouseWheel( const InputMouseWheelEvent & _event ) override
			{
				pybind::object py_result = m_cb( _event.x, _event.y, _event.wheel );

				if( py_result.is_none() == false )
				{
					LOGGER_ERROR( m_serviceProvider )("PyGlobalMouseHandlerWheel %s return value %s not None"
						, m_cb.repr()
						, py_result.repr()
						);
				}

				return false;
			}
		};
		//////////////////////////////////////////////////////////////////////////
		typedef FactoryPoolStore<PyGlobalMouseHandlerWheel, 32> TFactoryPyGlobalMouseHandlerWheels;
		TFactoryPyGlobalMouseHandlerWheels m_factoryPyGlobalMouseHandlerWheels;
		//////////////////////////////////////////////////////////////////////////
		uint32_t s_addMouseWheelHandler( const pybind::object & _cb )
		{
			GlobalHandleSystemInterface * globalHandleSystem = PLAYER_SERVICE( m_serviceProvider )
				->getGlobalHandleSystem();

			PyGlobalMouseHandlerWheel * handler = m_factoryPyGlobalMouseHandlerWheels.createObject();

			handler->initialize( m_serviceProvider, _cb );

			uint32_t id = globalHandleSystem->addGlobalHandler( handler );

			return id;
		}
		//////////////////////////////////////////////////////////////////////////
		class PyGlobalMouseHandlerButtonBegin
			: public PyGlobalBaseHandler
		{
		protected:
			//////////////////////////////////////////////////////////////////////////
			bool handleMouseButtonEventBegin( const InputMouseButtonEvent & _event ) override
			{
				pybind::object py_result = m_cb( _event.touchId, _event.x, _event.y, _event.button, _event.isDown );

				if( py_result.is_none() == false )
				{
					LOGGER_ERROR( m_serviceProvider )("PyGlobalMouseHandlerButtonBegin %s return value %s not None"
						, m_cb.repr()
						, py_result.repr()
						);
				}

				return false;
			}
		};
		//////////////////////////////////////////////////////////////////////////
		typedef FactoryPoolStore<PyGlobalMouseHandlerButtonBegin, 32> TFactoryPyGlobalMouseHandlerButtonBegins;
		TFactoryPyGlobalMouseHandlerButtonBegins m_factoryPyGlobalMouseHandlerButtonBegins;
		//////////////////////////////////////////////////////////////////////////
		uint32_t s_addMouseButtonHandlerBegin( const pybind::object & _cb )
		{
			GlobalHandleSystemInterface * globalHandleSystem = PLAYER_SERVICE(m_serviceProvider)
				->getGlobalHandleSystem();

			PyGlobalMouseHandlerButtonBegin * handler = m_factoryPyGlobalMouseHandlerButtonBegins.createObject();
			
			handler->initialize( m_serviceProvider, _cb );
			
			uint32_t id = globalHandleSystem->addGlobalHandler( handler );

			return id;
		}
		//////////////////////////////////////////////////////////////////////////
		class PyGlobalKeyHandler
			: public PyGlobalBaseHandler
		{
		protected:
			bool handleKeyEvent( const InputKeyEvent & _event ) override
			{				
				pybind::object py_result = m_cb( (uint32_t)_event.key, _event.x, _event.y, _event.code, _event.isDown, _event.isRepeat );

				if( py_result.is_none() == false )
				{
					LOGGER_ERROR(m_serviceProvider)("PyGlobalKeyHandler %s return value %s not None"
						, m_cb.repr()
						, py_result.repr()
						);
				}

				return false;
			}
		};
		//////////////////////////////////////////////////////////////////////////
		typedef FactoryPoolStore<PyGlobalKeyHandler, 32> TFactoryPyGlobalKeyHandler;
		TFactoryPyGlobalKeyHandler m_poolPyGlobalKeyHandler;
		//////////////////////////////////////////////////////////////////////////
		uint32_t s_addKeyHandler( const pybind::object & _cb )
		{
			GlobalHandleSystemInterface * globalHandleSystem = PLAYER_SERVICE(m_serviceProvider)
				->getGlobalHandleSystem();

			PyGlobalKeyHandler * handler = m_poolPyGlobalKeyHandler.createObject();

			handler->initialize( m_serviceProvider, _cb );

			uint32_t id = globalHandleSystem->addGlobalHandler( handler );

			return id;
		}
		//////////////////////////////////////////////////////////////////////////
		bool s_removeGlobalHandler( uint32_t _id )
		{
			GlobalHandleSystemInterface * globalHandleSystem = PLAYER_SERVICE(m_serviceProvider)
				->getGlobalHandleSystem();

			InputSystemHandler * handler = globalHandleSystem->removeGlobalHandler( _id );

			PyGlobalBaseHandler * py_handler = dynamic_cast<PyGlobalBaseHandler *>(handler);

			if( py_handler == nullptr )
			{
				LOGGER_ERROR(m_serviceProvider)("s_removeKeyHandler %d handler invalid PyGlobalKeyHandler"
					, _id
					);

				return false;
			}

			py_handler->finalize();

			py_handler->destroy();

			return true;
		}
		//////////////////////////////////////////////////////////////////////////
		bool s_enableGlobalHandler( uint32_t _id, bool _value )
		{
			GlobalHandleSystemInterface * globalHandleSystem = PLAYER_SERVICE( m_serviceProvider )
				->getGlobalHandleSystem();

			bool successful = globalHandleSystem->enableGlobalHandler( _id, _value );

			return successful;
		}
        //////////////////////////////////////////////////////////////////////////
        float s_getMovieDuration( const ConstString & _resourceName )
        {
			ResourceMoviePtr resourceMovie = RESOURCE_SERVICE( m_serviceProvider )
                ->getResourceT<ResourceMoviePtr>( _resourceName );

            if( resourceMovie == nullptr )
            {
                LOGGER_ERROR(m_serviceProvider)("Mengine.getMovieDuration invalid movie resource '%s'"
                    , _resourceName.c_str()
                    );

                return 0.f;
            }

            float duration = resourceMovie->getDuration();

            resourceMovie->decrementReference();

            return duration;
        }
		//////////////////////////////////////////////////////////////////////////
		float s_getGameAspect()
		{
			float aspect;
			Viewport viewport;

			APPLICATION_SERVICE( m_serviceProvider )
				->getGameViewport( aspect, viewport );

			return aspect;
		}
        //////////////////////////////////////////////////////////////////////////
        Viewport s_getGameViewport()
        {
            float aspect;
            Viewport viewport;

            APPLICATION_SERVICE(m_serviceProvider)
                ->getGameViewport( aspect, viewport );

            return viewport;
        }
        //////////////////////////////////////////////////////////////////////////
        bool s_hasGameParam( const ConstString & _paramName )
        {
            if(	GAME_SERVICE(m_serviceProvider)
                ->hasParam( _paramName ) == false )
            {
                return false;
            }

            return true;
        }
        //////////////////////////////////////////////////////////////////////////
        PyObject * s_getGameParam( const ConstString & _paramName )
        {
            if(	GAME_SERVICE(m_serviceProvider)
                ->hasParam( _paramName ) == false )
            {
                return pybind::ret_none();
            }

            const WString & val = GAME_SERVICE(m_serviceProvider)
                ->getParam( _paramName );

            return pybind::ptr(val);
        }
        //////////////////////////////////////////////////////////////////////////
        PyObject * s_getGameParamFloat( const ConstString & _paramName )
        {
            if(	GAME_SERVICE(m_serviceProvider)
                ->hasParam( _paramName ) == false )
            {
                return pybind::ret_none();
            }

            const WString & val = GAME_SERVICE(m_serviceProvider)
                ->getParam( _paramName );

            float value;
            Utils::wstringToFloat( val, value );

            return pybind::ptr(value);
        }
        //////////////////////////////////////////////////////////////////////////
        PyObject * s_getGameParamInt( const ConstString & _paramName )
        {
            if(	GAME_SERVICE(m_serviceProvider)
                ->hasParam( _paramName ) == false )
            {
                return pybind::ret_none();
            }

            const WString & val = GAME_SERVICE(m_serviceProvider)
                ->getParam( _paramName );

            int value;
            Utils::wstringToInt( val, value );

            return pybind::ptr(value);
        }
        //////////////////////////////////////////////////////////////////////////
        PyObject * s_getGameParamBool( const ConstString & _paramName )
        {
            if(	GAME_SERVICE(m_serviceProvider)
                ->hasParam( _paramName ) == false )
            {
                return pybind::ret_none();
            }

            const WString & val = GAME_SERVICE(m_serviceProvider)
                ->getParam( _paramName );

            int value;
            Utils::wstringToInt( val, value );

            bool b_value = value != 0;
            
            return pybind::ret_bool( b_value );
        }
        //////////////////////////////////////////////////////////////////////////
        bool s_openUrlInDefaultBrowser( const WString & _url )
        {
            bool val = PLATFORM_SERVICE(m_serviceProvider)
				->openUrlInDefaultBrowser( _url );

            return val;
        }
        //////////////////////////////////////////////////////////////////////////
        ConstString s_getDefaultResourceFontName()
        {
            const ConstString & defaultResourceFontName = TEXT_SERVICE(m_serviceProvider)
                ->getDefaultFontName();

            return defaultResourceFontName;
        }
		//////////////////////////////////////////////////////////////////////////
		class ResourceMovieVisitorNullLayers
			: public VisitorResourceMovie
		{
		public:
			ResourceMovieVisitorNullLayers( ServiceProviderInterface * _serviceProvider, const pybind::dict & _dictResult, float _frameDuration )
				: m_serviceProvider( _serviceProvider )
				, m_dictResult( _dictResult )
				, m_frameDuration( _frameDuration )
			{
			}

		protected:
			void visitLayer( const MovieFramePackInterfacePtr & _framePack, const MovieLayer & _layer ) override
			{
				if( _layer.source != CONST_STRING( m_serviceProvider, MovieSlot ) )
				{
					return;
				}

				if( _framePack->hasLayer( _layer.index ) == false )
				{
					return;
				}

				pybind::list py_list_frames;

				const MovieLayerFrame & frames = _framePack->getLayer( _layer.index );

				for( uint32_t i = 0; i != frames.count; ++i )
				{
					MovieFrameSource frame_source;
					_framePack->getLayerFrame( _layer.index, i, frame_source );

					pybind::dict py_dict_frame;

					py_dict_frame["position"] = frame_source.position;

					float frameTime = _layer.in + i * m_frameDuration;

					py_dict_frame["time"] = frameTime;

					py_list_frames.append( py_dict_frame );
				}

				m_dictResult[_layer.name] = py_list_frames;
			}

		protected:
			ServiceProviderInterface * m_serviceProvider;
			pybind::dict m_dictResult;
			float m_frameDuration;
		};
        //////////////////////////////////////////////////////////////////////////
        PyObject * s_getNullObjectsFromResourceVideo( ResourceMovie * _resource )
        {			
            if( _resource == nullptr )
            {
                return pybind::ret_none();
            }

			pybind::dict py_dict_result;

            float frameTime = _resource->getFrameDuration();
            ResourceMovieVisitorNullLayers visitor(m_serviceProvider, py_dict_result, frameTime);

            _resource->visitResourceMovie( &visitor );

			return py_dict_result.ret();
        }
        //////////////////////////////////////////////////////////////////////////
		bool s_hasMovieElement2( const ResourceMoviePtr & _resource, const ConstString & _slotName, const ConstString & _typeName )
        {
            const TVectorMovieLayers & layers = _resource->getLayers();

            for( TVectorMovieLayers::const_iterator
                it = layers.begin(),
                it_end = layers.end();
            it != it_end;
            ++it )
            {
                const MovieLayer & layer = *it;

                if( layer.type == CONST_STRING(m_serviceProvider, Movie) )
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
        bool s_hasMovieElement( const ConstString & _resourceName, const ConstString & _slotName, const ConstString & _typeName )
        {
			ResourceMoviePtr resource = RESOURCE_SERVICE( m_serviceProvider )
                ->getResourceT<ResourceMoviePtr>( _resourceName );

            if( resource == nullptr )
            {
                return false;
            }

            bool result = s_hasMovieElement2( resource, _slotName, _typeName );

            resource->decrementReference();

            return result;
        }
        //////////////////////////////////////////////////////////////////////////
        bool s_hasMovieSlot( const ConstString & _resourceName, const ConstString & _slotName )
        {
            bool result = s_hasMovieElement( _resourceName, _slotName, CONST_STRING(m_serviceProvider, MovieSlot) );

            return result;
        }
        //////////////////////////////////////////////////////////////////////////
        bool s_hasMovieSubMovie( const ConstString & _resourceName, const ConstString & _subMovieName )
        {
            bool result = s_hasMovieElement( _resourceName, _subMovieName, CONST_STRING(m_serviceProvider, SubMovie) );

            return result;
        }
        //////////////////////////////////////////////////////////////////////////
        bool s_hasMovieSocket( const ConstString & _resourceName, const ConstString & _socketName )
        {
            if( s_hasMovieElement( _resourceName, _socketName, CONST_STRING(m_serviceProvider, MovieSocketImage) ) == true )
            {
                return true;
            }

            if( s_hasMovieElement( _resourceName, _socketName, CONST_STRING(m_serviceProvider, MovieSocketShape) ) == true )
            {
                return true;
            }

            return false;
        }
        //////////////////////////////////////////////////////////////////////////
        bool s_hasMovieEvent( const ConstString & _resourceName, const ConstString & _eventName )
        {
            bool result = s_hasMovieElement( _resourceName, _eventName, CONST_STRING(m_serviceProvider, MovieEvent) );

            return result;
        }
        //////////////////////////////////////////////////////////////////////////
        typedef stdex::vector<String> TVectorConstString;
        //////////////////////////////////////////////////////////////////////////
        void s_visitChild( Node * _node, const pybind::object & _cb )
        {
            TListNodeChild & child = _node->getChildren();
			
			pybind::foreach_t( _cb, child.begin(), child.end() );
        }
		//////////////////////////////////////////////////////////////////////////
		class ResourceVisitorGetAlreadyCompiled
			: public Visitor
			, public ConcreteVisitor<ResourceReference>
		{
		public:
			ResourceVisitorGetAlreadyCompiled( const pybind::object & _cb )
				: m_cb( _cb )
			{
			}

			~ResourceVisitorGetAlreadyCompiled()
			{
			}

		protected:
			void accept( ResourceReference* _resource )
			{
				if( _resource->isCompile() == false )
				{
					return;
				}

				m_cb( _resource );
			}

		protected:
			pybind::object m_cb;
		};
        //////////////////////////////////////////////////////////////////////////
		void s_visitCompiledResources( const ConstString & _category, const ConstString & _groupName, const pybind::object & _cb )
        {
            ResourceVisitorGetAlreadyCompiled rv_gac(_cb);

            RESOURCE_SERVICE(m_serviceProvider)
                ->visitGroupResources( _category, _groupName, &rv_gac );						
        }
		//////////////////////////////////////////////////////////////////////////
		class MyResourceVisitor
			: public Visitor
			, public ConcreteVisitor<ResourceReference>
		{
		public:
			MyResourceVisitor( const pybind::object & _cb )
				: m_cb( _cb )
			{
			}

			~MyResourceVisitor()
			{
			}

		protected:
			void accept( ResourceReference* _resource ) override
			{
				m_cb( _resource );
			}

		protected:
			pybind::object m_cb;
		};
		//////////////////////////////////////////////////////////////////////////
		void s_visitResources( const ConstString & _category, const ConstString & _groupName, const pybind::object & _cb )
        {
            MyResourceVisitor rv_gac(_cb);

            RESOURCE_SERVICE(m_serviceProvider)
                ->visitGroupResources( _category, _groupName, &rv_gac );						
        }
		//////////////////////////////////////////////////////////////////////////
		class IncrefResourceVisitor
			: public Visitor
			, public ConcreteVisitor<ResourceReference>
		{
		protected:
			void accept( ResourceReference* _resource ) override
			{
				_resource->incrementReference();
			}
		};
		//////////////////////////////////////////////////////////////////////////		
		void s_incrementResources( const ConstString & _category, const ConstString & _groupName )
		{
			IncrefResourceVisitor rv_gac;

			RESOURCE_SERVICE(m_serviceProvider)
				->visitGroupResources( _category, _groupName, &rv_gac );						
		}
		//////////////////////////////////////////////////////////////////////////
		class DecrementResourceVisitor
			: public Visitor
			, public ConcreteVisitor<ResourceReference>
		{
		protected:
			void accept( ResourceReference * _resource ) override
			{
				_resource->decrementReference();
			}
		};
		//////////////////////////////////////////////////////////////////////////		
		void s_decrementResources( const ConstString & _category, const ConstString & _groupName )
		{
			DecrementResourceVisitor rv_gac;

			RESOURCE_SERVICE(m_serviceProvider)
				->visitGroupResources( _category, _groupName, &rv_gac );						
		}
        //////////////////////////////////////////////////////////////////////////
        bool s_validResource( const ConstString & _resourceName )
        {
            bool valid = RESOURCE_SERVICE(m_serviceProvider)
                ->validResource( _resourceName );

            return valid;
        }

    protected: 
        ServiceProviderInterface * m_serviceProvider;
    };
    //////////////////////////////////////////////////////////////////////////
    static void classWrapping( ServiceProviderInterface * _serviceProvider )
    {
        SCRIPT_CLASS_WRAPPING( _serviceProvider, Node );
        SCRIPT_CLASS_WRAPPING( _serviceProvider, Layer );
        SCRIPT_CLASS_WRAPPING( _serviceProvider, Layer2D );
		SCRIPT_CLASS_WRAPPING( _serviceProvider, Layer2DParallax );
		SCRIPT_CLASS_WRAPPING( _serviceProvider, Layer2DIsometric );
        //SCRIPT_CLASS_WRAPPING( _serviceProvider, Layer2DPhysic );
		SCRIPT_CLASS_WRAPPING( _serviceProvider, HotSpot );
		SCRIPT_CLASS_WRAPPING( _serviceProvider, HotSpotPolygon );
		SCRIPT_CLASS_WRAPPING( _serviceProvider, HotSpotBubbles );
        SCRIPT_CLASS_WRAPPING( _serviceProvider, HotSpotImage );
        SCRIPT_CLASS_WRAPPING( _serviceProvider, HotSpotShape );

		SCRIPT_CLASS_WRAPPING( _serviceProvider, ScriptHolder );

        //SCRIPT_CLASS_WRAPPING( Light2D );
        //SCRIPT_CLASS_WRAPPING( ShadowCaster2D );
        SCRIPT_CLASS_WRAPPING( _serviceProvider, Sprite );
        SCRIPT_CLASS_WRAPPING( _serviceProvider, Animation );
		SCRIPT_CLASS_WRAPPING( _serviceProvider, Gyroscope );
		SCRIPT_CLASS_WRAPPING( _serviceProvider, Isometric );
        SCRIPT_CLASS_WRAPPING( _serviceProvider, Arrow );
        SCRIPT_CLASS_WRAPPING( _serviceProvider, TextField );
        SCRIPT_CLASS_WRAPPING( _serviceProvider, SoundEmitter );
        SCRIPT_CLASS_WRAPPING( _serviceProvider, ParticleEmitter2 );
        SCRIPT_CLASS_WRAPPING( _serviceProvider, Movie );
        SCRIPT_CLASS_WRAPPING( _serviceProvider, MovieSlot );
        SCRIPT_CLASS_WRAPPING( _serviceProvider, MovieInternalObject );
		SCRIPT_CLASS_WRAPPING( _serviceProvider, Model3D );
        SCRIPT_CLASS_WRAPPING( _serviceProvider, Point );
		SCRIPT_CLASS_WRAPPING( _serviceProvider, Line );
		SCRIPT_CLASS_WRAPPING( _serviceProvider, Landscape2D );
		SCRIPT_CLASS_WRAPPING( _serviceProvider, Grid2D );
        //SCRIPT_CLASS_WRAPPING( TilePolygon );
        SCRIPT_CLASS_WRAPPING( _serviceProvider, Video );
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
        SCRIPT_CLASS_WRAPPING( _serviceProvider, Window );

		SCRIPT_CLASS_WRAPPING( _serviceProvider, Parallax );
		SCRIPT_CLASS_WRAPPING( _serviceProvider, RenderViewport );
        SCRIPT_CLASS_WRAPPING( _serviceProvider, Camera2D );		
		SCRIPT_CLASS_WRAPPING( _serviceProvider, CameraTarget2D );
		SCRIPT_CLASS_WRAPPING( _serviceProvider, CameraIsometric );
        //SCRIPT_CLASS_WRAPPING( Layer2DTexture );

        SCRIPT_CLASS_WRAPPING( _serviceProvider, ResourceReference );
        SCRIPT_CLASS_WRAPPING( _serviceProvider, ResourceImage );
		SCRIPT_CLASS_WRAPPING( _serviceProvider, ResourceImageData );
        SCRIPT_CLASS_WRAPPING( _serviceProvider, ResourceImageDefault );
        SCRIPT_CLASS_WRAPPING( _serviceProvider, ResourceAnimation );
        SCRIPT_CLASS_WRAPPING( _serviceProvider, ResourceMovie );
		SCRIPT_CLASS_WRAPPING( _serviceProvider, ResourceModel3D );
        SCRIPT_CLASS_WRAPPING( _serviceProvider, ResourceVideo );
        SCRIPT_CLASS_WRAPPING( _serviceProvider, ResourceSound );

        SCRIPT_CLASS_WRAPPING( _serviceProvider, ResourceImageSolid );
        SCRIPT_CLASS_WRAPPING( _serviceProvider, ResourceShape );
        //SCRIPT_CLASS_WRAPPING( _serviceProvider, ResourceFont );       
        SCRIPT_CLASS_WRAPPING( _serviceProvider, ResourceWindow );
        SCRIPT_CLASS_WRAPPING( _serviceProvider, ResourceImageSubstractRGBAndAlpha );
        SCRIPT_CLASS_WRAPPING( _serviceProvider, ResourceImageSubstract );
        SCRIPT_CLASS_WRAPPING( _serviceProvider, ResourceInternalObject );
		SCRIPT_CLASS_WRAPPING( _serviceProvider, ResourceHIT );
    }

    struct extract_TBlobject_type
        : public pybind::type_cast_result<Blobject>
    {
		bool apply( PyObject * _obj, Blobject & _value, bool _nothrow ) override
        {
			(void)_nothrow;

            if( pybind::string_check( _obj ) == true )
            {
                size_t size;
                const char * value_char = pybind::string_to_char_and_size( _obj, size );

                if( value_char == 0 )
                {
                    return false;
                }

                _value.assign( value_char, value_char + size );
            }
            else
            {
                return false;
            }

            return true;
        }

		PyObject * wrap( pybind::type_cast_result<Blobject>::TCastRef _value ) override
        {
            PyObject * py_value = pybind::string_from_char_size( reinterpret_cast<const char *>(&_value[0]), _value.size() );

            return py_value;
        }
    };

	typedef stdex::vector<ResourceImage *> TVectorResourceImage;

	void ScriptWrapper::nodeWrap( ServiceProviderInterface * _serviceProvider )
    {        
        NodeScriptMethod * nodeScriptMethod = new NodeScriptMethod(_serviceProvider);
        
        pybind::registration_type_cast<Blobject>( new extract_TBlobject_type );

		pybind::registration_stl_vector_type_cast<ResourceImage *, stdex::vector<ResourceImage *> >();
		pybind::registration_stl_vector_type_cast<HotSpotPolygon *, stdex::vector<HotSpotPolygon *> >();
		
		pybind::registration_stl_map_type_cast<ConstString, WString, stdex::map<ConstString, WString> >();

        classWrapping( _serviceProvider );

        pybind::interface_<Affector>("Affector", true)
            .def( "stop", &Affector::stop )
			.def( "getId", &Affector::getId )
			.def( "setFreeze", &Affector::setFreeze )
			.def( "getFreeze", &Affector::getFreeze )
			.def( "setSpeedFactor", &Affector::setSpeedFactor )
			.def( "getSpeedFactor", &Affector::getSpeedFactor )
            ;

        pybind::interface_<Scriptable>("Scriptable")
            ;

        pybind::interface_<Identity>("Identity")
            .def( "setName", &Identity::setName )
            .def( "getName", &Identity::getName )
            .def( "getType", &Identity::getType )
            ;

		pybind::interface_<BoundingBox>( "BoundingBox" )
			.def( "getBoundingBox", &BoundingBox::getBoundingBox )
			;

        pybind::interface_<Transformation3D>("Transformation3D")
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
            .def( "setCoordinate", &Transformation3D::setCoordinate )			
            .def( "getCoordinate", &Transformation3D::getCoordinate )
            .def( "setOrigin", &Transformation3D::setOrigin )
            .def( "getOrigin", &Transformation3D::getOrigin )
			.def( "setScale", &Transformation3D::setScale )
            .def( "getScale", &Transformation3D::getScale )
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

			.def( "setDirection", &Transformation3D::setDirection )
			.def( "lookAt", &Transformation3D::lookAt )

            .def( "translate", &Transformation3D::translate )

            .def( "resetTransformation", &Transformation3D::resetTransformation )
            .def( "setRelationTransformation", &Transformation3D::setRelationTransformation )
            .def_proxy_static( "removeRelationTransformation", nodeScriptMethod, &NodeScriptMethod::Transformation3D_removeRelationTransformation )
            //.def( "setRotate", &Transformation3D::setAngle ) //depricated
            ;

        //pybind::class_<FFCamera3D>("FFCamera3D")
        //	.def( pybind::init<>() )
        //	.def( "update", &FFCamera3D::update )
        //	.def( "activate", &FFCamera3D::activate )
        //	.def( "forward", &FFCamera3D::forward )
        //	.def( "left", &FFCamera3D::left )
        //	.def( "setActor", &FFCamera3D::setActor )
        //	.def( "yaw", &FFCamera3D::yaw )
        //	.def( "pitch", &FFCamera3D::pitch )
        //	.def( "zoom", &FFCamera3D::zoom )
        //	;

        //pybind::interface_<NodeRenderable>("NodeRenderable", false)
        //	.def( "hide", &NodeRenderable::hide )
        //	;

        pybind::interface_<Resource>("Resource")
            .def( "compile", &Resource::compile )
            .def( "release", &Resource::release )
            .def( "isCompile", &Resource::isCompile )
            ;

        pybind::interface_<Reference>("Reference")
            .def( "incrementReference", &Reference::incrementReference )
            .def( "decrementReference", &Reference::decrementReference )
            .def( "countReference", &Reference::countReference )
            ;

        pybind::interface_<ResourceReference, pybind::bases<Resource, Identity, Reference> >("ResourceReference", false)
			.def_smart_pointer()
            .def( "getCategory", &ResourceReference::getCategory )
            .def( "getGroup", &ResourceReference::getGroup )
			.def( "cache", &ResourceReference::cache )
			.def( "uncache", &ResourceReference::uncache )
            ;

        pybind::interface_<ResourceImage, pybind::bases<ResourceReference> >("ResourceImage", false)
			.def( "getMaxSize", &ResourceImage::getMaxSize )
			.def( "getSize", &ResourceImage::getSize )
			.def( "getOffset", &ResourceImage::getOffset )
			.def( "getUVImage", &ResourceImage::getUVImage )
			.def( "getUVAlpha", &ResourceImage::getUVAlpha )
			.def( "isAlpha", &ResourceImage::isAlpha )
			.def( "setColor", &ResourceImage::setColor )
			.def( "getColor", &ResourceImage::getColor )
            ;

		pybind::interface_<ResourceImageData, pybind::bases<ResourceReference> >("ResourceImageData", false)
			.def( "getImagePath", &ResourceImageData::getImagePath )
			.def( "getCodecType", &ResourceImageData::getCodecType )
			.def( "getImageMaxSize", &ResourceImageData::getImageMaxSize )
			;

        pybind::interface_<ResourceImageDefault, pybind::bases<ResourceImage> >("ResourceImageDefault", false)
			.def( "getFilePath", &ResourceImageDefault::getFilePath )
			.def( "getCodecType", &ResourceImageDefault::getCodecType )
            ;

		pybind::interface_<ResourceImageSubstractRGBAndAlpha, pybind::bases<ResourceImage> >("ResourceImageSubstractRGBAndAlpha", false)
			;

		pybind::interface_<ResourceImageSubstract, pybind::bases<ResourceImage> >("ResourceImageSubstract", false)
			;

		pybind::interface_<ResourceImageSolid, pybind::bases<ResourceImage> >( "ResourceImageSolid", false )
			;

        pybind::interface_<ResourceMovie, pybind::bases<ResourceReference> >("ResourceMovie", false)
			.def("getSize", &ResourceMovie::getSize)
			.def("getLoopSegment", &ResourceMovie::getLoopSegment)
			.def("getFrameCount", &ResourceMovie::getFrameCount)
			.def("getFrameDuration", &ResourceMovie::getFrameDuration)
			.def("getDuration", &ResourceMovie::getDuration)
			.def("getSocketResourceShape", &ResourceMovie::getSocketResourceShape)
			.def("hasAnchorPoint", &ResourceMovie::hasAnchorPoint)
			.def("getAnchorPoint", &ResourceMovie::getAnchorPoint)
			.def("hasBoundBox", &ResourceMovie::hasBoundBox)
			.def("getBoundBox", &ResourceMovie::getBoundBox)
			.def_proxy_static("hasLayer", nodeScriptMethod, &NodeScriptMethod::ResourceMovie_hasLayer)
			.def_proxy_static("hasLayerType", nodeScriptMethod, &NodeScriptMethod::ResourceMovie_hasLayerType)
			.def_proxy_static("getLayerPosition", nodeScriptMethod, &NodeScriptMethod::ResourceMovie_getLayerPosition)
            ;        

        pybind::interface_<ResourceAnimation, pybind::bases<ResourceReference> >("ResourceAnimation", false)
            ;            

		pybind::interface_<ResourceModel3D, pybind::bases<ResourceReference> >("ResourceModel3D", false)
			;

        pybind::interface_<ResourceVideo, pybind::bases<ResourceReference> >("ResourceVideo", false)
            ;            
        
        pybind::interface_<ResourceSound, pybind::bases<ResourceReference> >("ResourceSound", false)
            ;            
        
        pybind::interface_<ResourceInternalObject, pybind::bases<ResourceReference> >("ResourceInternalObject", false)
            ;                   

        pybind::interface_<ResourceShape, pybind::bases<ResourceReference> >("ResourceShape", false)
			.def( "getPolygon", &ResourceShape::getPolygon )
            ;

        pybind::interface_<ResourceWindow, pybind::bases<ResourceReference> >("ResourceWindow", false)
            ;    

		pybind::interface_<ResourceHIT, pybind::bases<ResourceReference> >("ResourceHIT", false)
			;        		

        pybind::interface_<Renderable>("Renderable")
            .def( "hide", &Renderable::hide )
            .def( "isHide", &Renderable::isHide )
            .def( "localHide", &Renderable::localHide )
            .def( "isLocalHide", &Renderable::isLocalHide )
            ;

        pybind::interface_<Colorable>("Colorable")
            .def( "setLocalColor", &Colorable::setLocalColor )
            .def( "getLocalColor", &Colorable::getLocalColor )
			.def( "setLocalColorR", &Colorable::setLocalColorR )
			.def( "getLocalColorR", &Colorable::getLocalColorR )
			.def( "setLocalColorG", &Colorable::setLocalColorG )
			.def( "getLocalColorG", &Colorable::getLocalColorG )
			.def( "setLocalColorB", &Colorable::setLocalColorB )
			.def( "getLocalColorB", &Colorable::getLocalColorB )
            .def( "setLocalAlpha", &Colorable::setLocalColorAlpha )
            .def( "getLocalAlpha", &Colorable::getLocalColorAlpha )
			.def( "setLocalColorRGB", &Colorable::setLocalColorRGB )
            .def( "setPersonalColor", &Colorable::setPersonalColor )
            .def( "getPersonalColor", &Colorable::getPersonalColor )
            .def( "setPersonalAlpha", &Colorable::setPersonalAlpha )
            .def( "getPersonalAlpha", &Colorable::getPersonalAlpha )			
            ;

        pybind::interface_<Animatable>("Animatable")
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
            ;

        pybind::interface_<Affectorable>("Affectorable")
            .def( "addAffector", &Affectorable::addAffector )
			.def( "stopAffector", &Affectorable::stopAffector )
			.def( "stopAllAffectors", &Affectorable::stopAllAffectors )
            ;

		pybind::interface_<Materialable>( "Materialable", false )
			.def( "setMaterialName", &Materialable::setMaterialName )
			.def( "getMaterialName", &Materialable::getMaterialName )
			.def( "setDisableTextureColor", &Materialable::setDisableTextureColor )
			.def( "getDisableTextureColor", &Materialable::getDisableTextureColor )
			.def( "setBlendMode", &Materialable::setBlendMode )
			.def( "getBlendMode", &Materialable::getBlendMode )
			;

		pybind::interface_<Node, pybind::bases<Scriptable, Identity, Transformation3D, BoundingBox, Colorable, Resource, Renderable, Affectorable> >( "Node", false )
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
            //.def_static( "getChild", &ScriptMethod::s_getChild )
            .def( "findChildren", &Node::findChild )            
            .def( "emptyChild", &Node::emptyChildren )
            .def( "hasChildren", &Node::hasChild )
            .def( "update", &Node::update )
            .def( "getParent", &Node::getParent )
			.def( "hasParent", &Node::hasParent )
            .def( "getScene", &Node::getScene )
            .def( "getLayer", &Node::getLayer )
            .def_native( "setEventListener", &Node::setEventListener )
            .def( "removeEventListener", &Node::removeEventListener )
            //.def( "getListener", &Node::getListener )

            .def( "getWorldPosition", &Node::getWorldPosition )
			.def( "setWorldPosition", &Node::setWorldPosition )
            //.def( "getWorldDirection", &Node::getWorldDirection )
            .def_proxy_static( "getCameraPosition", nodeScriptMethod, &NodeScriptMethod::s_getCameraPosition )
			.def_proxy_static( "getWorldOffsetPosition", nodeScriptMethod, &NodeScriptMethod::s_getWorldOffsetPosition )
			.def_proxy_static( "getLengthTo", nodeScriptMethod, &NodeScriptMethod::s_getLengthTo )

            .def( "getWorldColor", &Node::getWorldColor )

			.def( "setIsometricOffset", &Node::setIsometricOffset )
			.def( "getIsometricOffset", &Node::getIsometricOffset )

			.def( "setRenderViewport", &Node::setRenderViewport ) 
			.def( "getRenderViewport", &Node::getRenderViewport )
			.def( "getRenderViewportInheritance", &Node::getRenderViewportInheritance )
			
			.def( "setRenderCamera", &Node::setRenderCamera )
			.def( "getRenderCamera", &Node::getRenderCamera )
			.def( "getRenderCameraInheritance", &Node::getRenderCameraInheritance )

            .def_proxy_static( "createChildren", nodeScriptMethod, &NodeScriptMethod::createChildren )
			.def_proxy_static( "getAllChildren", nodeScriptMethod, &NodeScriptMethod::getAllChildren )

            .def_proxy_static( "colorTo", nodeScriptMethod, &NodeScriptMethod::colorTo )
            .def_proxy_static( "alphaTo", nodeScriptMethod, &NodeScriptMethod::alphaTo )
            .def_proxy_static( "colorStop", nodeScriptMethod, &NodeScriptMethod::colorStop )

            .def_proxy_static( "velocityTo", nodeScriptMethod, &NodeScriptMethod::velocityTo )

            .def_proxy_static( "moveTo", nodeScriptMethod, &NodeScriptMethod::moveTo )
            .def_proxy_static( "bezier2To", nodeScriptMethod, &NodeScriptMethod::bezier2To )
            .def_proxy_static( "bezier3To", nodeScriptMethod, &NodeScriptMethod::bezier3To )
			.def_proxy_static( "parabolaTo", nodeScriptMethod, &NodeScriptMethod::parabolaTo )
            .def_proxy_static( "moveStop", nodeScriptMethod, &NodeScriptMethod::moveStop )

            .def_proxy_static( "angleTo", nodeScriptMethod, &NodeScriptMethod::angleTo )
            .def_proxy_static( "angleStop", nodeScriptMethod, &NodeScriptMethod::angleStop )
            .def_proxy_static( "scaleTo", nodeScriptMethod, &NodeScriptMethod::scaleTo )
            .def_proxy_static( "scaleStop", nodeScriptMethod, &NodeScriptMethod::scaleStop )

            .def_proxy_static( "accMoveTo", nodeScriptMethod, &NodeScriptMethod::accMoveTo )
            .def_proxy_static( "accAngleTo", nodeScriptMethod, &NodeScriptMethod::accAngleTo )			
            ;

        pybind::interface_<ThreadTask>("Task")
            ;

		pybind::interface_<RenderViewportInterface>("RenderViewportInterface")
			.def( "getViewport", &RenderViewportInterface::getViewport )
			;

		pybind::interface_<RenderViewport, pybind::bases<Node, RenderViewportInterface> >("RenderViewport", false)
			.def( "setFixedViewport", &RenderViewport::setFixedViewport )
			.def( "getFixedViewport", &RenderViewport::getFixedViewport )
			;

        pybind::interface_<RenderCameraInterface>("RenderCameraInterface")
            .def( "getCameraRenderport", &RenderCameraInterface::getCameraRenderport )
			.def( "isOrthogonalProjection", &RenderCameraInterface::isOrthogonalProjection )			
            ;

        pybind::interface_<Camera2D, pybind::bases<Node, RenderCameraInterface> >("Camera2D", false)
            .def( "setRenderport", &Camera2D::setRenderport )
			.def( "setFixedRenderport", &Camera2D::setFixedRenderport )
			.def( "getFixedRenderport", &Camera2D::getFixedRenderport )
            ;		

		pybind::interface_<CameraIsometric, pybind::bases<Node, RenderCameraInterface> >( "CameraIsometric", false )
			.def( "setRenderport", &CameraIsometric::setRenderport )
			;					

		pybind::interface_<CameraTarget2D, pybind::bases<Node> >("CameraTarget2D", false)
			.def( "setCamera2D", &CameraTarget2D::setCamera2D )
			.def( "getCamera2D", &CameraTarget2D::getCamera2D )
			.def( "setSpeed", &CameraTarget2D::setSpeed )
			.def( "getSpeed", &CameraTarget2D::getSpeed )
			.def( "setFixedHorizont", &CameraTarget2D::setFixedHorizont )
			.def( "getFixedHorizont", &CameraTarget2D::getFixedHorizont )
			.def( "isFixedHorizont", &CameraTarget2D::isFixedHorizont )
			;

        //pybind::proxy_<SceneNode3D, pybind::bases<Node>>("SceneNode3D", false)
        //	.def( "getWorldOrient", &SceneNode3D::getWorldOrient )
        //	.def( "getWorldPosition", &SceneNode3D::getWorldPosition )
        //	.def( "getLocalOrient", &SceneNode3D::getLocalOrient )
        //	.def( "getPosition", &SceneNode3D::getLocalPosition )
        //	.def( "setPosition", &SceneNode3D::setLocalPosition )
        //	.def( "setOrient", &SceneNode3D::setLocalOrient )
        //	.def( "setScale", &SceneNode3D::setScale )
        //	.def( "yaw", &SceneNode3D::yaw )
        //	.def( "pitch", &SceneNode3D::pitch )
        //	.def( "roll", &SceneNode3D::roll )
        //	.def( "setFixedYawAxis", &SceneNode3D::setFixedYawAxis )
        //	.def( "translate", &SceneNode3D::translate )
        //	.def( "addChild", &SceneNode3D::addChild )
        //	.def( "setYawSpeed", &SceneNode3D::setYawSpeed )
        //	.def( "setYawLimits", &SceneNode3D::setYawLimits )
        //	.def( "getYaw", &SceneNode3D::getYaw )
        //	.def( "getPitch", &SceneNode3D::getPitch )
        //	.def( "setListener", &SceneNode3D::setListener )

        //	//.def( "getCamera", &SceneNode3D::getCamera )
        //	;

        {

            //pybind::proxy_<RigidBody3D, pybind::bases<Node>>("RigidBody3D", false)
            //	.def( "applyForce", &RigidBody3D::applyForce )
            //	.def( "applyImpulse", &RigidBody3D::applyImpulse )
            //	.def( "applyAngularImpulse", &RigidBody3D::applyAngularImpulse )
            //	.def( "applyTorque", &RigidBody3D::applyTorque )
            //	.def( "setLinearVelocity", &RigidBody3D::setLinearVelocity )
            //	.def( "setAngularVelocity", &RigidBody3D::setAngularVelocity )
            //	.def( "setActive", &RigidBody3D::setActive )
            //	;

            //pybind::proxy_<CapsuleController, pybind::bases<Node>>("CapsuleController", false)
            //	.def( "move", &CapsuleController::move )
            //	.def( "setPosition", &CapsuleController::setPosition )
            //	.def( "getFilteredPosition", &CapsuleController::getFilteredPosition )
            //	;

            //pybind::proxy_<Camera3D, pybind::bases<SceneNode3D> >("Camera3D", false)
            //	.def( "setPosition", &Camera3D::setPosition )
            //	.def( "lookAt", &Camera3D::lookAt )
            //	//	.def( "yaw", &Camera3D::yaw )
            //	//	.def( "pitch", &Camera3D::pitch )
            //	//	.def( "roll", &Camera3D::roll )
            //	.def( "getDirection", &Camera3D::getDirection )
            //	;

            //pybind::proxy_<DiscreteEntity, pybind::bases<SceneNode3D>>("DiscreteEntity", false)
            //	.def( "createRenderToTexture", &DiscreteEntity::createRenderToTexture )
            //	.def( "playAnimation", &DiscreteEntity::playAnimation )
            //	.def( "setMaterial", &DiscreteEntity::setMaterial )
            //	//.def( "playAnimation", &DiscreteEntity::playAnimation )
            //	;

            //pybind::interface_<Allocator3D>("Allocator3D", false)
            //	.def( "scale", &Allocator3D::scale )
            //	;

            //pybind::proxy_<SceneNode3D, pybind::bases<Node, Allocator3D> >("SceneNode3D", false)
            //	.def( "yaw", &SceneNode3D::yaw )
            //	.def( "pitch", &SceneNode3D::pitch )
            //	.def( "getYaw", &SceneNode3D::getYaw )
            //	.def( "getPitch", &SceneNode3D::getPitch )
            //	.def( "yawTime", &SceneNode3D::yawTime )
            //	.def( "pitchTime", &SceneNode3D::pitchTime )
            //	.def( "setListener", &SceneNode3D::setListener )
            //	;

            //pybind::proxy_<Layer3D, pybind::bases<SceneNode3D> >("Layer3D", false)
            //	//.def( "addCamera", &Layer3D::addCamera )
            //	//.def( "getCamera", &Layer3D::getCamera )	
            //	.def( "addController", &Layer3D::addController )
            //	.def( "getController", &Layer3D::getController )		
            //	//.def( "getNode", &Layer3D::getNode )
            //	;

            {

                //pybind::proxy_< RenderMesh, pybind::bases<SceneNode3D> >("RenderMesh", false)
                //	//.def( "createRenderTarget", &RenderMesh::createRenderTarget )
                //	//.def( "setMaterial", &RenderMesh::setMaterial )
                //	;

                //pybind::proxy_<Camera3D, pybind::bases<SceneNode3D> >("Camera3D", false)
                //	//	.def( "setPosition", &Camera3D::setPosition )
                //	.def( "lookAt", &Camera3D::lookAt )
                //	.def( "yaw", &Camera3D::yaw )
                //	.def( "pitch", &Camera3D::pitch )
                //	//.def( "roll", &Camera3D::roll )
                //	//	.def( "getDirection", &Camera3D::getDirection )
                //	;
            }

            {
                pybind::interface_<ParticleEmitter2, pybind::bases<Node, Animatable> >("ParticleEmitter2", false)
					.def( "setResourceParticle", &ParticleEmitter2::setResourceParticle )
					.def( "getResourceParticle", &ParticleEmitter2::getResourceParticle )

					.def( "setEmitterTranslateWithParticle", &ParticleEmitter2::setEmitterTranslateWithParticle )
					.def( "setEmitterRelative", &ParticleEmitter2::setEmitterRelative )

					.def( "changeEmitterImage", &ParticleEmitter2::changeEmitterImage )
					.def( "removeEmitterImage", &ParticleEmitter2::removeEmitterImage )
					.def( "changeEmitterPolygon", &ParticleEmitter2::changeEmitterPolygon )
					.def( "removeEmitterPolygon", &ParticleEmitter2::removeEmitterPolygon )

					.def( "getDuration", &ParticleEmitter2::getDuration )

					.def( "setEmitterRandomMode", &ParticleEmitter2::setEmitterRandomMode )
					.def( "getEmitterRandomMode", &ParticleEmitter2::getEmitterRandomMode )                    
                    ;

                pybind::interface_<SoundEmitter, pybind::bases<Node, Animatable> >("SoundEmitter", false)
					.def( "setResourceSound", &SoundEmitter::setResourceSound )
					.def( "getResourceSound", &SoundEmitter::getResourceSound )
                    .def( "setVolume", &SoundEmitter::setVolume )
                    .def( "getVolume", &SoundEmitter::getVolume )
                    .def( "setLoop", &SoundEmitter::setLoop )
                    .def( "isLooping", &SoundEmitter::getLoop )
                    ;

                pybind::interface_<TextField, pybind::bases<Node> >("TextField", false)
					.def_depricated( "setTextByKey", &TextField::setTextID, "use setTextID" )
					.def( "setTextID", &TextField::setTextID )
					.def( "removeTextID", &TextField::removeTextID )
					.def_proxy_native( "setTextFormatArgs", nodeScriptMethod, &NodeScriptMethod::textfield_setTextFormatArgs )
					.def_proxy_static( "getTextFormatArgs", nodeScriptMethod, &NodeScriptMethod::textfield_getTextFormatArgs )					
					.def( "removeTextFormatArgs", &TextField::removeTextFormatArgs )
					.def_depricated( "getTextKey", &TextField::getTextID, "use getTextID" )
					.def( "getTextID", &TextField::getTextID )
					.def_proxy_static( "getText", nodeScriptMethod, &NodeScriptMethod::textfield_getText )
					.def( "getTextExpectedArgument", &TextField::getTextExpectedArgument )

                    .def_depricated( "getHeight", &TextField::getFontHeight, "use getFontHeight" )
                    .def_depricated( "getAlphaHeight", &TextField::getFontHeight, "use getFontHeight" )
                    .def( "getFontHeight", &TextField::getFontHeight )
					.def( "setFontName", &TextField::setFontName )
					.def( "getFontName", &TextField::getFontName )

					.def( "setWrap", &TextField::setWrap )
					.def( "getWrap", &TextField::getWrap )


					.def( "enableOutline", &TextField::enableOutline )
					.def( "isOutline", &TextField::isOutline )
					.def( "setOutlineColor", &TextField::setOutlineColor )
                    .def( "getOutlineColor", &TextField::getOutlineColor )
					.def( "setFontColor", &TextField::setFontColor )
					.def( "getFontColor", &TextField::getFontColor )

					.def( "setLineOffset", &TextField::setLineOffset )
					.def( "getLineOffset", &TextField::getLineOffset )

					.def( "setCharOffset", &TextField::setCharOffset )
					.def( "getCharOffset", &TextField::getCharOffset )
                    
					.def_depricated( "getLength", &TextField::getTextSize, "use getTextSize" )
                    .def( "getTextSize", &TextField::getTextSize )
                    .def_depricated( "setMaxLen", &TextField::setMaxLength, "use setMaxLength" )
					.def( "setMaxLength", &TextField::setMaxLength )
					.def( "getMaxLength", &TextField::getMaxLength )

					.def_depricated( "setCenterAlign", &TextField::setHorizontalCenterAlign, "use setHorizontalCenterAlign" )
					.def_depricated( "isCenterAlign", &TextField::isHorizontalCenterAlign, "use isHorizontalCenterAlign" )
					.def_depricated( "setRightAlign", &TextField::setHorizontalRightAlign, "use setHorizontalRightAlign" )
					.def_depricated( "isRightAlign", &TextField::isHorizontalRightAlign, "use isHorizontalRightAlign" )
					.def_depricated( "setLeftAlign", &TextField::setHorizontalLeftAlign, "use setHorizontalLeftAlign" )
					.def_depricated( "isLeftAlign", &TextField::isHorizontalLeftAlign, "use isHorizontalLeftAlign" )

					.def( "setHorizontalCenterAlign", &TextField::setHorizontalCenterAlign )
					.def( "isHorizontalCenterAlign", &TextField::isHorizontalCenterAlign )
					.def( "setHorizontalRightAlign", &TextField::setHorizontalRightAlign )
					.def( "isHorizontalRightAlign", &TextField::isHorizontalRightAlign )
					.def( "setHorizontalLeftAlign", &TextField::setHorizontalLeftAlign )
					.def( "isHorizontalLeftAlign", &TextField::isHorizontalLeftAlign )

                    .def( "setVerticalTopAlign", &TextField::setVerticalTopAlign )
                    .def( "isVerticalTopAlign", &TextField::isVerticalTopAlign )
                    .def( "setVerticalCenterAlign", &TextField::setVerticalCenterAlign )
                    .def( "isVerticalCenterAlign", &TextField::isVerticalCenterAlign )
					
                    .def( "setPixelsnap", &TextField::setPixelsnap )
                    .def( "getPixelsnap", &TextField::getPixelsnap )

                    .def( "setMaxCharCount", &TextField::setMaxCharCount )
                    .def( "getMaxCharCount", &TextField::getMaxCharCount )
                    .def( "getCharCount", &TextField::getCharCount )
                    ;

				pybind::interface_<ScriptHolder, pybind::bases<Node> >("ScriptHolder", false)
					;

                pybind::interface_<Point, pybind::bases<Node> >("Point", false)
					.def( "setLinkedPoint", &Point::setLinkedPoint )
					.def( "removeLinkedPoint", &Point::removeLinkedPoint )
					.def( "getLinkedPoint", &Point::getLinkedPoint )
					.def( "setWidth", &Point::setWidth )
					.def( "getWidth", &Point::getWidth )
                    ;

				pybind::interface_<Line, pybind::bases<Node> >("Line", false)
					.def( "setFrom", &Line::setFrom )
					.def( "getFrom", &Line::getFrom )
					.def( "setTo", &Line::setTo )
					.def( "getTo", &Line::getTo )
					.def( "setWidth", &Line::setWidth )
					.def( "getWidth", &Line::getWidth )
					;

                pybind::interface_<Layer, pybind::bases<Node> >("Layer", false)
                    .def( "setMain", &Layer::setMain )
                    .def( "isMain", &Layer::isMain )
                    .def( "setSize", &Layer::setSize )
                    .def( "getSize", &Layer::getSize )
					;

				pybind::interface_<Layer2D, pybind::bases<Layer> >("Layer2D", false)
					.def( "setViewport", &Layer2D::setViewport )
					.def( "removeViewport", &Layer2D::removeViewport )
					.def( "cameraToLocal", &Layer2D::cameraToLocal )
					;

                pybind::interface_<Layer2DParallax, pybind::bases<Layer> >("Layer2DParallax", false)
                    .def( "setParallaxFactor", &Layer2DParallax::setParallaxFactor )
                    .def( "getParallaxFactor", &Layer2DParallax::getParallaxFactor )
					.def( "setParallaxLoop", &Layer2DParallax::setParallaxLoop )
					.def( "getParallaxLoop", &Layer2DParallax::getParallaxLoop )
                    ;

				pybind::interface_<Parallax, pybind::bases<Node> >( "Parallax", false )
					.def( "setParallaxFactor", &Parallax::setParallaxFactor )
					.def( "getParallaxFactor", &Parallax::getParallaxFactor )
					;

				
				pybind::interface_<Layer2DIsometric, pybind::bases<Layer> >("Layer2DIsometric", false)
					;

                //pybind::interface_<Layer2DPhysic, pybind::bases<Layer2D> >("Layer2DPhysic", false)
                //    ;

                //pybind::proxy_<Layer2DTexture, pybind::bases<Layer2D> >("Layer2DTexture", false)
                //	.def( "setCameraOffset", &Layer2DTexture::setCameraOffset )
                //	.def( "getCameraOffset", &Layer2DTexture::getCameraOffset )
                //	.def( "setRenderTargetName", &Layer2DTexture::setRenderTargetName )
                //	.def( "getRenderTargetName", &Layer2DTexture::getRenderTargetName )
                //	;

                pybind::interface_<MousePickerTrapInterface>("MousePickerTrap", false)
                    .def( "pick", &MousePickerTrapInterface::pick )
                    ;

                pybind::interface_<HotSpot, pybind::bases<Node> >("HotSpot", false)
                    .def( "testPoint", &HotSpot::testPoint )
					.def( "setOutward", &HotSpot::setOutward )
					.def( "getOutward", &HotSpot::getOutward )
					.def( "setGlobal", &HotSpotPolygon::setGlobal )
					.def( "getGlobal", &HotSpotPolygon::getGlobal )
                    .def( "setDefaultHandle", &HotSpot::setDefaultHandle )
                    .def( "getDefaultHandle", &HotSpot::getDefaultHandle )
					.def( "isMousePickerOver", &HotSpot::isMousePickerOver )
                    ;

				pybind::interface_<HotSpotPolygon, pybind::bases<HotSpot> >("HotSpotPolygon", false)					
					.def( "setPolygon", &HotSpotPolygon::setPolygon )
					.def( "getPolygon", &HotSpotPolygon::getPolygon )					
					.def( "getPolygonWM", &HotSpotPolygon::getPolygonWM )
					.def( "clearPoints", &HotSpotPolygon::clearPoints )
					.def_proxy_static( "getLocalPolygonCenter", nodeScriptMethod, &NodeScriptMethod::s_getLocalPolygonCenter )
					.def_proxy_static( "getWorldPolygonCenter", nodeScriptMethod, &NodeScriptMethod::s_getWorldPolygonCenter )
					.def_proxy_static( "getWorldPolygon", nodeScriptMethod, &NodeScriptMethod::s_getWorldPolygon )
					;			

				pybind::interface_<HotSpotShape, pybind::bases<HotSpotPolygon> >("HotSpotShape", false)
					.def( "setResourceShape", &HotSpotShape::setResourceShape )
					.def( "getResourceShape", &HotSpotShape::getResourceShape )
					;

                pybind::interface_<HotSpotImage, pybind::bases<HotSpot> >("HotSpotImage", false)
                    .def( "setResourceHIT", &HotSpotImage::setResourceHIT )
                    .def( "getResourceHIT", &HotSpotImage::getResourceHIT )
                    .def( "setAlphaTest", &HotSpotImage::setAlphaTest )
                    .def( "getAlphaTest", &HotSpotImage::getAlphaTest )
                    .def( "getWidth", &HotSpotImage::getWidth )
                    .def( "getHeight", &HotSpotImage::getHeight )
                    ;

				pybind::interface_<HotSpotBubbles, pybind::bases<HotSpot> >("HotSpotBubbles", false)
					.def( "addBubble", &HotSpotBubbles::addBubble )
					;

				pybind::interface_<Shape, pybind::bases<Node, Materialable> >("Shape", false)
					.def( "setCenterAlign", &Shape::setCenterAlign )
					.def( "getCenterAlign", &Shape::getCenterAlign )
					.def( "setFlipX", &Shape::setFlipX )
					.def( "getFlipX", &Shape::getFlipX )
					.def( "setFlipY", &Shape::setFlipY )
					.def( "getFlipY", &Shape::getFlipY )
					.def( "setPercentVisibility", &Shape::setPercentVisibility )
					.def( "getPercentVisibility", &Shape::getPercentVisibility )
					.def( "setCustomSize", &Shape::setCustomSize )
					.def( "getCustomSize", &Shape::getCustomSize )
					.def( "removeCustomSize", &Shape::removeCustomSize )
					.def( "hasCustomSize", &Shape::hasCustomSize )
					.def_proxy_static( "setPercentVisibilityTo", nodeScriptMethod, &NodeScriptMethod::setPercentVisibilityTo )
					.def_proxy_static( "setPercentVisibilityStop", nodeScriptMethod, &NodeScriptMethod::setPercentVisibilityStop )
					.def( "setTextureUVOffset", &Shape::setTextureUVOffset )
					.def( "getTextureUVOffset", &Shape::getTextureUVOffset )
					.def( "setTextureUVScale", &Shape::setTextureUVScale )
					.def( "getTextureUVScale", &Shape::getTextureUVScale )
					;

				pybind::interface_<Landscape2D, pybind::bases<Node, Materialable> >( "Landscape2D", false )
					.def( "setBackParts", &Landscape2D::setBackParts )
					;

				pybind::interface_<Grid2D, pybind::bases<Node, Materialable> >( "Grid2D", false )
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

				pybind::interface_<Sprite, pybind::bases<Shape> >( "Sprite", false )
					.def_depricated( "setImageResource", &Sprite::setResourceImage, "Use setResourceImage" )
					.def_depricated( "getImageResource", &Sprite::getResourceImage, "Use getResourceImage" )
					.def( "setResourceImage", &Sprite::setResourceImage )
					.def( "getResourceImage", &Sprite::getResourceImage )

					.def( "setMaterialName", &Sprite::setMaterialName )
					.def( "getMaterialName", &Sprite::getMaterialName )

                    .def_proxy_static( "getImageSize", nodeScriptMethod, &NodeScriptMethod::s_getImageSize )
                    
                    .def_proxy_static( "getLocalImageCenter", nodeScriptMethod, &NodeScriptMethod::s_getLocalImageCenter )
                    .def_proxy_static( "getWorldImageCenter", nodeScriptMethod, &NodeScriptMethod::s_getWorldImageCenter )			
                    ;

				pybind::interface_<Gyroscope, pybind::bases<Node> >("Gyroscope", false)
					;

				pybind::interface_<Isometric, pybind::bases<Node> >("Isometric", false)
					;

                {
                    pybind::interface_<Animation, pybind::bases<Sprite, Animatable> >("Animation", false)
                        .def( "setAnimationResource", &Animation::setResourceAnimation )
                        .def( "getAnimationResource", &Animation::getResourceAnimation )
                        .def( "getFrameCount", &Animation::getFrameCount )
                        .def( "getFrameDelay", &Animation::getFrameDelay )
                        .def( "setCurrentFrame", &Animation::setCurrentFrame )
                        .def( "getCurrentFrame", &Animation::getCurrentFrame )
                        ;
                }

				{
					pybind::interface_<Model3D, pybind::bases<Node, Animatable, Materialable> >( "Model3D", false )
						.def( "setResourceModel3D", &Model3D::setResourceModel3D )
						.def( "getResourceModel3D", &Model3D::getResourceModel3D )
						;
				}

                //pybind::proxy_<RigidBody2D, pybind::bases<Node> >("RigidBody2D", false)
                //	.def( "applyForce", &RigidBody2D::applyForce )
                //	.def( "applyImpulse", &RigidBody2D::applyImpulse )
                //	.def( "applyConstantForce", &RigidBody2D::applyConstantForce )
                //	.def( "removeConstantForce", &RigidBody2D::removeConstantForce )
                //	.def( "setDirectionForce", &RigidBody2D::setDirectionForce )
                //	.def( "wakeUp", &RigidBody2D::wakeUp )
                //	.def( "getMass", &RigidBody2D::getMass )
                //	.def( "getLinearVelocity", &RigidBody2D::getLinearVelocity )
                //	.def( "setLinearVelocity", &RigidBody2D::setLinearVelocity )
                //	.def( "unsetLinearVelocity", &RigidBody2D::unsetLinearVelocity )
                //	.def( "setCollisionMask", &RigidBody2D::setCollisionMask )
                //	.def( "enableStabilization", &RigidBody2D::enableStabilization )
                //	;

                //pybind::proxy_<PhysicalBody2D, pybind::bases<Node> >("PhysicalBody2D", false)
                //	.def( "applyForce", &PhysicalBody2D::applyForce )
                //	.def( "applyImpulse", &PhysicalBody2D::applyImpulse )
                //	.def( "applyConstantForce", &PhysicalBody2D::applyConstantForce )
                //	.def( "removeConstantForce", &PhysicalBody2D::removeConstantForce )
                //	.def( "setDirectionForce", &PhysicalBody2D::setDirectionForce )
                //	.def( "wakeUp", &PhysicalBody2D::wakeUp )
                //	.def( "getMass", &PhysicalBody2D::getMass )
                //	.def( "getLinearVelocity", &PhysicalBody2D::getLinearVelocity )
                //	.def( "setLinearVelocity", &PhysicalBody2D::setLinearVelocity )
                //	.def( "unsetLinearVelocity", &PhysicalBody2D::unsetLinearVelocity )
                //	.def( "enableStabilization", &PhysicalBody2D::enableStabilization )
                //	.def( "setOrientation", &PhysicalBody2D::setOrientation )
                //	.def( "addShapeBox", &PhysicalBody2D::addShapeBox )
                //	.def( "addShapeCircle", &PhysicalBody2D::addShapeCircle )
                //	.def( "addShapeConvex", &PhysicalBody2D::addShapeConvex )
                //	.def( "createBody", &PhysicalBody2D::createBody )
                //	.def( "setFilterData", &PhysicalBody2D::setFilterData )
                //	.def( "setCollisionMask", &PhysicalBody2D::setCollisionMask )
                //	.def( "setCategoryBits", &PhysicalBody2D::setCategoryBits )
                //	.def( "setGroupIndex", &PhysicalBody2D::setGroupIndex )
                //	.def( "setLinearDumping", &PhysicalBody2D::setLinearDumping )
                //	.def( "setAngularDumping", &PhysicalBody2D::setAngularDumping )
                //	.def( "setAllowSleep", &PhysicalBody2D::setAllowSleep )
                //	.def( "setIsBullet", &PhysicalBody2D::setIsBullet )
                //	.def( "setFixedRotation", &PhysicalBody2D::setFixedRotation )
                //	.def( "getCollisionMask", &PhysicalBody2D::getCollisionMask )
                //	.def( "getCategoryBits", &PhysicalBody2D::getCategoryBits )
                //	.def( "getGroupIndex", &PhysicalBody2D::getGroupIndex )
                //	.def( "getLinearDumping", &PhysicalBody2D::getLinearDumping )
                //	.def( "getAngularDumping", &PhysicalBody2D::getAngularDumping )
                //	.def( "getAllowSleep", &PhysicalBody2D::getAllowSleep )
                //	.def( "getIsBullet", &PhysicalBody2D::getIsBullet )
                //	.def( "getFixedRotation", &PhysicalBody2D::getFixedRotation )
                //	.def( "getIsSensor", &PhysicalBody2D::getIsSensor )
                //	;

                //pybind::proxy_<TilePolygon, pybind::bases<RigidBody2D> >("TilePolygon", false)
                //	;

                pybind::interface_<Movie, pybind::bases<Node, Animatable> >("Movie", false)
                    .def( "setResourceMovie", &Movie::setResourceMovie )
                    .def( "getResourceMovie", &Movie::getResourceMovie )
                    .def( "setReverse", &Movie::setReverse )
                    .def( "getReverse", &Movie::getReverse )
					.def( "hasMovieLayer", &Movie::hasMovieLayer )
					.def( "setEnableMovieLayer", &Movie::setEnableMovieLayer )
					.def( "setEnableMovieLayers", &Movie::setEnableMovieLayers )
					.def_proxy_static( "getWorldAnchorPoint", nodeScriptMethod, &NodeScriptMethod::movie_getWorldAnchorPoint )
					.def_proxy_static( "getEnableMovieLayer", nodeScriptMethod, &NodeScriptMethod::movie_getEnableMovieLayer )
					.def_proxy_static( "getMovieSlot", nodeScriptMethod, &NodeScriptMethod::movie_getMovieSlot )
					.def_proxy_static( "hasMovieSlot", nodeScriptMethod, &NodeScriptMethod::movie_hasMovieSlot )
					.def_proxy_static( "getMovieText", nodeScriptMethod, &NodeScriptMethod::movie_getMovieText )
					.def_proxy_static( "hasMovieText", nodeScriptMethod, &NodeScriptMethod::movie_hasMovieText )
					.def_proxy_static( "getSubMovie", nodeScriptMethod, &NodeScriptMethod::movie_getSubMovie )
					.def_proxy_static( "hasSubMovie", nodeScriptMethod, &NodeScriptMethod::movie_hasSubMovie )
					.def_proxy_static( "getSocket", nodeScriptMethod, &NodeScriptMethod::movie_getSocket )
					.def_proxy_static( "hasSocket", nodeScriptMethod, &NodeScriptMethod::movie_hasSocket )
					.def_proxy_static( "setMovieEvent", nodeScriptMethod, &NodeScriptMethod::movie_setMovieEvent )
					.def_proxy_static( "hasMovieEvent", nodeScriptMethod, &NodeScriptMethod::movie_hasMovieEvent )
                    .def_proxy_static( "getSockets", nodeScriptMethod, &NodeScriptMethod::movie_getSockets )	
					.def_proxy_static( "getSlots", nodeScriptMethod, &NodeScriptMethod::movie_getSlots )
					.def_proxy_static( "getSubMovies", nodeScriptMethod, &NodeScriptMethod::movie_getSubMovies )
					.def_proxy_static( "getMovieNode", nodeScriptMethod, &NodeScriptMethod::movie_getMovieNode )
					.def_proxy_static( "hasMovieNode", nodeScriptMethod, &NodeScriptMethod::movie_hasMovieNode )					
					.def_proxy_static( "filterLayers", nodeScriptMethod, &NodeScriptMethod::movie_filterLayers )										
                    .def_proxy_static( "getFrameDuration", nodeScriptMethod, &NodeScriptMethod::movie_getFrameDuration )
                    .def_proxy_static( "getDuration", nodeScriptMethod, &NodeScriptMethod::movie_getDuration )
                    .def_proxy_static( "getFrameCount", nodeScriptMethod, &NodeScriptMethod::movie_getFrameCount )
                    .def_proxy_static( "getSize", nodeScriptMethod, &NodeScriptMethod::movie_getSize )
					.def_proxy_static( "getLayerPathLength", nodeScriptMethod, &NodeScriptMethod::movie_getLayerPathLength )
					.def_proxy_static( "getLayerPath", nodeScriptMethod, &NodeScriptMethod::movie_getLayerPath )
					.def_proxy_static( "getLayerPath2", nodeScriptMethod, &NodeScriptMethod::movie_getLayerPath2 )
					.def_proxy_static( "getMovieSlotWorldPosition", nodeScriptMethod, &NodeScriptMethod::movie_getMovieSlotWorldPosition )
					.def_proxy_static( "getMovieSlotOffsetPosition", nodeScriptMethod, &NodeScriptMethod::movie_getMovieSlotOffsetPosition )
					.def_proxy_static( "attachMovieSlotNode", nodeScriptMethod, &NodeScriptMethod::movie_attachMovieSlotNode )
                    ;

                pybind::interface_<MovieSlot, pybind::bases<Node> >("MovieSlot", false)
                    ;

                pybind::interface_<MovieInternalObject, pybind::bases<Node> >("MovieInternalObject", false)
                    ;

				pybind::interface_<Video, pybind::bases<Node, Animatable, Materialable> >( "Video", false )
                    .def( "setResourceVideo", &Video::setResourceVideo )
                    .def( "getResourceVideo", &Video::getResourceVideo )                    
                    ;

                pybind::interface_<Window, pybind::bases<Node> >("Window", false)
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

            pybind::def_functor( "setCurrentScene", nodeScriptMethod, &NodeScriptMethod::setCurrentScene );
            pybind::def_functor( "getCurrentScene", nodeScriptMethod, &NodeScriptMethod::getCurrentScene );

            pybind::def_functor( "createScene", nodeScriptMethod, &NodeScriptMethod::s_createScene );

            pybind::def_functor( "getCamera2DPosition", nodeScriptMethod, &NodeScriptMethod::s_getCamera2DPosition );

            pybind::def_functor( "createNode", nodeScriptMethod, &NodeScriptMethod::createNode );
            //pybind::def_function( "createNodeFromBinary", &ScriptMethod::createNodeFromBinary );
            pybind::def_functor( "destroyNode", nodeScriptMethod, &NodeScriptMethod::destroyNode );

            pybind::def_functor( "timing", nodeScriptMethod, &NodeScriptMethod::timing );
            pybind::def_functor( "timingRemove", nodeScriptMethod, &NodeScriptMethod::timingRemove );
					
			pybind::interface_<ScheduleManagerInterface>("ScheduleManagerInterface", true)
				.def_proxy_static( "timing", nodeScriptMethod, &NodeScriptMethod::ScheduleManagerInterface_timing )
				.def_proxy_static( "schedule", nodeScriptMethod, &NodeScriptMethod::ScheduleManagerInterface_schedule )
				.def_proxy_static( "pipe", nodeScriptMethod, &NodeScriptMethod::ScheduleManagerInterface_pipe )
				.def( "refresh", &ScheduleManagerInterface::refresh )
				.def( "exist", &ScheduleManagerInterface::exist )
				.def( "remove", &ScheduleManagerInterface::remove )
				.def( "removeAll", &ScheduleManagerInterface::removeAll )
				.def( "freeze", &ScheduleManagerInterface::freeze )
				.def( "freezeAll", &ScheduleManagerInterface::freezeAll )
				.def( "isFreeze", &ScheduleManagerInterface::isFreeze )
				.def( "time", &ScheduleManagerInterface::time )
				.def( "setSpeedFactor", &ScheduleManagerInterface::setSpeedFactor )
				.def( "getSpeedFactor", &ScheduleManagerInterface::getSpeedFactor )
				.def( "getTiming", &ScheduleManagerInterface::getTiming )
				;

			pybind::def_functor( "createTiming", nodeScriptMethod, &NodeScriptMethod::createTiming );
			pybind::def_functor( "destroyTiming", nodeScriptMethod, &NodeScriptMethod::destroyTiming );
			
			pybind::def_functor( "createScheduler", nodeScriptMethod, &NodeScriptMethod::createScheduler );
			pybind::def_functor( "destroyScheduler", nodeScriptMethod, &NodeScriptMethod::destroyScheduler );				

            pybind::def_functor( "schedule", nodeScriptMethod, &NodeScriptMethod::schedule );
            pybind::def_functor( "scheduleRemove", nodeScriptMethod, &NodeScriptMethod::scheduleRemove );
            pybind::def_functor( "scheduleRemoveAll", nodeScriptMethod, &NodeScriptMethod::scheduleRemoveAll );
            pybind::def_functor( "scheduleFreeze", nodeScriptMethod, &NodeScriptMethod::s_scheduleFreeze );
            pybind::def_functor( "scheduleFreezeAll", nodeScriptMethod, &NodeScriptMethod::s_scheduleFreezeAll );
            pybind::def_functor( "scheduleResumeAll", nodeScriptMethod, &NodeScriptMethod::scheduleResumeAll );
            pybind::def_functor( "scheduleIsFreeze", nodeScriptMethod, &NodeScriptMethod::s_scheduleIsFreeze );
            pybind::def_functor( "scheduleTime", nodeScriptMethod, &NodeScriptMethod::s_scheduleTime );

            pybind::def_functor( "scheduleGlobal", nodeScriptMethod, &NodeScriptMethod::s_scheduleGlobal );
            pybind::def_functor( "scheduleGlobalRemove", nodeScriptMethod, &NodeScriptMethod::s_scheduleGlobalRemove );
            pybind::def_functor( "scheduleGlobalRemoveAll", nodeScriptMethod, &NodeScriptMethod::s_scheduleGlobalRemoveAll );
            pybind::def_functor( "scheduleGlobalFreeze", nodeScriptMethod, &NodeScriptMethod::s_scheduleGlobalFreeze );
            pybind::def_functor( "scheduleGlobalFreezeAll", nodeScriptMethod, &NodeScriptMethod::s_scheduleGlobalFreezeAll );
            pybind::def_functor( "scheduleGlobalResumeAll", nodeScriptMethod, &NodeScriptMethod::s_scheduleGlobalResumeAll );
            pybind::def_functor( "scheduleGlobalIsFreeze", nodeScriptMethod, &NodeScriptMethod::s_scheduleGlobalIsFreeze );
            pybind::def_functor( "scheduleGlobalTime", nodeScriptMethod, &NodeScriptMethod::s_scheduleGlobalTime );


            pybind::def_functor( "getCursorPosition", nodeScriptMethod, &NodeScriptMethod::s_getCursorPosition );
			pybind::def_functor( "getTouchPosition", nodeScriptMethod, &NodeScriptMethod::s_getTouchPosition );

            pybind::def_functor( "setArrow", nodeScriptMethod, &NodeScriptMethod::s_setArrow );
            pybind::def_functor( "getArrow", nodeScriptMethod, &NodeScriptMethod::s_getArrow );

            pybind::def_functor( "setArrowLayer", nodeScriptMethod, &NodeScriptMethod::s_setArrowLayer );

            pybind::def_functor( "directResourceCompile", nodeScriptMethod, &NodeScriptMethod::directResourceCompile );
            pybind::def_functor( "directResourceRelease", nodeScriptMethod, &NodeScriptMethod::directResourceRelease );
            //pybind::def_function( "directResourceFileCompile", &ScriptMethod::s_directResourceFileCompile );
            //pybind::def_function( "deferredResourceFileCompile", &ScriptMethod::s_deferredResourceFileCompile );
            //pybind::def_function( "directResourceFileRelease", &ScriptMethod::s_directResourceFileRelease );
            pybind::def_functor( "getResourceReference", nodeScriptMethod, &NodeScriptMethod::s_getResourceReference );
            //pybind::def_function( "loadResourceImage", &ScriptMethod::s_loadResourceImage );
            //pybind::def_function( "loadImageResources", &ScriptMethod::s_loadImageResources );

			pybind::def_functor( "directFontCompile", nodeScriptMethod, &NodeScriptMethod::directFontCompile );
			pybind::def_functor( "directFontRelease", nodeScriptMethod, &NodeScriptMethod::directFontRelease );

            pybind::def_functor( "quitApplication", nodeScriptMethod, &NodeScriptMethod::quitApplication );
            //pybind::def_function( "createShot", &ScriptMethod::createShot );
            pybind::def_functor( "setFullscreenMode", nodeScriptMethod, &NodeScriptMethod::s_setFullscreenMode );
            pybind::def_functor( "getFullscreenMode", nodeScriptMethod, &NodeScriptMethod::s_getFullscreenMode );
            pybind::def_functor( "setFixedContentResolution", nodeScriptMethod, &NodeScriptMethod::s_setFixedContentResolution );
            pybind::def_functor( "getFixedContentResolution", nodeScriptMethod, &NodeScriptMethod::s_getFixedContentResolution );
            
            pybind::def_functor( "renderOneFrame", nodeScriptMethod, &NodeScriptMethod::renderOneFrame );
            pybind::def_functor( "writeImageToFile", nodeScriptMethod, &NodeScriptMethod::writeImageToFile );
            pybind::def_functor( "createImageResource", nodeScriptMethod, &NodeScriptMethod::s_createImageResource );
			pybind::def_functor( "createImageSolidResource", nodeScriptMethod, &NodeScriptMethod::s_createImageSolidResource );
            //pybind::def_function( "createFolder", &ScriptMethod::createFolder );
            //pybind::def_function( "deleteFolder", &ScriptMethod::deleteFolder );
            pybind::def_functor( "minimizeWindow", nodeScriptMethod, &NodeScriptMethod::minimizeWindow );
			
            pybind::def_functor( "getCurrentResolution", nodeScriptMethod, &NodeScriptMethod::s_getCurrentResolution );
            pybind::def_functor( "getContentResolution", nodeScriptMethod, &NodeScriptMethod::s_getContentResolution );
            pybind::def_functor( "getHotSpotImageSize", nodeScriptMethod, &NodeScriptMethod::s_getHotSpotImageSize );

			pybind::def_functor( "setNopause", nodeScriptMethod, &NodeScriptMethod::s_setNopause );
			pybind::def_functor( "getNopause", nodeScriptMethod, &NodeScriptMethod::s_getNopause );

            //pybind::def_function( "setBlow", &ScriptMethod::setBlow );
            //pybind::def_function( "getBlow", &ScriptMethod::getBlow );
            //pybind::def_function( "setEnoughBlow", &ScriptMethod::setEnoughBlow );
            //pybind::def_function( "setBlowCallback", &ScriptMethod::setBlowCallback );

            pybind::def_functor( "isInViewport", nodeScriptMethod, &NodeScriptMethod::s_isInViewport );
            //pybind::def_function( "getResourceCount", &ScriptMethod::s_getResourceCount );
            pybind::def_functor( "enableTextureFiltering", nodeScriptMethod, &NodeScriptMethod::s_enableTextureFiltering );
            pybind::def_functor( "isTextureFilteringEnabled", nodeScriptMethod, &NodeScriptMethod::s_isTextureFilteringEnabled );

            pybind::def_functor( "existText", nodeScriptMethod, &NodeScriptMethod::s_existText );

            pybind::def_functor( "pickHotspot", nodeScriptMethod, &NodeScriptMethod::s_pickHotspot );

            pybind::def_functor( "blockInput", nodeScriptMethod, &NodeScriptMethod::s_setMousePickerBlockInput );
            pybind::def_functor( "setMousePickerHandleValue", nodeScriptMethod, &NodeScriptMethod::s_setMousePickerHandleValue );

            pybind::def_functor( "setInputMouseButtonEventBlock", nodeScriptMethod, &NodeScriptMethod::s_setInputMouseButtonEventBlock );
            pybind::def_functor( "getInputMouseButtonEventBlock", nodeScriptMethod, &NodeScriptMethod::s_getInputMouseButtonEventBlock );

            pybind::def_functor( "loadPlugin", nodeScriptMethod, &NodeScriptMethod::s_loadPlugin );

            //pybind::def_function( "loadText", &ScriptMethod::s_loadText );

            pybind::def_functor( "getRenderCamera2D", nodeScriptMethod, &NodeScriptMethod::s_getRenderCamera2D );

            pybind::def_functor( "testHotspot", nodeScriptMethod, &NodeScriptMethod::s_testHotspot);
            pybind::def_functor( "polygon_wm", nodeScriptMethod, &NodeScriptMethod::s_polygon_wm);
			pybind::def_functor( "polygon_anchor", nodeScriptMethod, &NodeScriptMethod::s_polygon_anchor );

            pybind::def_functor( "showKeyboard", nodeScriptMethod, &NodeScriptMethod::showKeyboard );			
            pybind::def_functor( "hideKeyboard", nodeScriptMethod, &NodeScriptMethod::hideKeyboard );

            pybind::def_functor( "hasResource", nodeScriptMethod, &NodeScriptMethod::hasResource );

            pybind::def_functor( "removeCurrentScene", nodeScriptMethod, &NodeScriptMethod::removeCurrentScene );

            pybind::def_functor( "addMouseMoveHandler", nodeScriptMethod, &NodeScriptMethod::s_addMouseMoveHandler );
            pybind::def_functor( "addMouseButtonHandler", nodeScriptMethod, &NodeScriptMethod::s_addMouseButtonHandler );
			pybind::def_functor( "addMouseButtonHandlerBegin", nodeScriptMethod, &NodeScriptMethod::s_addMouseButtonHandlerBegin );
			pybind::def_functor( "addMouseButtonHandlerEnd", nodeScriptMethod, &NodeScriptMethod::s_addMouseButtonHandlerEnd );
			pybind::def_functor( "addMouseWheelHandler", nodeScriptMethod, &NodeScriptMethod::s_addMouseWheelHandler );
			pybind::def_functor( "addKeyHandler", nodeScriptMethod, &NodeScriptMethod::s_addKeyHandler );
			
			pybind::def_functor( "removeGlobalHandler", nodeScriptMethod, &NodeScriptMethod::s_removeGlobalHandler );
			pybind::def_functor( "enableGlobalHandler", nodeScriptMethod, &NodeScriptMethod::s_enableGlobalHandler );

            pybind::def_functor( "visitChild", nodeScriptMethod, &NodeScriptMethod::s_visitChild );

            pybind::def_functor( "visitCompiledResources", nodeScriptMethod, &NodeScriptMethod::s_visitCompiledResources );
            pybind::def_functor( "visitResources", nodeScriptMethod, &NodeScriptMethod::s_visitResources );

			pybind::def_functor( "incrementResources", nodeScriptMethod, &NodeScriptMethod::s_incrementResources );
			pybind::def_functor( "decrementResources", nodeScriptMethod, &NodeScriptMethod::s_decrementResources );

            pybind::def_functor( "validResource", nodeScriptMethod, &NodeScriptMethod::s_validResource );

            //pybind::def_function( "createDistanceJoint", &ScriptMethod::s_createDistanceJoint );
            //pybind::def_function( "createHingeJoint", &ScriptMethod::s_createHingeJoint );
            //pybind::def_function( "createMouseJoint", &ScriptMethod::s_createMouseJoint );
            //
            //pybind::def_function( "createGearJoint", &ScriptMethod::s_createGearJoint );
            //pybind::def_function( "createPrismaticJoint", &ScriptMethod::s_createPrismaticJoint );
            //pybind::def_function( "createPulleyJoint", &ScriptMethod::s_createPulleyJoint );
            //pybind::def_function( "createRopeJoint", &ScriptMethod::s_createRopeJoint );
            //pybind::def_function( "createWheelJoint", &ScriptMethod::s_createWheelJoint );
            //pybind::def_function( "destroyJoint", &ScriptMethod::s_destroyJoint );
            //pybind::def_function( "physicsMouseMove", &ScriptMethod::s_physicsMouseMove );

            pybind::def_functor( "getPlatformName", nodeScriptMethod, &NodeScriptMethod::s_getPlatformName );
            pybind::def_functor( "getTimingFactor", nodeScriptMethod, &NodeScriptMethod::s_getTimingFactor );
            pybind::def_functor( "setTimingFactor", nodeScriptMethod, &NodeScriptMethod::s_setTimingFactor );

            pybind::def_functor( "addHomeless", nodeScriptMethod, &NodeScriptMethod::s_addHomeless );						
			pybind::def_functor( "isHomeless", nodeScriptMethod, &NodeScriptMethod::s_isHomeless );

            pybind::def_functor( "cancelTask", nodeScriptMethod, &NodeScriptMethod::s_cancelTask );
            pybind::def_functor( "joinTask", nodeScriptMethod, &NodeScriptMethod::s_joinTask );
			            
            pybind::def_functor( "getNullObjectsFromResourceVideo", nodeScriptMethod, &NodeScriptMethod::s_getNullObjectsFromResourceVideo );

            pybind::def_functor( "hasMovieSlot", nodeScriptMethod, &NodeScriptMethod::s_hasMovieSlot );
            pybind::def_functor( "hasMovieSubMovie", nodeScriptMethod, &NodeScriptMethod::s_hasMovieSubMovie );
            pybind::def_functor( "hasMovieSocket", nodeScriptMethod, &NodeScriptMethod::s_hasMovieSocket );
            pybind::def_functor( "hasMovieEvent", nodeScriptMethod, &NodeScriptMethod::s_hasMovieEvent );

            pybind::def_functor( "getMovieDuration", nodeScriptMethod, &NodeScriptMethod::s_getMovieDuration );

			pybind::def_functor( "getGameAspect", nodeScriptMethod, &NodeScriptMethod::s_getGameAspect );
			pybind::def_functor( "getGameViewport", nodeScriptMethod, &NodeScriptMethod::s_getGameViewport );
                        
            pybind::def_functor( "getGameParam", nodeScriptMethod, &NodeScriptMethod::s_getGameParam );
            pybind::def_functor( "getGameParamFloat", nodeScriptMethod, &NodeScriptMethod::s_getGameParamFloat );
            pybind::def_functor( "getGameParamInt", nodeScriptMethod, &NodeScriptMethod::s_getGameParamInt );
            pybind::def_functor( "getGameParamBool", nodeScriptMethod, &NodeScriptMethod::s_getGameParamBool );
            
            pybind::def_functor( "hasGameParam", nodeScriptMethod, &NodeScriptMethod::s_hasGameParam );
            pybind::def_functor( "openUrlInDefaultBrowser", nodeScriptMethod, &NodeScriptMethod::s_openUrlInDefaultBrowser );

            pybind::def_functor( "getDefaultResourceFontName", nodeScriptMethod, &NodeScriptMethod::s_getDefaultResourceFontName );


            pybind::def_functor( "pushMouseMove", nodeScriptMethod, &NodeScriptMethod::s_pushMouseMove );
            pybind::def_functor( "pushMouseButtonEvent", nodeScriptMethod, &NodeScriptMethod::s_pushMouseButtonEvent );

            pybind::def_functor( "platformEvent", nodeScriptMethod, &NodeScriptMethod::s_platformEvent );

			pybind::def_functor( "getProjectCodename", nodeScriptMethod, &NodeScriptMethod::s_getProjectCodename );

			pybind::def_functor( "sleep", nodeScriptMethod, &NodeScriptMethod::s_sleep );

			pybind::def_functor( "downloadAsset", nodeScriptMethod, &NodeScriptMethod::s_downloadAsset );
			pybind::def_functor( "cancelDownloadAsset", nodeScriptMethod, &NodeScriptMethod::s_cancelDownloadAsset );

			pybind::def_functor( "loadResourcePak", nodeScriptMethod, &NodeScriptMethod::s_loadResourcePak );
			
			pybind::def_functor( "existFile", nodeScriptMethod, &NodeScriptMethod::s_existFile );
			pybind::def_functor( "parseXml", nodeScriptMethod, &NodeScriptMethod::s_parseXml );

			pybind::def_functor( "visitFonts", nodeScriptMethod, &NodeScriptMethod::s_visitFonts );
			pybind::def_functor( "getFonts", nodeScriptMethod, &NodeScriptMethod::s_getFonts );
			pybind::def_functor( "hasFont", nodeScriptMethod, &NodeScriptMethod::s_hasFont );
			pybind::def_functor( "validateFont", nodeScriptMethod, &NodeScriptMethod::s_validateFont );

			pybind::def_functor( "prefetchResources", nodeScriptMethod, &NodeScriptMethod::s_prefetchResources );
			pybind::def_functor( "unfetchResources", nodeScriptMethod, &NodeScriptMethod::s_unfetchResources );

			pybind::def_functor( "cacheResources", nodeScriptMethod, &NodeScriptMethod::s_cacheResources );
			pybind::def_functor( "uncacheResources", nodeScriptMethod, &NodeScriptMethod::s_uncacheResources );

			pybind::def_functor( "rotateToTrimetric", nodeScriptMethod, &NodeScriptMethod::s_rotateToTrimetric );
			pybind::def_functor( "rotateToTrimetric2", nodeScriptMethod, &NodeScriptMethod::s_rotateToTrimetric2 );
						
			pybind::def_functor( "hotspotCorrect", nodeScriptMethod, &NodeScriptMethod::s_hotspotCorrect );

			pybind::def_functor( "copyUserPicture", nodeScriptMethod, &NodeScriptMethod::s_copyUserPicture );
			pybind::def_functor( "copyUserMusic", nodeScriptMethod, &NodeScriptMethod::s_copyUserMusic );

			pybind::def_functor( "screenToWorldPoint", nodeScriptMethod, &NodeScriptMethod::s_screenToWorldPoint );
			pybind::def_functor( "screenToWorldClick", nodeScriptMethod, &NodeScriptMethod::s_screenToWorldClick );

			pybind::def_functor( "getMovieSlotsPosition", nodeScriptMethod, &NodeScriptMethod::s_getMovieSlotsPosition );
			pybind::def_functor( "getMovieSlotPosition", nodeScriptMethod, &NodeScriptMethod::s_getMovieSlotPosition );

			pybind::def_functor( "gridBurnTransparency", nodeScriptMethod, &NodeScriptMethod::s_gridBurnTransparency );

			pybind::def_functor( "setLocale", nodeScriptMethod, &NodeScriptMethod::s_setLocale );
        }
    }
}
