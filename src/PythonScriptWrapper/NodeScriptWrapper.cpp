#	include "PythonScriptWrapper.h"

#	include "Config/Blobject.h"

#   include "Interface/ApplicationInterface.h"
#   include "Interface/FileSystemInterface.h"
#   include "Interface/TimelineInterface.h"
#   include "Interface/StringizeInterface.h"

#   include "Interface/InputSystemInterface.h"
#   include "Interface/NodeInterface.h"
#   include "Interface/MemoryInterface.h"
#	include "Interface/HttpSystemInterface.h"
#	include "Interface/PrefetcherInterface.h"
#	include "Interface/PlatformInterface.h"
#	include "Interface/PackageInterface.h"

#	include "Kernel/ThreadTask.h"
#	include "Kernel/Scene.h"
#	include "Kernel/Arrow.h"

#   include "Interface/ScriptSystemInterface.h"
#   include "Interface/ScheduleManagerInterface.h"

#   include "Interface/TextInterface.h"
#   include "Interface/AccountInterface.h"

#   include "Interface/UnicodeInterface.h"

#   include "Interface/ResourceInterface.h"

#	include "Kernel/ResourceImageData.h"

#	include "Menge/ResourceFile.h"
#	include "Menge/ResourceMovie.h"
#	include "Menge/ResourceAnimation.h"
#	include "Menge/ResourceModel3D.h"
#	include "Menge/ResourceVideo.h"
#	include "Menge/ResourceSound.h"
#   include "Menge/ResourceImageSolid.h"

#	include "Menge/ResourceImageDefault.h"
#	include "Menge/ResourceTestPick.h"
#	include "Menge/ResourceHIT.h"
#	include "Menge/ResourceShape.h"
#	include "Menge/ResourceInternalObject.h"

#   include "Menge/ResourceImageSubstractRGBAndAlpha.h"
#   include "Menge/ResourceImageSubstract.h"

#   include "Interface/ApplicationInterface.h"
#   include "Interface/MousePickerSystemInterface.h"

#   include "Menge/MovieSlot.h"
#	include "Menge/MovieInternalObject.h"
#   include "Menge/MovieEvent.h"
#	include "Menge/ParticleEmitter2.h"
#	include "Menge/Model3D.h"
#	include "Menge/HotSpot.h"
#	include "Menge/HotSpotPolygon.h"
#	include "Menge/HotSpotCircle.h"
#	include "Menge/HotSpotBubbles.h"
#	include "Menge/HotSpotImage.h"
#   include "Menge/HotSpotShape.h"
#	include "Menge/Landscape2D.h"
#	include "Menge/Grid2D.h"

#   include "Menge/ShapeQuadFixed.h"
#   include "Menge/ShapeQuadFlex.h"

//#	include "Light2D.h"
#	include "Menge/ShadowCaster2D.h"
#	include "Menge/Gyroscope.h"
#	include "Menge/TextField.h"
#	include "Menge/SoundEmitter.h"
#	include "Menge/Point.h"
#	include "Menge/Line.h"
#	include "Kernel/RenderCameraProjection.h"
//#	include "RigidBody3D.h"
//#	include "CapsuleController.h"
//#	include "Skeleton.h"

#	include "ScriptHolder.h"

#	include "Menge/SurfaceVideo.h"
#	include "Menge/SurfaceSound.h"
#	include "Menge/SurfaceImage.h"
#	include "Menge/SurfaceImageSequence.h"
#	include "Menge/SurfaceSolidColor.h"


#	include "Kernel/Isometric.h"
#	include "Kernel/Parallax.h"
#	include "Kernel/RenderViewport.h"
#	include "Kernel/RenderClipplane.h"
#	include "Kernel/RenderCameraOrthogonal.h"
#	include "Kernel/RenderCameraProjection.h"
#	include "Kernel/RenderCameraOrthogonalTarget.h"

#   include "Kernel/ResourceImage.h"

#	include "Menge/Layer2D.h"


#	include "Menge/TilePolygon.h"

#	include "Menge/Movie.h"
#	include "Menge/Meshget.h"

#	include "Menge/Window.h"


#	include "Kernel/Shape.h"
#	include "Kernel/Entity.h"

//#	include "DiscreteEntity.h"

//#	include "SoundEngine.h"
#	include "Logger/Logger.h"

#   include "Interface/RenderSystemInterface.h"
#   include "Interface/PhysicSystemInterface.h"
#   include "Interface/ThreadSystemInterface.h"

#	include "Kernel/Identity.h"
#	include "Kernel/Affector.h"
#   include "Kernel/ThreadTask.h"
#   include "Kernel/ScriptEventReceiver.h"
#   include "Kernel/ScriptWrapper.h"

#   include "PythonAnimatableEventReceiver.h"

#	include "Core/Polygon.h"
#	include "Core/MemoryHelper.h"
#	include "Core/ValueFollower.h"

#	include "Math/angle.h"
#	include "Math/vec4.h"
#	include "Math/mat3.h"
#	include "Math/mat4.h"
#	include "Math/quat.h"
#	include "Math/utils.h"

#	include "Core/Rect.h"
#	include "Core/String.h"
#	include "Core/Polygon.h"
#	include "Core/ValueFollower.h"

#	include "Factory/FactoryPool.h"

#	include "pybind/stl_type_cast.hpp"
#	include "stdex/xml_sax_parser.h"
#   include "utf8.h"

#	include <sstream>

#	include <math.h>

namespace Menge
{
	//////////////////////////////////////////////////////////////////////////
	typedef stdex::vector<ResourceImage *> TVectorResourceImage;
	typedef stdex::vector<HotSpotPolygon *> TVectorHotSpotPolygon;
	//////////////////////////////////////////////////////////////////////////
	class NodeScriptMethod
	{
	public:
		NodeScriptMethod( ServiceProviderInterface * _serviceProvider )
			: m_serviceProvider( _serviceProvider )
			, m_creatorAffectorNodeFollowerLocalAlpha( _serviceProvider )
			, m_creatorAffectorNodeFollowerCustomSize( _serviceProvider )
			, m_creatorAffectorNodeFollowerTextureUVScale( _serviceProvider )
			, m_factoryAffectorVelocity2( _serviceProvider )
			, m_nodeAffectorCreatorInterpolateLinear( _serviceProvider )
			, m_nodeAffectorCreatorInterpolateLinearFloat( _serviceProvider )
			, m_nodeAffectorCreatorInterpolateQuadratic( _serviceProvider )
			, m_nodeAffectorCreatorInterpolateQuadraticBezier( _serviceProvider )
			, m_nodeAffectorCreatorInterpolateCubicBezier( _serviceProvider )
			, m_nodeAffectorCreatorInterpolateQuarticBezier( _serviceProvider )
			, m_nodeAffectorCreatorInterpolateParabolic( _serviceProvider )
			, m_nodeAffectorCreatorInterpolateQuadraticFloat( _serviceProvider )
			, m_nodeAffectorCreatorInterpolateLinearColour( _serviceProvider )
			, m_nodeAffectorCreatorInterpolateLinearVec4( _serviceProvider )
			, m_nodeAffectorCreatorFollowTo( _serviceProvider )
			, m_nodeAffectorCreatorFollowToW( _serviceProvider )
			, m_nodeAffectorCreatorAccumulateLinear( _serviceProvider )
		{
			m_factoryPyObjectTimingListener = new FactoryPool<PyScheduleTimerInterface, 8>( m_serviceProvider );
			m_factoryPySchedulePipeInterface = new FactoryPool<PySchedulePipeInterface, 8>( m_serviceProvider );
			m_factoryDelaySchedulePipeInterface = new FactoryPool<DelaySchedulePipeInterface, 8>( m_serviceProvider );
			m_factoryPyObjectScheduleListener = new FactoryPool<PyScheduleEventInterface, 8>( m_serviceProvider );
			m_factoryPythonSceneChangeCallback = new FactoryPool<PythonSceneChangeCallback, 8>( m_serviceProvider );
			m_factoryAffectorGridBurnTransparency = new FactoryPool<AffectorGridBurnTransparency, 4>( m_serviceProvider );
			m_factoryAffectorUser = new FactoryPool<AffectorUser, 4>( m_serviceProvider );
			m_factoryNodeAffectorCallback = new FactoryPool<ScriptableAffectorCallback, 4>( m_serviceProvider );
			m_factoryPyGlobalMouseMoveHandlers = new FactoryPool<PyGlobalMouseMoveHandler, 32>( m_serviceProvider );
			m_factoryPyGlobalMouseHandlerButtons = new FactoryPool<PyGlobalMouseHandlerButton, 32>( m_serviceProvider );
			m_factoryPyGlobalMouseHandlerButtonEnds = new FactoryPool<PyGlobalMouseHandlerButtonEnd, 32>( m_serviceProvider );
			m_factoryPyGlobalMouseHandlerWheels = new FactoryPool<PyGlobalMouseHandlerWheel, 32>( m_serviceProvider );
			m_factoryPyGlobalMouseHandlerButtonBegins = new FactoryPool<PyGlobalMouseHandlerButtonBegin, 32>( m_serviceProvider );
			m_factoryPyGlobalKeyHandler = new FactoryPool<PyGlobalKeyHandler, 32>( m_serviceProvider );
			m_factoryPyGlobalTextHandler = new FactoryPool<PyGlobalTextHandler, 32>( m_serviceProvider );
		}

	public:
		//////////////////////////////////////////////////////////////////////////
		PyObject * textfield_setTextFormatArgs( pybind::kernel_interface * _kernel, TextField * _textField, PyObject * _args, PyObject * _kwds )
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
						LOGGER_ERROR( m_serviceProvider )("textfield_setTextFormatArgs %s invalid get str %s"
							, pybind::object_repr( py_string )
							);

						return pybind::ret_false();
					}

					cs_args.push_back( key );
				}
				else if( pybind::unicode_check( py_string ) == true )
				{
					WString key;
					if( pybind::extract_value( _kernel, py_string, key, false ) == false )
					{
						LOGGER_ERROR( m_serviceProvider )("textfield_setTextFormatArgs %s invalid get unicode %s"
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
						LOGGER_ERROR( m_serviceProvider )("textfield_setTextFormatArgs %s not suport arg %s"
							, pybind::object_repr( py_string )
							);

						return pybind::ret_false();
					}

					cs_args.push_back( String( value ) );
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
		//WString textfield_getText( TextField * _textField )
		//{
		//	const U32String & utf8 = _textField->getText();

		//	WString unicode;
		//	Helper::utf8ToUnicode( m_serviceProvider, utf8, unicode );

		//	return unicode;
		//}
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
		Node * movie_getMovieSlot( Movie * _movie, const ConstString & _name )
		{
			Node * node;
			Movie * submovie;
			if( _movie->getMovieNode( _name, STRINGIZE_STRING_LOCAL( m_serviceProvider, "MovieSlot" ), &node, &submovie ) == false )
			{
				return nullptr;
			}

			return node;
		}
		//////////////////////////////////////////////////////////////////////////
		RenderClipplane * movie_getMovieClipplane( Movie * _movie, const ConstString & _name )
		{
			RenderClipplane * clipplane;
			if( _movie->getMovieClipplane( _name, &clipplane ) == false )
			{
				return nullptr;
			}

			return clipplane;
		}
		//////////////////////////////////////////////////////////////////////////
		bool movie_hasMovieClipplane( Movie * _movie, const ConstString & _name )
		{
			bool result = _movie->hasMovieClipplane( _name );

			return result;
		}
		//////////////////////////////////////////////////////////////////////////
		bool movie_hasMovieSlot( Movie * _movie, const ConstString & _name )
		{
			Node * node;
			Movie * submovie;
			bool successful = _movie->hasMovieNode( _name, STRINGIZE_STRING_LOCAL( m_serviceProvider, "MovieSlot" ), &node, &submovie );

			return successful;
		}
		//////////////////////////////////////////////////////////////////////////
		Node * movie_getMovieText( Movie * _movie, const ConstString & _name )
		{
			Node * node;
			Movie * submovie;

			if( _movie->getMovieNode( _name, STRINGIZE_STRING_LOCAL( m_serviceProvider, "MovieText" ), &node, &submovie ) == false )
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

			bool successful = _movie->hasMovieNode( _name, STRINGIZE_STRING_LOCAL( m_serviceProvider, "MovieText" ), &node, &submovie );

			return successful;
		}
		//////////////////////////////////////////////////////////////////////////
		Node * movie_getSubMovie( Movie * _movie, const ConstString & _name )
		{
			Node * node;
			Movie * submovie;
			if( _movie->getMovieNode( _name, STRINGIZE_STRING_LOCAL( m_serviceProvider, "SubMovie" ), &node, &submovie ) == false )
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
			bool successful = _movie->hasMovieNode( _name, STRINGIZE_STRING_LOCAL( m_serviceProvider, "SubMovie" ), &node, &submovie );

			return successful;
		}
		//////////////////////////////////////////////////////////////////////////
		Node * movie_getSocket( Movie * _movie, const ConstString & _name )
		{
			Node * node;
			Movie * submovie;

			if( _movie->hasMovieNode( _name, STRINGIZE_STRING_LOCAL( m_serviceProvider, "MovieSocketImage" ), &node, &submovie ) == true )
			{
				return node;
			}

			if( _movie->hasMovieNode( _name, STRINGIZE_STRING_LOCAL( m_serviceProvider, "MovieSocketShape" ), &node, &submovie ) == true )
			{
				return node;
			}

			LOGGER_ERROR( m_serviceProvider )("Movie::getSocket: movie %s resource %s not found %s"
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

			if( _movie->hasMovieNode( _name, STRINGIZE_STRING_LOCAL( m_serviceProvider, "MovieSocketImage" ), &node, &submovie ) == true )
			{
				return true;
			}

			if( _movie->hasMovieNode( _name, STRINGIZE_STRING_LOCAL( m_serviceProvider, "MovieSocketShape" ), &node, &submovie ) == true )
			{
				return true;
			}

			return false;
		}
		//////////////////////////////////////////////////////////////////////////
		bool movie_setMovieEvent( Movie * _movie, const ConstString & _name, const pybind::object & _cb, const pybind::detail::args_operator_t & _args )
		{
			Node * node;
			Movie * submovie;

			if( _movie->getMovieNode( _name, STRINGIZE_STRING_LOCAL( m_serviceProvider, "MovieEvent" ), &node, &submovie ) == false )
			{
				return false;
			}

			MovieEvent * ev = static_node_cast<MovieEvent *>(node);

			ev->setEvent( _cb, _args );

			return true;
		}
		//////////////////////////////////////////////////////////////////////////
		bool movie_removeMovieEvent( Movie * _movie, const ConstString & _name )
		{
			Node * node;
			Movie * submovie;

			if( _movie->getMovieNode( _name, STRINGIZE_STRING_LOCAL( m_serviceProvider, "MovieEvent" ), &node, &submovie ) == false )
			{
				return false;
			}

			MovieEvent * ev = static_node_cast<MovieEvent *>(node);

			ev->removeEvent();

			return true;
		}
		//////////////////////////////////////////////////////////////////////////
		bool movie_hasMovieEvent( Movie * _movie, const ConstString & _name )
		{
			Node * node;
			Movie * submovie;
			bool successful = _movie->hasMovieNode( _name, STRINGIZE_STRING_LOCAL( m_serviceProvider, "MovieEvent" ), &node, &submovie );

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
			void visitMovieNode( Movie * _movie, Node * _node ) override
			{
				MovieSlot * slot = static_node_cast<MovieSlot *>(_node);

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
			_movie->visitMovieLayer( STRINGIZE_STRING_LOCAL( m_serviceProvider, "MovieSlot" ), &visitor );

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
			void visitMovieNode( Movie * _movie, Node * _node ) override
			{
				HotSpot * hotspot = static_node_cast<HotSpot *>(_node);

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
			_movie->visitMovieLayer( STRINGIZE_STRING_LOCAL( m_serviceProvider, "MovieSocketImage" ), &visitor );
			_movie->visitMovieLayer( STRINGIZE_STRING_LOCAL( m_serviceProvider, "MovieSocketShape" ), &visitor );

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
			void visitMovieNode( Movie * _movie, Node * _node ) override
			{
				Movie * subMovie = static_node_cast<Movie *>(_node);

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
			_movie->visitMovieLayer( STRINGIZE_STRING_LOCAL( m_serviceProvider, "SubMovie" ), &visitor );

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
			void visitMovieNode( Movie * _movie, Node * _layer ) override
			{
				m_list.append( pybind::make_tuple_t( m_kernel, _movie, _layer ) );
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
				LOGGER_ERROR( m_serviceProvider )("Movie.getFrameDuration: '%s' not activate"
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
				LOGGER_ERROR( m_serviceProvider )("Movie.getDuration: '%s' not activate"
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
				LOGGER_ERROR( m_serviceProvider )("Movie.getFrameCount: '%s' not activate"
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
				LOGGER_ERROR( m_serviceProvider )("Movie.getSize: '%s' not activate"
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
			Movie * sub_movie;
			if( _movie->getMovieLayer( _name, &layer, &sub_movie ) == false )
			{
				LOGGER_ERROR( m_serviceProvider )("Movie::getLayerPathLength: '%s' not found layer '%s'"
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
		pybind::list movie_getLayerPath( pybind::kernel_interface * _kernel, Movie * _movie, const ConstString & _name )
		{
			const MovieLayer * layer;
			Movie * sub_movie;
			if( _movie->getMovieLayer( _name, &layer, &sub_movie ) == false )
			{
				LOGGER_ERROR( m_serviceProvider )("Movie::getLayerPathLength: '%s' not found layer '%s'"
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
				LOGGER_ERROR( m_serviceProvider )("Movie::getLayerPathLength: sub_movie '%s' not found layer '%s' frame pack is null"
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
			Movie * sub_movie;
			if( _movie->getMovieLayer( _name, &layer, &sub_movie ) == false )
			{
				LOGGER_ERROR( m_serviceProvider )("Movie::getLayerPathLength: '%s' not found layer '%s'"
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
			Movie * sub_movie;
			if( _movie->getMovieLayer( _name, &layer, &sub_movie ) == false )
			{
				LOGGER_ERROR( m_serviceProvider )("Movie::getLayerPathLength: '%s' not found layer '%s'"
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
					LOGGER_ERROR( m_serviceProvider )("Movie::getLayerPathLength: '%s' invalid compile"
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

				pos.y *= mt::m_sqrt2;

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
			Movie * sub_movie;
			if( _movie->getMovieLayer( _name, &layer, &sub_movie ) == false )
			{
				LOGGER_ERROR( m_serviceProvider )("Movie::getLayerPathLength: '%s' not found layer '%s'"
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
					LOGGER_ERROR( m_serviceProvider )("Movie::getLayerPathLength: '%s' invalid compile"
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
			Node * node;
			Movie * submovie;
			if( _movie->getMovieNode( _slotName, STRINGIZE_STRING_LOCAL( m_serviceProvider, "MovieSlot" ), &node, &submovie ) == false )
			{
				LOGGER_ERROR( m_serviceProvider )("Movie::getMovieSlotWorldPosition %s not found slot '%s"
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
			Node * node;
			Movie * submovie;
			if( _movie->getMovieNode( _slotName, STRINGIZE_STRING_LOCAL( m_serviceProvider, "MovieSlot" ), &node, &submovie ) == false )
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
			mt::mul_v3_v3_m4( wap, ap, wm );

			const mt::vec3f & wp = node->getWorldPosition();

			mt::vec3f sop = wp - wap;

			return sop;
		}
		//////////////////////////////////////////////////////////////////////////
		bool movie_attachMovieSlotNode( Movie * _movie, const ConstString & _slotName, Node * _node )
		{
			Node * node;
			Movie * submovie;
			if( _movie->getMovieNode( _slotName, STRINGIZE_STRING_LOCAL( m_serviceProvider, "MovieSlot" ), &node, &submovie ) == false )
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
		bool movie_removeAllMovieSlotNode( Movie * _movie, const ConstString & _slotName )
		{
			Node * node;
			Movie * submovie;
			if( _movie->getMovieNode( _slotName, STRINGIZE_STRING_LOCAL( m_serviceProvider, "MovieSlot" ), &node, &submovie ) == false )
			{
				LOGGER_ERROR( m_serviceProvider )("Movie::removeAllMovieSlotNode %s not found slot '%s"
					, _movie->getName().c_str()
					, _slotName.c_str()
					);

				return false;
			}

			node->removeChildren();

			return true;
		}
		//////////////////////////////////////////////////////////////////////////
		void Transformation3D_setAngleDeg( Transformation3D * _transformation, float _angle )
		{
			float rad = _angle * mt::m_deg2rad;

			_transformation->setOrientationX( rad );
		}
		//////////////////////////////////////////////////////////////////////////
		void Transformation3D_removeRelationTransformation( Transformation3D * _transformation )
		{
			_transformation->setRelationTransformation( nullptr );
		}
		//////////////////////////////////////////////////////////////////////////
		void Transformation3D_coordinate( Transformation3D * _transformation, const mt::vec3f & _coordinate )
		{
			const mt::vec3f & origin = _transformation->getOrigin();
			_transformation->setOrigin( origin + _coordinate );
			_transformation->translate( _coordinate );
		}
		//////////////////////////////////////////////////////////////////////////
		mt::vec3f ResourceMovie_getLayerPosition( ResourceMovie * _movie, const ConstString & _name )
		{
			const MovieLayer * layer;
			bool successful = _movie->hasMovieLayer( _name, &layer );

			if( successful == false )
			{
				LOGGER_ERROR( m_serviceProvider )("ResourceMovie::getLayerPosition %s not found layer '%s'"
					, _movie->getName().c_str()
					, _name.c_str()
					);

				return mt::vec3f( 0.f, 0.f, 0.f );
			}

			return layer->position;
		}
		//////////////////////////////////////////////////////////////////////////
		float ResourceMovie_getLayerIn( ResourceMovie * _movie, const ConstString & _name )
		{
			const MovieLayer * layer;
			bool successful = _movie->hasMovieLayer( _name, &layer );

			if( successful == false )
			{
				LOGGER_ERROR( m_serviceProvider )("ResourceMovie::getLayerIn %s not found layer '%s'"
					, _movie->getName().c_str()
					, _name.c_str()
					);

				return 0.f;
			}

			return layer->in;
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
		mt::vec2f s_getScreenPolygonCenter( HotSpotPolygon * _hs )
		{
			const Resolution & contentResolution = APPLICATION_SERVICE( m_serviceProvider )
				->getContentResolution();

			const RenderCameraInterface * camera = _hs->getRenderCameraInheritance();
			const RenderViewportInterface * viewport = _hs->getRenderViewportInheritance();

			mt::box2f b1;
			_hs->getPolygonScreen( camera, viewport, contentResolution, &b1, nullptr );

			mt::vec2f c;
			mt::get_center_box( b1, c );

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
		}
		//////////////////////////////////////////////////////////////////////////
		bool s_loadPlugin( const WString & _pluginName )
		{
			bool successful = PLUGIN_SERVICE( m_serviceProvider )
				->loadPlugin( _pluginName );

			return successful;
		}
		//////////////////////////////////////////////////////////////////////////
		void s_setMousePickerBlockInput( bool _value )
		{
			MousePickerSystemInterface * mousePickerSystem = PLAYER_SERVICE( m_serviceProvider )
				->getMousePickerSystem();

			mousePickerSystem->setBlock( _value );
		}
		//////////////////////////////////////////////////////////////////////////
		void s_setMousePickerHandleValue( bool _value )
		{
			MousePickerSystemInterface * mousePickerSystem = PLAYER_SERVICE( m_serviceProvider )
				->getMousePickerSystem();

			mousePickerSystem->setHandleValue( _value );
		}
		//////////////////////////////////////////////////////////////////////////
		void s_setInputMouseButtonEventBlock( bool _value )
		{
			APPLICATION_SERVICE( m_serviceProvider )
				->setInputMouseButtonEventBlock( _value );
		}
		//////////////////////////////////////////////////////////////////////////
		bool s_getInputMouseButtonEventBlock()
		{
			return APPLICATION_SERVICE( m_serviceProvider )
				->getInputMouseButtonEventBlock();
		}
		//////////////////////////////////////////////////////////////////////////
		class PyScheduleTimerInterface
			: public ScheduleTimerInterface
		{
		public:
			PyScheduleTimerInterface()
				: m_serviceProvider( nullptr )
			{
			}

			~PyScheduleTimerInterface()
			{
			}

		public:
			void initialize( ServiceProviderInterface * _serviceProvider, const pybind::object & _cb, const pybind::detail::args_operator_t & _args )
			{
				m_serviceProvider = _serviceProvider;
				m_cb = _cb;
				m_args = _args;
			}

		protected:
			void onScheduleUpdate( uint32_t _id, uint32_t _iterate, float _delay ) override
			{
				m_cb.call_args( _id, _iterate, _delay, false, false, m_args );
			}

			void onScheduleComplete( uint32_t _id ) override
			{
				m_cb.call_args( _id, 0, 0.f, true, false, m_args );
			}

			void onScheduleStop( uint32_t _id ) override
			{
				m_cb.call_args( _id, 0, 0.f, false, true, m_args );
			}

		protected:
			ServiceProviderInterface * m_serviceProvider;
			pybind::object m_cb;
			pybind::detail::args_operator_t m_args;
		};
		//////////////////////////////////////////////////////////////////////////
		FactoryPtr m_factoryPyObjectTimingListener;
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
			void initialize( ServiceProviderInterface * _serviceProvider, const pybind::object & _cb, const pybind::detail::args_operator_t & _args )
			{
				m_serviceProvider = _serviceProvider;
				m_cb = _cb;
				m_args = _args;
			}

		protected:
			float onSchedulePipe( uint32_t _id, uint32_t _index ) override
			{
				float delay = m_cb.call_args( _id, _index, m_args );

				return delay;
			}

		protected:
			ServiceProviderInterface * m_serviceProvider;
			pybind::object m_cb;
			pybind::detail::args_operator_t m_args;
		};
		//////////////////////////////////////////////////////////////////////////
		FactoryPtr m_factoryPySchedulePipeInterface;
		//////////////////////////////////////////////////////////////////////////
		class DelaySchedulePipeInterface
			: public SchedulePipeInterface
		{
		public:
			DelaySchedulePipeInterface()
				: m_serviceProvider( nullptr )
				, m_delay( 0.f )
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
		FactoryPtr m_factoryDelaySchedulePipeInterface;
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
			void initialize( ServiceProviderInterface * _serviceProvider, const pybind::object & _cb, const pybind::detail::args_operator_t & _args )
			{
				m_serviceProvider = _serviceProvider;
				m_cb = _cb;
				m_args = _args;
			}

		protected:
			void onScheduleComplete( uint32_t _id ) override
			{
				m_cb.call_args( _id, false, m_args );
			}

			void onScheduleStop( uint32_t _id ) override
			{
				m_cb.call_args( _id, true, m_args );
			}

		protected:
			ServiceProviderInterface * m_serviceProvider;
			pybind::object m_cb;
			pybind::detail::args_operator_t m_args;
		};
		//////////////////////////////////////////////////////////////////////////
		FactoryPtr m_factoryPyObjectScheduleListener;
		//////////////////////////////////////////////////////////////////////////
		uint32_t timing( float _delay, const pybind::object & _listener, const pybind::detail::args_operator_t & _args )
		{
			const ScheduleManagerInterfacePtr & tm = PLAYER_SERVICE( m_serviceProvider )
				->getScheduleManager();

			DelaySchedulePipeInterface * pipe = m_factoryDelaySchedulePipeInterface->createObject();

			pipe->initialize( m_serviceProvider, _delay );

			PyScheduleTimerInterface * listener = m_factoryPyObjectTimingListener->createObject();

			listener->initialize( m_serviceProvider, _listener, _args );

			uint32_t id = tm->timing( pipe, listener );

			return id;
		}
		//////////////////////////////////////////////////////////////////////////
		bool timingRemove( uint32_t _id )
		{
			const ScheduleManagerInterfacePtr & tm = PLAYER_SERVICE( m_serviceProvider )
				->getScheduleManager();

			bool successful = tm->remove( _id );

			return successful;
		}
		//////////////////////////////////////////////////////////////////////////
		ScheduleManagerInterfacePtr createScheduler()
		{
			const ScheduleManagerInterfacePtr & sm = PLAYER_SERVICE( m_serviceProvider )
				->createSchedulerManager();

			return sm;
		}
		//////////////////////////////////////////////////////////////////////////
		bool destroyScheduler( const ScheduleManagerInterfacePtr & _sm )
		{
			if( _sm == nullptr )
			{
				LOGGER_ERROR( m_serviceProvider )("Menge.destroyScheduler destroy scheduler is NULL"
					);

				return false;
			}

			bool successful = PLAYER_SERVICE( m_serviceProvider )
				->destroySchedulerManager( _sm );

			return successful;
		}
		//////////////////////////////////////////////////////////////////////////
		uint32_t schedule( float _timing, const pybind::object & _script, const pybind::detail::args_operator_t & _args )
		{
			const ScheduleManagerInterfacePtr & sm = PLAYER_SERVICE( m_serviceProvider )
				->getScheduleManager();

			PyScheduleEventInterface * sl = m_factoryPyObjectScheduleListener->createObject();

			sl->initialize( m_serviceProvider, _script, _args );

			uint32_t id = sm->event( _timing, sl );

			return id;
		}
		//////////////////////////////////////////////////////////////////////////
		uint32_t ScheduleManagerInterface_schedule( ScheduleManagerInterface * _scheduleManager, float _timing, const pybind::object & _script, const pybind::detail::args_operator_t & _args )
		{
			PyScheduleEventInterface * sl = m_factoryPyObjectScheduleListener->createObject();

			sl->initialize( m_serviceProvider, _script, _args );

			uint32_t id = _scheduleManager->event( _timing, sl );

			return id;
		}
		//////////////////////////////////////////////////////////////////////////
		uint32_t ScheduleManagerInterface_timing( ScheduleManagerInterface * _scheduleManager, float _delay, const pybind::object & _listener, const pybind::detail::args_operator_t & _args )
		{
			DelaySchedulePipeInterface * pipe = m_factoryDelaySchedulePipeInterface->createObject();

			pipe->initialize( m_serviceProvider, _delay );

			PyScheduleTimerInterface * tl = m_factoryPyObjectTimingListener->createObject();

			tl->initialize( m_serviceProvider, _listener, _args );

			uint32_t id = _scheduleManager->timing( pipe, tl );

			return id;
		}
		//////////////////////////////////////////////////////////////////////////
		uint32_t ScheduleManagerInterface_pipe( ScheduleManagerInterface * _scheduleManager, const pybind::object & _pipe, const pybind::object & _listener, const pybind::detail::args_operator_t & _args )
		{
			PySchedulePipeInterface * pipe = m_factoryPySchedulePipeInterface->createObject();

			pipe->initialize( m_serviceProvider, _pipe, _args );

			PyScheduleTimerInterface * tl = m_factoryPyObjectTimingListener->createObject();

			tl->initialize( m_serviceProvider, _listener, _args );

			uint32_t id = _scheduleManager->timing( pipe, tl );

			return id;
		}
		//////////////////////////////////////////////////////////////////////////
		bool scheduleRemove( uint32_t _id )
		{
			const ScheduleManagerInterfacePtr & sm = PLAYER_SERVICE( m_serviceProvider )
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
			const ScheduleManagerInterfacePtr & sm = PLAYER_SERVICE( m_serviceProvider )
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
			const ScheduleManagerInterfacePtr & sm = PLAYER_SERVICE( m_serviceProvider )
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
			const ScheduleManagerInterfacePtr & sm = PLAYER_SERVICE( m_serviceProvider )
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
			const ScheduleManagerInterfacePtr & sm = PLAYER_SERVICE( m_serviceProvider )
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
			const ScheduleManagerInterfacePtr & sm = PLAYER_SERVICE( m_serviceProvider )
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
			const ScheduleManagerInterfacePtr & sm = PLAYER_SERVICE( m_serviceProvider )
				->getScheduleManager();

			if( sm == nullptr )
			{
				return 0.f;
			}

			float time = sm->time( _id );

			return time;
		}
		//////////////////////////////////////////////////////////////////////////
		uint32_t s_scheduleGlobal( float _timing, const pybind::object & _script, const pybind::detail::args_operator_t & _args )
		{
			const ScheduleManagerInterfacePtr & sm = PLAYER_SERVICE( m_serviceProvider )
				->getScheduleManagerGlobal();

			if( sm == nullptr )
			{
				return 0;
			}

			PyScheduleEventInterface * sl = m_factoryPyObjectScheduleListener->createObject();

			sl->initialize( m_serviceProvider, _script, _args );

			uint32_t id = sm->event( _timing, sl );

			return id;
		}
		//////////////////////////////////////////////////////////////////////////
		bool s_scheduleGlobalRemove( uint32_t _id )
		{
			const ScheduleManagerInterfacePtr & sm = PLAYER_SERVICE( m_serviceProvider )
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
			const ScheduleManagerInterfacePtr & sm = PLAYER_SERVICE( m_serviceProvider )
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
			const ScheduleManagerInterfacePtr & sm = PLAYER_SERVICE( m_serviceProvider )
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
			const ScheduleManagerInterfacePtr & sm = PLAYER_SERVICE( m_serviceProvider )
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
			const ScheduleManagerInterfacePtr & sm = PLAYER_SERVICE( m_serviceProvider )
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
			const ScheduleManagerInterfacePtr & sm = PLAYER_SERVICE( m_serviceProvider )
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
			const ScheduleManagerInterfacePtr & sm = PLAYER_SERVICE( m_serviceProvider )
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
			GAME_SERVICE( m_serviceProvider )
				->setTimingFactor( _factor );
		}
		//////////////////////////////////////////////////////////////////////////
		void s_addHomeless( Node * _node )
		{
			NODE_SERVICE( m_serviceProvider )
				->addHomeless( _node );

			_node->release();
		}
		//////////////////////////////////////////////////////////////////////////
		bool s_isHomeless( Node * _node )
		{
			bool is = NODE_SERVICE( m_serviceProvider )
				->isHomeless( _node );

			return is;
		}
		//////////////////////////////////////////////////////////////////////////
		float s_getTimingFactor()
		{
			float factor = GAME_SERVICE( m_serviceProvider )
				->getTimingFactor();

			return factor;
		}
		//////////////////////////////////////////////////////////////////////////
		class PythonSceneChangeCallback
			: public SceneChangeCallbackInterface
		{
		public:
			void initialize( ServiceProviderInterface * _serviceProvider, const pybind::object & _cb, const pybind::detail::args_operator_t & _args )
			{
				m_serviceProvider = _serviceProvider;

				m_cb = _cb;
				m_args = _args;
			}

		public:
			void onSceneChange( Scene * _scene, bool _enable, bool _remove ) override
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
			ServiceProviderInterface * m_serviceProvider;

			pybind::object m_cb;
			pybind::detail::args_operator_t m_args;
		};
		//////////////////////////////////////////////////////////////////////////
		typedef stdex::intrusive_ptr<PythonSceneChangeCallback> PythonSceneChangeCallbackPtr;
		//////////////////////////////////////////////////////////////////////////
		FactoryPtr m_factoryPythonSceneChangeCallback;
		//////////////////////////////////////////////////////////////////////////
		bool setCurrentScene( const ConstString & _prototype, const ConstString & _name, bool _destroyOld, const pybind::object & _cb, const pybind::detail::args_operator_t & _args )
		{
			LOGGER_INFO( m_serviceProvider )("set current scene '%s'"
				, _name.c_str()
				);

			//if( PLAYER_SERVICE( m_serviceProvider )
			//	->isChangedScene() == true )
			//{
			//	return false;
			//}

			PythonSceneChangeCallbackPtr py_cb = m_factoryPythonSceneChangeCallback->createObject();

			py_cb->initialize( m_serviceProvider, _cb, _args );

			Scene * currentScene = PLAYER_SERVICE( m_serviceProvider )
				->getCurrentScene();

			if( currentScene != nullptr && currentScene->getName() == _name )
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
					->generatePrototype( STRINGIZE_STRING_LOCAL( m_serviceProvider, "Scene" ), _prototype );

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
			Scene * scene = PLAYER_SERVICE( m_serviceProvider )
				->getCurrentScene();

			return scene;
		}
		//////////////////////////////////////////////////////////////////////////
		Scene * s_createScene( const ConstString & _prototype, const ConstString & _name )
		{
			Scene * scene = PROTOTYPE_SERVICE( m_serviceProvider )
				->generatePrototype( STRINGIZE_STRING_LOCAL( m_serviceProvider, "Scene" ), _prototype );

			scene->setName( _name );

			return scene;
		}
		//////////////////////////////////////////////////////////////////////////
		bool createGlobalScene()
		{
			bool successful = PLAYER_SERVICE( m_serviceProvider )
				->createGlobalScene();

			return successful;
		}
		//////////////////////////////////////////////////////////////////////////
		void removeGlobalScene()
		{
			PLAYER_SERVICE( m_serviceProvider )
				->removeGlobalScene();
		}
		//////////////////////////////////////////////////////////////////////////
		Scene * getGlobalScene()
		{
			Scene * scene = PLAYER_SERVICE( m_serviceProvider )
				->getGlobalScene();

			return scene;
		}
		//////////////////////////////////////////////////////////////////////////
		void s_setArrow( const ConstString & _prototype )
		{
			Arrow * arrow = PROTOTYPE_SERVICE( m_serviceProvider )
				->generatePrototype( STRINGIZE_STRING_LOCAL( m_serviceProvider, "Arrow" ), _prototype );

			if( arrow == nullptr )
			{
				LOGGER_ERROR( m_serviceProvider )("Error: can't setup arrow '%s'"
					, _prototype.c_str()
					);

				return;
			}

			PLAYER_SERVICE( m_serviceProvider )
				->setArrow( arrow );
		}
		//////////////////////////////////////////////////////////////////////////
		const pybind::object & s_getArrow()
		{
			Arrow * arrow = PLAYER_SERVICE( m_serviceProvider )
				->getArrow();

			if( arrow == nullptr )
			{
				return pybind::object::get_invalid();
			}

			const pybind::object & py_arrow = arrow->getScriptObject();

			return py_arrow;
		}
		//////////////////////////////////////////////////////////////////////////
		void s_hideArrow( bool _hide )
		{
			Arrow * arrow = PLAYER_SERVICE( m_serviceProvider )
				->getArrow();

			arrow->setLocalHide( _hide );
		}
		//////////////////////////////////////////////////////////////////////////
		const Resolution & s_getCurrentResolution()
		{
			return APPLICATION_SERVICE( m_serviceProvider )
				->getCurrentResolution();
		}
		//////////////////////////////////////////////////////////////////////////
		const Resolution & s_getContentResolution()
		{
			return APPLICATION_SERVICE( m_serviceProvider )
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

			Arrow * arrow = PLAYER_SERVICE( m_serviceProvider )
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
				return mt::vec2f( 0.f, 0.f );
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
			const RenderViewportInterface * rv = PLAYER_SERVICE( m_serviceProvider )
				->getRenderViewport();

			const Viewport & vp = rv->getViewport();

			return vp.begin;
		}
		//////////////////////////////////////////////////////////////////////////
		uint32_t s_Animatable_play( Animatable * _animatable )
		{
			float time = TIMELINE_SERVICE( m_serviceProvider )
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

#	ifdef _DEBUG
			if( py_kwds.empty() == false )
			{
				for( pybind::dict::iterator
					it = py_kwds.begin(),
					it_end = py_kwds.end();
					it != it_end;
					++it )
				{
					std::string k = it.key();

					LOGGER_ERROR( m_serviceProvider )("SurfaceVideo::setEventListener invalid kwds '%s'\n"
						, k.c_str()
						);
				}

				throw;
			}
#	endif

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

#	ifdef _DEBUG
			if( py_kwds.empty() == false )
			{
				for( pybind::dict::iterator
					it = py_kwds.begin(),
					it_end = py_kwds.end();
					it != it_end;
					++it )
				{
					std::string k = it.key();

					LOGGER_ERROR( m_serviceProvider )("SurfaceImageSequence::setEventListener invalid kwds '%s'\n"
						, k.c_str()
						);
				}

				throw;
			}
#	endif

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

#	ifdef _DEBUG
			if( py_kwds.empty() == false )
			{
				for( pybind::dict::iterator
					it = py_kwds.begin(),
					it_end = py_kwds.end();
					it != it_end;
					++it )
				{
					std::string k = it.key();

					LOGGER_ERROR( m_serviceProvider )("SurfaceSound::setEventListener invalid kwds '%s'\n"
						, k.c_str()
						);
				}

				throw;
			}
#	endif

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

#	ifdef _DEBUG
			if( py_kwds.empty() == false )
			{
				for( pybind::dict::iterator
					it = py_kwds.begin(),
					it_end = py_kwds.end();
					it != it_end;
					++it )
				{
					std::string k = it.key();

					LOGGER_ERROR( m_serviceProvider )("Meshget::setEventListener invalid kwds '%s'\n"
						, k.c_str()
						);
				}

				throw;
			}
#	endif

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

#	ifdef _DEBUG
			if( py_kwds.empty() == false )
			{
				for( pybind::dict::iterator
					it = py_kwds.begin(),
					it_end = py_kwds.end();
					it != it_end;
					++it )
				{
					std::string k = it.key();

					LOGGER_ERROR( m_serviceProvider )("ParticleEmitter2::setEventListener invalid kwds '%s'\n"
						, k.c_str()
						);
				}

				throw;
			}
#	endif

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

#	ifdef _DEBUG
			if( py_kwds.empty() == false )
			{
				for( pybind::dict::iterator
					it = py_kwds.begin(),
					it_end = py_kwds.end();
					it != it_end;
					++it )
				{
					std::string k = it.key();

					LOGGER_ERROR( m_serviceProvider )("ScriptHolder::setEventListener invalid kwds '%s'\n"
						, k.c_str()
						);
				}

				throw;
			}
#	endif

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

#	ifdef _DEBUG
			if( py_kwds.empty() == false )
			{
				for( pybind::dict::iterator
					it = py_kwds.begin(),
					it_end = py_kwds.end();
					it != it_end;
					++it )
				{
					std::string k = it.key();
					LOGGER_ERROR( m_serviceProvider )("HotSpot::setEventListener %s kwds %s"
						, _node->getName().c_str()
						, k.c_str()
						);
				}

				throw;
			}
#	endif

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

			Node * onMovieActivateInternal( const pybind::object & _internal ) override
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

#	ifdef _DEBUG
			if( py_kwds.empty() == false )
			{
				for( pybind::dict::iterator
					it = py_kwds.begin(),
					it_end = py_kwds.end();
					it != it_end;
					++it )
				{
					std::string k = it.key();

					LOGGER_ERROR(m_serviceProvider)( "Movie::setEventListener invalid kwds '%s'\n"
						, k.c_str()
					);
				}

				throw;
			}
#	endif

			return pybind::ret_none();
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
			if( _node == nullptr )
			{
				LOGGER_ERROR( m_serviceProvider )("Menge.destroyNode invalid take None object"
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

			NODE_SERVICE( m_serviceProvider )
				->addHomeless( node );

			return node;
		}
		//////////////////////////////////////////////////////////////////////////
		SurfacePtr createSurface( const ConstString & _type )
		{
			SurfacePtr surface = PROTOTYPE_SERVICE( m_serviceProvider )
				->generatePrototype( STRINGIZE_STRING_LOCAL( m_serviceProvider, "Surface" ), _type );

			return surface;
		}
		//////////////////////////////////////////////////////////////////////////
		ShapeQuadFixed * createSprite( const ConstString & _name, const ResourceReferencePtr & _resource )
		{
			if( _resource == nullptr )
			{
				LOGGER_ERROR( m_serviceProvider )("Menge.createSprite: '%s' resource is NULL"
					, _name.c_str()
					);

				return nullptr;
			}

			SurfaceImagePtr surface = PROTOTYPE_SERVICE( m_serviceProvider )
				->generatePrototype( STRINGIZE_STRING_LOCAL( m_serviceProvider, "Surface" ), STRINGIZE_STRING_LOCAL( m_serviceProvider, "SurfaceImage" ) );

			if( surface == nullptr )
			{
				LOGGER_ERROR( m_serviceProvider )("Menge.createSprite: '%s' resource '%s' invalid create surface 'SurfaceImage'"
					, _name.c_str()
					, _resource->getName().c_str()
					);

				return nullptr;
			}

			surface->setName( _name );
			surface->setResourceImage( _resource );

			ShapeQuadFixed * shape = NODE_SERVICE( m_serviceProvider )
				->createNodeT<ShapeQuadFixed *>( STRINGIZE_STRING_LOCAL( m_serviceProvider, "ShapeQuadFixed" ) );

			if( shape == nullptr )
			{
				LOGGER_ERROR( m_serviceProvider )("Menge.createSprite: '%s' resource '%s' invalid create shape 'ShapeQuadFixed'"
					, _name.c_str()
					, _resource->getName().c_str()
					);

				return nullptr;
			}

			shape->setName( _name );
			shape->setSurface( surface );

			return shape;
		}
		//////////////////////////////////////////////////////////////////////////
		void quitApplication()
		{
			APPLICATION_SERVICE( m_serviceProvider )
				->quit();
		}
		//////////////////////////////////////////////////////////////////////////
		ResourceReferencePtr createResource( const ConstString& _type )
		{
			ResourceReferencePtr resource = RESOURCE_SERVICE( m_serviceProvider )
				->generateResource( _type );

			if( resource == nullptr )
			{
				LOGGER_ERROR( m_serviceProvider )("createResource: invalid create resource '%s'"
					, _type.c_str()
					);

				return nullptr;
			}

			return resource;
		}
		//////////////////////////////////////////////////////////////////////////
		bool directResourceCompile( const ConstString & _nameResource )
		{
			ResourceReferencePtr resource;
			if( RESOURCE_SERVICE( m_serviceProvider )
				->hasResource( _nameResource, &resource ) == false )
			{
				LOGGER_ERROR( m_serviceProvider )("directResourceCompile: not found resource '%s'"
					, _nameResource.c_str()
					);

				return false;
			}

			if( resource->incrementReference() == false )
			{
				LOGGER_ERROR( m_serviceProvider )("directResourceCompile: resource '%s' type '%s' invalid compile"
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
			if( RESOURCE_SERVICE( m_serviceProvider )
				->hasResource( _nameResource, &resource ) == false )
			{
				LOGGER_ERROR( m_serviceProvider )("directResourceRelease: not found resource '%s'"
					, _nameResource.c_str()
					);

				return false;
			}

			if( resource->decrementReference() == false )
			{
				LOGGER_ERROR( m_serviceProvider )("directResourceCompile: resource '%s' type '%s' invalid release"
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
		ResourceReferencePtr s_getResourceReference( const ConstString & _name )
		{
			const ResourceReferencePtr & resource = RESOURCE_SERVICE( m_serviceProvider )
				->getResourceReference( _name );

			if( resource == nullptr )
			{
				LOGGER_ERROR( m_serviceProvider )("Menge.getResourceReference: not exist resource %s"
					, _name.c_str()
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
			return THREAD_SERVICE( m_serviceProvider )
				->joinTask( _task );
		}
		//////////////////////////////////////////////////////////////////////////
		void s_setFullscreenMode( bool _fullscreen )
		{
			APPLICATION_SERVICE( m_serviceProvider )
				->setFullscreenMode( _fullscreen );
		}
		//////////////////////////////////////////////////////////////////////////
		bool s_getFullscreenMode()
		{
			return APPLICATION_SERVICE( m_serviceProvider )
				->getFullscreenMode();
		}
		//////////////////////////////////////////////////////////////////////////
		void s_setFixedContentResolution( bool _fixedContentResolution )
		{
			APPLICATION_SERVICE( m_serviceProvider )
				->setFixedContentResolution( _fixedContentResolution );
		}
		//////////////////////////////////////////////////////////////////////////
		bool s_getFixedContentResolution()
		{
			return APPLICATION_SERVICE( m_serviceProvider )
				->getFixedContentResolution();
		}
		//////////////////////////////////////////////////////////////////////////
		void s_setFixedDisplayResolution( bool _fixedDisplayResolution )
		{
			APPLICATION_SERVICE( m_serviceProvider )
				->setFixedDisplayResolution( _fixedDisplayResolution );
		}
		//////////////////////////////////////////////////////////////////////////
		bool s_getFixedDisplayResolution()
		{
			return APPLICATION_SERVICE( m_serviceProvider )
				->getFixedDisplayResolution();
		}
		//////////////////////////////////////////////////////////////////////////
		void renderOneFrame()
		{
			RENDER_SERVICE( m_serviceProvider )
				->beginScene();

			GAME_SERVICE( m_serviceProvider )
				->render();

			RENDER_SERVICE( m_serviceProvider )
				->endScene();
		}
		//////////////////////////////////////////////////////////////////////////
		void writeImageToFile( const ConstString& _resource, const FilePath& _fileName )
		{
			ResourceImagePtr resource = RESOURCE_SERVICE( m_serviceProvider )
				->getResourceT<ResourceImagePtr>( _resource );

			if( resource == nullptr )
			{
				LOGGER_ERROR( m_serviceProvider )("Error: Image resource not getting '%s'"
					, _resource.c_str()
					);

				return;
			}

			const RenderTextureInterfacePtr & texture = resource->getTexture();

			RENDERTEXTURE_SERVICE( m_serviceProvider )
				->saveImage( texture, STRINGIZE_STRING_LOCAL( m_serviceProvider, "user" ), STRINGIZE_STRING_LOCAL( m_serviceProvider, "pngImage" ), _fileName );
		}
		//////////////////////////////////////////////////////////////////////////
		void setParticlesEnabled( bool _enabled )
		{
			APPLICATION_SERVICE( m_serviceProvider )
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
				->generateResourceT<ResourceImageDefaultPtr>( STRINGIZE_STRING_LOCAL( m_serviceProvider, "ResourceImageDefault" ) );

			if( resource == nullptr )
			{
				return nullptr;
			}

			resource->setName( _resourceName );

			mt::uv4f uv_image;
			mt::uv4f uv_alpha;

			resource->setup( _fileName, ConstString::none(), uv_image, uv_alpha, maxSize );

			return resource;
		}
		//////////////////////////////////////////////////////////////////////////
		ResourceImageSolidPtr s_createImageSolidResource( const ConstString & _resourceName, const ColourValue & _colour, const mt::vec2f & _maxSize )
		{
			ResourceImageSolidPtr resource = RESOURCE_SERVICE( m_serviceProvider )
				->generateResourceT<ResourceImageSolidPtr>( STRINGIZE_STRING_LOCAL( m_serviceProvider, "ResourceImageSolid" ) );

			if( resource == nullptr )
			{
				return nullptr;
			}

			resource->setName( _resourceName );

			resource->setColor( _colour );

			resource->setMaxSize( _maxSize );
			resource->setSize( _maxSize );

			return resource;
		}
		//////////////////////////////////////////////////////////////////////////
		void minimizeWindow()
		{
			APPLICATION_SERVICE( m_serviceProvider )
				->minimizeWindow();
		}
		//////////////////////////////////////////////////////////////////////////
		bool s_calcMouseScreenPosition( const mt::vec2f & _pos, mt::vec2f & _screen )
		{
			Arrow * arrow = PLAYER_SERVICE( m_serviceProvider )
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
			const mt::vec2f & cp = INPUT_SERVICE( m_serviceProvider )
				->getCursorPosition( _touchId );

			mt::vec2f pos_screen;
			this->s_calcMouseScreenPosition( _pos, pos_screen );

			mt::vec2f mp = pos_screen - cp;

			INPUT_SERVICE( m_serviceProvider )
				->pushMouseMoveEvent( _touchId, cp.x, cp.y, mp.x, mp.y, 0.f );
		}
		//////////////////////////////////////////////////////////////////////////
		void s_pushMouseButtonEvent( uint32_t _touchId, const mt::vec2f & _pos, uint32_t _button, bool _isDown )
		{
			mt::vec2f pos_screen;
			this->s_calcMouseScreenPosition( _pos, pos_screen );

			INPUT_SERVICE( m_serviceProvider )
				->pushMouseButtonEvent( _touchId, pos_screen.x, pos_screen.y, _button, 0.f, _isDown );
		}
		//////////////////////////////////////////////////////////////////////////
		void s_platformEvent( const ConstString & _event, const TMapWParams & _params )
		{
			PLATFORM_SERVICE( m_serviceProvider )
				->onEvent( _event, _params );
		}
		//////////////////////////////////////////////////////////////////////////
		const ConstString & s_getProjectCodename()
		{
			const ConstString & codename = APPLICATION_SERVICE( m_serviceProvider )
				->getProjectCodename();

			return codename;
		}
		//////////////////////////////////////////////////////////////////////////
		void s_sleep( uint32_t _time )
		{
			THREAD_SYSTEM( m_serviceProvider )
				->sleep( _time );
		}
		//////////////////////////////////////////////////////////////////////////
		class PyHttpReceiver
			: public HttpReceiver
		{
		public:
			PyHttpReceiver( const pybind::object & _cb, const pybind::detail::args_operator_t & _args )
				: m_cb( _cb )
				, m_args( _args )
			{
			}

		protected:
			void onHttpRequestComplete( HttpRequestID _id, uint32_t _status, const String & _response, uint32_t _code, bool _successful ) override
			{
                m_cb.call_args( _id, _status, _response, _code, _successful, m_args );
			}

		protected:
			pybind::object m_cb;
			pybind::detail::args_operator_t m_args;

		};		
		//////////////////////////////////////////////////////////////////////////
		HttpRequestID s_downloadAsset( const String & _url, const String & _login, const String & _password, const ConstString & _category, const FilePath & _filepath, const pybind::object & _cb, const pybind::detail::args_operator_t & _args )
		{
			uint32_t id = HTTP_SYSTEM( m_serviceProvider )
				->downloadAsset( _url, _login, _password, _category, _filepath, new PyHttpReceiver( _cb, _args ) );

			return id;
		}
		//////////////////////////////////////////////////////////////////////////
		HttpRequestID s_postMessage( const String & _url, const TMapParams & _params, const pybind::object & _cb, const pybind::detail::args_operator_t & _args )
		{
			HttpRequestID id = HTTP_SYSTEM( m_serviceProvider )
				->postMessage( _url, _params, new PyHttpReceiver( _cb, _args ) );

			return id;
		}
        //////////////////////////////////////////////////////////////////////////
        HttpRequestID s_headerData( const String & _url, const TVectorString & _headers, const String & _data, const pybind::object & _cb, const pybind::detail::args_operator_t & _args )
        {
            HttpRequestID id = HTTP_SYSTEM( m_serviceProvider )
                ->headerData( _url, _headers, _data, new PyHttpReceiver( _cb, _args ) );

            return id;
        }
		//////////////////////////////////////////////////////////////////////////
		HttpRequestID s_getMessage( const String & _url, const pybind::object & _cb, const pybind::detail::args_operator_t & _args )
		{
			HttpRequestID id = HTTP_SYSTEM( m_serviceProvider )
				->getMessage( _url, new PyHttpReceiver( _cb, _args ) );

			return id;
		}
		//////////////////////////////////////////////////////////////////////////
		void s_cancelRequest( HttpRequestID _id )
		{
			HTTP_SYSTEM( m_serviceProvider )
				->cancelRequest( _id );
		}
		//////////////////////////////////////////////////////////////////////////
		bool s_mountResourcePak( const ConstString & _fileGroup
			, const ConstString & _name
			, const ConstString & _type
			, const ConstString & _category
			, const FilePath & _path
			, const FilePath & _descriptionPath
		)
		{
			FileGroupInterfacePtr fileGroup;
			if( FILE_SERVICE( m_serviceProvider )
				->hasFileGroup( _fileGroup, &fileGroup ) == false )
			{
				LOGGER_ERROR( m_serviceProvider )("Menge.loadResourcePak invalid found file group %s"
					, _fileGroup.c_str()
					);

				return false;
			}
            
            LOGGER_WARNING(m_serviceProvider)("mountResourcePak name '%s' type '%s' category '%s' path '%s'"
                                              , _name.c_str()
                                              , _type.c_str()
                                              , _category.c_str()
                                              , _path.c_str()
                                              );

			PackageDesc desc;
			desc.dev = false;
			desc.immediately = true;
			desc.preload = false;

			desc.name = _name;
			desc.type = _type;

			desc.category = _category;
			desc.path = _path;
			desc.descriptionPath = _descriptionPath;         


			bool result = PACKAGE_SERVICE( m_serviceProvider )
				->addPackage( desc );

			return result;
		}
		//////////////////////////////////////////////////////////////////////////
		bool s_hasPackage( const ConstString & _name )
		{
			bool result = PACKAGE_SERVICE( m_serviceProvider )
				->hasPackage( _name );

			return result;
		}
		//////////////////////////////////////////////////////////////////////////
		bool s_unmountResourcePak( const ConstString & _name )
		{
			bool result = PACKAGE_SERVICE( m_serviceProvider )
				->removePackage( _name );

			return result;
		}
		//////////////////////////////////////////////////////////////////////////
		bool s_existFile( const ConstString & _fileGroup, const FilePath & _path )
		{
			bool result = FILE_SERVICE( m_serviceProvider )
				->existFile( _fileGroup, _path, nullptr );

			return result;
		}
		//////////////////////////////////////////////////////////////////////////
		bool s_removeFile( const FilePath & _path )
		{
			WString unicode_path;
			Helper::utf8ToUnicode( m_serviceProvider, _path, unicode_path );

			bool result = PLATFORM_SERVICE( m_serviceProvider )
				->removeFile( unicode_path );

			return result;
		}
		//////////////////////////////////////////////////////////////////////////
		class PythonSaxCallback
		{
		public:
			PythonSaxCallback( ServiceProviderInterface * _serviceProvider, pybind::kernel_interface * _kernel, const pybind::object & _cb )
				: m_serviceProvider( _serviceProvider )
				, m_kernel( _kernel )
				, m_cb( _cb )
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

				py_cb_begin.call( _node );
			}

			void callback_node_attributes( const char * _node, uint32_t _count, const char ** _keys, const char ** _values )
			{
				(void)_node;

				pybind::dict py_attr( m_kernel, _count );

				for( uint32_t i = 0; i != _count; ++i )
				{
					const char * key = _keys[i];
					const char * value = _values[i];

					py_attr[key] = value;
				}

				pybind::object py_cb_attr = m_cb.get_attr( "attr" );

				py_cb_attr.call( py_attr );
			}

			void callback_end_node( const char * _node )
			{
				(void)_node;

				pybind::object py_cb_end = m_cb.get_attr( "end" );

				py_cb_end.call();
			}

		protected:
			ServiceProviderInterface * m_serviceProvider;
			pybind::kernel_interface * m_kernel;
			pybind::object m_cb;
		};
		//////////////////////////////////////////////////////////////////////////
		bool s_parseXml( pybind::kernel_interface * _kernel, const ConstString & _fileGroup, const FilePath & _path, const pybind::object & _cb )
		{
			MemoryInterfacePtr binary_buffer = Helper::createMemoryCacheFileString( m_serviceProvider, _fileGroup, _path, false, __FILE__, __LINE__ );

			if( binary_buffer == nullptr )
			{
				return false;
			}

			char * memory = binary_buffer->getMemory();

			PythonSaxCallback pysc( m_serviceProvider, _kernel, _cb );
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

				m_cb.call( name );
			}

		protected:
			pybind::object m_cb;

		private:
			void operator = ( const MyVisitorTextFont & )
			{
			}
		};
		//////////////////////////////////////////////////////////////////////////
		void s_visitFonts( const pybind::object & _cb )
		{
			MyVisitorTextFont mvtf( _cb );

			TEXT_SERVICE( m_serviceProvider )
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
			void operator = ( const MyVisitorCollectTextFont & )
			{
			}
		};
		//////////////////////////////////////////////////////////////////////////
		pybind::list s_getFonts( pybind::kernel_interface * _kernel )
		{
			pybind::list l( _kernel );
			MyVisitorCollectTextFont mvtf( l );

			TEXT_SERVICE( m_serviceProvider )
				->visitFonts( &mvtf );

			return l;
		}
		//////////////////////////////////////////////////////////////////////////
		bool s_hasFont( const ConstString & _fontName )
		{
			TextFontInterfacePtr font;
			bool has = TEXT_SERVICE( m_serviceProvider )
				->existFont( _fontName, font );

			return has;
		}
		//////////////////////////////////////////////////////////////////////////
		bool s_validateFont( const ConstString & _fontName, const String & _text, const pybind::object & _cb )
		{
			TextFontInterfacePtr font;
			if( TEXT_SERVICE( m_serviceProvider )
				->existFont( _fontName, font ) == false )
			{
				return false;
			}

			WString text_ws;

			if( Helper::utf8ToUnicode( m_serviceProvider, _text, text_ws ) == false )
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
                WChar ws_str[2] = { ws_ch, L'\0' };

				Char text_utf8[8];
				size_t text_utf8_len = 0;
				if( UNICODE_SYSTEM( m_serviceProvider )
					->unicodeToUtf8( ws_str, 1, text_utf8, 8, &text_utf8_len ) == false )
				{
					_cb.call( "invalid utf8 ", 0, ws_str );

					continue;
				}

                text_utf8[text_utf8_len] = '\0';

				uint32_t code = 0;
				const char * test_text = text_utf8;
				utf8::internal::utf_error err = utf8::internal::validate_next( test_text, test_text + text_utf8_len, code );

				if( err != utf8::internal::UTF8_OK )
				{
					_cb.call( "validate utf8 ", code, ws_str );

					continue;
				}

				GlyphCode glyphChar = code;

				if( font->hasGlyph( glyphChar ) == false )
				{
					_cb.call( "not found glyph ", code, ws_str );

					continue;
				}
			}

			return true;
		}
		//////////////////////////////////////////////////////////////////////////
		class PrefetchResourceVisitor
			: public Visitor
			, public ConcreteVisitor<ResourceImageDefault>
			, public ConcreteVisitor<ResourceHIT>
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
				const FilePath & filePath = _resource->getFilePath();
				const ConstString & codecType = _resource->getCodecType();

				PREFETCHER_SERVICE( m_serviceProvider )
					->prefetchImageDecoder( m_category, filePath, codecType );
			}

			void accept( ResourceHIT * _resource ) override
			{
				_resource->incrementReference();
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

			RESOURCE_SERVICE( m_serviceProvider )
				->visitGroupResources( _category, _groupName, &rv_gac );
		}
		//////////////////////////////////////////////////////////////////////////
		class UnfetchResourceVisitor
			: public Visitor
			, public ConcreteVisitor<ResourceImageDefault>
			, public ConcreteVisitor<ResourceHIT>
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
				const FilePath & filePath = _resource->getFilePath();

				PREFETCHER_SERVICE( m_serviceProvider )
					->unfetch( category, filePath );
			}

			void accept( ResourceHIT * _resource ) override
			{
				_resource->decrementReference();
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
				const FilePath & filePath = _resource->getFilePath();

				PREFETCHER_SERVICE( m_serviceProvider )
					->unfetch( category, filePath );
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

			RESOURCE_SERVICE( m_serviceProvider )
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

			RESOURCE_SERVICE( m_serviceProvider )
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

			if( resourceFile == nullptr )
			{
				return false;
			}

			const ConstString & category = resourceFile->getCategory();

			const FilePath & filePath = resourceFile->getFilePath();

			InputStreamInterfacePtr stream = FILE_SERVICE( m_serviceProvider )
				->openInputFile( category, filePath, false );

			if( stream == nullptr )
			{
				return false;
			}

			size_t size = stream->size();

			void * memory_buffer = _blob->newMemory( size, __FILE__, __LINE__ );

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
			MemoryInterfacePtr memory = MEMORY_SERVICE( m_serviceProvider )
				->createMemory();

			if( this->s_copyFile_( _resourceFileName, memory ) == false )
			{
				return false;
			}

			const WString & projectName = APPLICATION_SERVICE( m_serviceProvider )
				->getProjectName();

			WString wc_fileName;
			if( Helper::utf8ToUnicode( m_serviceProvider, _fileName, wc_fileName ) == false )
			{
				return false;
			}

			if( PLATFORM_SERVICE( m_serviceProvider )
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

			const WString & projectName = APPLICATION_SERVICE( m_serviceProvider )
				->getProjectName();

			WString wc_fileName;
			if( Helper::utf8ToUnicode( m_serviceProvider, _fileName, wc_fileName ) == false )
			{
				return false;
			}

			if( PLATFORM_SERVICE( m_serviceProvider )
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
				_arrow = PLAYER_SERVICE( m_serviceProvider )
					->getArrow();

				if( _arrow == nullptr )
				{
					return mt::vec2f( 0.f, 0.f );
				}
			}

			if( _camera == nullptr )
			{
				_camera = PLAYER_SERVICE( m_serviceProvider )
					->getRenderCamera();
			}

			if( _viewport == nullptr )
			{
				_viewport = PLAYER_SERVICE( m_serviceProvider )
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
				_arrow = PLAYER_SERVICE( m_serviceProvider )
					->getArrow();

				if( _arrow == nullptr )
				{
					return mt::vec2f( 0.f, 0.f );
				}
			}

			if( _camera == nullptr )
			{
				_camera = PLAYER_SERVICE( m_serviceProvider )
					->getRenderCamera();
			}

			if( _viewport == nullptr )
			{
				_viewport = PLAYER_SERVICE( m_serviceProvider )
					->getRenderViewport();
			}

			mt::vec2f wp;
			_arrow->calcPointClick( _camera, _viewport, _screenPoint, wp );

			return wp;
		}
		//////////////////////////////////////////////////////////////////////////
		pybind::object s_getMovieSlotsPosition( pybind::kernel_interface * _kernel, const ConstString & _groupName, const ConstString & _movieName )
		{
			stdex::array_string<128> buffer;
			buffer.append( "Movie" );
			buffer.append( _groupName );
			buffer.append( "_" );
			buffer.append( _movieName );

			ConstString resourceMovieName = Helper::stringizeStringSize( m_serviceProvider, buffer.c_str(), buffer.size() );

			ResourceMoviePtr resourceMovie;

			if( RESOURCE_SERVICE( m_serviceProvider )
				->hasResourceT<ResourceMoviePtr>( resourceMovieName, &resourceMovie ) == false )
			{
				LOGGER_ERROR( m_serviceProvider )("s_getMovieSlotsPosition: not found resource movie %s"
					, resourceMovieName.c_str()
					);

				return pybind::make_none_t( _kernel );
			}

			pybind::list py_list( _kernel );

			const TVectorMovieLayers & layers = resourceMovie->getLayers();

			for( TVectorMovieLayers::const_iterator
				it = layers.begin(),
				it_end = layers.end();
				it != it_end;
				++it )
			{
				const MovieLayer & layer = *it;

				if( layer.type != STRINGIZE_STRING_LOCAL( m_serviceProvider, "MovieSlot" ) )
				{
					continue;
				}

				py_list.append( pybind::make_tuple_t( _kernel, layer.name, layer.position ) );
			}

			return py_list;
		}
		//////////////////////////////////////////////////////////////////////////
		PyObject * s_getMovieSlotPosition( pybind::kernel_interface * _kernel, const ConstString & _groupName, const ConstString & _movieName, const ConstString & _slotName )
		{
			stdex::array_string<128> buffer;
			buffer.append( "Movie" );
			buffer.append( _groupName );
			buffer.append( "_" );
			buffer.append( _movieName );

			ConstString resourceMovieName = Helper::stringizeStringSize( m_serviceProvider, buffer.c_str(), buffer.size() );

			ResourceMoviePtr resourceMovie;

			if( RESOURCE_SERVICE( m_serviceProvider )
				->hasResourceT<ResourceMoviePtr>( resourceMovieName, &resourceMovie ) == false )
			{
				LOGGER_ERROR( m_serviceProvider )("getMovieSlotPosition: not found resource movie %s"
					, resourceMovieName.c_str()
					);

				return pybind::ret_none();
			}

			const MovieLayer * layer;
			if( resourceMovie->hasMovieLayer( _slotName, &layer ) == false )
			{
				LOGGER_ERROR( m_serviceProvider )("getMovieSlotPosition: movie %s not found slot %s"
					, resourceMovieName.c_str()
					, _slotName.c_str()
					);

				return pybind::ret_none();
			}

			PyObject * py_position = pybind::ptr( _kernel, layer->position );

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
					m_cb.call( true );
				}
			}

			void stop() override
			{
				if( m_cb.is_callable() == true )
				{
					m_cb.call( false );
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
		FactoryPtr m_factoryAffectorGridBurnTransparency;
		//////////////////////////////////////////////////////////////////////////
		uint32_t s_gridBurnTransparency( Grid2D * _grid, const mt::vec2f & _pos, float _time, float _radius, float _ellipse, float _penumbra, const pybind::object & _cb )
		{
			AffectorGridBurnTransparency * affector = m_factoryAffectorGridBurnTransparency->createObject();

			affector->setServiceProvider( m_serviceProvider );
			affector->setAffectorType( ETA_USER );

			affector->initialize( _grid, _pos, _time, _radius, _ellipse, _penumbra, _cb );

			uint32_t id = _grid->addAffector( affector );

			return id;
		}
		//////////////////////////////////////////////////////////////////////////
		class AffectorUser
			: public Affector
		{
		public:
			bool initialize( const pybind::object & _cb, const pybind::detail::args_operator_t & _args )
			{
				m_cb = _cb;
				m_args = _args;

				return true;
			}

		protected:
			bool _affect( float _timing ) override
			{
				bool complete = m_cb.call_args( _timing, m_args );

				return complete;
			}

			void complete() override
			{
			}

			void stop() override
			{
			}

		protected:
			pybind::object m_cb;
			pybind::detail::args_operator_t m_args;
		};
		//////////////////////////////////////////////////////////////////////////
		FactoryPtr m_factoryAffectorUser;
		//////////////////////////////////////////////////////////////////////////		
		Affector * s_createAffector( const pybind::object & _cb, const pybind::detail::args_operator_t & _args )
		{
			AffectorUser * affector = m_factoryAffectorUser->createObject();

			if( affector->initialize( _cb, _args ) == false )
			{
				affector->destroy();

				return nullptr;
			}

			return affector;
		}
		//////////////////////////////////////////////////////////////////////////
		AFFECTOR_ID s_addAffector( const pybind::object & _cb, const pybind::detail::args_operator_t & _args )
		{
			AffectorUser * affector = m_factoryAffectorUser->createObject();

			if( affector->initialize( _cb, _args ) == false )
			{
				affector->destroy();

				return 0;
			}

			Affectorable * affectorable = PLAYER_SERVICE( m_serviceProvider )
				->getAffectorableGlobal();

			AFFECTOR_ID id = affectorable->addAffector( affector );

			return id;
		}
		//////////////////////////////////////////////////////////////////////////
		bool s_removeAffector( AFFECTOR_ID _id )
		{
			Affectorable * affectorable = PLAYER_SERVICE( m_serviceProvider )
				->getAffectorableGlobal();

			bool successful = affectorable->stopAffector( _id );

			return successful;
		}
		//////////////////////////////////////////////////////////////////////////
		class AffectorFollower
			: public Affector
			, public pybind::bindable
		{
		public:
			virtual void follow( const pybind::object & _target ) = 0;

		protected:
			PyObject * _embedded() override
			{
				pybind::kernel_interface * kernel = pybind::get_kernel();

				PyObject * py_obj = kernel->scope_create_holder_t( this );

				return py_obj;
			}
		};
		//////////////////////////////////////////////////////////////////////////
		template<class T_Node>
		class AffectorNodeFollower
			: public AffectorFollower
		{
		public:
			AffectorNodeFollower()
				: m_node( nullptr )
			{
			}

			~AffectorNodeFollower()
			{
			}

		public:
			void setNode( T_Node * _node )
			{
				m_node = _node;
			}

			T_Node * getNode() const
			{
				return m_node;
			}

		protected:
			void complete() override
			{
				//Empty
			}

			void stop() override
			{
				AFFECTOR_ID id = this->getId();

				m_node->stopAffector( id );
			}

		protected:
			T_Node * m_node;
		};
		//////////////////////////////////////////////////////////////////////////
		template<class T_Node, class T_Value, class T_Setter, class T_Getter>
		class AffectorNodeFollowerMethod
			: public AffectorNodeFollower<T_Node>
		{
		public:
			AffectorNodeFollowerMethod()
			{
			}

			~AffectorNodeFollowerMethod()
			{
			}

		public:
			bool initialize( T_Node * _node, T_Setter _setter, T_Getter _getter, const T_Value & _value, const T_Value & _target, float _speed )
			{
				if( _node == nullptr )
				{
					return false;
				}

				this->setNode( _node );

				m_setter = _setter;
				m_getter = _getter;

				m_follower.setValue( _value );
				m_follower.setSpeed( _speed );

				m_follower.follow( _target );

				(_node->*m_setter)(_value);

				return true;
			}

		public:
			void follow( const pybind::object & _target ) override
			{
				T_Value value_target = _target.extract();

				m_follower.follow( value_target );
			}

		protected:
			bool _affect( float _timing ) override
			{
				T_Node * node = this->getNode();

				T_Value current_value = (node->*m_getter)();

				m_follower.setValue( current_value );

				m_follower.update( _timing );

				T_Value value = m_follower.getValue();

				(node->*m_setter)(value);

				return false;
			}

		protected:
			ValueFollowerLinear<T_Value> m_follower;

			T_Setter m_setter;
			T_Getter m_getter;
		};
		//////////////////////////////////////////////////////////////////////////
		template<class T_Node, class T_Value, class T_Setter, class T_Getter>
		class AffectorNodeFollowerCreator
		{
		public:
			AffectorNodeFollowerCreator( ServiceProviderInterface * _serviceProvider )
			{
				m_factory = new FactoryPool<TAffectorNodeFollowerMethod, 4>( _serviceProvider );
			}

		protected:
			typedef AffectorNodeFollowerMethod<T_Node, T_Value, T_Setter, T_Getter> TAffectorNodeFollowerMethod;

		public:
			AffectorFollower * create( T_Node * _node, T_Setter _setter, T_Getter _getter, const T_Value & _value, const T_Value & _target, float _speed )
			{
				TAffectorNodeFollowerMethod * affector = m_factory->createObject();

				if( affector->initialize( _node, _setter, _getter, _value, _target, _speed ) == false )
				{
					affector->destroy();

					return nullptr;
				}

				if( _node->addAffector( affector ) == INVALID_AFFECTOR_ID )
				{
					affector->destroy();

					return nullptr;
				}

				return affector;
			}

		protected:
			FactoryPtr m_factory;
		};
		//////////////////////////////////////////////////////////////////////////
		AffectorNodeFollowerCreator<Node, float, void(Node::*)(float), float(Node::*)()const> m_creatorAffectorNodeFollowerLocalAlpha;
		//////////////////////////////////////////////////////////////////////////
		AffectorFollower * s_addNodeFollowerLocalAlpha( Node * _node, float _value, float _target, float _speed )
		{
			AffectorFollower * affector = m_creatorAffectorNodeFollowerLocalAlpha.create( _node, &Node::setLocalAlpha, &Node::getLocalAlpha, _value, _target, _speed );

			return affector;
		}
		//////////////////////////////////////////////////////////////////////////
		AffectorNodeFollowerCreator<ShapeQuadFlex, mt::vec2f, void(ShapeQuadFlex::*)(const mt::vec2f &), const mt::vec2f &(ShapeQuadFlex::*)()const> m_creatorAffectorNodeFollowerCustomSize;
		//////////////////////////////////////////////////////////////////////////
		AffectorFollower * s_addShapeFollowerCustomSize( ShapeQuadFlex * _node, const mt::vec2f & _value, const mt::vec2f & _target, float _speed )
		{
			AffectorFollower * affector = m_creatorAffectorNodeFollowerCustomSize.create( _node, &ShapeQuadFlex::setCustomSize, &ShapeQuadFlex::getCustomSize, _value, _target, _speed );

			return affector;
		}
		//////////////////////////////////////////////////////////////////////////
		AffectorNodeFollowerCreator<ShapeQuadFlex, mt::vec2f, void(ShapeQuadFlex::*)(const mt::vec2f &), const mt::vec2f &(ShapeQuadFlex::*)()const> m_creatorAffectorNodeFollowerTextureUVScale;
		//////////////////////////////////////////////////////////////////////////
		AffectorFollower * s_addShapeFollowerTextureUVScale( ShapeQuadFlex * _node, const mt::vec2f & _value, const mt::vec2f & _target, float _speed )
		{
			AffectorFollower * affector = m_creatorAffectorNodeFollowerTextureUVScale.create( _node, &ShapeQuadFlex::setTextureUVScale, &ShapeQuadFlex::getTextureUVScale, _value, _target, _speed );

			return affector;
		}
		//////////////////////////////////////////////////////////////////////////
		void s_removeNodeFollower( AffectorFollower * _affector )
		{
			if( _affector == nullptr )
			{
				LOGGER_ERROR( m_serviceProvider )("removeNodeFollower take NULL affector"
					);

				return;
			}

			_affector->stop();
		}
		//////////////////////////////////////////////////////////////////////////		
		void s_moduleMessage( const ConstString & _moduleName, const ConstString & _messageName, const TMapWParams & _params )
		{
			MODULE_SERVICE( m_serviceProvider )
				->message( _moduleName, _messageName, _params );
		}
		//////////////////////////////////////////////////////////////////////////
		mt::vec3f s_getCameraPosition( const RenderCameraInterface * _camera, Node * _node )
		{
			const mt::vec3f & wp = _node->getWorldPosition();

			const mt::mat4f & vm = _camera->getCameraViewMatrix();

			mt::vec3f wp_screen;
			mt::mul_m4_v3( wp_screen, vm, wp );

			return wp_screen;
		}
		//////////////////////////////////////////////////////////////////////////
		Scene * s_findNodeScene( Node * _node )
		{
			Node * node_iterator = _node;

			while( node_iterator )
			{
				Scene * node_scene = dynamic_cast<Scene *>(node_iterator);

				if( node_scene != nullptr )
				{
					return node_scene;
				}

				node_iterator = node_iterator->getParent();
			}

			return nullptr;
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
		mt::vec2f s_getMousePosition()
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
			const RenderViewportInterface * renderViewport = PLAYER_SERVICE( m_serviceProvider )
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
			bool exist = TEXT_SERVICE( m_serviceProvider )
				->existText( _key, nullptr );

			return exist;
		}
		//////////////////////////////////////////////////////////////////////////
		pybind::list s_pickHotspot( pybind::kernel_interface * _kernel, const mt::vec2f & _point )
		{
			MousePickerSystemInterface * mousePickerSystem = PLAYER_SERVICE( m_serviceProvider )
				->getMousePickerSystem();

			TVectorPickerTraps traps;
			mousePickerSystem->pickTrap( _point, traps );

			pybind::list pyret( _kernel );

			bool onFocus = APPLICATION_SERVICE( m_serviceProvider )
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
		bool s_testPlatformTags( const Tags & _tags )
		{
			const Tags & platformTags = PLATFORM_SERVICE( m_serviceProvider )
				->getPlatformTags();

			bool successful = platformTags.inTags( _tags );

			return successful;
		}
		//////////////////////////////////////////////////////////////////////////
		bool s_hasTouchpad()
		{
			bool touchpad = PLATFORM_SERVICE( m_serviceProvider )
				->hasTouchpad();

			return touchpad;
		}
		//////////////////////////////////////////////////////////////////////////
		const mt::vec2f & s_getSurfaceSize( Shape * _shape )
		{
			const SurfacePtr & surface = _shape->getSurface();

			if( surface == nullptr )
			{
				LOGGER_ERROR( m_serviceProvider )("s_getSurfaceSize shape %s not setup surface"
					, _shape->getName().c_str()
					);

				return mt::vec2f::identity();
			}

			const mt::vec2f & size = surface->getSize();

			return size;
		}
		//////////////////////////////////////////////////////////////////////////
		mt::vec2f s_getLocalImageCenter( Shape * _shape )
		{
			const SurfacePtr & surface = _shape->getSurface();

			if( surface == nullptr )
			{
				LOGGER_ERROR( m_serviceProvider )("s_getLocalImageCenter shape %s not setup surface"
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
		mt::vec2f s_getWorldImageCenter( Shape * _shape )
		{
			mt::vec2f imageCenter = s_getLocalImageCenter( _shape );

			const mt::mat4f & wm = _shape->getWorldMatrix();

			mt::vec2f imageCenter_wm;
			mt::mul_v2_v2_m4( imageCenter_wm, imageCenter, wm );

			return imageCenter_wm;
		}
		//////////////////////////////////////////////////////////////////////////
		Node * createChildren( Node * _node, const ConstString & _type )
		{
			Node * newNode = NODE_SERVICE( m_serviceProvider )
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
		pybind::list getAllChildren( pybind::kernel_interface * _kernel, Node * _node )
		{
			TListNodeChild & children = _node->getChildren();

			size_t size = children.size();

			pybind::list py_children( _kernel, size );

			size_t index = 0;

			for( TSlugChild it( children ); it.eof() == false; )
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
			_node->setLinearSpeed( mt::vec3f( 0.f, 0.f, 0.f ) );
		}
		//////////////////////////////////////////////////////////////////////////
		class ScriptableAffectorCallback
			: public AffectorCallback
		{
		public:
			ScriptableAffectorCallback()
				: m_serviceProvider( nullptr )
				, m_scriptable( nullptr )
			{
			}

			~ScriptableAffectorCallback()
			{
			}

		public:
			void initialize( ServiceProviderInterface * _serviceProvider, Scriptable * _scriptable, const pybind::object & _cb, const pybind::detail::args_operator_t & _args )
			{
				m_serviceProvider = _serviceProvider;
				m_scriptable = _scriptable;
				m_cb = _cb;
				m_args = _args;
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

				m_cb.call_args( m_scriptable, _id, _isEnd, m_args );
			}

		protected:
			ServiceProviderInterface * m_serviceProvider;
			Scriptable * m_scriptable;

			pybind::object m_cb;
			pybind::detail::args_operator_t m_args;
		};
		//////////////////////////////////////////////////////////////////////////
		typedef stdex::intrusive_ptr<ScriptableAffectorCallback> ScriptableAffectorCallbackPtr;
		//////////////////////////////////////////////////////////////////////////
		FactoryPtr m_factoryNodeAffectorCallback;
		//////////////////////////////////////////////////////////////////////////
		ScriptableAffectorCallbackPtr createNodeAffectorCallback( Scriptable * _scriptable, const pybind::object & _cb, const pybind::detail::args_operator_t & _args )
		{
			ScriptableAffectorCallbackPtr callback = m_factoryNodeAffectorCallback->createObject();

			callback->initialize( m_serviceProvider, _scriptable, _cb, _args );

			return callback;
		}
		//////////////////////////////////////////////////////////////////////////
		NodeAffectorCreator::NodeAffectorCreatorAccumulateLinear<Node, void (Node::*)(const mt::vec3f &), mt::vec3f> m_nodeAffectorCreatorAccumulateLinear;
		//////////////////////////////////////////////////////////////////////////
		uint32_t velocityTo( Node * _node, float _speed, const mt::vec3f& _dir, const pybind::object & _cb, const pybind::detail::args_operator_t & _args )
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

			Affector * affector = m_nodeAffectorCreatorAccumulateLinear.create( ETA_POSITION
				, callback, _node, &Node::setLocalPosition
				, _node->getLocalPosition(), _dir, _speed
			);

			if( affector == nullptr )
			{
				return 0;
			}

			moveStop( _node );

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
			: public CallbackAffector
		{
		public:
			AffectorVelocity2()
				: m_node( nullptr )
				, m_velocity( 0.f, 0.f, 0.f )
				, m_time( 0.f )
			{
			}

		public:
			void setNode( Node * _node )
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
			bool _affect( float _timing ) override
			{
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
			Node * m_node;

			mt::vec3f m_velocity;
			float m_time;
		};
		//////////////////////////////////////////////////////////////////////////
		class FactoryAffectorVelocity2
		{
		public:
			FactoryAffectorVelocity2( ServiceProviderInterface * _serviceProvider )
				: m_serviceProvider( _serviceProvider )
			{
				m_factory = new FactoryPool<AffectorVelocity2, 4>( m_serviceProvider );
			}

		public:
			Affector * create( EAffectorType _type, const AffectorCallbackPtr & _cb
				, Node * _node, const mt::vec3f & _velocity, float _time )
			{
				AffectorVelocity2 * affector = m_factory->createObject();

				affector->setServiceProvider( m_serviceProvider );
				affector->setAffectorType( _type );

				affector->setCallback( _cb );

				affector->setNode( _node );

				affector->initialize( _velocity, _time );

				return affector;
			}

		protected:
			ServiceProviderInterface * m_serviceProvider;

			FactoryPtr m_factory;
		};
		//////////////////////////////////////////////////////////////////////////
		FactoryAffectorVelocity2 m_factoryAffectorVelocity2;
		//////////////////////////////////////////////////////////////////////////
		uint32_t velocityTo2( Node * _node, const mt::vec3f & _velocity, float _time, const pybind::object & _cb, const pybind::detail::args_operator_t & _args )
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

			Affector * affector = m_factoryAffectorVelocity2.create( ETA_POSITION
				, callback, _node, _velocity, _time
			);

			if( affector == nullptr )
			{
				return 0;
			}

			moveStop( _node );

			if( _node->isActivate() == false )
			{
				return 0;
			}

			_node->setLinearSpeed( _velocity );

			AFFECTOR_ID id = _node->addAffector( affector );

			return id;
		}
		//////////////////////////////////////////////////////////////////////////
		NodeAffectorCreator::NodeAffectorCreatorInterpolateLinear<Node, void (Node::*)(const mt::vec3f &), mt::vec3f> m_nodeAffectorCreatorInterpolateLinear;
		//////////////////////////////////////////////////////////////////////////
		uint32_t moveTo( Node * _node, float _time, const mt::vec3f& _point, const pybind::object & _cb, const pybind::detail::args_operator_t & _args )
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

			Affector* affector =
				m_nodeAffectorCreatorInterpolateLinear.create( ETA_POSITION
					, callback, _node, &Node::setLocalPosition
					, _node->getLocalPosition(), _point, _time
				);

			if( affector == nullptr )
			{
				return 0;
			}

			moveStop( _node );

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
		NodeAffectorCreator::NodeAffectorCreatorInterpolateQuadratic<Node, void (Node::*)(const mt::vec3f &), mt::vec3f> m_nodeAffectorCreatorInterpolateQuadratic;
		//////////////////////////////////////////////////////////////////////////
		uint32_t accMoveTo( Node * _node, float _time, const mt::vec3f& _point, const pybind::object & _cb, const pybind::detail::args_operator_t & _args )
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

			Affector* affector = m_nodeAffectorCreatorInterpolateQuadratic.create( ETA_POSITION
				, callback, _node, &Node::setLocalPosition
				, _node->getLocalPosition(), _point, linearSpeed, _time
			);

			if( affector == nullptr )
			{
				return 0;
			}

			moveStop( _node );

			if( _node->isActivate() == false )
			{
				return 0;
			}

			AFFECTOR_ID id = _node->addAffector( affector );

			return id;
		}
		//////////////////////////////////////////////////////////////////////////
		NodeAffectorCreator::NodeAffectorCreatorInterpolateBezier<Node, void (Node::*)(const mt::vec3f &), mt::vec3f, 1> m_nodeAffectorCreatorInterpolateQuadraticBezier;
		//////////////////////////////////////////////////////////////////////////
		uint32_t bezier2To( Node * _node
			, float _time
			, const mt::vec3f& _to
			, const mt::vec3f& _v0
			, const pybind::object & _cb
			, const pybind::detail::args_operator_t & _args )
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

			Affector* affector = m_nodeAffectorCreatorInterpolateQuadraticBezier.create( ETA_POSITION
				, callback, _node, &Node::setLocalPosition
				, _node->getLocalPosition(), _to, v, _time
			);

			if( affector == nullptr )
			{
				return 0;
			}

			moveStop( _node );

			if( _node->isActivate() == false )
			{
				return 0;
			}

			AFFECTOR_ID id = _node->addAffector( affector );

			return id;
		}
		//////////////////////////////////////////////////////////////////////////
		NodeAffectorCreator::NodeAffectorCreatorInterpolateBezier<Node, void (Node::*)(const mt::vec3f &), mt::vec3f, 2> m_nodeAffectorCreatorInterpolateCubicBezier;
		//////////////////////////////////////////////////////////////////////////
		uint32_t bezier3To( Node * _node
			, float _time
			, const mt::vec3f& _to
			, const mt::vec3f& _v0
			, const mt::vec3f& _v1
			, const pybind::object & _cb
			, const pybind::detail::args_operator_t & _args )
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

			Affector* affector =
				m_nodeAffectorCreatorInterpolateCubicBezier.create( ETA_POSITION
					, callback, _node, &Node::setLocalPosition
					, _node->getLocalPosition(), _to, v, _time
				);

			if( affector == nullptr )
			{
				return 0;
			}

			moveStop( _node );

			if( _node->isActivate() == false )
			{
				return 0;
			}

			AFFECTOR_ID id = _node->addAffector( affector );

			return id;
		}
		//////////////////////////////////////////////////////////////////////////
		NodeAffectorCreator::NodeAffectorCreatorInterpolateBezier<Node, void (Node::*)(const mt::vec3f &), mt::vec3f, 3> m_nodeAffectorCreatorInterpolateQuarticBezier;
		//////////////////////////////////////////////////////////////////////////
		uint32_t bezier4To( Node * _node
			, float _time
			, const mt::vec3f& _to
			, const mt::vec3f& _v0
			, const mt::vec3f& _v1
			, const mt::vec3f& _v2
			, const pybind::object & _cb
			, const pybind::detail::args_operator_t & _args )
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

			Affector* affector =
				m_nodeAffectorCreatorInterpolateQuarticBezier.create( ETA_POSITION
					, callback, _node, &Node::setLocalPosition
					, _node->getLocalPosition(), _to, v, _time
				);

			if( affector == nullptr )
			{
				return 0;
			}

			moveStop( _node );

			if( _node->isActivate() == false )
			{
				return 0;
			}

			AFFECTOR_ID id = _node->addAffector( affector );

			return id;
		}
		class AffectorCreatorInterpolateParabolic
			: public CallbackAffector
		{
		public:
			AffectorCreatorInterpolateParabolic()
				: m_node( nullptr )
				, m_speed( 5.f )
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
					mt::dir_v3_v3( m_targetDir, prev_position, _position );
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
		//////////////////////////////////////////////////////////////////////////
		class FactoryAffectorInterpolateParabolic
		{
		public:
			FactoryAffectorInterpolateParabolic( ServiceProviderInterface * _serviceProvider )
				: m_serviceProvider( _serviceProvider )
			{
				m_factory = new FactoryPool<AffectorCreatorInterpolateParabolic, 4>( m_serviceProvider );
			}

		public:
			Affector * create( EAffectorType _type, const AffectorCallbackPtr & _cb
				, Node * _node, const mt::vec3f & _end, const mt::vec3f & _v0, float _time )
			{
				AffectorCreatorInterpolateParabolic * affector = m_factory->createObject();

				affector->setServiceProvider( m_serviceProvider );
				affector->setAffectorType( _type );

				affector->setCallback( _cb );

				affector->setNode( _node );

				affector->initialize( _end, _v0, _time );

				return affector;
			}

		protected:
			ServiceProviderInterface * m_serviceProvider;

			FactoryPtr m_factory;
		};
		//////////////////////////////////////////////////////////////////////////
		FactoryAffectorInterpolateParabolic m_nodeAffectorCreatorInterpolateParabolic;
		//////////////////////////////////////////////////////////////////////////
		uint32_t parabolaTo( Node * _node
			, float _time
			, const mt::vec3f& _end
			, const mt::vec3f& _v0
			, const pybind::object & _cb
			, const pybind::detail::args_operator_t & _args )
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

			Affector* affector =
				m_nodeAffectorCreatorInterpolateParabolic.create( ETA_POSITION
					, callback, _node, _end, _v0, _time
				);

			if( affector == nullptr )
			{
				return 0;
			}

			moveStop( _node );

			if( _node->isActivate() == false )
			{
				return 0;
			}

			AFFECTOR_ID id = _node->addAffector( affector );

			return id;
		}
		class AffectorCreatorFollowTo
			: public CallbackAffector
		{
		public:
			AffectorCreatorFollowTo()
				: m_node( nullptr )
				, m_target( nullptr )
				, m_offset( 0.f, 0.f, 0.f )
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
			bool initialize( Node * _node, Node * _target, const mt::vec3f & _offset, float _distance, float _moveSpeed, float _moveAcceleration, float _moveLimit, bool _rotate, float _rotationSpeed, float _rotationAcceleration, float _rotationLimit )
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
			bool _affect( float _timing ) override
			{
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
			Node * m_node;
			Node * m_target;

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
		class FactoryAffectorFollowTo
		{
		public:
			FactoryAffectorFollowTo( ServiceProviderInterface * _serviceProvider )
				: m_serviceProvider( _serviceProvider )
			{
				m_factory = new FactoryPool<AffectorCreatorFollowTo, 4>( m_serviceProvider );
			}

		public:
			Affector * create( EAffectorType _type, const AffectorCallbackPtr & _cb
				, Node * _node, Node * _target, const mt::vec3f & _offset, float _distance, float _moveSpeed, float _moveAcceleration, float _moveLimit
				, bool _rotate
				, float _rotationSpeed, float _rotationAcceleration, float _rotationLimit )
			{
				AffectorCreatorFollowTo * affector = m_factory->createObject();

				affector->setServiceProvider( m_serviceProvider );
				affector->setAffectorType( _type );

				affector->setCallback( _cb );

				if( affector->initialize( _node, _target, _offset, _distance, _moveSpeed, _moveAcceleration, _moveLimit, _rotate, _rotationSpeed, _rotationAcceleration, _rotationLimit ) == false )
				{
					affector->destroy();

					return nullptr;
				}

				return affector;
			}

		protected:
			ServiceProviderInterface * m_serviceProvider;

			FactoryPtr m_factory;
		};
		//////////////////////////////////////////////////////////////////////////
		FactoryAffectorFollowTo m_nodeAffectorCreatorFollowTo;
		//////////////////////////////////////////////////////////////////////////		
		uint32_t followTo( Node * _node
			, Node * _target
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
			, const pybind::detail::args_operator_t & _args )
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

			Affector * affector =
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

			moveStop( _node );

			if( _node->isActivate() == false )
			{
				return 0;
			}

			AFFECTOR_ID id = _node->addAffector( affector );

			return id;
		}
		//////////////////////////////////////////////////////////////////////////
		class AffectorCreatorFollowToW
			: public CallbackAffector
		{
		public:
			AffectorCreatorFollowToW()
				: m_node( nullptr )
				, m_target( nullptr )
				, m_offset( 0.f, 0.f, 0.f )
				, m_distance( 0.f )
				, m_moveSpeed( 0.f )
				, m_moveAcceleration( 0.f )
				, m_moveLimit( 0.f )
			{
			}

		public:
			bool initialize( Node * _node, Node * _target, const mt::vec3f & _offset, float _distance, float _moveSpeed, float _moveAcceleration, float _moveLimit )
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
			bool _affect( float _timing ) override
			{
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
			Node * m_node;
			Node * m_target;

			mt::vec3f m_offset;
			float m_distance;
			float m_moveSpeed;
			float m_moveAcceleration;
			float m_moveLimit;
		};
		//////////////////////////////////////////////////////////////////////////
		class FactoryAffectorFollowToW
		{
		public:
			FactoryAffectorFollowToW( ServiceProviderInterface * _serviceProvider )
				: m_serviceProvider( _serviceProvider )
			{
				m_factory = new FactoryPool<AffectorCreatorFollowToW, 4>( m_serviceProvider );
			}

		public:
			Affector * create( EAffectorType _type, const AffectorCallbackPtr & _cb
				, Node * _node, Node * _target, const mt::vec3f & _offset, float _distance, float _moveSpeed, float _moveAcceleration, float _moveLimit )
			{
				AffectorCreatorFollowToW * affector = m_factory->createObject();

				affector->setServiceProvider( m_serviceProvider );
				affector->setAffectorType( _type );

				affector->setCallback( _cb );

				if( affector->initialize( _node, _target, _offset, _distance, _moveSpeed, _moveAcceleration, _moveLimit ) == false )
				{
					affector->destroy();

					return nullptr;
				}

				return affector;
			}

		protected:
			ServiceProviderInterface * m_serviceProvider;

			FactoryPtr m_factory;
		};
		//////////////////////////////////////////////////////////////////////////
		FactoryAffectorFollowToW m_nodeAffectorCreatorFollowToW;
		//////////////////////////////////////////////////////////////////////////		
		uint32_t followToW( Node * _node
			, Node * _target
			, const mt::vec3f & _offset
			, float _distance
			, float _moveSpeed
			, float _moveAcceleration
			, float _moveLimit
			, const pybind::object & _cb
			, const pybind::detail::args_operator_t & _args )
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

			Affector * affector =
				m_nodeAffectorCreatorFollowToW.create( ETA_POSITION
					, callback, _node, _target, _offset, _distance
					, _moveSpeed, _moveAcceleration, _moveLimit
				);

			if( affector == nullptr )
			{
				return 0;
			}

			moveStop( _node );

			if( _node->isActivate() == false )
			{
				return 0;
			}

			AFFECTOR_ID id = _node->addAffector( affector );

			return id;
		}
		//////////////////////////////////////////////////////////////////////////
		void angleStop( Node * _node )
		{
			_node->stopAffectors( ETA_ANGLE );
			_node->setAngularSpeed( 0.f );
		}
		//////////////////////////////////////////////////////////////////////////
		NodeAffectorCreator::NodeAffectorCreatorInterpolateLinear<Node, void (Node::*)(float), float> m_nodeAffectorCreatorInterpolateLinearFloat;
		//////////////////////////////////////////////////////////////////////////
		uint32_t angleTo( Node * _node, float _time, float _angle, const pybind::object & _cb, const pybind::detail::args_operator_t & _args )
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

			Affector* affector = m_nodeAffectorCreatorInterpolateLinearFloat.create( ETA_ANGLE
				, callback, _node, &Node::setOrientationX
				, correct_angle_from, correct_angle_to, _time
			);

			if( affector == nullptr )
			{
				return 0;
			}

			angleStop( _node );

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
		NodeAffectorCreator::NodeAffectorCreatorInterpolateQuadratic<Node, void (Node::*)(float), float> m_nodeAffectorCreatorInterpolateQuadraticFloat;
		//////////////////////////////////////////////////////////////////////////
		uint32_t accAngleTo( Node * _node, float _time, float _angle, const pybind::object & _cb, const pybind::detail::args_operator_t & _args )
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

			Affector* affector =
				m_nodeAffectorCreatorInterpolateQuadraticFloat.create( ETA_ANGLE
					, callback, _node, &Node::setOrientationX
					, correct_angle_from, correct_angle_to, angularSpeed, _time
				);

			if( affector == nullptr )
			{
				return 0;
			}

			angleStop( _node );

			if( _node->isActivate() == false )
			{
				return 0;
			}

			AFFECTOR_ID id = _node->addAffector( affector );

			return id;
		}
		//////////////////////////////////////////////////////////////////////////
		void scaleStop( Node * _node )
		{
			_node->stopAffectors( ETA_SCALE );
		}
		//////////////////////////////////////////////////////////////////////////
		uint32_t scaleTo( Node * _node, float _time, const mt::vec3f& _scale, const pybind::object & _cb, const pybind::detail::args_operator_t & _args )
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

			ScriptableAffectorCallbackPtr callback = createNodeAffectorCallback( _node, _cb, _args );

			Affector* affector = m_nodeAffectorCreatorInterpolateLinear.create( ETA_SCALE
				, callback, _node, &Node::setScale
				, _node->getScale(), _scale, _time
			);

			if( affector == nullptr )
			{
				return 0;
			}

			scaleStop( _node );

			if( _node->isActivate() == false )
			{
				return 0;
			}

			AFFECTOR_ID id = _node->addAffector( affector );

			return id;
		}
		//////////////////////////////////////////////////////////////////////////
		void colorStop( Node * _node )
		{
			_node->stopAffectors( ETA_COLOR );
		}
		//////////////////////////////////////////////////////////////////////////
		NodeAffectorCreator::NodeAffectorCreatorInterpolateLinear<Node, void (Node::*)(const ColourValue &), ColourValue> m_nodeAffectorCreatorInterpolateLinearColour;
		//////////////////////////////////////////////////////////////////////////		
		inline static float s_length_color( const ColourValue & _rColor )
		{
			(void)_rColor;

			return 1.0f;
		}
		//////////////////////////////////////////////////////////////////////////
		uint32_t colorTo( Node * _node, float _time, const ColourValue& _color, const pybind::object & _cb, const pybind::detail::args_operator_t & _args )
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

			Affector* affector =
				m_nodeAffectorCreatorInterpolateLinearColour.create( ETA_COLOR
					, callback, _node, &Colorable::setLocalColor
					, _node->getLocalColor(), _color, _time
				);

			if( affector == nullptr )
			{
				return 0;
			}

			colorStop( _node );

			if( _node->isActivate() == false )
			{
				return 0;
			}

			AFFECTOR_ID id = _node->addAffector( affector );

			return id;
		}
		//////////////////////////////////////////////////////////////////////////
		uint32_t alphaTo( Node * _node, float _time, float _alpha, const pybind::object & _cb, const pybind::detail::args_operator_t & _args )
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

			uint32_t id = colorTo( _node, _time, color, _cb, _args );

			return id;
		}
		//////////////////////////////////////////////////////////////////////////
		NodeAffectorCreator::NodeAffectorCreatorInterpolateLinear<ShapeQuadFlex, void (ShapeQuadFlex::*)(const mt::vec4f &), mt::vec4f> m_nodeAffectorCreatorInterpolateLinearVec4;
		//////////////////////////////////////////////////////////////////////////
		uint32_t setPercentVisibilityTo( ShapeQuadFlex * _shape, float _time, const mt::vec4f& _percent, const pybind::object & _cb, const pybind::detail::args_operator_t & _args )
		{
			if( _shape->isActivate() == false )
			{
				return 0;
			}

			ScriptableAffectorCallbackPtr callback = createNodeAffectorCallback( _shape, _cb, _args );

			Affector* affector = m_nodeAffectorCreatorInterpolateLinearVec4.create( ETA_VISIBILITY
				, callback, _shape, &ShapeQuadFlex::setPercentVisibility
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
		void setPercentVisibilityStop( ShapeQuadFlex * _shape )
		{
			_shape->stopAffectors( ETA_VISIBILITY );
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
			const RenderCameraInterface * renderCamera = PLAYER_SERVICE( m_serviceProvider )
				->getRenderCamera();

			return renderCamera;
		}
		//////////////////////////////////////////////////////////////////////////
		void showKeyboard()
		{
			APPLICATION_SERVICE( m_serviceProvider )
				->showKeyboard();
		}
		//////////////////////////////////////////////////////////////////////////
		void hideKeyboard()
		{
			APPLICATION_SERVICE( m_serviceProvider )
				->hideKeyboard();
		}
		//////////////////////////////////////////////////////////////////////////
		bool hasResource( const ConstString & _name )
		{
			return RESOURCE_SERVICE( m_serviceProvider )
				->hasResource( _name, nullptr );
		}
		//////////////////////////////////////////////////////////////////////////
		void removeCurrentScene( const pybind::object & _cb, const pybind::detail::args_operator_t & _args )
		{
			PythonSceneChangeCallbackPtr py_cb = m_factoryPythonSceneChangeCallback->createObject();

			py_cb->initialize( m_serviceProvider, _cb, _args );

			PLAYER_SERVICE( m_serviceProvider )
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
			void initialize( ServiceProviderInterface * _serviceProvider, const pybind::object & _cb, const pybind::detail::args_operator_t & _args )
			{
				m_serviceProvider = _serviceProvider;
				m_cb = _cb;
				m_args = _args;
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

			bool handleTextEvent( const InputTextEvent & _event ) override
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

			bool handleMouseMove( const InputMouseMoveEvent & _event ) override
			{
				(void)_event;

				return false;
			}

			bool handleMouseWheel( const InputMouseWheelEvent & _event ) override
			{
				(void)_event;

				return false;
			}

		protected:
			ServiceProviderInterface * m_serviceProvider;
			pybind::object m_cb;
			pybind::detail::args_operator_t m_args;
		};
		//////////////////////////////////////////////////////////////////////////
		class PyGlobalMouseMoveHandler
			: public PyGlobalBaseHandler
		{
		protected:
			bool handleMouseMove( const InputMouseMoveEvent & _event ) override
			{
				mt::vec2f point( _event.x, _event.y );
				mt::vec2f delta( _event.dx, _event.dy );

				mt::vec2f wp;
				PLAYER_SERVICE( m_serviceProvider )
					->calcGlobalMouseWorldPosition( point, wp );

				mt::vec2f wd;
				PLAYER_SERVICE( m_serviceProvider )
					->calcGlobalMouseWorldDelta( point, delta, wd );

				pybind::object py_result = m_cb.call_args( _event.touchId, wp.x, wp.y, wd.x, wd.y, m_args );

				if( py_result.is_none() == false )
				{
					LOGGER_ERROR( m_serviceProvider )("PyGlobalMouseMoveHandler %s return value %s not None"
						, m_cb.repr()
						, py_result.repr()
						);
				}

				return false;
			}
		};
		//////////////////////////////////////////////////////////////////////////
		FactoryPtr m_factoryPyGlobalMouseMoveHandlers;
		//////////////////////////////////////////////////////////////////////////
		uint32_t s_addMouseMoveHandler( const pybind::object & _cb, const pybind::detail::args_operator_t & _args )
		{
			GlobalHandleSystemInterface * globalHandleSystem = PLAYER_SERVICE( m_serviceProvider )
				->getGlobalHandleSystem();

			PyGlobalMouseMoveHandler * handler = m_factoryPyGlobalMouseMoveHandlers->createObject();

			handler->initialize( m_serviceProvider, _cb, _args );

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
				mt::vec2f point( _event.x, _event.y );

				mt::vec2f wp;
				PLAYER_SERVICE( m_serviceProvider )
					->calcGlobalMouseWorldPosition( point, wp );

				pybind::object py_result = m_cb.call_args( _event.touchId, wp.x, wp.y, _event.button, _event.pressure, _event.isDown, _event.isPressed, m_args );

				if( py_result.is_none() == false )
				{
					LOGGER_ERROR( m_serviceProvider )("PyGlobalMouseHandlerButton %s return value %s not None"
						, m_cb.repr()
						, py_result.repr()
						);
				}

				return false;
			}
		};
		//////////////////////////////////////////////////////////////////////////
		FactoryPtr m_factoryPyGlobalMouseHandlerButtons;
		//////////////////////////////////////////////////////////////////////////
		uint32_t s_addMouseButtonHandler( const pybind::object & _cb, const pybind::detail::args_operator_t & _args )
		{
			GlobalHandleSystemInterface * globalHandleSystem = PLAYER_SERVICE( m_serviceProvider )
				->getGlobalHandleSystem();

			PyGlobalMouseHandlerButton * handler = m_factoryPyGlobalMouseHandlerButtons->createObject();

			handler->initialize( m_serviceProvider, _cb, _args );

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
				mt::vec2f point( _event.x, _event.y );

				mt::vec2f wp;
				PLAYER_SERVICE( m_serviceProvider )
					->calcGlobalMouseWorldPosition( point, wp );

				pybind::object py_result = m_cb.call_args( _event.touchId, wp.x, wp.y, _event.button, _event.pressure, _event.isDown, _event.isPressed, m_args );

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
		FactoryPtr m_factoryPyGlobalMouseHandlerButtonEnds;
		//////////////////////////////////////////////////////////////////////////
		uint32_t s_addMouseButtonHandlerEnd( const pybind::object & _cb, const pybind::detail::args_operator_t & _args )
		{
			GlobalHandleSystemInterface * globalHandleSystem = PLAYER_SERVICE( m_serviceProvider )
				->getGlobalHandleSystem();

			PyGlobalMouseHandlerButtonEnd * handler = m_factoryPyGlobalMouseHandlerButtonEnds->createObject();

			handler->initialize( m_serviceProvider, _cb, _args );

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
				pybind::object py_result = m_cb.call_args( _event.button, _event.x, _event.y, _event.wheel, m_args );

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
		FactoryPtr m_factoryPyGlobalMouseHandlerWheels;
		//////////////////////////////////////////////////////////////////////////
		uint32_t s_addMouseWheelHandler( const pybind::object & _cb, const pybind::detail::args_operator_t & _args )
		{
			GlobalHandleSystemInterface * globalHandleSystem = PLAYER_SERVICE( m_serviceProvider )
				->getGlobalHandleSystem();

			PyGlobalMouseHandlerWheel * handler = m_factoryPyGlobalMouseHandlerWheels->createObject();

			handler->initialize( m_serviceProvider, _cb, _args );

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
				mt::vec2f point( _event.x, _event.y );

				mt::vec2f wp;
				PLAYER_SERVICE( m_serviceProvider )
					->calcGlobalMouseWorldPosition( point, wp );

				pybind::object py_result = m_cb.call_args( _event.touchId, wp.x, wp.y, _event.button, _event.pressure, _event.isDown, _event.isPressed, m_args );

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
		FactoryPtr m_factoryPyGlobalMouseHandlerButtonBegins;
		//////////////////////////////////////////////////////////////////////////
		uint32_t s_addMouseButtonHandlerBegin( const pybind::object & _cb, const pybind::detail::args_operator_t & _args )
		{
			GlobalHandleSystemInterface * globalHandleSystem = PLAYER_SERVICE( m_serviceProvider )
				->getGlobalHandleSystem();

			PyGlobalMouseHandlerButtonBegin * handler = m_factoryPyGlobalMouseHandlerButtonBegins->createObject();

			handler->initialize( m_serviceProvider, _cb, _args );

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
				pybind::object py_result = m_cb.call_args( (uint32_t)_event.code, _event.x, _event.y, _event.isDown, _event.isRepeat, m_args );

				if( py_result.is_none() == false )
				{
					LOGGER_ERROR( m_serviceProvider )("PyGlobalKeyHandler %s return value %s not None"
						, m_cb.repr()
						, py_result.repr()
						);
				}

				return false;
			}
		};
		//////////////////////////////////////////////////////////////////////////
		FactoryPtr m_factoryPyGlobalKeyHandler;
		//////////////////////////////////////////////////////////////////////////
		uint32_t s_addKeyHandler( const pybind::object & _cb, const pybind::detail::args_operator_t & _args )
		{
			GlobalHandleSystemInterface * globalHandleSystem = PLAYER_SERVICE( m_serviceProvider )
				->getGlobalHandleSystem();

			PyGlobalKeyHandler * handler = m_factoryPyGlobalKeyHandler->createObject();

			handler->initialize( m_serviceProvider, _cb, _args );

			uint32_t id = globalHandleSystem->addGlobalHandler( handler );

			return id;
		}
		//////////////////////////////////////////////////////////////////////////
		class PyGlobalTextHandler
			: public PyGlobalBaseHandler
		{
		protected:
			bool handleTextEvent( const InputTextEvent & _event ) override
			{
				pybind::object py_result = m_cb.call_args( _event.key, _event.x, _event.y, m_args );

				if( py_result.is_none() == false )
				{
					LOGGER_ERROR( m_serviceProvider )("PyGlobalTextHandler %s return value %s not None"
						, m_cb.repr()
						, py_result.repr()
						);
				}

				return false;
			}
		};
		//////////////////////////////////////////////////////////////////////////
		FactoryPtr m_factoryPyGlobalTextHandler;
		//////////////////////////////////////////////////////////////////////////
		uint32_t s_addTextHandler( const pybind::object & _cb, const pybind::detail::args_operator_t & _args )
		{
			GlobalHandleSystemInterface * globalHandleSystem = PLAYER_SERVICE( m_serviceProvider )
				->getGlobalHandleSystem();

			PyGlobalKeyHandler * handler = m_factoryPyGlobalTextHandler->createObject();

			handler->initialize( m_serviceProvider, _cb, _args );

			uint32_t id = globalHandleSystem->addGlobalHandler( handler );

			return id;
		}
		//////////////////////////////////////////////////////////////////////////
		bool s_removeGlobalHandler( uint32_t _id )
		{
			GlobalHandleSystemInterface * globalHandleSystem = PLAYER_SERVICE( m_serviceProvider )
				->getGlobalHandleSystem();

			InputSystemHandler * handler = globalHandleSystem->removeGlobalHandler( _id );

			PyGlobalBaseHandler * py_handler = dynamic_cast<PyGlobalBaseHandler *>(handler);

			if( py_handler == nullptr )
			{
				LOGGER_ERROR( m_serviceProvider )("s_removeKeyHandler %d handler invalid PyGlobalKeyHandler"
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
				LOGGER_ERROR( m_serviceProvider )("Mengine.getMovieDuration invalid movie resource '%s'"
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

			APPLICATION_SERVICE( m_serviceProvider )
				->getGameViewport( aspect, viewport );

			return viewport;
		}
		//////////////////////////////////////////////////////////////////////////
		bool s_hasGameParam( const ConstString & _paramName )
		{
			if( GAME_SERVICE( m_serviceProvider )
				->hasParam( _paramName ) == false )
			{
				return false;
			}

			return true;
		}
		//////////////////////////////////////////////////////////////////////////
		PyObject * s_getGameParam( pybind::kernel_interface * _kernel, const ConstString & _paramName )
		{
			if( GAME_SERVICE( m_serviceProvider )
				->hasParam( _paramName ) == false )
			{
				return pybind::ret_none();
			}

			const WString & val = GAME_SERVICE( m_serviceProvider )
				->getParam( _paramName );

			return pybind::ptr( _kernel, val );
		}
		//////////////////////////////////////////////////////////////////////////
		PyObject * s_getGameParamFloat( pybind::kernel_interface * _kernel, const ConstString & _paramName )
		{
			if( GAME_SERVICE( m_serviceProvider )
				->hasParam( _paramName ) == false )
			{
				return pybind::ret_none();
			}

			const WString & val = GAME_SERVICE( m_serviceProvider )
				->getParam( _paramName );

			float value;
			Helper::wstringToFloat( val, value );

			return pybind::ptr( _kernel, value );
		}
		//////////////////////////////////////////////////////////////////////////
		PyObject * s_getGameParamInt( pybind::kernel_interface * _kernel, const ConstString & _paramName )
		{
			if( GAME_SERVICE( m_serviceProvider )
				->hasParam( _paramName ) == false )
			{
				return pybind::ret_none();
			}

			const WString & val = GAME_SERVICE( m_serviceProvider )
				->getParam( _paramName );

			int value;
			Helper::wstringToInt( val, value );

			return pybind::ptr( _kernel, value );
		}
		//////////////////////////////////////////////////////////////////////////
		PyObject * s_getGameParamBool( pybind::kernel_interface * _kernel, const ConstString & _paramName )
		{
			(void)_kernel;

			if( GAME_SERVICE( m_serviceProvider )
				->hasParam( _paramName ) == false )
			{
				return pybind::ret_none();
			}

			const WString & val = GAME_SERVICE( m_serviceProvider )
				->getParam( _paramName );

			int value;
			Helper::wstringToInt( val, value );

			bool b_value = value != 0;

			return pybind::ret_bool( b_value );
		}
		//////////////////////////////////////////////////////////////////////////
		bool s_openUrlInDefaultBrowser( const WString & _url )
		{
			bool val = PLATFORM_SERVICE( m_serviceProvider )
				->openUrlInDefaultBrowser( _url );

			return val;
		}
		//////////////////////////////////////////////////////////////////////////
		ConstString s_getDefaultResourceFontName()
		{
			const ConstString & defaultResourceFontName = TEXT_SERVICE( m_serviceProvider )
				->getDefaultFontName();

			return defaultResourceFontName;
		}
		//////////////////////////////////////////////////////////////////////////
		class ResourceMovieVisitorNullLayers
			: public VisitorResourceMovie
		{
		public:
			ResourceMovieVisitorNullLayers( ServiceProviderInterface * _serviceProvider, pybind::kernel_interface * _kernel, const pybind::dict & _dictResult, float _frameDuration )
				: m_serviceProvider( _serviceProvider )
				, m_kernel( _kernel )
				, m_dictResult( _dictResult )
				, m_frameDuration( _frameDuration )
			{
			}

		protected:
			void visitLayer( const MovieFramePackInterfacePtr & _framePack, const MovieLayer & _layer ) override
			{
				if( _layer.source != STRINGIZE_STRING_LOCAL( m_serviceProvider, "MovieSlot" ) )
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
			ServiceProviderInterface * m_serviceProvider;
			pybind::kernel_interface * m_kernel;
			pybind::dict m_dictResult;
			float m_frameDuration;
		};
		//////////////////////////////////////////////////////////////////////////
		PyObject * s_getNullObjectsFromResourceVideo( pybind::kernel_interface * _kernel, ResourceMovie * _resource )
		{
			if( _resource == nullptr )
			{
				return pybind::ret_none();
			}

			pybind::dict py_dict_result( _kernel );

			float frameTime = _resource->getFrameDuration();
			ResourceMovieVisitorNullLayers visitor( m_serviceProvider, _kernel, py_dict_result, frameTime );

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

				if( layer.type == STRINGIZE_STRING_LOCAL( m_serviceProvider, "Movie" ) )
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
			bool result = s_hasMovieElement( _resourceName, _slotName, STRINGIZE_STRING_LOCAL( m_serviceProvider, "MovieSlot" ) );

			return result;
		}
		//////////////////////////////////////////////////////////////////////////
		bool s_hasMovieSubMovie( const ConstString & _resourceName, const ConstString & _subMovieName )
		{
			bool result = s_hasMovieElement( _resourceName, _subMovieName, STRINGIZE_STRING_LOCAL( m_serviceProvider, "SubMovie" ) );

			return result;
		}
		//////////////////////////////////////////////////////////////////////////
		bool s_hasMovieSocket( const ConstString & _resourceName, const ConstString & _socketName )
		{
			if( s_hasMovieElement( _resourceName, _socketName, STRINGIZE_STRING_LOCAL( m_serviceProvider, "MovieSocketImage" ) ) == true )
			{
				return true;
			}

			if( s_hasMovieElement( _resourceName, _socketName, STRINGIZE_STRING_LOCAL( m_serviceProvider, "MovieSocketShape" ) ) == true )
			{
				return true;
			}

			return false;
		}
		//////////////////////////////////////////////////////////////////////////
		bool s_hasMovieEvent( const ConstString & _resourceName, const ConstString & _eventName )
		{
			bool result = s_hasMovieElement( _resourceName, _eventName, STRINGIZE_STRING_LOCAL( m_serviceProvider, "MovieEvent" ) );

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

				m_cb.call( _resource );
			}

		protected:
			pybind::object m_cb;
		};
		//////////////////////////////////////////////////////////////////////////
		void s_visitCompiledResources( const ConstString & _category, const ConstString & _groupName, const pybind::object & _cb )
		{
			ResourceVisitorGetAlreadyCompiled rv_gac( _cb );

			RESOURCE_SERVICE( m_serviceProvider )
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
				m_cb.call( _resource );
			}

		protected:
			pybind::object m_cb;
		};
		//////////////////////////////////////////////////////////////////////////
		void s_visitResources( const ConstString & _category, const ConstString & _groupName, const pybind::object & _cb )
		{
			MyResourceVisitor rv_gac( _cb );

			RESOURCE_SERVICE( m_serviceProvider )
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

			RESOURCE_SERVICE( m_serviceProvider )
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

			RESOURCE_SERVICE( m_serviceProvider )
				->visitGroupResources( _category, _groupName, &rv_gac );
		}
		//////////////////////////////////////////////////////////////////////////
		class GetResourceVisitor
			: public Visitor
			, public ConcreteVisitor<ResourceReference>
		{
		public:
			GetResourceVisitor( pybind::kernel_interface * _kernel )
				: m_kernel( _kernel )
				, m_l( _kernel )
			{
				m_scope = m_kernel->class_scope<ResourceReference>();
			}

			const pybind::list & getResult() const
			{
				return m_l;
			}

		protected:
			void accept( ResourceReference* _resource ) override
			{
				PyObject * py_obj = m_scope->create_holder( (void *)_resource );

				m_l.append( py_obj );
			}

		protected:
			pybind::kernel_interface * m_kernel;
			pybind::class_type_scope_interface_ptr m_scope;
			pybind::list m_l;
		};
		//////////////////////////////////////////////////////////////////////////		
		pybind::list s_getResources( pybind::kernel_interface * _kernel, const ConstString & _category, const ConstString & _groupName )
		{
			GetResourceVisitor rv_gac( _kernel );

			RESOURCE_SERVICE( m_serviceProvider )
				->visitGroupResources( _category, _groupName, &rv_gac );

			const pybind::list & l = rv_gac.getResult();

			return l;
		}
		//////////////////////////////////////////////////////////////////////////
		bool s_validResource( const ConstString & _resourceName )
		{
			bool valid = RESOURCE_SERVICE( m_serviceProvider )
				->validResource( _resourceName );

			return valid;
		}

	protected:
		ServiceProviderInterface * m_serviceProvider;
	};
	//////////////////////////////////////////////////////////////////////////
	static void classWrapping( ServiceProviderInterface * _serviceProvider )
	{
# define SCRIPT_CLASS_WRAPPING( serviceProvider, Class )\
    SCRIPT_SERVICE(serviceProvider)->setWrapper( Helper::stringizeString(serviceProvider, #Class), new ScriptWrapper<Class>() )

		SCRIPT_CLASS_WRAPPING( _serviceProvider, Node );
		SCRIPT_CLASS_WRAPPING( _serviceProvider, Layer );
		SCRIPT_CLASS_WRAPPING( _serviceProvider, Layer2D );
		//SCRIPT_CLASS_WRAPPING( _serviceProvider, Layer2DParallax );
		//SCRIPT_CLASS_WRAPPING( _serviceProvider, Layer2DIsometric );
		//SCRIPT_CLASS_WRAPPING( _serviceProvider, Layer2DPhysic );
		SCRIPT_CLASS_WRAPPING( _serviceProvider, HotSpot );
		SCRIPT_CLASS_WRAPPING( _serviceProvider, HotSpotPolygon );
		SCRIPT_CLASS_WRAPPING( _serviceProvider, HotSpotCircle );
		SCRIPT_CLASS_WRAPPING( _serviceProvider, HotSpotBubbles );
		SCRIPT_CLASS_WRAPPING( _serviceProvider, HotSpotImage );
		SCRIPT_CLASS_WRAPPING( _serviceProvider, HotSpotShape );

		SCRIPT_CLASS_WRAPPING( _serviceProvider, ScriptHolder );

		//SCRIPT_CLASS_WRAPPING( Light2D );
		//SCRIPT_CLASS_WRAPPING( ShadowCaster2D );
		SCRIPT_CLASS_WRAPPING( _serviceProvider, Gyroscope );
		SCRIPT_CLASS_WRAPPING( _serviceProvider, Isometric );
		SCRIPT_CLASS_WRAPPING( _serviceProvider, Arrow );
		SCRIPT_CLASS_WRAPPING( _serviceProvider, TextField );
		SCRIPT_CLASS_WRAPPING( _serviceProvider, SoundEmitter );
		SCRIPT_CLASS_WRAPPING( _serviceProvider, ParticleEmitter2 );
		SCRIPT_CLASS_WRAPPING( _serviceProvider, Movie );
		SCRIPT_CLASS_WRAPPING( _serviceProvider, MovieSlot );
		SCRIPT_CLASS_WRAPPING( _serviceProvider, MovieInternalObject );
		SCRIPT_CLASS_WRAPPING( _serviceProvider, Meshget );
		SCRIPT_CLASS_WRAPPING( _serviceProvider, Model3D );
		SCRIPT_CLASS_WRAPPING( _serviceProvider, Point );
		SCRIPT_CLASS_WRAPPING( _serviceProvider, Line );
		SCRIPT_CLASS_WRAPPING( _serviceProvider, Landscape2D );
		SCRIPT_CLASS_WRAPPING( _serviceProvider, Grid2D );

		SCRIPT_CLASS_WRAPPING( _serviceProvider, ShapeQuadFixed );
		SCRIPT_CLASS_WRAPPING( _serviceProvider, ShapeQuadFlex );

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
		SCRIPT_CLASS_WRAPPING( _serviceProvider, RenderCameraOrthogonal );
		SCRIPT_CLASS_WRAPPING( _serviceProvider, RenderCameraProjection );
		SCRIPT_CLASS_WRAPPING( _serviceProvider, RenderCameraOrthogonalTarget );
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

		SCRIPT_CLASS_WRAPPING( _serviceProvider, Surface );
		SCRIPT_CLASS_WRAPPING( _serviceProvider, SurfaceVideo );
		SCRIPT_CLASS_WRAPPING( _serviceProvider, SurfaceSound );
		SCRIPT_CLASS_WRAPPING( _serviceProvider, SurfaceImage );
		SCRIPT_CLASS_WRAPPING( _serviceProvider, SurfaceImageSequence );
		SCRIPT_CLASS_WRAPPING( _serviceProvider, SurfaceSolidColor );

# undef SCRIPT_CLASS_WRAPPING
	}

	struct extract_TBlobject_type
		: public pybind::type_cast_result<Blobject>
	{
		bool apply( pybind::kernel_interface * _kernel, PyObject * _obj, Blobject & _value, bool _nothrow ) override
		{
			(void)_kernel;
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

		PyObject * wrap( pybind::kernel_interface * _kernel, pybind::type_cast_result<Blobject>::TCastRef _value ) override
		{
			(void)_kernel;

			PyObject * py_value = pybind::string_from_char_size( reinterpret_cast<const char *>(&_value[0]), _value.size() );

			return py_value;
		}
	};

	struct extract_Tags_type
		: public pybind::type_cast_result<Tags>
	{
		bool apply( pybind::kernel_interface * _kernel, PyObject * _obj, Tags & _tags, bool _nothrow ) override
		{
			(void)_kernel;
			(void)_nothrow;

			if( pybind::list_check( _obj ) == true )
			{
				pybind::list l( _kernel, _obj, pybind::borrowed() );

				size_t tags_size = l.size();

				for( size_t i = 0; i != tags_size; ++i )
				{
					ConstString tag = l[i];

					_tags.addTag( tag );
				}
			}
			else
			{
				return false;
			}

			return true;
		}

		PyObject * wrap( pybind::kernel_interface * _kernel, pybind::type_cast_result<Tags>::TCastRef _value ) override
		{
			(void)_kernel;

			const TVectorConstString & tags = _value.getTags();

			PyObject * py_tags = pybind::list_new( 0 );

			for( TVectorConstString::const_iterator
				it = tags.begin(),
				it_end = tags.end();
				it != it_end;
				++it )
			{
				const ConstString & tag = *it;

				PyObject * py_tag = pybind::ptr( _kernel, tag );

				pybind::list_appenditem( py_tags, py_tag );

				pybind::decref( py_tag );
			}

			return py_tags;
		}
	};

	void PythonScriptWrapper::nodeWrap( ServiceProviderInterface * _serviceProvider )
	{
		NodeScriptMethod * nodeScriptMethod = new NodeScriptMethod( _serviceProvider );

		pybind::kernel_interface * kernel = pybind::get_kernel();

		pybind::registration_type_cast<Blobject>(kernel, new extract_TBlobject_type);
		pybind::registration_type_cast<Tags>(kernel, new extract_Tags_type);

		pybind::registration_stl_vector_type_cast<ResourceImage *, TVectorResourceImage>(kernel);
		pybind::registration_stl_vector_type_cast<HotSpotPolygon *, TVectorHotSpotPolygon>(kernel);

		pybind::registration_stl_map_type_cast<ConstString, WString, TMapWParams>(kernel);
		pybind::registration_stl_map_type_cast<ConstString, String, TMapParams>(kernel);

		classWrapping( _serviceProvider );

		pybind::interface_<Affector>( kernel, "Affector", true )
			.def( "stop", &Affector::stop )
			.def( "getId", &Affector::getId )
			.def( "setFreeze", &Affector::setFreeze )
			.def( "getFreeze", &Affector::getFreeze )
			.def( "setSpeedFactor", &Affector::setSpeedFactor )
			.def( "getSpeedFactor", &Affector::getSpeedFactor )
			;

		pybind::interface_<Scriptable>( kernel, "Scriptable" )
			.def_bindable()
			;

		pybind::interface_<Identity>( kernel, "Identity" )
			.def( "setName", &Identity::setName )
			.def( "getName", &Identity::getName )
			.def( "getType", &Identity::getType )
			;

		pybind::interface_<BoundingBox>( kernel, "BoundingBox" )
			.def( "getBoundingBox", &BoundingBox::getBoundingBox )
			;

		pybind::interface_<Transformation3D>( kernel, "Transformation3D" )
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
			.def_proxy_static( "setAngleDeg", nodeScriptMethod, &NodeScriptMethod::Transformation3D_setAngleDeg )

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
			.def_proxy_static( "removeRelationTransformation", nodeScriptMethod, &NodeScriptMethod::Transformation3D_removeRelationTransformation )
			;

		pybind::interface_<Resource>( kernel, "Resource" )
			.def( "compile", &Resource::compile )
			.def( "release", &Resource::release )
			.def( "isCompile", &Resource::isCompile )
			;

		pybind::interface_<Reference>( kernel, "Reference" )
			.def( "incrementReference", &Reference::incrementReference )
			.def( "decrementReference", &Reference::decrementReference )
			.def( "countReference", &Reference::countReference )
			;

		pybind::interface_<ResourceReference, pybind::bases<Scriptable, Resource, Identity, Reference> >( kernel, "ResourceReference", false )
			.def_smart_pointer()
			.def( "getCategory", &ResourceReference::getCategory )
			.def( "getGroup", &ResourceReference::getGroup )
			.def( "cache", &ResourceReference::cache )
			.def( "uncache", &ResourceReference::uncache )
			;

		pybind::interface_<ResourceImage, pybind::bases<ResourceReference> >( kernel, "ResourceImage", false )
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
			.def( "setColor", &ResourceImage::setColor )
			.def( "getColor", &ResourceImage::getColor )
			;

		pybind::interface_<ResourceImageData, pybind::bases<ResourceReference> >( kernel, "ResourceImageData", false )
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

		pybind::interface_<ResourceMovie, pybind::bases<ResourceReference> >( kernel, "ResourceMovie", false )
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
			.def_proxy_static( "hasLayer", nodeScriptMethod, &NodeScriptMethod::ResourceMovie_hasLayer )
			.def_proxy_static( "hasLayerType", nodeScriptMethod, &NodeScriptMethod::ResourceMovie_hasLayerType )
			.def_proxy_static( "getLayerPosition", nodeScriptMethod, &NodeScriptMethod::ResourceMovie_getLayerPosition )
			.def_proxy_static( "getLayerIn", nodeScriptMethod, &NodeScriptMethod::ResourceMovie_getLayerIn )
			;

		pybind::interface_<ResourceAnimation, pybind::bases<ResourceReference> >( kernel, "ResourceAnimation", false )
			;

		pybind::interface_<ResourceModel3D, pybind::bases<ResourceReference> >( kernel, "ResourceModel3D", false )
			;

		pybind::interface_<ResourceVideo, pybind::bases<ResourceReference> >( kernel, "ResourceVideo", false )
			;

		pybind::interface_<ResourceSound, pybind::bases<ResourceReference> >( kernel, "ResourceSound", false )
			;

		pybind::interface_<ResourceInternalObject, pybind::bases<ResourceReference> >( kernel, "ResourceInternalObject", false )
			;

		pybind::interface_<ResourceShape, pybind::bases<ResourceReference> >( kernel, "ResourceShape", false )
			.def( "getPolygon", &ResourceShape::getPolygon )
			;

		pybind::interface_<ResourceWindow, pybind::bases<ResourceReference> >( kernel, "ResourceWindow", false )
			;

		pybind::interface_<ResourceHIT, pybind::bases<ResourceReference> >( kernel, "ResourceHIT", false )
			.def( "getWidth", &ResourceHIT::getWidth )
			.def( "getHeight", &ResourceHIT::getHeight )
			;

		pybind::interface_<Renderable>( kernel, "Renderable" )
			.def( "hide", &Renderable::setHide )
			.def( "isHide", &Renderable::getHide )
			.def( "localHide", &Renderable::setLocalHide )
			.def( "isLocalHide", &Renderable::getLocalHide )
			;

		pybind::interface_<Colorable>( kernel, "Colorable" )
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

		pybind::interface_<Animatable>( kernel, "Animatable" )
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

		pybind::interface_<Eventable>( kernel, "Eventable" )
			//.def_native( "setEventListener", &Eventable::setEventListener )
			//.def( "removeEventListener", &Eventable::removeEventListener )
			;

		pybind::interface_<Soundable>( kernel, "Soundable" )
			.def( "setVolume", &Soundable::setVolume )
			.def( "getVolume", &Soundable::getVolume )
			;

		pybind::interface_<Affectorable>( kernel, "Affectorable" )
			.def( "addAffector", &Affectorable::addAffector )
			.def( "stopAffector", &Affectorable::stopAffector )
			.def( "stopAllAffectors", &Affectorable::stopAllAffectors )
			.def( "setLinearSpeed", &Affectorable::setLinearSpeed )
			.def( "getLinearSpeed", &Affectorable::getLinearSpeed )
			;

		pybind::interface_<Materialable>( kernel, "Materialable", false )
			.def( "setMaterialName", &Materialable::setMaterialName )
			.def( "getMaterialName", &Materialable::getMaterialName )
			.def( "setDisableTextureColor", &Materialable::setDisableTextureColor )
			.def( "getDisableTextureColor", &Materialable::getDisableTextureColor )
			.def( "setBlendMode", &Materialable::setBlendMode )
			.def( "getBlendMode", &Materialable::getBlendMode )
			;

		pybind::interface_<Node, pybind::bases<Scriptable, Identity, Transformation3D, BoundingBox, Colorable, Resource, Renderable, Affectorable> >( kernel, "Node", false )
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
			.def( "emptyChild", &Node::emptyChildren )
			.def( "hasChildren", &Node::hasChild )
			.def( "update", &Node::update )
			.def( "getParent", &Node::getParent )
			.def( "hasParent", &Node::hasParent )

			.def( "getWorldPosition", &Node::getWorldPosition )
			.def( "setWorldPosition", &Node::setWorldPosition )
			//.def( "getWorldDirection", &Node::getWorldDirection )
			.def_proxy_static( "getWorldOffsetPosition", nodeScriptMethod, &NodeScriptMethod::s_getWorldOffsetPosition )
			.def_proxy_static( "getLengthTo", nodeScriptMethod, &NodeScriptMethod::s_getLengthTo )

			.def( "getWorldColor", &Node::getWorldColor )

			.def( "setRenderViewport", &Node::setRenderViewport )
			.def( "getRenderViewport", &Node::getRenderViewport )
			.def( "getRenderViewportInheritance", &Node::getRenderViewportInheritance )

			.def( "setRenderCamera", &Node::setRenderCamera )
			.def( "getRenderCamera", &Node::getRenderCamera )
			.def( "getRenderCameraInheritance", &Node::getRenderCameraInheritance )

            .def( "setRenderClipplane", &Node::setRenderClipplane )
            .def( "getRenderClipplane", &Node::getRenderClipplane )
            .def( "getRenderClipplaneInheritance", &Node::getRenderClipplaneInheritance )
            
            .def( "setRenderTarget", &Node::setRenderTarget )
            .def( "getRenderTarget", &Node::getRenderTarget )
            .def( "getRenderTargetInheritance", &Node::getRenderTargetInheritance )            

			.def_proxy_static( "createChildren", nodeScriptMethod, &NodeScriptMethod::createChildren )
			.def_proxy_static_kernel( "getAllChildren", nodeScriptMethod, &NodeScriptMethod::getAllChildren )

			.def_proxy_args_static( "colorTo", nodeScriptMethod, &NodeScriptMethod::colorTo )
			.def_proxy_args_static( "alphaTo", nodeScriptMethod, &NodeScriptMethod::alphaTo )
			.def_proxy_static( "colorStop", nodeScriptMethod, &NodeScriptMethod::colorStop )

			.def_proxy_args_static( "velocityTo", nodeScriptMethod, &NodeScriptMethod::velocityTo )
			.def_proxy_args_static( "velocityTo2", nodeScriptMethod, &NodeScriptMethod::velocityTo2 )

			.def_proxy_args_static( "moveTo", nodeScriptMethod, &NodeScriptMethod::moveTo )
			.def_proxy_args_static( "bezier2To", nodeScriptMethod, &NodeScriptMethod::bezier2To )
			.def_proxy_args_static( "bezier3To", nodeScriptMethod, &NodeScriptMethod::bezier3To )
			.def_proxy_args_static( "bezier4To", nodeScriptMethod, &NodeScriptMethod::bezier4To )
			.def_proxy_args_static( "parabolaTo", nodeScriptMethod, &NodeScriptMethod::parabolaTo )
			.def_proxy_args_static( "followTo", nodeScriptMethod, &NodeScriptMethod::followTo )
			.def_proxy_args_static( "followToW", nodeScriptMethod, &NodeScriptMethod::followToW )
			.def_proxy_static( "moveStop", nodeScriptMethod, &NodeScriptMethod::moveStop )

			.def_proxy_args_static( "angleTo", nodeScriptMethod, &NodeScriptMethod::angleTo )
			.def_proxy_static( "angleStop", nodeScriptMethod, &NodeScriptMethod::angleStop )
			.def_proxy_args_static( "scaleTo", nodeScriptMethod, &NodeScriptMethod::scaleTo )
			.def_proxy_static( "scaleStop", nodeScriptMethod, &NodeScriptMethod::scaleStop )

			.def_proxy_args_static( "accMoveTo", nodeScriptMethod, &NodeScriptMethod::accMoveTo )
			.def_proxy_args_static( "accAngleTo", nodeScriptMethod, &NodeScriptMethod::accAngleTo )
			;

		pybind::interface_<Surface, pybind::bases<Scriptable, Identity, Materialable, Resource> >( kernel, "Surface", false )
			.def_smart_pointer()
			.def( "getMaxSize", &Surface::getMaxSize )
			.def( "getSize", &Surface::getSize )
			.def( "getOffset", &Surface::getOffset )

			.def( "getUVCount", &Surface::getUVCount )
			.def( "getUV", &Surface::getUV )

			.def( "getColour", &Surface::getColour )
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


		pybind::interface_<ThreadTask>( kernel, "Task" )
			;

		pybind::interface_<RenderViewportInterface>( kernel, "RenderViewportInterface" )
			.def( "getViewport", &RenderViewportInterface::getViewport )
			;

		pybind::interface_<RenderViewport, pybind::bases<Node, RenderViewportInterface> >( kernel, "RenderViewport", false )
			.def( "setFixedViewport", &RenderViewport::setFixedViewport )
			.def( "getFixedViewport", &RenderViewport::getFixedViewport )
			.def( "setViewport", &RenderViewport::setViewport )
			;

		pybind::interface_<RenderCameraInterface>( kernel, "RenderCameraInterface" )
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


        pybind::interface_<RenderClipplaneInterface>( kernel, "RenderClipplaneInterface", false )
            .def( "getCount", &RenderClipplaneInterface::getCount )
            .def( "getPlane", &RenderClipplaneInterface::getPlane )
            ;

		pybind::interface_<RenderClipplane, pybind::bases<Node, RenderClipplaneInterface> >( kernel, "RenderClipplane", false )
			;

        pybind::interface_<RenderTargetInterface>( kernel, "RenderTargetInterface", false )
            .def( "getWidth", &RenderTargetInterface::getWidth )
            .def( "getHeight", &RenderTargetInterface::getHeight )
            ;
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
					.def_depricated( "setTextByKey", &TextField::setTextID, "use setTextID" )
					.def( "setTextID", &TextField::setTextID )
					.def( "removeTextID", &TextField::removeTextID )
					.def_proxy_native_kernel( "setTextFormatArgs", nodeScriptMethod, &NodeScriptMethod::textfield_setTextFormatArgs )
					.def_proxy_static( "getTextFormatArgs", nodeScriptMethod, &NodeScriptMethod::textfield_getTextFormatArgs )
					.def( "removeTextFormatArgs", &TextField::removeTextFormatArgs )
					.def_depricated( "getTextKey", &TextField::getTextID, "use getTextID" )
					.def( "getTextID", &TextField::getTextID )
					//.def_proxy_static( "getText", nodeScriptMethod, &NodeScriptMethod::textfield_getText )
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

				//pybind::interface_<Layer2DParallax, pybind::bases<Layer> >( "Layer2DParallax", false )
				//	.def( "setParallaxFactor", &Layer2DParallax::setParallaxFactor )
				//	.def( "getParallaxFactor", &Layer2DParallax::getParallaxFactor )
				//	.def( "setParallaxLoop", &Layer2DParallax::setParallaxLoop )
				//	.def( "getParallaxLoop", &Layer2DParallax::getParallaxLoop )
				//	;

				pybind::interface_<Parallax, pybind::bases<Node> >( kernel, "Parallax", false )
					.def( "setParallaxFactor", &Parallax::setParallaxFactor )
					.def( "getParallaxFactor", &Parallax::getParallaxFactor )
					;


				//pybind::interface_<Layer2DIsometric, pybind::bases<Layer> >( "Layer2DIsometric", false )
				//	;

				//pybind::interface_<Layer2DPhysic, pybind::bases<Layer2D> >("Layer2DPhysic", false)
				//    ;

				//pybind::proxy_<Layer2DTexture, pybind::bases<Layer2D> >("Layer2DTexture", false)
				//	.def( "setCameraOffset", &Layer2DTexture::setCameraOffset )
				//	.def( "getCameraOffset", &Layer2DTexture::getCameraOffset )
				//	.def( "setRenderTargetName", &Layer2DTexture::setRenderTargetName )
				//	.def( "getRenderTargetName", &Layer2DTexture::getRenderTargetName )
				//	;

				pybind::interface_<MousePickerTrapInterface>( kernel, "MousePickerTrap", false )
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
					.def_proxy_static( "getLocalPolygonCenter", nodeScriptMethod, &NodeScriptMethod::s_getLocalPolygonCenter )
					.def_proxy_static( "getWorldPolygonCenter", nodeScriptMethod, &NodeScriptMethod::s_getWorldPolygonCenter )
					.def_proxy_static( "getScreenPolygonCenter", nodeScriptMethod, &NodeScriptMethod::s_getScreenPolygonCenter )
					.def_proxy_static( "getWorldPolygon", nodeScriptMethod, &NodeScriptMethod::s_getWorldPolygon )
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

					.def_proxy_static( "getSurfaceSize", nodeScriptMethod, &NodeScriptMethod::s_getSurfaceSize )
					.def_proxy_static( "getLocalImageCenter", nodeScriptMethod, &NodeScriptMethod::s_getLocalImageCenter )
					.def_proxy_static( "getWorldImageCenter", nodeScriptMethod, &NodeScriptMethod::s_getWorldImageCenter )
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
					.def_proxy_static( "setPercentVisibilityTo", nodeScriptMethod, &NodeScriptMethod::setPercentVisibilityTo )
					.def_proxy_static( "setPercentVisibilityStop", nodeScriptMethod, &NodeScriptMethod::setPercentVisibilityStop )
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
					.def_proxy_static( "getMovieClipplane", nodeScriptMethod, &NodeScriptMethod::movie_getMovieClipplane )
					.def_proxy_static( "hasMovieClipplane", nodeScriptMethod, &NodeScriptMethod::movie_hasMovieClipplane )
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

				pybind::interface_<Meshget, pybind::bases<Node, Eventable, Materialable> >( kernel, "Meshget", false )
					.def( "setResourceImage", &Meshget::setResourceImage )
					.def( "getResourceImage", &Meshget::getResourceImage )
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

			pybind::def_functor_args( kernel, "setCurrentScene", nodeScriptMethod, &NodeScriptMethod::setCurrentScene );
			pybind::def_functor( kernel, "getCurrentScene", nodeScriptMethod, &NodeScriptMethod::getCurrentScene );

			pybind::def_functor( kernel, "createGlobalScene", nodeScriptMethod, &NodeScriptMethod::createGlobalScene );
			pybind::def_functor( kernel, "removeGlobalScene", nodeScriptMethod, &NodeScriptMethod::removeGlobalScene );
			pybind::def_functor( kernel, "getGlobalScene", nodeScriptMethod, &NodeScriptMethod::getGlobalScene );

			pybind::def_functor( kernel, "createScene", nodeScriptMethod, &NodeScriptMethod::s_createScene );

			pybind::def_functor( kernel, "getCamera2DPosition", nodeScriptMethod, &NodeScriptMethod::s_getCamera2DPosition );

			pybind::def_functor( kernel, "createNode", nodeScriptMethod, &NodeScriptMethod::createNode );
			pybind::def_functor( kernel, "destroyNode", nodeScriptMethod, &NodeScriptMethod::destroyNode );

			pybind::def_functor( kernel, "createSurface", nodeScriptMethod, &NodeScriptMethod::createSurface );
			pybind::def_functor( kernel, "createSprite", nodeScriptMethod, &NodeScriptMethod::createSprite );

			pybind::def_functor_args( kernel, "timing", nodeScriptMethod, &NodeScriptMethod::timing );
			pybind::def_functor( kernel, "timingRemove", nodeScriptMethod, &NodeScriptMethod::timingRemove );

			pybind::interface_<ScheduleManagerInterface>( kernel, "ScheduleManagerInterface", true )
				.def_smart_pointer()
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

			pybind::def_functor( kernel, "createScheduler", nodeScriptMethod, &NodeScriptMethod::createScheduler );
			pybind::def_functor( kernel, "destroyScheduler", nodeScriptMethod, &NodeScriptMethod::destroyScheduler );

			pybind::def_functor_args( kernel, "schedule", nodeScriptMethod, &NodeScriptMethod::schedule );
			pybind::def_functor( kernel, "scheduleRemove", nodeScriptMethod, &NodeScriptMethod::scheduleRemove );
			pybind::def_functor( kernel, "scheduleRemoveAll", nodeScriptMethod, &NodeScriptMethod::scheduleRemoveAll );
			pybind::def_functor( kernel, "scheduleFreeze", nodeScriptMethod, &NodeScriptMethod::s_scheduleFreeze );
			pybind::def_functor( kernel, "scheduleFreezeAll", nodeScriptMethod, &NodeScriptMethod::s_scheduleFreezeAll );
			pybind::def_functor( kernel, "scheduleResumeAll", nodeScriptMethod, &NodeScriptMethod::scheduleResumeAll );
			pybind::def_functor( kernel, "scheduleIsFreeze", nodeScriptMethod, &NodeScriptMethod::s_scheduleIsFreeze );
			pybind::def_functor( kernel, "scheduleTime", nodeScriptMethod, &NodeScriptMethod::s_scheduleTime );

			pybind::def_functor_args( kernel, "scheduleGlobal", nodeScriptMethod, &NodeScriptMethod::s_scheduleGlobal );
			pybind::def_functor( kernel, "scheduleGlobalRemove", nodeScriptMethod, &NodeScriptMethod::s_scheduleGlobalRemove );
			pybind::def_functor( kernel, "scheduleGlobalRemoveAll", nodeScriptMethod, &NodeScriptMethod::s_scheduleGlobalRemoveAll );
			pybind::def_functor( kernel, "scheduleGlobalFreeze", nodeScriptMethod, &NodeScriptMethod::s_scheduleGlobalFreeze );
			pybind::def_functor( kernel, "scheduleGlobalFreezeAll", nodeScriptMethod, &NodeScriptMethod::s_scheduleGlobalFreezeAll );
			pybind::def_functor( kernel, "scheduleGlobalResumeAll", nodeScriptMethod, &NodeScriptMethod::s_scheduleGlobalResumeAll );
			pybind::def_functor( kernel, "scheduleGlobalIsFreeze", nodeScriptMethod, &NodeScriptMethod::s_scheduleGlobalIsFreeze );
			pybind::def_functor( kernel, "scheduleGlobalTime", nodeScriptMethod, &NodeScriptMethod::s_scheduleGlobalTime );


			pybind::def_functor( kernel, "getCursorPosition", nodeScriptMethod, &NodeScriptMethod::s_getCursorPosition );
			pybind::def_functor( kernel, "getTouchPosition", nodeScriptMethod, &NodeScriptMethod::s_getTouchPosition );
			pybind::def_functor( kernel, "getMousePosition", nodeScriptMethod, &NodeScriptMethod::s_getMousePosition );

			pybind::def_functor( kernel, "setArrow", nodeScriptMethod, &NodeScriptMethod::s_setArrow );
			pybind::def_functor( kernel, "getArrow", nodeScriptMethod, &NodeScriptMethod::s_getArrow );
			pybind::def_functor( kernel, "hideArrow", nodeScriptMethod, &NodeScriptMethod::s_hideArrow );

			pybind::def_functor( kernel, "setArrowLayer", nodeScriptMethod, &NodeScriptMethod::s_setArrowLayer );

			pybind::def_functor( kernel, "createResource", nodeScriptMethod, &NodeScriptMethod::createResource );
			pybind::def_functor( kernel, "directResourceCompile", nodeScriptMethod, &NodeScriptMethod::directResourceCompile );
			pybind::def_functor( kernel, "directResourceRelease", nodeScriptMethod, &NodeScriptMethod::directResourceRelease );
			pybind::def_functor( kernel, "getResourceReference", nodeScriptMethod, &NodeScriptMethod::s_getResourceReference );

			pybind::def_functor( kernel, "directFontCompile", nodeScriptMethod, &NodeScriptMethod::directFontCompile );
			pybind::def_functor( kernel, "directFontRelease", nodeScriptMethod, &NodeScriptMethod::directFontRelease );

			pybind::def_functor( kernel, "quitApplication", nodeScriptMethod, &NodeScriptMethod::quitApplication );
			pybind::def_functor( kernel, "setFullscreenMode", nodeScriptMethod, &NodeScriptMethod::s_setFullscreenMode );
			pybind::def_functor( kernel, "getFullscreenMode", nodeScriptMethod, &NodeScriptMethod::s_getFullscreenMode );
			pybind::def_functor( kernel, "setFixedContentResolution", nodeScriptMethod, &NodeScriptMethod::s_setFixedContentResolution );
			pybind::def_functor( kernel, "getFixedContentResolution", nodeScriptMethod, &NodeScriptMethod::s_getFixedContentResolution );
			pybind::def_functor( kernel, "setFixedDisplayResolution", nodeScriptMethod, &NodeScriptMethod::s_setFixedDisplayResolution );
			pybind::def_functor( kernel, "getFixedDisplayResolution", nodeScriptMethod, &NodeScriptMethod::s_getFixedDisplayResolution );


			pybind::def_functor( kernel, "renderOneFrame", nodeScriptMethod, &NodeScriptMethod::renderOneFrame );
			pybind::def_functor( kernel, "writeImageToFile", nodeScriptMethod, &NodeScriptMethod::writeImageToFile );
			pybind::def_functor( kernel, "createImageResource", nodeScriptMethod, &NodeScriptMethod::s_createImageResource );
			pybind::def_functor( kernel, "createImageSolidResource", nodeScriptMethod, &NodeScriptMethod::s_createImageSolidResource );
			pybind::def_functor( kernel, "minimizeWindow", nodeScriptMethod, &NodeScriptMethod::minimizeWindow );

			pybind::def_functor( kernel, "getCurrentResolution", nodeScriptMethod, &NodeScriptMethod::s_getCurrentResolution );
			pybind::def_functor( kernel, "getContentResolution", nodeScriptMethod, &NodeScriptMethod::s_getContentResolution );
			pybind::def_functor( kernel, "getHotSpotImageSize", nodeScriptMethod, &NodeScriptMethod::s_getHotSpotImageSize );

			pybind::def_functor( kernel, "setNopause", nodeScriptMethod, &NodeScriptMethod::s_setNopause );
			pybind::def_functor( kernel, "getNopause", nodeScriptMethod, &NodeScriptMethod::s_getNopause );

			pybind::def_functor( kernel, "isInViewport", nodeScriptMethod, &NodeScriptMethod::s_isInViewport );
			pybind::def_functor( kernel, "enableTextureFiltering", nodeScriptMethod, &NodeScriptMethod::s_enableTextureFiltering );
			pybind::def_functor( kernel, "isTextureFilteringEnabled", nodeScriptMethod, &NodeScriptMethod::s_isTextureFilteringEnabled );

			pybind::def_functor( kernel, "existText", nodeScriptMethod, &NodeScriptMethod::s_existText );

			pybind::def_functor( kernel, "pickHotspot", nodeScriptMethod, &NodeScriptMethod::s_pickHotspot );

			pybind::def_functor( kernel, "blockInput", nodeScriptMethod, &NodeScriptMethod::s_setMousePickerBlockInput );
			pybind::def_functor( kernel, "setMousePickerHandleValue", nodeScriptMethod, &NodeScriptMethod::s_setMousePickerHandleValue );

			pybind::def_functor( kernel, "setInputMouseButtonEventBlock", nodeScriptMethod, &NodeScriptMethod::s_setInputMouseButtonEventBlock );
			pybind::def_functor( kernel, "getInputMouseButtonEventBlock", nodeScriptMethod, &NodeScriptMethod::s_getInputMouseButtonEventBlock );

			pybind::def_functor( kernel, "loadPlugin", nodeScriptMethod, &NodeScriptMethod::s_loadPlugin );

			pybind::def_functor( kernel, "getRenderCamera2D", nodeScriptMethod, &NodeScriptMethod::s_getRenderCamera2D );

			pybind::def_functor( kernel, "testHotspot", nodeScriptMethod, &NodeScriptMethod::s_testHotspot );
			pybind::def_functor( kernel, "polygon_wm", nodeScriptMethod, &NodeScriptMethod::s_polygon_wm );
			pybind::def_functor( kernel, "polygon_anchor", nodeScriptMethod, &NodeScriptMethod::s_polygon_anchor );

			pybind::def_functor( kernel, "showKeyboard", nodeScriptMethod, &NodeScriptMethod::showKeyboard );
			pybind::def_functor( kernel, "hideKeyboard", nodeScriptMethod, &NodeScriptMethod::hideKeyboard );

			pybind::def_functor( kernel, "hasResource", nodeScriptMethod, &NodeScriptMethod::hasResource );

			pybind::def_functor_args( kernel, "removeCurrentScene", nodeScriptMethod, &NodeScriptMethod::removeCurrentScene );

			pybind::def_functor_args( kernel, "addMouseMoveHandler", nodeScriptMethod, &NodeScriptMethod::s_addMouseMoveHandler );
			pybind::def_functor_args( kernel, "addMouseButtonHandler", nodeScriptMethod, &NodeScriptMethod::s_addMouseButtonHandler );
			pybind::def_functor_args( kernel, "addMouseButtonHandlerBegin", nodeScriptMethod, &NodeScriptMethod::s_addMouseButtonHandlerBegin );
			pybind::def_functor_args( kernel, "addMouseButtonHandlerEnd", nodeScriptMethod, &NodeScriptMethod::s_addMouseButtonHandlerEnd );
			pybind::def_functor_args( kernel, "addMouseWheelHandler", nodeScriptMethod, &NodeScriptMethod::s_addMouseWheelHandler );
			pybind::def_functor_args( kernel, "addKeyHandler", nodeScriptMethod, &NodeScriptMethod::s_addKeyHandler );
			pybind::def_functor_args( kernel, "addTextHandler", nodeScriptMethod, &NodeScriptMethod::s_addTextHandler );

			pybind::def_functor( kernel, "removeGlobalHandler", nodeScriptMethod, &NodeScriptMethod::s_removeGlobalHandler );
			pybind::def_functor( kernel, "enableGlobalHandler", nodeScriptMethod, &NodeScriptMethod::s_enableGlobalHandler );

			pybind::def_functor( kernel, "visitChild", nodeScriptMethod, &NodeScriptMethod::s_visitChild );

			pybind::def_functor( kernel, "visitCompiledResources", nodeScriptMethod, &NodeScriptMethod::s_visitCompiledResources );
			pybind::def_functor( kernel, "visitResources", nodeScriptMethod, &NodeScriptMethod::s_visitResources );

			pybind::def_functor( kernel, "incrementResources", nodeScriptMethod, &NodeScriptMethod::s_incrementResources );
			pybind::def_functor( kernel, "decrementResources", nodeScriptMethod, &NodeScriptMethod::s_decrementResources );

			pybind::def_functor( kernel, "validResource", nodeScriptMethod, &NodeScriptMethod::s_validResource );

			pybind::def_functor_kernel( kernel, "getResources", nodeScriptMethod, &NodeScriptMethod::s_getResources );

			pybind::def_functor( kernel, "testPlatformTags", nodeScriptMethod, &NodeScriptMethod::s_testPlatformTags );
			pybind::def_functor( kernel, "hasTouchpad", nodeScriptMethod, &NodeScriptMethod::s_hasTouchpad );
			pybind::def_functor( kernel, "getTimingFactor", nodeScriptMethod, &NodeScriptMethod::s_getTimingFactor );
			pybind::def_functor( kernel, "setTimingFactor", nodeScriptMethod, &NodeScriptMethod::s_setTimingFactor );

			pybind::def_functor( kernel, "addHomeless", nodeScriptMethod, &NodeScriptMethod::s_addHomeless );
			pybind::def_functor( kernel, "isHomeless", nodeScriptMethod, &NodeScriptMethod::s_isHomeless );

			pybind::def_functor( kernel, "cancelTask", nodeScriptMethod, &NodeScriptMethod::s_cancelTask );
			pybind::def_functor( kernel, "joinTask", nodeScriptMethod, &NodeScriptMethod::s_joinTask );

			pybind::def_functor_kernel( kernel, "getNullObjectsFromResourceVideo", nodeScriptMethod, &NodeScriptMethod::s_getNullObjectsFromResourceVideo );

			pybind::def_functor( kernel, "hasMovieSlot", nodeScriptMethod, &NodeScriptMethod::s_hasMovieSlot );
			pybind::def_functor( kernel, "hasMovieSubMovie", nodeScriptMethod, &NodeScriptMethod::s_hasMovieSubMovie );
			pybind::def_functor( kernel, "hasMovieSocket", nodeScriptMethod, &NodeScriptMethod::s_hasMovieSocket );
			pybind::def_functor( kernel, "hasMovieEvent", nodeScriptMethod, &NodeScriptMethod::s_hasMovieEvent );

			pybind::def_functor( kernel, "getMovieDuration", nodeScriptMethod, &NodeScriptMethod::s_getMovieDuration );

			pybind::def_functor( kernel, "getGameAspect", nodeScriptMethod, &NodeScriptMethod::s_getGameAspect );
			pybind::def_functor( kernel, "getGameViewport", nodeScriptMethod, &NodeScriptMethod::s_getGameViewport );

			pybind::def_functor_kernel( kernel, "getGameParam", nodeScriptMethod, &NodeScriptMethod::s_getGameParam );
			pybind::def_functor_kernel( kernel, "getGameParamFloat", nodeScriptMethod, &NodeScriptMethod::s_getGameParamFloat );
			pybind::def_functor_kernel( kernel, "getGameParamInt", nodeScriptMethod, &NodeScriptMethod::s_getGameParamInt );
			pybind::def_functor_kernel( kernel, "getGameParamBool", nodeScriptMethod, &NodeScriptMethod::s_getGameParamBool );

			pybind::def_functor( kernel, "hasGameParam", nodeScriptMethod, &NodeScriptMethod::s_hasGameParam );
			pybind::def_functor( kernel, "openUrlInDefaultBrowser", nodeScriptMethod, &NodeScriptMethod::s_openUrlInDefaultBrowser );

			pybind::def_functor( kernel, "getDefaultResourceFontName", nodeScriptMethod, &NodeScriptMethod::s_getDefaultResourceFontName );


			pybind::def_functor( kernel, "pushMouseMove", nodeScriptMethod, &NodeScriptMethod::s_pushMouseMove );
			pybind::def_functor( kernel, "pushMouseButtonEvent", nodeScriptMethod, &NodeScriptMethod::s_pushMouseButtonEvent );

			pybind::def_functor( kernel, "platformEvent", nodeScriptMethod, &NodeScriptMethod::s_platformEvent );

			pybind::def_functor( kernel, "getProjectCodename", nodeScriptMethod, &NodeScriptMethod::s_getProjectCodename );

			pybind::def_functor( kernel, "sleep", nodeScriptMethod, &NodeScriptMethod::s_sleep );


			pybind::def_functor_args( kernel, "getMessage", nodeScriptMethod, &NodeScriptMethod::s_getMessage );
			pybind::def_functor_args( kernel, "postMessage", nodeScriptMethod, &NodeScriptMethod::s_postMessage );
            pybind::def_functor_args( kernel, "headerData", nodeScriptMethod, &NodeScriptMethod::s_headerData );            
			pybind::def_functor_args( kernel, "downloadAsset", nodeScriptMethod, &NodeScriptMethod::s_downloadAsset );
			pybind::def_functor( kernel, "cancelRequest", nodeScriptMethod, &NodeScriptMethod::s_cancelRequest );

			pybind::def_functor( kernel, "mountResourcePak", nodeScriptMethod, &NodeScriptMethod::s_mountResourcePak );
			pybind::def_functor( kernel, "unmountResourcePak", nodeScriptMethod, &NodeScriptMethod::s_unmountResourcePak );

			pybind::def_functor( kernel, "existFile", nodeScriptMethod, &NodeScriptMethod::s_existFile );
			pybind::def_functor( kernel, "removeFile", nodeScriptMethod, &NodeScriptMethod::s_removeFile );
			pybind::def_functor_kernel( kernel, "parseXml", nodeScriptMethod, &NodeScriptMethod::s_parseXml );

			pybind::def_functor( kernel, "visitFonts", nodeScriptMethod, &NodeScriptMethod::s_visitFonts );
			pybind::def_functor_kernel( kernel, "getFonts", nodeScriptMethod, &NodeScriptMethod::s_getFonts );
			pybind::def_functor( kernel, "hasFont", nodeScriptMethod, &NodeScriptMethod::s_hasFont );
			pybind::def_functor( kernel, "validateFont", nodeScriptMethod, &NodeScriptMethod::s_validateFont );

			pybind::def_functor( kernel, "prefetchResources", nodeScriptMethod, &NodeScriptMethod::s_prefetchResources );
			pybind::def_functor( kernel, "unfetchResources", nodeScriptMethod, &NodeScriptMethod::s_unfetchResources );

			pybind::def_functor( kernel, "cacheResources", nodeScriptMethod, &NodeScriptMethod::s_cacheResources );
			pybind::def_functor( kernel, "uncacheResources", nodeScriptMethod, &NodeScriptMethod::s_uncacheResources );

			pybind::def_functor( kernel, "rotateToTrimetric", nodeScriptMethod, &NodeScriptMethod::s_rotateToTrimetric );
			pybind::def_functor( kernel, "rotateToTrimetric2", nodeScriptMethod, &NodeScriptMethod::s_rotateToTrimetric2 );

			pybind::def_functor( kernel, "hotspotCorrect", nodeScriptMethod, &NodeScriptMethod::s_hotspotCorrect );

			pybind::def_functor( kernel, "copyUserPicture", nodeScriptMethod, &NodeScriptMethod::s_copyUserPicture );
			pybind::def_functor( kernel, "copyUserMusic", nodeScriptMethod, &NodeScriptMethod::s_copyUserMusic );

			pybind::def_functor( kernel, "screenToWorldPoint", nodeScriptMethod, &NodeScriptMethod::s_screenToWorldPoint );
			pybind::def_functor( kernel, "screenToWorldClick", nodeScriptMethod, &NodeScriptMethod::s_screenToWorldClick );

			pybind::def_functor_kernel( kernel, "getMovieSlotsPosition", nodeScriptMethod, &NodeScriptMethod::s_getMovieSlotsPosition );
			pybind::def_functor_kernel( kernel, "getMovieSlotPosition", nodeScriptMethod, &NodeScriptMethod::s_getMovieSlotPosition );

			pybind::def_functor( kernel, "gridBurnTransparency", nodeScriptMethod, &NodeScriptMethod::s_gridBurnTransparency );

			pybind::def_functor( kernel, "setLocale", nodeScriptMethod, &NodeScriptMethod::s_setLocale );

			pybind::def_functor_args( kernel, "createAffector", nodeScriptMethod, &NodeScriptMethod::s_createAffector );
			pybind::def_functor_args( kernel, "addAffector", nodeScriptMethod, &NodeScriptMethod::s_addAffector );
			pybind::def_functor( kernel, "removeAffector", nodeScriptMethod, &NodeScriptMethod::s_removeAffector );

			pybind::interface_<NodeScriptMethod::AffectorFollower, pybind::bases<Affector> >( kernel, "AffectorFollower" )
				.def( "follow", &NodeScriptMethod::AffectorFollower::follow )
				;

			pybind::def_functor( kernel, "addNodeFollowerLocalAlpha", nodeScriptMethod, &NodeScriptMethod::s_addNodeFollowerLocalAlpha );
			pybind::def_functor( kernel, "addShapeFollowerCustomSize", nodeScriptMethod, &NodeScriptMethod::s_addShapeFollowerCustomSize );
			pybind::def_functor( kernel, "addShapeFollowerTextureUVScale", nodeScriptMethod, &NodeScriptMethod::s_addShapeFollowerTextureUVScale );
			pybind::def_functor( kernel, "removeNodeFollower", nodeScriptMethod, &NodeScriptMethod::s_removeNodeFollower );

			pybind::def_functor( kernel, "moduleMessage", nodeScriptMethod, &NodeScriptMethod::s_moduleMessage );

			pybind::def_functor( kernel, "findNodeScene", nodeScriptMethod, &NodeScriptMethod::s_findNodeScene );

			pybind::def_functor( kernel, "getCameraPosition", nodeScriptMethod, &NodeScriptMethod::s_getCameraPosition );
		}
	}
}
