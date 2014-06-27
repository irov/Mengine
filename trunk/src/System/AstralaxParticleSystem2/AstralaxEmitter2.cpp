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
    bool AstralaxEmitter2::initialize( ServiceProviderInterface * _serviceProvider, HM_EMITTER _id, const ConstString & _containerName )
    {
        m_serviceProvider = _serviceProvider;
        m_emitterId = _id;

        m_containerName = _containerName;

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
			int typeCount = Magic_GetParticlesTypeCount( m_emitterId );

			for( int i = 0; i != typeCount; ++i )
			{
				Magic_LockParticlesType( m_emitterId, i );

				MAGIC_ORIENTATION orientation;
				orientation.orientation = MAGIC_ORIENTATION_Z;
				orientation.x = 0.f;
				orientation.y = 0.f;
				orientation.z = 0.f;

				Magic_SetOrientation( &orientation );

				Magic_UnlockParticlesType();
			}	

			MAGIC_RECT rect;
			float backgroundScale = Magic_GetBackgroundRect( m_emitterId, &rect );

			if( fabsf( backgroundScale - 1.f ) > 0.0001f )
			{
				LOGGER_ERROR(m_serviceProvider)("AstralaxEmitter::setupBasePosition_ %s background scale is not 1.f (%f if is zero, add background!) Please remove scale from source and re-export!"
					, m_containerName.c_str()
					, backgroundScale
					);

				return false;
			}

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

				m_width = rect.right - rect.left;
				m_height = rect.bottom - rect.top;

				m_background = true;
			}
		}
		else
		{
			MAGIC_VIEW view;
			if( Magic_GetView( m_emitterId, &view ) != MAGIC_SUCCESS )
			{
				return false;
			}

			m_width = view.viewport_width;
			m_height = view.viewport_height;
		}

        return true;
    }
	//////////////////////////////////////////////////////////////////////////
	const ConstString & AstralaxEmitter2::getContainerName() const
	{
		return m_containerName;
	}
	//////////////////////////////////////////////////////////////////////////
	static mt::box2f s_getEmitterBBox( HM_EMITTER _emitter )
	{
		mt::box2f box( mt::vec2f (0.f, 0.f), mt::vec2f(0.f, 0.f) );

		bool isFolder = Magic_IsFolder( _emitter );

		if( isFolder == true )
		{
			int count = Magic_GetEmitterCount( _emitter );

			for( int i = 0; i < count; ++i )
			{
				HM_EMITTER child = Magic_GetEmitter( _emitter, i );
				mt::box2f boxChild =  s_getEmitterBBox( child );
				box.minimum.x = std::min( box.minimum.x, boxChild.minimum.x );
				box.minimum.y = std::min( box.minimum.y, boxChild.minimum.y );
				box.maximum.x = std::max( box.maximum.x, boxChild.maximum.x );
				box.maximum.y = std::max( box.maximum.y, boxChild.maximum.y );
			}
		}
		else
		{
			MAGIC_BBOX magic_box;

			double duration = Magic_GetInterval2( _emitter );
			double timing = Magic_GetUpdateTime( _emitter );

			double curTime = Magic_GetInterval1( _emitter );
			//size_t height = 0;
			//size_t width = 0;

			Magic_Restart( _emitter );
			Magic_EmitterToInterval1( _emitter, 1, nullptr );

			while( curTime  <  duration )
			{
				Magic_SetPosition( _emitter, curTime );
				Magic_Update( _emitter, timing );
				Magic_RecalcBBox( _emitter );
				Magic_GetBBox( _emitter , &magic_box );
												
				box.minimum.x = std::min( box.minimum.x, magic_box.corner1.x );
				box.minimum.y = std::min( box.minimum.y, magic_box.corner1.y );
				box.maximum.x = std::max( box.maximum.x, magic_box.corner2.x );
				box.maximum.y = std::max( box.maximum.y, magic_box.corner2.y );
				
				curTime+=100;
			}

			Magic_Restart( _emitter );
			Magic_EmitterToInterval1( _emitter, 1, nullptr );
		}
		return box;
	}
	//////////////////////////////////////////////////////////////////////////
	void AstralaxEmitter2::getBoundingBox( mt::box2f& _box ) const
	{
		mt::box2f box = s_getEmitterBBox(m_emitterId);
		_box = box;
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
		//if ( m_start == false ) 
		{ 
			Magic_Restart( m_emitterId );

			if( Magic_IsInterval1( m_emitterId ) == true )
			{
				//Magic_SetInterval1( m_id, m_leftBorder );
				Magic_EmitterToInterval1( m_emitterId, 1.f, nullptr );
			}
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

		float aspect_ratio = float(view.viewport_width) / float(view.viewport_height) * view.aspect_factor;

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
		_camera.aspect = aspect_ratio;
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
			Magic_GetView( m_emitterId, &view );

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
	bool AstralaxEmitter2::changeEmitterImage( int _width, int _height, unsigned char* _data, int _bytes )
	{
		if( Magic_ChangeImage( m_emitterId, -1, _width, _height, _data, _bytes ) == MAGIC_ERROR )
		{
			return false;
		}

		return true;
	}
	//////////////////////////////////////////////////////////////////////////
	bool AstralaxEmitter2::changeEmitterModel( float * _points, int _count )
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
		m_angle = _radians * 180.f / 3.1415926f;

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
	static void s_fillParticles_( ParticleVertices * _particles, size_t _offset, size_t _count, const mt::mat4f & _vpm, float _width, float _height )
	{
		for( size_t i = 0; i != _count; ++i )
		{
			MAGIC_PARTICLE_VERTEXES vertexes;
			Magic_GetNextParticleVertexes( &vertexes );

			ParticleVertices & rp = _particles[_offset + i];

			mt::vec4f v0;
			v0.x = vertexes.vertex1.x;
			v0.y = vertexes.vertex1.y;
			v0.z = vertexes.vertex1.z;
			v0.w = 0.f;

			mt::vec4f v0_vpm;
			mt::mul_v4_m4( v0_vpm, v0, _vpm );

			rp.v[0].x = (v0_vpm.x / v0_vpm.w + 1.f) * _width * 0.5f;
			rp.v[0].y = (1.f - v0_vpm.y / v0_vpm.w) * _height * 0.5f;
			rp.v[0].z = v0_vpm.z / v0_vpm.w;
			
			mt::vec4f v1;
			v1.x = vertexes.vertex2.x;
			v1.y = vertexes.vertex2.y;
			v1.z = vertexes.vertex2.z;
			v1.w = 0.f;

			mt::vec4f v1_vpm;
			mt::mul_v4_m4( v1_vpm, v1, _vpm );

			rp.v[1].x = (v1_vpm.x / v1_vpm.w + 1.f) * _width * 0.5f;
			rp.v[1].y = (1.f - v1_vpm.y / v1_vpm.w) * _height * 0.5f;
			rp.v[1].z = v1_vpm.z / v1_vpm.w;

			mt::vec4f v2;
			v2.x = vertexes.vertex3.x;
			v2.y = vertexes.vertex3.y;
			v2.z = vertexes.vertex3.z;
			v2.w = 0.f;

			mt::vec4f v2_vpm;
			mt::mul_v4_m4( v2_vpm, v2, _vpm );
			
			rp.v[2].x = (v2_vpm.x / v2_vpm.w + 1.f) * _width * 0.5f;
			rp.v[2].y = (1.f - v2_vpm.y / v2_vpm.w) * _height * 0.5f;
			rp.v[2].z = v2_vpm.z / v2_vpm.w;

			mt::vec4f v3;
			v3.x = vertexes.vertex4.x;
			v3.y = vertexes.vertex4.y;
			v3.z = vertexes.vertex4.z;
			v3.w = 0.f;

			mt::vec4f v3_vpm;
			mt::mul_v4_m4( v3_vpm, v3, _vpm );

			rp.v[3].x = (v3_vpm.x / v3_vpm.w + 1.0f) * _width * 0.5f;
			rp.v[3].y = (1.f - v3_vpm.y / v3_vpm.w) * _height * 0.5f;
			rp.v[3].z = v3_vpm.z / v3_vpm.w;

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
	bool AstralaxEmitter2::flushParticles( ParticleMesh * _meshes, size_t _meshLimit, ParticleVertices * _particles, size_t _particlesLimit, ParticleEmitterRenderFlush & _flush )
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
			mt::make_lookat_m4( vm, pc.pos, pc.dir, pc.up, 1.f );

			mt::mat4f pm;
			mt::make_projection_fov_m4( pm, pc.fov, pc.aspect, pc.znear, pc.zfar );

			mt::mul_m4_m4( vpm, vm, pm );
		}
		else
		{
			mt::mat4f vm;
			mt::ident_m4( vm );
		
			mt::mat4f pm;
			mt::make_projection_ortho_lh_m4( pm, 0.f, m_width, m_height, 0.f, -1000.f, 1000.f );

			mt::mul_m4_m4( vpm, vm, pm );
		}
		
		while( rendering.count != 0 )
		{
			if( _particlesLimit <= _flush.particleCount + rendering.count || 
				_meshLimit <= _flush.meshCount )
			{
				return false;
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