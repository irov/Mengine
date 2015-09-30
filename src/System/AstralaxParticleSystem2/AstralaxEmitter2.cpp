#	include "AstralaxEmitter2.h"

#	include <Utils/Logger/Logger.h>

namespace Menge
{
	//////////////////////////////////////////////////////////////////////////
	AstralaxEmitter2::AstralaxEmitter2()
		: m_serviceProvider( nullptr )
		, m_emitterId( 0 )
		, m_updateSpeed( 0.f )
		, m_leftBorder( 0.0 )
		, m_rightBorder( 0.0 )
		, m_time( 0.0 )
		, m_rect( 0.f, 0.f, 0.f, 0.f )
		, m_angle( 0.f )
		, m_start( false )
		, m_looped( false )
		, m_background( false )
	{
	}
	//////////////////////////////////////////////////////////////////////////
	AstralaxEmitter2::~AstralaxEmitter2()
	{
	}
    //////////////////////////////////////////////////////////////////////////
    bool AstralaxEmitter2::initialize( ServiceProviderInterface * _serviceProvider, HM_EMITTER _id )
    {
        m_serviceProvider = _serviceProvider;
        m_emitterId = _id;

        Magic_SetRandomMode( m_emitterId, false );

        // set interpolation
        Magic_SetInterpolationMode( m_emitterId, true );

		m_updateSpeed = Magic_GetUpdateSpeed( m_emitterId );
        m_leftBorder = Magic_GetInterval1( m_emitterId );
        m_rightBorder = Magic_GetInterval2( m_emitterId );

        if( this->setupBasePosition_() == false )
        {
            return false;
        }
               
        return true;
    }
	//////////////////////////////////////////////////////////////////////////
	void AstralaxEmitter2::finalize()
	{
		Magic_UnloadEmitter( m_emitterId );

		m_emitterId = 0;
	}
    //////////////////////////////////////////////////////////////////////////
    bool AstralaxEmitter2::setupBasePosition_()
    {
		bool is3d = Magic_Is3d( m_emitterId );

		if( is3d == false )
		{
			MAGIC_RECT rect;
			float backgroundScale = Magic_GetBackgroundRect( m_emitterId, &rect );

			if( rect.left == rect.right || rect.bottom == rect.top )
			{
				m_rect.x = -1024.f;
				m_rect.y = -1024.f;
				m_rect.z = 1024.f;
				m_rect.w = 1024.f;

				m_background = false;
			}
			else
			{
				if( mt::equal_f_f( backgroundScale, 1.f ) == false )
				{
					LOGGER_ERROR(m_serviceProvider)("AstralaxEmitter::setupBasePosition_ background scale is not 1.f (%f if is zero, add background!) Please remove scale from source and re-export!"
						, backgroundScale
						);

					return false;
				}

				m_rect.x = (float)rect.left;
				m_rect.y = (float)rect.top;
				m_rect.z = (float)rect.right;
				m_rect.w = (float)rect.bottom;

				m_background = true;
			}
		}
		else
		{
			MAGIC_POSITION pos;
			Magic_GetEmitterPosition( m_emitterId, &pos );

			MAGIC_VIEW view;
			if( Magic_GetView( m_emitterId, &view ) != MAGIC_SUCCESS )
			{
				return false;
			}
			
			m_rect.x = (float)0.f;
			m_rect.y = (float)0.f;
			m_rect.z = (float)view.viewport_width;
			m_rect.w = (float)view.viewport_height;

			m_background = true;
		}

        return true;
    }
	//////////////////////////////////////////////////////////////////////////
	void AstralaxEmitter2::getBoundingBox( mt::box2f& _box ) const
	{
		(void)_box;
	}
	//////////////////////////////////////////////////////////////////////////
	void AstralaxEmitter2::setLeftBorder( float _leftBorder )
	{
		m_leftBorder = (double)_leftBorder;

		Magic_SetInterval1( m_emitterId, m_leftBorder );
	}
	//////////////////////////////////////////////////////////////////////////
	void AstralaxEmitter2::play()
	{
		Magic_Restart( m_emitterId );

		if( Magic_IsInterval1( m_emitterId ) == true )
		{
			//Magic_SetInterval1( m_id, m_leftBorder );
			Magic_EmitterToInterval1( m_emitterId, 1.f, nullptr );
		}

        m_time = 0.0;

		m_start = true;
	}
	//////////////////////////////////////////////////////////////////////////
	void AstralaxEmitter2::setLoop( bool _loop )
	{
		Magic_SetLoopMode( m_emitterId, _loop? 1: 0 );
	}
	//////////////////////////////////////////////////////////////////////////
	bool AstralaxEmitter2::getLoop() const
	{
        int loopMode = Magic_GetLoopMode( m_emitterId );

		return loopMode == 1;
	}
	//////////////////////////////////////////////////////////////////////////
	void AstralaxEmitter2::interrupt()
	{
		if( Magic_IsInterrupt( m_emitterId ) == true )
		{
			return;
		}

		Magic_Update( m_emitterId, 0.f );
		Magic_SetLoopMode( m_emitterId, 0 );
		Magic_SetInterrupt( m_emitterId, true );
	}
	//////////////////////////////////////////////////////////////////////////
	bool AstralaxEmitter2::is3d() const
	{
		bool is3d = Magic_Is3d( m_emitterId );

		return is3d;
	}
	//////////////////////////////////////////////////////////////////////////
	bool AstralaxEmitter2::getCamera( ParticleCamera & _camera ) const
	{
		MAGIC_VIEW view;
		int user_camera = Magic_GetView( m_emitterId, &view );
				
		if( user_camera != MAGIC_SUCCESS )
		{
			return false;
		}

		_camera.pos.x = view.pos.x;
		_camera.pos.y = view.pos.y;
		_camera.pos.z = view.pos.z;

		_camera.dir.x = view.dir.x;
		_camera.dir.y = view.dir.y;
		_camera.dir.z = view.dir.z;

		_camera.up.x = view.up.x;
		_camera.up.y = view.up.y;
		_camera.up.z = view.up.z;

		_camera.fov = view.fov;
		_camera.aspect = view.aspect_ratio;
		_camera.znear = view.znear;
		_camera.zfar = view.zfar;

		_camera.width = (float)view.viewport_width;
		_camera.height = (float)view.viewport_height;
		
		return true;
	}
	//////////////////////////////////////////////////////////////////////////
	void AstralaxEmitter2::stop()
	{
		m_start = false;

		Magic_Stop( m_emitterId );
	}
	//////////////////////////////////////////////////////////////////////////
	void AstralaxEmitter2::pause()
	{
		m_start = false;
	}
	//////////////////////////////////////////////////////////////////////////
	void AstralaxEmitter2::resume()
	{
		m_start = true;
	}
	//////////////////////////////////////////////////////////////////////////
	bool AstralaxEmitter2::update( float _timing, bool & _stop )
	{
		if( m_start == false )
		{
			return false;
		}

        m_time += _timing;

		bool is3d = this->is3d();

		if( is3d == true )
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

		float total_timing = _timing / m_updateSpeed;
		
		bool restart = Magic_Update( m_emitterId, total_timing );

        if( restart == false )
        { 
            m_start = false;

			_stop = true;
        }
		else
		{
			_stop = false;
		}

        return true;
	}
	//////////////////////////////////////////////////////////////////////////
	HM_EMITTER AstralaxEmitter2::getId() const
	{
		return m_emitterId;
	}
	//////////////////////////////////////////////////////////////////////////
	bool AstralaxEmitter2::inInterval() const
	{
		if( m_emitterId == 0 )
		{
			return false;				 
		}

		bool result = Magic_InInterval( m_emitterId );
		
		return result;
	}
	//////////////////////////////////////////////////////////////////////////
	void AstralaxEmitter2::setEmitterTranslateWithParticle( bool _value )
	{
		Magic_SetEmitterPositionMode( m_emitterId, _value );
	}
	//////////////////////////////////////////////////////////////////////////
	bool AstralaxEmitter2::changeEmitterImage( uint32_t _width, uint32_t _height, unsigned char * _data, size_t _bytes )
	{
		if( Magic_ChangeImage( m_emitterId, -1, (int)_width, (int)_height, _data, (int)_bytes ) == MAGIC_ERROR )
		{
			return false;
		}

		return true;
	}
	//////////////////////////////////////////////////////////////////////////
	bool AstralaxEmitter2::changeEmitterModel( float * _points, uint32_t _count )
	{
		MAGIC_TRIANGLE * triangle = reinterpret_cast<MAGIC_TRIANGLE *>(_points);

		if( Magic_ChangeModel( m_emitterId, -1, _count, triangle ) == MAGIC_ERROR )
		{
			return false;
		}

		return true;
	}
	//////////////////////////////////////////////////////////////////////////
	void AstralaxEmitter2::setPosition( const mt::vec3f & _pos )
	{
		MAGIC_POSITION pos;
		pos.x = _pos.x;
		pos.y = _pos.y;
        pos.z = _pos.z;

		Magic_SetEmitterPosition( m_emitterId, &pos );
	}  
	//////////////////////////////////////////////////////////////////////////
	void AstralaxEmitter2::getPosition( mt::vec3f & _pos )
	{
		MAGIC_POSITION pos;
		Magic_GetEmitterPosition( m_emitterId, &pos );

		_pos.x = pos.x;
		_pos.y = pos.y;
        _pos.z = pos.z;
	}
	//////////////////////////////////////////////////////////////////////////
	void AstralaxEmitter2::setScale( float _scale )
	{
		Magic_SetScale( m_emitterId, _scale );
	}
	//////////////////////////////////////////////////////////////////////////
	void AstralaxEmitter2::restart()
	{
		Magic_Restart( m_emitterId );
		if( Magic_IsInterval1( m_emitterId ) == true )
		{
			//Magic_SetInterval1( m_id, m_leftBorder );
			Magic_EmitterToInterval1( m_emitterId, 1.f, nullptr );
		}
	}
	//////////////////////////////////////////////////////////////////////////
	void AstralaxEmitter2::setAngle( float _radians )
	{
		m_angle = _radians / mt::m_pi * 180.f;

		Magic_SetDiagramAddition( m_emitterId, MAGIC_DIAGRAM_DIRECTION, -1, m_angle );
		int k_par = Magic_GetParticlesTypeCount( m_emitterId );
		for( int j = 0; j < k_par; j++ )
		{
			Magic_SetDiagramAddition( m_emitterId, MAGIC_DIAGRAM_DIRECTION, j, m_angle );
		}
	}
    //////////////////////////////////////////////////////////////////////////
    bool AstralaxEmitter2::getBackgroundBox( mt::box2f & _box )
    {
		if( Magic_Is3d( m_emitterId ) == false )
		{        
			MAGIC_RECT rect;
			if( Magic_GetBackgroundRect( m_emitterId, &rect ) == MAGIC_ERROR )
			{
				return false;
			}

			if( rect.left == rect.right || rect.top == rect.bottom )
			{
				return false;
			}

			_box.minimum.x = (float)rect.left;
			_box.minimum.y = (float)rect.top;

			_box.maximum.x = (float)rect.right;
			_box.maximum.y = (float)rect.bottom;
		}
		else
		{
			MAGIC_VIEW view;
			if( Magic_GetView( m_emitterId, &view ) != MAGIC_SUCCESS )
			{
				return false;
			}

			_box.minimum.x = 0.f;
			_box.minimum.y = 0.f;

			_box.maximum.x = (float)view.viewport_width;
			_box.maximum.y = (float)view.viewport_height;
		}

        return true;
    }
	//////////////////////////////////////////////////////////////////////////
	float AstralaxEmitter2::getLeftBorder() const
	{
        float float_leftBorder = (float)m_leftBorder;

		return float_leftBorder;
	}
	//////////////////////////////////////////////////////////////////////////
	float AstralaxEmitter2::getRightBorder() const
	{
        float float_rightBorder = (float)m_rightBorder;

		return float_rightBorder;
	}
	//////////////////////////////////////////////////////////////////////////
	float AstralaxEmitter2::getDuration() const
	{
		double duration = (m_rightBorder - m_leftBorder);

        float float_duration = (float)duration;

		return float_duration;
	}
	//////////////////////////////////////////////////////////////////////////
	void AstralaxEmitter2::seek( float _time )
	{
		Magic_Restart( m_emitterId );

		float duration = this->getDuration();

		if( _time >= duration )
		{	
			m_time = (double)duration;

			return;
		}

		m_time = (double)_time;

		if( Magic_IsInterval1( m_emitterId ) == true )
		{
			Magic_EmitterToInterval1( m_emitterId, 1.f, nullptr );
		}

		double rate = Magic_GetUpdateTime( m_emitterId );
		
		double time = _time;

		while( time >= rate )
		{
			time -= rate;

			if( Magic_Update( m_emitterId, rate ) == false )
			{
				return;
			}
		}

		Magic_Update( m_emitterId, time );	
	}
	//////////////////////////////////////////////////////////////////////////
	float AstralaxEmitter2::getUpdateTemp() const
	{
		float tempScale = Magic_GetUpdateSpeed( m_emitterId );

        return tempScale;
	}
	//////////////////////////////////////////////////////////////////////////
	void AstralaxEmitter2::setRandomMode( bool _randomMode )
	{
		Magic_SetRandomMode( m_emitterId, _randomMode );
	}
	//////////////////////////////////////////////////////////////////////////
	bool AstralaxEmitter2::getRandomMode() const
	{
		bool mode = Magic_IsRandomMode( m_emitterId );

		return mode;
	}
	//////////////////////////////////////////////////////////////////////////
	bool AstralaxEmitter2::isBackground() const
	{
		return m_background;
	}
	//////////////////////////////////////////////////////////////////////////
	bool AstralaxEmitter2::prepareParticles( ParticleEmitterRenderFlush & _flush )
	{
		_flush.meshCount = 0;
		_flush.vertexCount = 0;
		_flush.indexCount = 0;
		_flush.arrays = 0;
		_flush.context = nullptr;

		if( this->inInterval() == false )
		{
			return false;
		}

		MAGIC_RENDERING_START start;
		void * context = Magic_PrepareRenderArrays( m_emitterId, &start, 100, MAGIC_ABGR );

		if( start.vertices == 0 || start.indexes == 0 )
		{
			return false;
		}

		_flush.vertexCount = start.vertices;
		_flush.indexCount = start.indexes;
		_flush.arrays = start.arrays;
		_flush.context = context;

		return true;
	}
	//////////////////////////////////////////////////////////////////////////
	bool AstralaxEmitter2::flushParticles( ParticleMesh * _meshes, uint32_t _meshLimit, RenderVertex2D * _vertices, RenderIndices * _indices, ParticleEmitterRenderFlush & _flush )
	{
		for( uint32_t i = 0; i != _flush.arrays; ++i )
		{
			MAGIC_ARRAY_INFO array_info;
			Magic_GetRenderArrayData( _flush.context, i, &array_info );

			switch( array_info.type )
			{
			case MAGIC_VERTEX_FORMAT_INDEX:
				{
					Magic_SetRenderArrayData( _flush.context, i, _indices, 0, sizeof( RenderIndices ) );
				}break;
			case MAGIC_VERTEX_FORMAT_POSITION:
				{
					Magic_SetRenderArrayData( _flush.context, i, _vertices, offsetof( RenderVertex2D, pos ), sizeof( RenderVertex2D ) );
				}break;
			case MAGIC_VERTEX_FORMAT_COLOR:
				{
					Magic_SetRenderArrayData( _flush.context, i, _vertices, offsetof( RenderVertex2D, color ), sizeof( RenderVertex2D ) );
				}break;
			case MAGIC_VERTEX_FORMAT_UV:
				{
					if( array_info.index >= MENGINE_RENDER_VERTEX_UV_COUNT )
					{
						return false;
					}

					Magic_SetRenderArrayData( _flush.context, i, _vertices, offsetof( RenderVertex2D, uv ) + sizeof( mt::vec2f ) * array_info.index, sizeof( RenderVertex2D ) );
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

		Magic_FillRenderArrays( _flush.context );

		uint32_t indexOffset = 0;

		MAGIC_RENDER_VERTICES vrts;
		while( Magic_GetVertices( _flush.context, &vrts ) == true )
		{
			if( _flush.meshCount >= _meshLimit )
			{
				return false;
			}

			ParticleMesh & mesh = _meshes[_flush.meshCount];

			mesh.vertexOffset = indexOffset * 4 / 6;
			mesh.vertexCount = vrts.vertices * 4 / 6;
			mesh.indexOffset = indexOffset;
			mesh.indexCount = vrts.vertices;

			RenderIndices vertexOffset = indexOffset * 4 / 6;

			if( vertexOffset != 0 )
			{
				for( RenderIndices
					*it = _indices + mesh.indexOffset,
					*it_end = _indices + mesh.indexOffset + mesh.indexCount;
				it != it_end;
				++it )
				{
					RenderIndices & indices = *it;

					indices -= vertexOffset;
				}
			}

			MAGIC_RENDER_STATE state;
			while( Magic_GetNextRenderState( _flush.context, &state ) == MAGIC_SUCCESS )
			{
				if( state.state != MAGIC_RENDER_STATE_TEXTURE )
				{
					return false;
				}

				if( state.index == -1 )
				{
					return false;
				}

				mesh.texture[state.index] = state.value;
			}

			mesh.material = vrts.material;

			MAGIC_MATERIAL m;
			if( Magic_GetMaterial( vrts.material, &m ) != MAGIC_SUCCESS )
			{
				return false;
			}
						
			mesh.textures = m.textures;

			_flush.meshCount++;

			indexOffset += vrts.vertices;
		}
		
		mt::mat4f vpm;

		bool is3d = this->is3d();

		if( is3d == true )
		{
			ParticleCamera pc;
			if( this->getCamera( pc ) == false )
			{
				return false;
			}

			mt::mat4f vm;
			mt::make_lookat_m4( vm, pc.pos, pc.dir, -pc.up, -1.f );

			mt::mat4f pm;
			mt::make_projection_fov_m4( pm, pc.fov, pc.aspect, pc.znear, pc.zfar );

			mt::mul_m4_m4( vpm, vm, pm );
		}
		else
		{
			mt::mat4f vm;
			mt::ident_m4( vm );
		
			mt::mat4f pm;
			mt::make_projection_ortho_lh_m4( pm, m_rect.x, m_rect.z, m_rect.y, m_rect.w, -1000.f, 1000.f );

			mt::mul_m4_m4( vpm, vm, pm );
		}

		float half_width = (m_rect.z - m_rect.x) * 0.5f;
		float half_height = (m_rect.w - m_rect.y) * 0.5f;
		
		for( uint32_t i = 0; i != _flush.vertexCount; ++i )
		{
			RenderVertex2D & v = _vertices[i];

			mt::vec3f v_vpm;
			mt::mul_v3_v3_m4_homogenize( v_vpm, v.pos, vpm );

			v.pos.x = (1.f + v_vpm.x) * half_width;
			v.pos.y = (1.f + v_vpm.y) * half_height;
			v.pos.z = v_vpm.z;
		}

		return true;
	}
}