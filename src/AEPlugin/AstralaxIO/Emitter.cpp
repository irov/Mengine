#	include "Emitter.h"

namespace Menge
{
	//////////////////////////////////////////////////////////////////////////
	Emitter::Emitter()
		: m_emitterId(0)
		, m_interval1(0.0)
	{
	}
	//////////////////////////////////////////////////////////////////////////
	Emitter::~Emitter()
	{	
	}
	//////////////////////////////////////////////////////////////////////////
	bool Emitter::initialize( HM_FILE _mf, std::auto_ptr<char> _buffer, HM_EMITTER _emitterId )
	{
		m_mf = _mf;
		m_buffer = _buffer;
		m_emitterId = _emitterId;

		Magic_SetRandomMode( m_emitterId, false );

		// set interpolation
		Magic_SetInterpolationMode( m_emitterId, false );

		if( Magic_Is3d( m_emitterId ) == false )
		{
			MAGIC_RECT rect;
			float backgroundScale = Magic_GetBackgroundRect( m_emitterId, &rect );

			(void)backgroundScale;
			
			if( rect.left != rect.right && rect.top != rect.bottom )
			{
				MAGIC_POSITION pos;
				Magic_GetEmitterPosition( m_emitterId, &pos );

				MAGIC_POSITION adapt_pos;
				adapt_pos.x = pos.x - (float)rect.left;
				adapt_pos.y = pos.y - (float)rect.top;
				adapt_pos.z = pos.z;

				Magic_SetEmitterPosition( m_emitterId, &adapt_pos );

				m_width = rect.right - rect.left;
				m_height = rect.bottom - rect.top;
			}
			else
			{
				MAGIC_POSITION pos;
				pos.x = 1024.f;
				pos.y = 1024.f;
				pos.z = 0.f;

				Magic_SetEmitterPosition( m_emitterId, &pos );

				m_width = 2048;
				m_height = 2048;
			}
		}
		else
		{
			MAGIC_VIEW view;
			int successful = Magic_GetView( m_emitterId, &view );

			if( successful != MAGIC_SUCCESS )
			{
				//_logger.message("Emitter::initialize emitter %d Magic_GetView return %d (please setup camera for 3d particle)"
				//	, m_emitterId
				//	, successful
				//	);

				return false;
			}

			m_width = view.viewport_width;
			m_height = view.viewport_height;
		}

		Magic_Restart( m_emitterId );

		if( Magic_IsInterval1( m_emitterId ) == true )
		{
			Magic_EmitterToInterval1( m_emitterId, 1.f, NULL );			
		}

		m_interval1 = Magic_GetInterval1( m_emitterId );

		return true;
	}
	//////////////////////////////////////////////////////////////////////////
	void Emitter::finalize()
	{
		Magic_UnloadEmitter( m_emitterId );
		Magic_CloseFile( m_mf );

		m_buffer.reset();
	}
	//////////////////////////////////////////////////////////////////////////
	HM_EMITTER Emitter::getId() const
	{
		return m_emitterId;
	}
	//////////////////////////////////////////////////////////////////////////
	const char * Emitter::getName() const
	{
		const char * name = Magic_GetEmitterName( m_emitterId );

		return name;
	}
	//////////////////////////////////////////////////////////////////////////
	double Emitter::getTime() const
	{
		double timePos = Magic_GetPosition( m_emitterId );

		timePos -= m_interval1;

		return timePos;
	}
	//////////////////////////////////////////////////////////////////////////
	double Emitter::getDuration() const
	{
		double duration = Magic_GetInterval2( m_emitterId );

		duration -= m_interval1;

		return duration;
	}
	//////////////////////////////////////////////////////////////////////////
	bool Emitter::getDimension( int & _w, int & _h ) const
	{
		if( Magic_Is3d( m_emitterId ) == false )
		{
			MAGIC_RECT rect;
			float backgroundScale = Magic_GetBackgroundRect( m_emitterId, &rect );

			(void)backgroundScale;

			if( rect.left != rect.right && rect.top != rect.bottom )
			{
				_w = rect.right - rect.left;
				_h = rect.bottom - rect.top;				
			}
			else
			{
				_w = 2048;
				_h = 2048;				
			}

			return true;
		}

		MAGIC_VIEW view;
		if( Magic_GetView( m_emitterId, &view ) != MAGIC_SUCCESS )
		{
			return false;
		}

		_w = view.viewport_width;
		_h = view.viewport_height;

		return true;
	}
	//////////////////////////////////////////////////////////////////////////
	bool Emitter::update( double _time )
	{
		if( Magic_Is3d( m_emitterId ) == true )
		{
			MAGIC_VIEW view;
			if( Magic_GetView( m_emitterId, &view ) != MAGIC_SUCCESS )
			{
				return false;
			}

			MAGIC_CAMERA camera;
			camera.pos = view.pos;
			camera.dir = view.dir;

			Magic_SetCamera( &camera );
		}

		double pos = this->getTime();

		double timing = 0.0;

		if( pos < _time )
		{
			timing = _time - pos;
		}
		else
		{
			Magic_Restart( m_emitterId );

			if( Magic_IsInterval1( m_emitterId ) == true )
			{
				///Magic_SetInterval1( m_id, m_leftBorder );
				Magic_EmitterToInterval1( m_emitterId, 1.f, NULL );
			}

			timing = _time;
		}

		float updateSpeed = Magic_GetUpdateSpeed( m_emitterId );

		timing /= double(updateSpeed);

		double rate = Magic_GetUpdateTime( m_emitterId );

		while( timing >= rate )
		{
			timing -= rate;
			bool restart = Magic_Update( m_emitterId, rate );

			if( restart == false )
			{
				break;
			}
		}

		return true;
	}
	//////////////////////////////////////////////////////////////////////////
	bool Emitter::flushEmitter( ParticleEmitterRenderFlush & _flush )
	{		
		if( Magic_InInterval( m_emitterId ) == false )
		{
			return true;
		}

		MAGIC_POSITION pos;
		Magic_GetEmitterPosition( m_emitterId, &pos );

		MAGIC_RENDERING_START start;
		void * context = Magic_PrepareRenderArrays( m_emitterId, &start, 100, MAGIC_ABGR, true );
		
		if( start.vertices == 0 || start.indexes == 0 )
		{
			return true;
		}

		_flush.vertices.resize( start.vertices );
		_flush.indices.resize( start.indexes );
		
		Vertex * vertex = &_flush.vertices[0];
		Indices * indices = &_flush.indices[0];

		for( int i = 0; i != start.arrays; ++i )
		{
			MAGIC_ARRAY_INFO array_info;
			Magic_GetRenderArrayData( context, i, &array_info );

			switch( array_info.type )
			{
			case MAGIC_VERTEX_FORMAT_INDEX:
				{
					Magic_SetRenderArrayData( context, i, indices, 0, sizeof( Indices ) );
				}break;
			case MAGIC_VERTEX_FORMAT_POSITION:
				{
					Magic_SetRenderArrayData( context, i, vertex, offsetof( Vertex, pos ), sizeof( Vertex ) );
				}break;
			case MAGIC_VERTEX_FORMAT_COLOR:
				{
					Magic_SetRenderArrayData( context, i, vertex, offsetof( Vertex, color ), sizeof( Vertex ) );
				}break;
			case MAGIC_VERTEX_FORMAT_UV:
				{
					if( array_info.index >= 8 )
					{
						return false;
					}

					Magic_SetRenderArrayData( context, i, vertex, offsetof( Vertex, uv ) + sizeof( mt::vec2f ) * array_info.index, sizeof( Vertex ) );
				}break;
			case MAGIC_VERTEX_FORMAT_NORMAL:
				{

				}break;
			case MAGIC_VERTEX_FORMAT_TANGENT:
				{

				}break;
			case MAGIC_VERTEX_FORMAT_BINORMAL:
				{

				}break;
			}
		}

		Magic_FillRenderArrays( context );

		MAGIC_RENDER_VERTICES vrts;
		while( Magic_GetVertices( context, &vrts ) == MAGIC_SUCCESS )
		{
			ParticleMesh mesh;

			MAGIC_RENDER_STATE state;
			while( Magic_GetNextRenderState( context, &state ) == MAGIC_SUCCESS )
			{
				mesh.texture[state.index] = state.value;
			}
			
			mesh.vertexCount = vrts.indexes_count;
			mesh.indexBegin = vrts.starting_index;
			mesh.primCount = vrts.indexes_count / 3;
			mesh.material = vrts.material;

			_flush.meshes.push_back( mesh );
		}

		return true;
	}
	//////////////////////////////////////////////////////////////////////////
	bool Emitter::getTransform( mt::mat4f & _projection, mt::mat4f & _view )
	{
		if( Magic_Is3d( m_emitterId ) == false )
		{
			mt::make_projection_ortho_lh_m4( _projection, 0.f, (FLOAT)m_width, 0.f, (FLOAT)m_height, -10000.f, 10000.f );
			mt::ident_m4( _view );
		}
		else
		{
			MAGIC_VIEW view;
			if( Magic_GetView( m_emitterId, &view ) != MAGIC_SUCCESS )
			{
				return false;
			}

			mt::make_projection_fov_m4( _projection, view.fov, view.aspect_ratio, view.znear, view.zfar );

			mt::vec3f view_pos = {view.pos.x, view.pos.y, view.pos.z};
			mt::vec3f view_dir = {view.dir.x, view.dir.y, view.dir.z};
			mt::vec3f view_up = {view.up.x, view.up.y, view.up.z};

			mt::make_lookat_m4( _view, view_pos, view_dir, view_up, 1.f );
		}

		return true;
	}
}