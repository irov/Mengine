#	include "AstralaxEmitter2.h"

#	include <Utils/Logger/Logger.h>

namespace Menge
{
	//////////////////////////////////////////////////////////////////////////
	AstralaxEmitter2::AstralaxEmitter2()
		: m_serviceProvider(nullptr)
        , m_emitterId(0)
		, m_start(false)
		, m_leftBorder(0.f)
		, m_rightBorder(0.f)
        , m_rate(0.f)
		, m_total_rate(0.f)
        , m_time(0.f)
		, m_looped(false)
		, m_angle(0.f)
		, m_tempScale(1)
        , m_basePosition(0.f, 0.f, 0.f)
		, m_background(false)
		, m_width(0.f)
		, m_height(0.f)
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

        m_leftBorder = Magic_GetInterval1( m_emitterId );
        m_rightBorder = Magic_GetInterval2( m_emitterId );

        m_rate = Magic_GetUpdateTime( m_emitterId );

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
		if( Magic_Is3d( m_emitterId ) == false )
		{
			MAGIC_RECT rect;
			float backgroundScale = Magic_GetBackgroundRect( m_emitterId, &rect );

			if( rect.left == rect.right || rect.bottom == rect.top )
			{
				MAGIC_POSITION pos;
				pos.x = 0.f;
				pos.y = 0.f;
				pos.z = 0.f;

				m_basePosition.x = 0.f;
				m_basePosition.y = 0.f;
				m_basePosition.z = 0.f;

				Magic_SetEmitterPosition( m_emitterId, &pos );

				m_width = 2048.f;
				m_height = 2048.f;

				m_background = false;
			}
			else
			{
				if( mt::cmp_f_f( backgroundScale, 1.f ) == false )
				{
					LOGGER_ERROR(m_serviceProvider)("AstralaxEmitter::setupBasePosition_ background scale is not 1.f (%f if is zero, add background!) Please remove scale from source and re-export!"
						, backgroundScale
						);

					return false;
				}

				MAGIC_POSITION pos;
				Magic_GetEmitterPosition( m_emitterId, &pos );

				MAGIC_POSITION adapt_pos;
				adapt_pos.x = pos.x - (float)rect.left;
				adapt_pos.y = pos.y - (float)rect.top;
				adapt_pos.z = pos.z;

				Magic_SetEmitterPosition( m_emitterId, &adapt_pos );

				m_basePosition.x = adapt_pos.x;
				m_basePosition.y = adapt_pos.y;
				m_basePosition.z = adapt_pos.z;

				m_width = (float)(rect.right - rect.left);
				m_height = (float)(rect.bottom - rect.top);

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

			m_width = (float)view.viewport_width;
			m_height = (float)view.viewport_height;

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

		m_total_rate = 0.0;
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

		if( user_camera != MAGIC_SUCCESS )
		{
			return false;
		}

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

		m_total_rate += _timing;
        m_time += _timing;

		if( this->is3d() == true )
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
		
		bool restart = Magic_Update( m_emitterId, m_total_rate );
		m_total_rate = 0.f;

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
	bool AstralaxEmitter2::createFirstRenderedParticlesList( MAGIC_RENDERING * _rendering )
	{
		if( m_emitterId == 0 )
		{
			return false;				 
		}
		
		if( Magic_CreateFirstRenderedParticlesList( m_emitterId, _rendering ) != MAGIC_SUCCESS )
		{
			return false;
		}

		return true;
	}
	//////////////////////////////////////////////////////////////////////////
	void AstralaxEmitter2::setEmitterTranslateWithParticle( bool _value )
	{
		Magic_SetEmitterPositionMode( m_emitterId, _value );
	}
	//////////////////////////////////////////////////////////////////////////
	bool AstralaxEmitter2::isIntensive() const
	{
		bool intensive = Magic_IsIntensive();

		return intensive;
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
    void AstralaxEmitter2::getBasePosition( mt::vec3f & _pos )
    {
        _pos = m_basePosition;
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
		double duration = (m_rightBorder - m_leftBorder) / m_tempScale;

        float float_duration = (float)duration;

		return float_duration;
	}
	//////////////////////////////////////////////////////////////////////////
	void AstralaxEmitter2::seek( float _time )
	{
		double rate = Magic_GetUpdateTime( m_emitterId );
		bool randomMode = Magic_IsRandomMode(m_emitterId);
		Magic_SetRandomMode( m_emitterId, false );

		double currentTime = Magic_GetPosition( m_emitterId );
		double delta = currentTime - m_total_rate;
		delta = _time;

		this->restart();

		while( delta >= rate )
		{
			delta -= rate;
			bool isCanUpdate = Magic_Update( m_emitterId,rate );
			if( !isCanUpdate )
			{
				restart();
			}
		}
		
		Magic_SetRandomMode( m_emitterId, randomMode );

		m_total_rate = _time;
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
	static void s_fillParticles_( ParticleVertices * _particles, uint32_t _offset, uint32_t _count, const mt::mat4f & _vpm, float _width, float _height )
	{
		for( uint32_t i = 0; i != _count; ++i )
		{
			MAGIC_PARTICLE_VERTEXES vertexes;
			Magic_GetNextParticleVertexes( &vertexes );

			ParticleVertices & rp = _particles[_offset + i];

			mt::vec3f v0;
			v0.x = vertexes.vertex1.x;
			v0.y = vertexes.vertex1.y;
			v0.z = vertexes.vertex1.z;

			mt::vec3f v0_vpm;
			mt::mul_v3_v3_m4_homogenize( v0_vpm, v0, _vpm );

			rp.v[0].x = (1.f + v0_vpm.x) * _width * 0.5f;
			rp.v[0].y = (1.f + v0_vpm.y) * _height * 0.5f;
			rp.v[0].z = v0_vpm.z;
			
			mt::vec3f v1;
			v1.x = vertexes.vertex2.x;
			v1.y = vertexes.vertex2.y;
			v1.z = vertexes.vertex2.z;

			mt::vec3f v1_vpm;
			mt::mul_v3_v3_m4_homogenize( v1_vpm, v1, _vpm );

			rp.v[1].x = (1.f + v1_vpm.x) * _width * 0.5f;
			rp.v[1].y = (1.f + v1_vpm.y) * _height * 0.5f;
			rp.v[1].z = v1_vpm.z;

			mt::vec3f v2;
			v2.x = vertexes.vertex3.x;
			v2.y = vertexes.vertex3.y;
			v2.z = vertexes.vertex3.z;

			mt::vec3f v2_vpm;
			mt::mul_v3_v3_m4_homogenize( v2_vpm, v2, _vpm );
			
			rp.v[2].x = (1.f + v2_vpm.x) * _width * 0.5f;
			rp.v[2].y = (1.f + v2_vpm.y) * _height * 0.5f;
			rp.v[2].z = v2_vpm.z;

			mt::vec3f v3;
			v3.x = vertexes.vertex4.x;
			v3.y = vertexes.vertex4.y;
			v3.z = vertexes.vertex4.z;

			mt::vec3f v3_vpm;
			mt::mul_v3_v3_m4_homogenize( v3_vpm, v3, _vpm );

			rp.v[3].x = (1.f + v3_vpm.x) * _width * 0.5f;
			rp.v[3].y = (1.f + v3_vpm.y) * _height * 0.5f;
			rp.v[3].z = v3_vpm.z;

			rp.uv[0].x = vertexes.u1;
			rp.uv[0].y = vertexes.v1;
			rp.uv[1].x = vertexes.u2;
			rp.uv[1].y = vertexes.v2;
			rp.uv[2].x = vertexes.u3;
			rp.uv[2].y = vertexes.v3;
			rp.uv[3].x = vertexes.u4;
			rp.uv[3].y = vertexes.v4;

			rp.color = vertexes.color;
		}
	}
	//////////////////////////////////////////////////////////////////////////
	bool AstralaxEmitter2::flushParticles( ParticleMesh * _meshes, uint32_t _meshLimit, ParticleVertices * _particles, uint32_t _particlesLimit, ParticleEmitterRenderFlush & _flush )
	{
		_flush.particleCount = 0;
		_flush.meshCount = 0;

		if( this->inInterval() == false )
		{
			return false;
		}

		MAGIC_RENDERING rendering;
		if( this->createFirstRenderedParticlesList( &rendering ) == false )
		{
			return false;
		}
		
		mt::mat4f vpm;

		if( this->is3d() == true )
		{
			ParticleCamera pc;
			this->getCamera( pc );

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
			mt::make_projection_ortho_lh_m4( pm, m_width * -0.5f, m_width * 0.5f, m_height * -0.5f, m_height * 0.5f, -1000.f, 1000.f );

			mt::mul_m4_m4( vpm, vm, pm );
		}
		
		while( rendering.count != 0 )
		{
			if( _particlesLimit <= _flush.particleCount + rendering.count || 
				_meshLimit <= _flush.meshCount )
			{
				return true;
			}

			ParticleMesh & mesh = _meshes[_flush.meshCount];

			mesh.begin = _flush.particleCount;
			mesh.size = rendering.count;
			mesh.texture = rendering.texture_id;
			mesh.intense = rendering.intense;

			s_fillParticles_( _particles, _flush.particleCount, mesh.size, vpm, m_width, m_height );

			_flush.particleCount += rendering.count;
			++_flush.meshCount;

			Magic_CreateNextRenderedParticlesList( &rendering );
		}

		return true;
	}
}