#	include "Movie2.h"

#   include "Interface/TimelineInterface.h"
#   include "Interface/NodeInterface.h"
#   include "Interface/StringizeInterface.h"
#   include "Interface/PrototypeManagerInterface.h"

#	include "SurfaceVideo.h"
#	include "SurfaceSound.h"

#	include "Consts.h"

#	include "Kernel/Materialable.h"

#	include "Logger/Logger.h"

#	include "Math/quat.h"

namespace Menge
{
	//////////////////////////////////////////////////////////////////////////
	Movie2::Movie2()
		: m_composition(nullptr)
	{	
	}
	//////////////////////////////////////////////////////////////////////////
	Movie2::~Movie2()
	{
	}
	//////////////////////////////////////////////////////////////////////////
	void Movie2::setResourceMovie2( const ResourceMovie2Ptr & _resourceMovie2 )
	{
		if( m_resourceMovie2 == _resourceMovie2 )
		{
			return;
		}

		m_resourceMovie2 = _resourceMovie2;

		this->recompile();
	}
	//////////////////////////////////////////////////////////////////////////
	const ResourceMovie2Ptr & Movie2::getResourceMovie2() const
	{
		return m_resourceMovie2;
	}
	//////////////////////////////////////////////////////////////////////////
	void Movie2::setCompositionName( const ConstString & _name )
	{
		m_compositionName = _name;
	}
	//////////////////////////////////////////////////////////////////////////
	const ConstString & Movie2::getCompositionName() const
	{
		return m_compositionName;
	}
	//////////////////////////////////////////////////////////////////////////
	float Movie2::getDuration() const
	{
		float duration = ae_get_movie_composition_duration( m_composition );

		return duration;
	}
	//////////////////////////////////////////////////////////////////////////
	void Movie2::setWorkAreaFromEvent( const ConstString & _eventName )
	{
		float a, b;
		ae_bool_t ok = ae_get_movie_composition_node_in_out_time( m_composition, _eventName.c_str(), AE_MOVIE_LAYER_TYPE_EVENT, &a, &b );

		if( ok == AE_FALSE )
		{
			return;
		}

		ae_set_movie_composition_work_area( m_composition, a, b );
	}
	//////////////////////////////////////////////////////////////////////////
	void Movie2::removeWorkArea()
	{
		ae_remove_movie_composition_work_area( m_composition );
	}
	//////////////////////////////////////////////////////////////////////////
	bool Movie2::_play( float _time )
	{
		(void)_time;

		ae_play_movie_composition( m_composition, _time );

		return true;
	}
	//////////////////////////////////////////////////////////////////////////
	bool Movie2::_restart( float _time, uint32_t _enumerator )
	{
		(void)_time;
		(void)_enumerator;

		return true;
	}
	//////////////////////////////////////////////////////////////////////////
	void Movie2::_pause( uint32_t _enumerator )
	{
		(void)_enumerator;
	}
	//////////////////////////////////////////////////////////////////////////
	void Movie2::_resume( float _time, uint32_t _enumerator )
	{
		(void)_time;
		(void)_enumerator;
	}
	//////////////////////////////////////////////////////////////////////////
	void Movie2::_stop( uint32_t _enumerator )
	{
		(void)_enumerator;
	}
	//////////////////////////////////////////////////////////////////////////
	void Movie2::_end( uint32_t _enumerator )
	{
		(void)_enumerator;
	}
	//////////////////////////////////////////////////////////////////////////
	static void * ae_movie_composition_node_camera( const ae_char_t * _name, const ae_vector3_t _position, const ae_vector3_t _direction, float _fov, float _width, float _height, void * _data )
	{
		Movie2 * movie2 = (Movie2 *)_data;

		ServiceProviderInterface * serviceProvider = movie2->getServiceProvider();

		ConstString c_name = Helper::stringizeString( serviceProvider, _name );

		Movie2::Camera * old_camera;
		if( movie2->getCamera( c_name, &old_camera ) == true )
		{
			return old_camera;
		}
		
		RenderCameraProjection * renderCameraProjection = NODE_SERVICE( serviceProvider )
			->createNodeT<RenderCameraProjection *>( CONST_STRING( serviceProvider, RenderCameraProjection ) );

		renderCameraProjection->setName( c_name );

		mt::vec3f cameraPosition;
		cameraPosition.from_f3( _position );
		mt::vec3f cameraDirection;
		cameraDirection.from_f3( _direction );

		float aspect = _width / _height;
		
		renderCameraProjection->setLocalPosition( cameraPosition );
		renderCameraProjection->setDirection( cameraDirection, mt::vec3f(0.f, 1.f, 0.f) );
		renderCameraProjection->setCameraFOV( _fov );
		renderCameraProjection->setCameraAspect( aspect );
		
		RenderViewport * renderViewport = NODE_SERVICE( serviceProvider )
			->createNodeT<RenderViewport *>( CONST_STRING( serviceProvider, RenderViewport ) );

		renderViewport->setName( c_name );

		Viewport vp;
		vp.begin.x = 0.f;
		vp.begin.y = 0.f;

		vp.end.x = _width;
		vp.end.y = _height;

		renderViewport->setViewport( vp );
		
		Movie2::Camera * new_camera = movie2->addCamera( c_name, renderCameraProjection, renderViewport );

		return new_camera;
	}
	//////////////////////////////////////////////////////////////////////////
	static void * ae_movie_composition_node_provider( const aeMovieLayerData * _layerData, const aeMovieResource * _resource, const ae_matrix4_t _matrix, void * _data )
	{
		Movie2 * movie2 = (Movie2 *)_data;

		ServiceProviderInterface * serviceProvider = movie2->getServiceProvider();

		ConstString c_name = Helper::stringizeString( serviceProvider, _layerData->name );

		uint8_t type = _layerData->type;

		switch( type )
		{
		case AE_MOVIE_LAYER_TYPE_VIDEO:
			{
				SurfaceVideo * surfaceVideo = PROTOTYPE_SERVICE( serviceProvider )
					->generatePrototypeT<SurfaceVideo *>( CONST_STRING( serviceProvider, Surface ), CONST_STRING( serviceProvider, SurfaceVideo ) );

				surfaceVideo->setName( c_name );

				ResourceVideo * resourceVideo = (ResourceVideo *)(_resource->data);

				surfaceVideo->setResourceVideo( resourceVideo );

				EMaterialBlendMode blend_mode = EMB_NORMAL;

				switch( _layerData->blend_mode )
				{
				case AE_MOVIE_BLEND_ADD:
					blend_mode = EMB_ADD;
					break;
				case AE_MOVIE_BLEND_SCREEN:
					blend_mode = EMB_SCREEN;
					break;
				};

				surfaceVideo->setBlendMode( blend_mode );

				return surfaceVideo;
			}break;
		case AE_MOVIE_LAYER_TYPE_SOUND:
			{
				SurfaceSound * surfaceSound = PROTOTYPE_SERVICE( serviceProvider )
					->generatePrototypeT<SurfaceSound *>( CONST_STRING( serviceProvider, Surface ), CONST_STRING( serviceProvider, SurfaceSound ) );

				surfaceSound->setName( c_name );

				ResourceSound * resourceSound = (ResourceSound *)(_resource->data);

				surfaceSound->setResourceSound( resourceSound );
				
				return surfaceSound;
			}break;
		}
				
		return AE_NULL;
	}
	//////////////////////////////////////////////////////////////////////////
	static void ae_movie_composition_node_destroyer( const void * _element, uint32_t _type, void * _data )
	{

	}
	//////////////////////////////////////////////////////////////////////////
	static void ae_movie_composition_node_update( const void * _element, uint32_t _type, ae_bool_t _loop, aeMovieNodeUpdateState _state, float _offset, const ae_matrix4_t _matrix, float _opacity, void * _data )
	{
		if( _state == AE_MOVIE_NODE_UPDATE_UPDATE )
		{
			switch( _type )
			{
			case AE_MOVIE_LAYER_TYPE_PARTICLE:
				{
					//printf( "AE_MOVIE_LAYER_TYPE_PARTICLE %f %f\n"
					//	, _matrix[12]
					//	, _matrix[13]
					//	);
				}break;
			case AE_MOVIE_LAYER_TYPE_SLOT:
				{
					//printf( "AE_MOVIE_LAYER_TYPE_SLOT %f %f\n"
					//	, _matrix[12]
					//	, _matrix[13]
					//	);
				}break;
			}
		}
		else if( _state == AE_MOVIE_NODE_UPDATE_BEGIN )
		{
			switch( _type )
			{
			case AE_MOVIE_LAYER_TYPE_VIDEO:
				{
					SurfaceVideo * surfaceVide = (SurfaceVideo *)_element;

					Movie2 * movie2 = (Movie2 *)_data;

					ServiceProviderInterface * serviceProvider = movie2->getServiceProvider();

					float time = TIMELINE_SERVICE( serviceProvider )
						->getTime();

					surfaceVide->setTiming( _offset );

					if( surfaceVide->play( time ) == 0 )
					{
						return;
					}
				}break;
			case AE_MOVIE_LAYER_TYPE_SOUND:
				{
					SurfaceSound * surfaceSound = (SurfaceSound *)_element;

					Movie2 * movie2 = (Movie2 *)_data;

					ServiceProviderInterface * serviceProvider = movie2->getServiceProvider();

					float time = TIMELINE_SERVICE( serviceProvider )
						->getTime();

					surfaceSound->setTiming( _offset );

					if( surfaceSound->play( time ) == 0 )
					{
						return;
					}
				}break;
			}
		}
		else if( _state == AE_MOVIE_NODE_UPDATE_END )
		{
			switch( _type )
			{
			case AE_MOVIE_LAYER_TYPE_VIDEO:
				{
					SurfaceVideo * surfaceVide = (SurfaceVideo *)_element;

					surfaceVide->stop();
				}break;
			case AE_MOVIE_LAYER_TYPE_SOUND:
				{
					SurfaceSound * surfaceSound = (SurfaceSound *)_element;

					surfaceSound->stop();
				}break;
			}
		}
	}
	//////////////////////////////////////////////////////////////////////////
	static void * ae_movie_composition_track_matte_update( const void * _element, uint32_t _type, ae_bool_t _loop, aeMovieNodeUpdateState _state, float _offset, const aeMovieRenderMesh * _mesh, void * _track_matte_data, void * _data )
	{
		switch( _state )
		{
		case AE_MOVIE_NODE_UPDATE_BEGIN:
			{
			}break;
		case AE_MOVIE_NODE_UPDATE_UPDATE:
			{
			}break;
		case AE_MOVIE_NODE_UPDATE_END:
			{
			}break;
		}

		return nullptr;
	}
	//////////////////////////////////////////////////////////////////////////
	static void ae_movie_composition_state( aeMovieCompositionStateFlag _state, void * _data )
	{
		if( _state == AE_MOVIE_COMPOSITION_END )
		{
			printf( "AE_MOVIE_COMPOSITION_END\n" );

			Movie2 * m2 = (Movie2 *)(_data);
			m2->stop();
			//ae_destroy_movie_composition( m2->m_composition );
			//m2->m_composition = nullptr;
			


		}
	}
	static void ae_movie_node_event_t( const void * _element, const char * _name, const ae_matrix4_t _matrix, float _opacity, ae_bool_t _begin, void * _data )
	{

	}
	//////////////////////////////////////////////////////////////////////////
	Movie2::Camera * Movie2::addCamera( const ConstString & _name, RenderCameraProjection * _projection, RenderViewport * _viewport )
	{
		this->addChild( _projection );
		this->addChild( _viewport );

		Camera c;
		c.projection = _projection;
		c.viewport = _viewport;
		
		TMapCamera::iterator it_found = m_cameras.insert( std::make_pair(_name, c) ).first;

		Camera * new_camera = &it_found->second;

		return new_camera;
	}
	//////////////////////////////////////////////////////////////////////////
	bool Movie2::hasCamera( const ConstString & _name ) const
	{ 
		TMapCamera::const_iterator it_found = m_cameras.find( _name );

		if( it_found == m_cameras.end() )
		{
			return false;
		}

		return true;
	}
	//////////////////////////////////////////////////////////////////////////
	bool Movie2::getCamera( const ConstString & _name, Camera ** _camera )
	{
		TMapCamera::iterator it_found = m_cameras.find( _name );

		if( it_found == m_cameras.end() )
		{
			return false;
		}

		Camera & camera = it_found->second;

		*_camera = &camera;

		return true;
	}
	//////////////////////////////////////////////////////////////////////////
	bool Movie2::_compile()
	{
		if( m_resourceMovie2 == nullptr )
		{
			LOGGER_ERROR(m_serviceProvider)("Movie2::_compile: '%s' can't setup resource"
				, this->getName().c_str()
				);

			return false;
		}

		if( m_resourceMovie2.compile() == false )
		{
			LOGGER_ERROR(m_serviceProvider)("Movie2::_compile '%s' resource %s not compile"
				, m_name.c_str() 
				, m_resourceMovie2->getName().c_str()
				);

			return false;
		}

		const aeMovieData * movieData = m_resourceMovie2->getMovieData();

		const aeMovieCompositionData * compositionData = m_resourceMovie2->getCompositionData( m_compositionName );

		if( compositionData == nullptr )
		{
			LOGGER_ERROR( m_serviceProvider )("Movie2::_compile '%s' resource %s not found composition '%s'"
				, m_name.c_str()
				, m_resourceMovie2->getName().c_str()
				, m_compositionName.c_str()
				);

			return false;
		}

		aeMovieCompositionProviders providers;
		providers.camera_provider = &ae_movie_composition_node_camera;
		providers.node_provider = &ae_movie_composition_node_provider;
		providers.node_destroyer = &ae_movie_composition_node_destroyer;

		providers.node_update = &ae_movie_composition_node_update;
		providers.track_matte_update = &ae_movie_composition_track_matte_update;

		providers.event = &ae_movie_node_event_t;
		providers.composition_state = &ae_movie_composition_state;
		

		aeMovieComposition * composition = ae_create_movie_composition( movieData, compositionData, &providers, this );

		if( composition == nullptr )
		{
			return false;
		}

		ae_set_movie_composition_loop( composition, AE_TRUE );
		ae_set_movie_composition_interpolate( composition, AE_FALSE );

		//float a, b;
		//bool ok = ae_get_movie_composition_node_in_out_time( composition, "freespins_win", AE_MOVIE_LAYER_TYPE_EVENT, &a, &b );

		//ae_set_movie_composition_work_area( composition, a, b );

		uint32_t max_render_node = ae_get_movie_composition_max_render_node( composition );

		m_meshes.reserve( max_render_node );

		m_composition = composition;
				
		return true;
	}
	//////////////////////////////////////////////////////////////////////////
	void Movie2::_release()
	{	
		Node::_release();

		ae_destroy_movie_composition( m_composition );
		m_composition = nullptr;

		m_resourceMovie2.release();
	}
	//////////////////////////////////////////////////////////////////////////
	bool Movie2::_activate()
	{
		if( Node::_activate() == false )
		{
			return false;
		}

		ae_vector3_t anchorPoint;
		if( ae_get_movie_composition_anchor_point( m_composition, anchorPoint ) == AE_TRUE )
		{	
			mt::vec3f origin;

			origin.from_f3( anchorPoint );

			this->setOrigin( origin );
		}

		return true;
	}
	//////////////////////////////////////////////////////////////////////////
	void Movie2::_deactivate()
	{
		Node::_deactivate();
	}
	//////////////////////////////////////////////////////////////////////////
	void Movie2::_changeParent( Node * _oldParent, Node * _newParent )
	{
	}
	//////////////////////////////////////////////////////////////////////////
	void Movie2::_addChild( Node * _node )
	{
	}
	//////////////////////////////////////////////////////////////////////////
	void Movie2::_removeChild( Node * _node )
	{
	}
	//////////////////////////////////////////////////////////////////////////
	void Movie2::_afterActivate()
	{
		Node::_afterActivate();

		bool autoPlay = this->getAutoPlay();

		if( autoPlay == true )
		{
			float time = TIMELINE_SERVICE( m_serviceProvider )
				->getTime();

			if( this->play( time ) == 0 )
			{
				LOGGER_ERROR( m_serviceProvider )("Movie2::_afterActivate '%s' resource '%s' auto play return 0"
					, this->getName().c_str()
					, this->m_resourceMovie2->getName().c_str()
					);

				return;
			}
		}		
	}
	//////////////////////////////////////////////////////////////////////////
	void Movie2::_setLoop( bool _value )
	{
		if( m_composition == nullptr )
		{
			return;
		}

		ae_set_movie_composition_loop( m_composition, _value );
	}
	//////////////////////////////////////////////////////////////////////////
	void Movie2::_setTiming( float _timing )
	{
		if( m_composition == nullptr )
		{
			return;
		}

		ae_set_movie_composition_time( m_composition, _timing );
	}
	//////////////////////////////////////////////////////////////////////////
	float Movie2::_getTiming() const
	{
		float timing = ae_get_movie_composition_time( m_composition );

		return timing;
	}
	//////////////////////////////////////////////////////////////////////////
	void Movie2::_update( float _current, float _timing )
	{
		(void)_current;
		(void)_timing;

		if( this->isPlay() == false )
		{
			return;
		}

		static float a = 0.f;

		if( a < 500.f )
		{
			a += _timing;
			return;
		}
				
		ae_update_movie_composition( m_composition, _timing );
		

		printf( "time %f\n"
				, ae_get_movie_composition_time(m_composition)
				);
		
		//if( a < 10000.f )
		//{
		//	a += _timing;
		//}

		//if( a > 10000.f )
		//{
		//	set_movie_composition_timing( m_composition, 6000.f );
		//}
	}
	//////////////////////////////////////////////////////////////////////////
	void Movie2::_render( const RenderObjectState * _state )
	{
		m_meshes.clear();

		const mt::mat4f & wm = this->getWorldMatrix();

		uint32_t mesh_iterator = 0;

		aeMovieRenderMesh mesh;
		while( ae_compute_movie_mesh( m_composition, &mesh_iterator, &mesh ) == AE_TRUE )
		{			
			if( mesh.track_matte_data != nullptr )
			{
				continue;
			}

			ResourceReference * resource_reference = (ResourceReference *)mesh.resource_data;

			RenderObjectState state;
			//if( mesh.camera_data != nullptr )
			//{
			//	Movie2::Camera * camera = (Movie2::Camera *)mesh.camera_data;

			//	state.camera = camera->projection;
			//	state.viewport = camera->viewport;
			//	state.clipplane = _state->clipplane;
			//	state.target = _state->target;
			//}
			//else
			{
				state.camera = _state->camera;
				state.viewport = _state->viewport;
				state.clipplane = _state->clipplane;
				state.target = _state->target;
			}

			switch( mesh.layer_type )
			{
			case AE_MOVIE_LAYER_TYPE_SHAPE:
				{
					m_meshes.push_back( Mesh() );
					Mesh & m = m_meshes.back();

					m.vertices.resize( mesh.vertexCount );

					ColourValue_ARGB color = Helper::makeARGB( mesh.r, mesh.g, mesh.b, mesh.a );

					for( uint32_t index = 0; index != mesh.vertexCount; ++index )
					{
						RenderVertex2D & v = m.vertices[index];

						mt::vec3f vp;
						vp.from_f3( &mesh.position[index][0] );

						mt::mul_v3_v3_m4( v.position, vp, wm );

						mt::vec2f uv;
						uv.from_f2( &mesh.uv[index][0] );

						v.uv[0] = uv;
						v.uv[1] = uv;

						v.color = color;
					}

					m.indices.assign( mesh.indices, mesh.indices + mesh.indexCount );

					EMaterialBlendMode blend_mode = EMB_NORMAL;

					switch( mesh.blend_mode )
					{
					case AE_MOVIE_BLEND_ADD:
						blend_mode = EMB_ADD;
						break;
					};

					m.material = Helper::makeTextureMaterial( m_serviceProvider, nullptr, 0, ConstString::none(), blend_mode, false, false, false );

					RENDER_SERVICE( m_serviceProvider )
						->addRenderObject( &state, m.material, &m.vertices[0], m.vertices.size(), &m.indices[0], m.indices.size(), nullptr, false );
				}break;
			case AE_MOVIE_LAYER_TYPE_SOLID:
				{
					m_meshes.push_back( Mesh() );
					Mesh & m = m_meshes.back();

					m.vertices.resize( mesh.vertexCount );

					ColourValue_ARGB color = Helper::makeARGB( mesh.r, mesh.g, mesh.b, mesh.a );

					for( uint32_t index = 0; index != mesh.vertexCount; ++index )
					{
						RenderVertex2D & v = m.vertices[index];

						mt::vec3f vp;
						vp.from_f3( &mesh.position[index][0] );

						mt::mul_v3_v3_m4( v.position, vp, wm );

						mt::vec2f uv;
						uv.from_f2( &mesh.uv[index][0] );

						v.uv[0] = uv;
						v.uv[1] = uv;

						v.color = color;
					}

					m.indices.assign( mesh.indices, mesh.indices + mesh.indexCount );

					EMaterialBlendMode blend_mode = EMB_NORMAL;

					switch( mesh.blend_mode )
					{
					case AE_MOVIE_BLEND_ADD:
						blend_mode = EMB_ADD;
						break;
					};

					m.material = Helper::makeTextureMaterial( m_serviceProvider, nullptr, 0, ConstString::none(), blend_mode, false, false, false );

					RENDER_SERVICE( m_serviceProvider )
						->addRenderObject( &state, m.material, &m.vertices[0], m.vertices.size(), &m.indices[0], m.indices.size(), nullptr, false );
				}break;
			case AE_MOVIE_LAYER_TYPE_SEQUENCE:
			case AE_MOVIE_LAYER_TYPE_IMAGE:
				{
					ResourceImage * resource_image = static_cast<ResourceImage *>(resource_reference);

					m_meshes.push_back( Mesh() );
					Mesh & m = m_meshes.back();

					m.vertices.resize( mesh.vertexCount );

					ColourValue_ARGB color = Helper::makeARGB( mesh.r, mesh.g, mesh.b, mesh.a );

					for( uint32_t index = 0; index != mesh.vertexCount; ++index )
					{
						RenderVertex2D & v = m.vertices[index];

						mt::vec3f vp;
						vp.from_f3( &mesh.position[index][0] );

						mt::mul_v3_v3_m4( v.position, vp, wm );

						mt::vec2f uv;
						uv.from_f2( &mesh.uv[index][0] );

						const mt::uv4f & uv_image = resource_image->getUVImage();

						mt::multiply_tetragon_uv4_v2( v.uv[0], uv_image, uv );

						const mt::uv4f & uv_alpha = resource_image->getUVAlpha();
						
						mt::multiply_tetragon_uv4_v2( v.uv[1], uv_alpha, uv );

						v.color = color;
					}					

					m.indices.assign( mesh.indices, mesh.indices + mesh.indexCount );

					EMaterialBlendMode blend_mode = EMB_NORMAL;

					switch( mesh.blend_mode )
					{
					case AE_MOVIE_BLEND_ADD:
						blend_mode = EMB_ADD;
						break;
					};

					m.material = Helper::makeImageMaterial( m_serviceProvider, resource_image, ConstString::none(), blend_mode, false, false );

					//printf( "%f %f\n", ae_get_movie_composition_time( m_composition ), mesh.a );
					
					RENDER_SERVICE( m_serviceProvider )
						->addRenderObject( &state, m.material, &m.vertices[0], m.vertices.size(), &m.indices[0], m.indices.size(), nullptr, false );
				}break;
			case AE_MOVIE_LAYER_TYPE_VIDEO:
				{
					//ResourceVideo * resource_video = static_cast<ResourceVideo *>(resource_reference);
					SurfaceVideo * surfaceVideo = static_cast<SurfaceVideo *>(mesh.element_data);

					m_meshes.push_back( Mesh() );
					Mesh & m = m_meshes.back();

					m.vertices.resize( mesh.vertexCount );

					ColourValue_ARGB color = Helper::makeARGB( mesh.r, mesh.g, mesh.b, mesh.a );

					for( uint32_t index = 0; index != mesh.vertexCount; ++index )
					{
						RenderVertex2D & v = m.vertices[index];

						mt::vec3f vp;
						vp.from_f3( &mesh.position[index][0] );

						mt::mul_v3_v3_m4( v.position, vp, wm );

						mt::vec2f uv;
						uv.from_f2( &mesh.uv[index][0] );

						const mt::vec4f & uv_video_mask = surfaceVideo->getUV();

						mt::uv4f uv_video( uv_video_mask );

						mt::multiply_tetragon_uv4_v2( v.uv[0], uv_video, uv );
						mt::multiply_tetragon_uv4_v2( v.uv[1], uv_video, uv );

						v.color = color;
					}

					m.indices.assign( mesh.indices, mesh.indices + mesh.indexCount );

					m.material = surfaceVideo->getMaterial();

					RENDER_SERVICE( m_serviceProvider )
						->addRenderObject( &state, m.material, &m.vertices[0], m.vertices.size(), &m.indices[0], m.indices.size(), nullptr, false );
				}break;
			}
		}
	}
	//////////////////////////////////////////////////////////////////////////
	bool Movie2::_interrupt( uint32_t _enumerator )
	{
		(void)_enumerator;

		ae_interrupt_movie_composition( m_composition, AE_TRUE );

		return true;
	}
}