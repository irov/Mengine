#	include "AstralaxEmitter.h"
#	include "AstralaxEmitterContainer.h"

#	include <Utils/Logger/Logger.h>

namespace Menge
{
	//////////////////////////////////////////////////////////////////////////
	AstralaxEmitter::AstralaxEmitter()
		: m_serviceProvider(nullptr)
        , m_container(nullptr)
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
	AstralaxEmitter::~AstralaxEmitter()
	{
	}
    //////////////////////////////////////////////////////////////////////////
    bool AstralaxEmitter::initialize( ServiceProviderInterface * _serviceProvider, const AstralaxEmitterContainerPtr & _container, HM_EMITTER _id, const char * _emitterName )
    {
        m_serviceProvider = _serviceProvider;
        m_container = _container;
        m_emitterId = _id;
        
		strcpy( m_emitterName, _emitterName );

        m_typesCount = Magic_GetParticlesTypeCount( m_emitterId );

        if( m_typesCount >= 20 )
        {
            return false;
        }

        for( int i = 0; i != m_typesCount; ++i )
        {
            m_factor[i] = Magic_GetDiagramFactor( m_emitterId, i, MAGIC_DIAGRAM_NUMBER );
        }

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
	void AstralaxEmitter::finalize()
	{
		m_container->releaseEmitterId( m_emitterName, m_emitterId );
		m_container = nullptr;

		m_emitterId = 0;
	}
    //////////////////////////////////////////////////////////////////////////
    bool AstralaxEmitter::setupBasePosition_()
    {
		MAGIC_RECT rect;
		float backgroundScale = Magic_GetBackgroundRect( m_emitterId, &rect );

        if( rect.left == rect.right || rect.bottom == rect.top )
        {
            MAGIC_POSITION pos;
            pos.x = 0.f;
            pos.y = 0.f;

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
			if( mt::equal_f_f( backgroundScale, 1.f ) == false )
			{
				LOGGER_ERROR(m_serviceProvider)("AstralaxEmitter::setupBasePosition_ %s background scale is not 1.f (%f if is zero, add background!) Please remove scale from source and re-export!"
					, m_emitterName
					, backgroundScale
					);

				return false;
			}

            MAGIC_POSITION pos;
            Magic_GetEmitterPosition( m_emitterId, &pos );

			MAGIC_POSITION adapt_pos;
			adapt_pos.x = pos.x - (float)rect.left;
			adapt_pos.y = pos.y - (float)rect.top;

			Magic_SetEmitterPosition( m_emitterId, &adapt_pos );

			m_basePosition.x = adapt_pos.x;
			m_basePosition.y = adapt_pos.y;
			m_basePosition.z = 0.f;

			m_background = true;
        }

        return true;
    }
	//////////////////////////////////////////////////////////////////////////
	const AstralaxEmitterContainerPtr & AstralaxEmitter::getContainer() const
	{
		return m_container;
	}
	//////////////////////////////////////////////////////////////////////////
	static mt::box2f s_getEmitterBBox( HM_EMITTER _emitter )
	{
		mt::box2f box(mt::vec2f(0.f, 0.f), mt::vec2f(0.f, 0.f));

		bool isFolder = Magic_IsFolder( _emitter );

		if( isFolder )
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
	void AstralaxEmitter::getBoundingBox( mt::box2f& _box ) const
	{
		mt::box2f box = s_getEmitterBBox(m_emitterId);
		_box = box;
	}
	//////////////////////////////////////////////////////////////////////////
	void AstralaxEmitter::setLeftBorder( float _leftBorder )
	{
		m_leftBorder = (double)_leftBorder;

		Magic_SetInterval1( m_emitterId, m_leftBorder );
	}
	//////////////////////////////////////////////////////////////////////////
	void AstralaxEmitter::play()
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

		for( int i = 0; i < m_typesCount; ++i )
		{
			Magic_SetDiagramFactor( m_emitterId, i, MAGIC_DIAGRAM_NUMBER, m_factor[i] );
		}

		m_total_rate = 0.0;
        m_time = 0.0;

		m_start = true;
	}
	//////////////////////////////////////////////////////////////////////////
	void AstralaxEmitter::setLoop( bool _loop )
	{
		Magic_SetLoopMode( m_emitterId, _loop? 1: 0 );
	}
	//////////////////////////////////////////////////////////////////////////
	bool AstralaxEmitter::getLoop() const
	{
        int loopMode = Magic_GetLoopMode( m_emitterId );

		return loopMode == 1;
	}
	//////////////////////////////////////////////////////////////////////////
	void AstralaxEmitter::interrupt()
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
	bool AstralaxEmitter::is3d() const
	{
		return false;
	}
	//////////////////////////////////////////////////////////////////////////
	bool AstralaxEmitter::getCamera( ParticleCamera & _camera ) const
	{
		(void)_camera;

		return false;
	}
	//////////////////////////////////////////////////////////////////////////
	void AstralaxEmitter::stop()
	{
		m_start = false;
		Magic_Stop( m_emitterId );
		//Magic_SetDiagramFactor( m_id, 0, 3, 0.0f );
		for( int i = 0; i < m_typesCount; i++ )
		{
			Magic_SetDiagramFactor( m_emitterId, i, MAGIC_DIAGRAM_NUMBER, 0.0f );
		}
	}
	//////////////////////////////////////////////////////////////////////////
	void AstralaxEmitter::pause()
	{
		m_start = false;
	}
	//////////////////////////////////////////////////////////////////////////
	void AstralaxEmitter::resume()
	{
		m_start = true;
	}
	//////////////////////////////////////////////////////////////////////////
	bool AstralaxEmitter::update( float _timing, bool & _stop )
	{
		if( m_start == false )
		{
			return false;
		}

		m_total_rate += _timing;
        m_time += _timing;

        //if( m_total_rate < m_rate )
        //{
        //    bool oldFirstUpdate = m_firstUpdate;
        //    m_firstUpdate = false;

        //    return oldFirstUpdate;
        //}

		bool restart = Magic_Update( m_emitterId, m_total_rate );
		m_total_rate = 0.f;

		//bool restart = true;
		//while( m_total_rate >= m_rate )
		//{
		//	m_total_rate -= m_rate;

		//	test();
		//	restart = Magic_Update( m_id, m_rate );
		//	test();

  //          if( restart == false )
  //          {
  //              break;
  //          }
		//}

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
	HM_EMITTER AstralaxEmitter::getId() const
	{
		return m_emitterId;
	}
	//////////////////////////////////////////////////////////////////////////
	bool AstralaxEmitter::inInterval() const
	{
		if( m_emitterId == 0 )
		{
			return false;				 
		}

		bool result = Magic_InInterval( m_emitterId );
		
		return result;
	}
	//////////////////////////////////////////////////////////////////////////
	void AstralaxEmitter::setEmitterTranslateWithParticle( bool _value )
	{
		Magic_SetEmitterPositionMode( m_emitterId, _value );
	}
	//////////////////////////////////////////////////////////////////////////
	bool AstralaxEmitter::changeEmitterImage( uint32_t _width, uint32_t _height, unsigned char * _data, size_t _bytes )
	{
		if( Magic_ChangeImage( m_emitterId, -1, (int)_width, (int)_height, _data, (int)_bytes ) == MAGIC_ERROR )
		{
			return false;
		}

		return true;
	}
	//////////////////////////////////////////////////////////////////////////
	bool AstralaxEmitter::changeEmitterModel( float * _points, uint32_t _count )
	{
		MAGIC_TRIANGLE * triangle = reinterpret_cast<MAGIC_TRIANGLE *>(_points);

		if( Magic_ChangeModel( m_emitterId, -1, _count, triangle ) == MAGIC_ERROR )
		{
			return false;
		}

		return true;
	}
	//////////////////////////////////////////////////////////////////////////
	void AstralaxEmitter::setPosition( const mt::vec3f & _pos )
	{
		MAGIC_POSITION pos;
		pos.x = _pos.x;
		pos.y = _pos.y;

		Magic_SetEmitterPosition( m_emitterId, &pos );
	}  
	//////////////////////////////////////////////////////////////////////////
	void AstralaxEmitter::getPosition( mt::vec3f & _pos )
	{
		MAGIC_POSITION pos;
		Magic_GetEmitterPosition( m_emitterId, &pos );

		_pos.x = pos.x;
		_pos.y = pos.y;
		_pos.z = 0.f;
	}
    //////////////////////////////////////////////////////////////////////////
    void AstralaxEmitter::getBasePosition( mt::vec3f & _pos )
    {
        _pos = m_basePosition;
    }
	//////////////////////////////////////////////////////////////////////////
	void AstralaxEmitter::setScale( float _scale )
	{
		Magic_SetScale( m_emitterId, _scale );
	}
	//////////////////////////////////////////////////////////////////////////
	void AstralaxEmitter::restart()
	{
		Magic_Restart( m_emitterId );
		if( Magic_IsInterval1( m_emitterId ) == true )
		{
			//Magic_SetInterval1( m_id, m_leftBorder );
			Magic_EmitterToInterval1( m_emitterId, 1.f, nullptr );
		}
	}
	//////////////////////////////////////////////////////////////////////////
	void AstralaxEmitter::setAngle( float _radians )
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
    bool AstralaxEmitter::getBackgroundBox( mt::box2f & _box )
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

        return true;
    }
	//////////////////////////////////////////////////////////////////////////
	float AstralaxEmitter::getLeftBorder() const
	{
        float float_leftBorder = (float)m_leftBorder;

		return float_leftBorder;
	}
	//////////////////////////////////////////////////////////////////////////
	float AstralaxEmitter::getRightBorder() const
	{
        float float_rightBorder = (float)m_rightBorder;

		return float_rightBorder;
	}
	//////////////////////////////////////////////////////////////////////////
	float AstralaxEmitter::getDuration() const
	{
		double duration = (m_rightBorder - m_leftBorder) / m_tempScale;

        float float_duration = (float)duration;

		return float_duration;
	}
	//////////////////////////////////////////////////////////////////////////
	void AstralaxEmitter::seek( float _time )
	{
		double rate = Magic_GetUpdateTime( m_emitterId );
		bool randomMode = Magic_IsRandomMode(m_emitterId);
		Magic_SetRandomMode( m_emitterId, false );

		double currentTime = Magic_GetPosition( m_emitterId );
		double delta = currentTime - m_total_rate;
		delta = _time;

		this->restart();
		/*	
		if( delta <= 0)
		{
			restart();
			delta = _time;
		}
		*/
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
	float AstralaxEmitter::getUpdateTemp() const
	{
		float tempScale = Magic_GetUpdateSpeed( m_emitterId );

        return tempScale;
	}
	//////////////////////////////////////////////////////////////////////////
	void AstralaxEmitter::setRandomMode( bool _randomMode )
	{
		Magic_SetRandomMode( m_emitterId, _randomMode );
	}
	//////////////////////////////////////////////////////////////////////////
	bool AstralaxEmitter::getRandomMode() const
	{
		bool mode = Magic_IsRandomMode( m_emitterId );

		return mode;
	}
	//////////////////////////////////////////////////////////////////////////
	bool AstralaxEmitter::isBackground() const
	{
		return m_background;
	}
	//////////////////////////////////////////////////////////////////////////
	bool AstralaxEmitter::prepareParticles( ParticleEmitterRenderFlush & _flush )
	{
		_flush.meshCount = 0;
		_flush.verticesCount = 0;
		_flush.indicesCount = 0;
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

		_flush.verticesCount = start.vertices;
		_flush.indicesCount = start.indexes;
		_flush.arrays = start.arrays;
		_flush.context = context;

		return true;
	}
	//////////////////////////////////////////////////////////////////////////
	bool AstralaxEmitter::flushParticles( ParticleMesh * _meshes, uint32_t _meshLimit, RenderVertex2D * _vertices, RenderIndices * _indices, ParticleEmitterRenderFlush & _flush )
	{
		for( int i = 0; i != _flush.arrays; ++i )
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
					Magic_SetRenderArrayData( _flush.context, i, _vertices, offsetof( RenderVertex2D, uv[array_info.index] ), sizeof( RenderVertex2D ) );
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
		
		unsigned int start_index = 0;

		MAGIC_RENDER_VERTICES vrts;
		Magic_GetVertices( _flush.context, &vrts );
		int vertex_count = vrts.vertices;

		while( vertex_count != 0 && _flush.meshCount < _meshLimit )
		{
			ParticleMesh & mesh = _meshes[_flush.meshCount];

			mesh.vertexCount = vertex_count;
			mesh.indexBegin = start_index;
			mesh.primCount = vertex_count / 3;

			MAGIC_RENDER_STATE state;
			while( Magic_GetNextRenderState( _flush.context, &state ) == MAGIC_SUCCESS )
			{
				mesh.texture[state.index] = state.value;
			}

			mesh.material = vrts.material;

			_flush.meshCount++;
		}

		return true;
	}
}