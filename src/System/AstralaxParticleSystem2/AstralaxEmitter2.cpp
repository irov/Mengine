#	include "AstralaxEmitter2.h"

#	include <Utils/Logger/Logger.h>

namespace Menge
{
	//////////////////////////////////////////////////////////////////////////
	AstralaxEmitter2::AstralaxEmitter2()
		: m_serviceProvider(nullptr)
        , m_id(0)
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
	{
	}
	//////////////////////////////////////////////////////////////////////////
	AstralaxEmitter2::~AstralaxEmitter2()
	{
	}
    //////////////////////////////////////////////////////////////////////////
    bool AstralaxEmitter2::initialize( ServiceProviderInterface * _serviceProvider, HM_EMITTER _id, const ConstString & _name )
    {
        m_serviceProvider = _serviceProvider;
        m_id = _id;
        m_name = _name;

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

        m_rate = Magic_GetUpdateTime( m_id );

        if( this->setupBasePosition_() == false )
        {
            return false;
        }
               
        return true;
    }
	//////////////////////////////////////////////////////////////////////////
	void AstralaxEmitter2::finalize()
	{
		Magic_UnloadEmitter( m_id );

		m_id = 0;
	}
    //////////////////////////////////////////////////////////////////////////
    bool AstralaxEmitter2::setupBasePosition_()
    {
        MAGIC_RECT rect;
        float backgroundScale = Magic_GetBackgroundRect( m_id, &rect );

        if( fabsf( backgroundScale - 1.f ) > 0.0001f )
        {
            LOGGER_ERROR(m_serviceProvider)("AstralaxEmitter::setupBasePosition_ %s background scale is not 1.f (%f if is zero, add background!) Please remove scale from source and re-export!"
                , m_name.c_str()
                , backgroundScale
                );

            return false;
        }

        if( rect.left == rect.right || rect.bottom == rect.top )
        {
            MAGIC_POSITION pos;
            pos.x = 0.f;
            pos.y = 0.f;

            m_basePosition.x = 0.f;
            m_basePosition.y = 0.f;
#ifdef MAGIC_3D
            m_basePosition.z = 0.f;
#endif

            Magic_SetEmitterPosition( m_id, &pos );

			m_background = false;
        }
        else
        {
            MAGIC_POSITION pos;
            Magic_GetEmitterPosition( m_id, &pos );

            MAGIC_POSITION adapt_pos;
            adapt_pos.x = pos.x - (float)rect.left;
            adapt_pos.y = pos.y - (float)rect.top;

            Magic_SetEmitterPosition( m_id, &adapt_pos );

            m_basePosition.x = adapt_pos.x;
            m_basePosition.y = adapt_pos.y;

#ifdef MAGIC_3D
            m_basePosition.z = adapt_pos.z;
#else
            m_basePosition.z = 0.f;
#endif            

			m_background = true;
        }

        return true;
    }
	//////////////////////////////////////////////////////////////////////////
	const ConstString & AstralaxEmitter2::getName() const
	{
		return m_name;
	}
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
		mt::box2f box = s_getEmitterBBox(m_id);
		_box = box;
	}
	//////////////////////////////////////////////////////////////////////////
	void AstralaxEmitter2::setLeftBorder( float _leftBorder )
	{
		m_leftBorder = (double)_leftBorder;

		Magic_SetInterval1( m_id, m_leftBorder );
	}
	//////////////////////////////////////////////////////////////////////////
	void AstralaxEmitter2::play()
	{
		//if ( m_start == false ) 
		{ 
			Magic_Restart( m_id );

			if( Magic_IsInterval1( m_id ) == true )
			{
				//Magic_SetInterval1( m_id, m_leftBorder );
				Magic_EmitterToInterval1( m_id, 1.f, nullptr );
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
	void AstralaxEmitter2::setLoop( bool _loop )
	{
		Magic_SetLoopMode( m_id, _loop? 1: 0 );
	}
	//////////////////////////////////////////////////////////////////////////
	bool AstralaxEmitter2::getLoop() const
	{
        int loopMode = Magic_GetLoopMode( m_id );

		return loopMode == 1;
	}
	//////////////////////////////////////////////////////////////////////////
	void AstralaxEmitter2::interrupt()
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
	void AstralaxEmitter2::stop()
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

        //if( m_total_rate < m_rate )
        //{
        //    bool oldFirstUpdate = m_firstUpdate;
        //    m_firstUpdate = false;

        //    return oldFirstUpdate;
        //}

		bool restart = Magic_Update( m_id, m_total_rate );
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
	HM_EMITTER AstralaxEmitter2::getId() const
	{
		return m_id;
	}
	//////////////////////////////////////////////////////////////////////////
	bool AstralaxEmitter2::inInterval() const
	{
		if( m_id == 0 )
		{
			return false;				 
		}

		bool result = Magic_InInterval( m_id );
		
		return result;
	}
	//////////////////////////////////////////////////////////////////////////
	bool AstralaxEmitter2::createFirstRenderedParticlesList( MAGIC_RENDERING * _rendering )
	{
		if( m_id == 0 )
		{
			return false;				 
		}
		
		if( Magic_CreateFirstRenderedParticlesList( m_id, _rendering ) != MAGIC_SUCCESS )
		{
			return false;
		}

		return true;
	}
	//////////////////////////////////////////////////////////////////////////
	void AstralaxEmitter2::setEmitterTranslateWithParticle( bool _value )
	{
		Magic_SetEmitterPositionMode( m_id, _value );
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
		if( Magic_ChangeImage( m_id, -1, _width, _height, _data, _bytes ) == MAGIC_ERROR )
		{
			return false;
		}

		return true;
	}
	//////////////////////////////////////////////////////////////////////////
	bool AstralaxEmitter2::changeEmitterModel( float * _points, int _count )
	{
		MAGIC_TRIANGLE * triangle = reinterpret_cast<MAGIC_TRIANGLE *>(_points);

		if( Magic_ChangeModel( m_id, -1, _count, triangle ) == MAGIC_ERROR )
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

#ifdef MAGIC_3D
        pos.z = _pos.z;
#endif

		Magic_SetEmitterPosition( m_id, &pos );
	}  
	//////////////////////////////////////////////////////////////////////////
	void AstralaxEmitter2::getPosition( mt::vec3f & _pos )
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
    void AstralaxEmitter2::getBasePosition( mt::vec3f & _pos )
    {
        _pos = m_basePosition;
    }
	//////////////////////////////////////////////////////////////////////////
	void AstralaxEmitter2::setScale( float _scale )
	{
		Magic_SetScale( m_id, _scale );
	}
	//////////////////////////////////////////////////////////////////////////
	void AstralaxEmitter2::restart()
	{
		Magic_Restart( m_id );
		if( Magic_IsInterval1( m_id ) == true )
		{
			//Magic_SetInterval1( m_id, m_leftBorder );
			Magic_EmitterToInterval1( m_id, 1.f, nullptr );
		}
	}
	//////////////////////////////////////////////////////////////////////////
	void AstralaxEmitter2::setAngle( float _radians )
	{
		m_angle = _radians * 180.f / 3.1415926f;

		Magic_SetDiagramAddition( m_id, MAGIC_DIAGRAM_DIRECTION, -1, m_angle );
		int k_par = Magic_GetParticlesTypeCount( m_id );
		for( int j = 0; j < k_par; j++ )
		{
			Magic_SetDiagramAddition( m_id, MAGIC_DIAGRAM_DIRECTION, j, m_angle );
		}
	}
    //////////////////////////////////////////////////////////////////////////
    bool AstralaxEmitter2::getBackgroundBox( mt::box2f & _box )
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
	float AstralaxEmitter2::getUpdateTemp() const
	{
		float tempScale = Magic_GetUpdateSpeed( m_id );

        return tempScale;
	}
	//////////////////////////////////////////////////////////////////////////
	void AstralaxEmitter2::setRandomMode( bool _randomMode )
	{
		Magic_SetRandomMode( m_id, _randomMode );
	}
	//////////////////////////////////////////////////////////////////////////
	bool AstralaxEmitter2::getRandomMode() const
	{
		bool mode = Magic_IsRandomMode( m_id );

		return mode;
	}
	//////////////////////////////////////////////////////////////////////////
	bool AstralaxEmitter2::isBackground() const
	{
		return m_background;
	}
}