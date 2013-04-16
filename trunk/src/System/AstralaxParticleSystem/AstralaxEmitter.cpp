#	include "AstralaxEmitter.h"
#	include <assert.h>

#	include <Utils/Logger/Logger.h>

namespace Menge
{
	//////////////////////////////////////////////////////////////////////////
	AstralaxEmitter::AstralaxEmitter( AstralaxEmitterContainer * _container, HM_EMITTER _id, const ConstString & _name )
		: m_id(_id)
		, m_container(_container)
		, m_name(_name)
		, m_start(false)
		, m_leftBorder(0.f)
		, m_rightBorder(0.f)
        , m_rate(0.f)
		, m_total_rate(0.f)
        , m_time(0.f)
		, m_looped(false)
		, m_listener(NULL)
		, m_angle(0.f)
		, m_tempScale(1)
        , m_basePosition(0.f, 0.f, 0.f)
	{
	}
	//////////////////////////////////////////////////////////////////////////
	AstralaxEmitter::~AstralaxEmitter()
	{
	}
    //////////////////////////////////////////////////////////////////////////
    bool AstralaxEmitter::initialize()
    {
        m_typesCount = Magic_GetParticlesTypeCount( m_id );

        if( m_typesCount >= 20 )
        {
            return false;
        }

        for( int i = 0; i != m_typesCount; ++i )
        {
            m_factor[i] = Magic_GetDiagramFactor( m_id, i, MAGIC_DIAGRAM_NUMBER );
        }

        Magic_SetRandomMode( m_id, false );

        // set interpolation
        Magic_SetInterpolationMode( m_id, true );

        m_leftBorder = Magic_GetInterval1( m_id );
        m_rightBorder = Magic_GetInterval2( m_id );

        MAGIC_RECT rect;

        if( Magic_GetBackgroundRect( m_id, &rect ) != MAGIC_SUCCESS )
        {
            return false;
        }

        MAGIC_POSITION pos;
        Magic_GetEmitterPosition( m_id, &pos );

        pos.x -= (float)rect.left;
        pos.y -= (float)rect.top;

        Magic_SetEmitterPosition( m_id, &pos );

        m_basePosition.x = pos.x;
        m_basePosition.y = pos.y;

#ifdef MAGIC_3D
        m_basePosition.z = pos.z;
#else
        m_basePosition.z = 0.f;
#endif

        m_rate = Magic_GetUpdateTime( m_id );
               
        return true;
    }
	//////////////////////////////////////////////////////////////////////////
	const ConstString & AstralaxEmitter::getName() const
	{
		return m_name;
	}
	//////////////////////////////////////////////////////////////////////////
	AstralaxEmitterContainer * AstralaxEmitter::getContainer() const
	{
		return m_container;
	}
	//////////////////////////////////////////////////////////////////////////
	//void AstralaxEmitter::getBoundingBox( int & left, int & top, int & right, int & bottom )  const
	//{
	//	assert( !"deprecated" );
	//	//Magic_GetRect( m_id, &left, &top, &right, &bottom );
	//}
	//////////////////////////////////////////////////////////////////////////
	static mt::box2f s_getEmitterBBox( HM_EMITTER _emitter )
	{
		mt::box2f box( mt::vec2f (0.f, 0.f), mt::vec2f(0.f, 0.f) );

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
			Magic_EmitterToInterval1( _emitter, 1 ,NULL );

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
			Magic_EmitterToInterval1( _emitter, 1 ,NULL );
		}
		return box;
	}
	//////////////////////////////////////////////////////////////////////////
	void AstralaxEmitter::getBoundingBox( mt::box2f& _box ) const
	{
		mt::box2f box = s_getEmitterBBox(m_id);
		_box = box;
	}
	//////////////////////////////////////////////////////////////////////////
	void AstralaxEmitter::setLeftBorder( float _leftBorder )
	{
		m_leftBorder = (double)_leftBorder;

		Magic_SetInterval1( m_id, m_leftBorder );
	}
	//////////////////////////////////////////////////////////////////////////
	void AstralaxEmitter::play()
	{
		//if ( m_start == false ) 
		{ 
			Magic_Restart( m_id );

			if( Magic_IsInterval1( m_id ) == true )
			{
				//Magic_SetInterval1( m_id, m_leftBorder );
				Magic_EmitterToInterval1( m_id, 1.f, NULL );
			}
		}

		for( int i = 0; i < m_typesCount; ++i )
		{
			Magic_SetDiagramFactor( m_id, i, MAGIC_DIAGRAM_NUMBER, m_factor[i] );
		}

		m_total_rate = 0.0;
        m_time = 0.0;

		m_start = true;
	}
	//////////////////////////////////////////////////////////////////////////
	void AstralaxEmitter::setLoop( bool _loop )
	{
		Magic_SetLoopMode( m_id, _loop? 1: 0 );
	}
	//////////////////////////////////////////////////////////////////////////
	bool AstralaxEmitter::getLoop() const
	{
		return Magic_GetLoopMode( m_id ) == 1;
	}
	//////////////////////////////////////////////////////////////////////////
	void AstralaxEmitter::interrupt()
	{
		if( Magic_IsInterrupt( m_id ) == true )
		{
			return;
		}

		Magic_Update( m_id, 0.f );
		Magic_SetLoopMode( m_id, 0 );
		Magic_SetInterrupt( m_id, true );
	}
	//////////////////////////////////////////////////////////////////////////
	void AstralaxEmitter::stop()
	{
		m_start = false;
		Magic_Stop( m_id );
		//Magic_SetDiagramFactor( m_id, 0, 3, 0.0f );
		for( int i = 0; i < m_typesCount; i++ )
		{
			Magic_SetDiagramFactor( m_id, i, MAGIC_DIAGRAM_NUMBER, 0.0f );
		}
	}
	//////////////////////////////////////////////////////////////////////////
	void AstralaxEmitter::pause()
	{
		m_start = false;
	}
	//////////////////////////////////////////////////////////////////////////
	bool AstralaxEmitter::update( float _timing )
	{
		if( m_start == false )
		{
			return false;
		}

		m_total_rate += _timing;
        m_time += _timing;

        if( m_total_rate < m_rate )
        {
            bool oldFirstUpdate = m_firstUpdate;
            m_firstUpdate = false;

            return oldFirstUpdate;
        }

        bool restart = true;

		while( m_total_rate >= m_rate )
		{
			m_total_rate -= m_rate;
			restart = Magic_Update( m_id, m_rate );
			//bool restart = Magic_Update( m_id, _timing );

            if( restart == false )
            {
                break;
            }
		}

        if( restart == false )
        { 
            m_start = false;

            if( m_listener )
            {
                m_listener->onParticleEmitterStopped();
            }
        }

        return true;
	}
	//////////////////////////////////////////////////////////////////////////
	HM_EMITTER AstralaxEmitter::getId() const
	{
		return m_id;
	}
	//////////////////////////////////////////////////////////////////////////
	void AstralaxEmitter::setEmitterTranslateWithParticle( bool _value )
	{
		Magic_SetEmitterPositionMode( m_id, _value );
	}
	//////////////////////////////////////////////////////////////////////////
	bool AstralaxEmitter::isIntensive() const
	{
		bool intensive = Magic_IsIntensive();

		return intensive;
	}
	//////////////////////////////////////////////////////////////////////////
	bool AstralaxEmitter::changeEmitterImage( int _width, int _height, unsigned char* _data, int _bytes )
	{
		if( Magic_ChangeImage( m_id, -1, _width, _height, _data, _bytes ) == MAGIC_ERROR )
		{
			return false;
		}

		return true;
	}
	//////////////////////////////////////////////////////////////////////////
	bool AstralaxEmitter::changeEmitterModel( float * _points, int _count )
	{
		MAGIC_TRIANGLE * triangle = reinterpret_cast<MAGIC_TRIANGLE *>(_points);

		if( Magic_ChangeModel( m_id, -1, _count, triangle ) == MAGIC_ERROR )
		{
			return false;
		}

		return true;
	}
	//////////////////////////////////////////////////////////////////////////
	void AstralaxEmitter::setListener( ParticleEmitterListenerInterface* _listener )
	{
		m_listener = _listener;
	}
	//////////////////////////////////////////////////////////////////////////
	void AstralaxEmitter::setPosition( const mt::vec3f & _pos )
	{
		MAGIC_POSITION pos;
		pos.x = _pos.x;
		pos.y = _pos.y;

#ifdef MAGIC_3D
        pos.z = _pos.z;
#endif

		Magic_SetEmitterPosition( m_id, &pos );
	}  
	//////////////////////////////////////////////////////////////////////////
	void AstralaxEmitter::getPosition( mt::vec3f & _pos )
	{
		MAGIC_POSITION pos;
		Magic_GetEmitterPosition( m_id, &pos );

		_pos.x = pos.x;
		_pos.y = pos.y;

#ifdef MAGIC_3D
        _pos.z = pos.z;
#else   
        _pos.z = 0.f;
#endif
	}
    //////////////////////////////////////////////////////////////////////////
    void AstralaxEmitter::getBasePosition( mt::vec3f & _pos )
    {
        _pos = m_basePosition;
    }
	//////////////////////////////////////////////////////////////////////////
	void AstralaxEmitter::setScale( float _scale )
	{
		Magic_SetScale( m_id, _scale );
	}
	//////////////////////////////////////////////////////////////////////////
	void AstralaxEmitter::restart()
	{
		Magic_Restart( m_id );
		if( Magic_IsInterval1( m_id ) == true )
		{
			//Magic_SetInterval1( m_id, m_leftBorder );
			Magic_EmitterToInterval1( m_id, 1.f, NULL );
		}
	}
	//////////////////////////////////////////////////////////////////////////
	void AstralaxEmitter::setAngle( float _radians )
	{
		m_angle = _radians * 180.0f / 3.1415926f;

		Magic_SetDiagramAddition( m_id, MAGIC_DIAGRAM_DIRECTION, -1, m_angle );
		int k_par = Magic_GetParticlesTypeCount( m_id );
		for( int j = 0; j < k_par; j++ )
		{
			Magic_SetDiagramAddition( m_id, MAGIC_DIAGRAM_DIRECTION, j, m_angle );
		}
	}
    //////////////////////////////////////////////////////////////////////////
    bool AstralaxEmitter::getBackgroundBox( mt::box2f & _box )
    {
        MAGIC_RECT rect;

        if( Magic_GetBackgroundRect( m_id, &rect ) == MAGIC_ERROR )
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
		double rate = Magic_GetUpdateTime( m_id );
		bool randomMode = Magic_IsRandomMode(m_id);
		Magic_SetRandomMode( m_id, false );

		double currentTime = Magic_GetPosition( m_id );
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
			bool isCanUpdate = Magic_Update( m_id,rate );
			if( !isCanUpdate )
			{
				restart();
			}
		}
		
		Magic_SetRandomMode( m_id, randomMode );
		m_total_rate = _time;
	}
	//////////////////////////////////////////////////////////////////////////
	float AstralaxEmitter::getUpdateTemp() const
	{
		float tempScale = Magic_GetUpdateTemp( m_id );

        return tempScale;
	}
	//////////////////////////////////////////////////////////////////////////
	void AstralaxEmitter::setRandomMode( bool _randomMode )
	{
		Magic_SetRandomMode( m_id, _randomMode );
	}
	//////////////////////////////////////////////////////////////////////////
	bool AstralaxEmitter::getRandomMode() const
	{
		bool mode = Magic_IsRandomMode( m_id );

		return mode;
	}
	//////////////////////////////////////////////////////////////////////////
}