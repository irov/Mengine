#	include "AstralaxEmitter.h"
#	include <assert.h>

#	include <Utils/Logger/Logger.h>

namespace Menge
{
	//////////////////////////////////////////////////////////////////////////
	AstralaxEmitter::AstralaxEmitter( AstralaxEmitterContainer * _container, HM_EMITTER _id, const std::string & _name )
		: m_id(_id)
		, m_container(_container)
		, m_name(_name)
		, m_start( false )
		, m_leftBorder( 0.0f )
		, m_total_rate( 0.0f )
		, m_looped( false )
		, m_listener( NULL )
		, m_angle( 0.0f )
		//, m_posX( 0.0f )
		//, m_posY( 0.0f )
	{
		m_typesCount = Magic_GetParticlesTypeCount( m_id );
		assert( m_typesCount < 20 && "Particles type count over 20!" );
		for( int i = 0; i != m_typesCount; ++i )
		{
			m_factor[i] = Magic_GetDiagramFactor( m_id, i, MAGIC_DIAGRAM_NUMBER );
		}

		// set randomize
		Magic_SetRandomMode( m_id, true );

		// set interpolation
		Magic_SetInterpolationMode( m_id, true );

		m_leftBorder = Magic_GetInterval1( m_id );
	}
	//////////////////////////////////////////////////////////////////////////
	AstralaxEmitter::~AstralaxEmitter()
	{
	}
	//////////////////////////////////////////////////////////////////////////
	const String & AstralaxEmitter::getName() const
	{
		return m_name;
	}
	//////////////////////////////////////////////////////////////////////////
	AstralaxEmitterContainer * AstralaxEmitter::getContainer() const
	{
		return m_container;
	}
	//////////////////////////////////////////////////////////////////////////
	void AstralaxEmitter::getBoundingBox( int & left, int & top, int & right, int & bottom )  const
	{
		assert( !"deprecated" );
		//Magic_GetRect( m_id, &left, &top, &right, &bottom );
	}
	//////////////////////////////////////////////////////////////////////////
	void AstralaxEmitter::setLeftBorder( float _leftBorder )
	{
		m_leftBorder = _leftBorder;
		Magic_SetInterval1( m_id, m_leftBorder );
	}
	//////////////////////////////////////////////////////////////////////////
	void AstralaxEmitter::play()
	{
		if ( m_start == false ) 
		{ 
			Magic_Restart( m_id );
			if( Magic_IsInterval1( m_id ) == true )
			{
				//Magic_SetInterval1( m_id, m_leftBorder );
				Magic_EmitterToInterval1( m_id, NULL );
			}
		}

		for( int i = 0; i < m_typesCount; i++ )
		{
			Magic_SetDiagramFactor( m_id, i, MAGIC_DIAGRAM_NUMBER, m_factor[i] );
		}

		m_total_rate = 0.0f;
		m_start = true;
	}
	//////////////////////////////////////////////////////////////////////////
	void AstralaxEmitter::setLoop( int _loop )
	{
		Magic_SetLoopMode( m_id, _loop );
	}
	//////////////////////////////////////////////////////////////////////////
	int AstralaxEmitter::getLoop() const
	{
		return Magic_GetLoopMode( m_id );
	}
	//////////////////////////////////////////////////////////////////////////
	void AstralaxEmitter::stop()
	{
		//m_start = false;

		//Magic_Stop( m_id );
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
	void AstralaxEmitter::update( float _timing )
	{
		if( m_start == false )
		{
			return;
		}

		m_total_rate += _timing;

		double rate = Magic_GetUpdateTime( m_id );

		while( m_total_rate >= rate )
		{
			m_total_rate -= rate;
			bool restart = Magic_Update( m_id, rate );
			//bool restart = Magic_Update( m_id, _timing );

			if( restart == false )
			{ 
				m_start = false;
				if( m_listener )
				{
					m_listener->onParticleEmitterStopped();
				}
			}
		}
	}
	//////////////////////////////////////////////////////////////////////////
	HM_EMITTER AstralaxEmitter::getId() const
	{
		return m_id;
	}
	//////////////////////////////////////////////////////////////////////////
	bool AstralaxEmitter::isIntensive() const
	{
		return Magic_IsIntensive();
	}
	//////////////////////////////////////////////////////////////////////////
	void AstralaxEmitter::changeEmitterImage( int _width, int _height, unsigned char* _data, int _bytes )
	{
		//unsigned char * data = new unsigned char[100*100];
		//for( int i = 0; i != 100*100;++i)
		//{
		//	data[i] = 254;
		//	}
		//int res = Magic_ChangeImage( m_id, -1, 100, 100, data, _bytes );
		//int a = res;

		if( Magic_ChangeImage( m_id, -1, _width, _height, _data, _bytes ) == MAGIC_ERROR )
		{
			return;
		}
	}
	//////////////////////////////////////////////////////////////////////////
	void AstralaxEmitter::changeEmitterModel( float * _points, int _count )
	{
		MAGIC_TRIANGLE * triangle = reinterpret_cast<MAGIC_TRIANGLE *>(_points);

		if( Magic_ChangeModel( m_id, -1, _count, triangle ) == MAGIC_ERROR )
		{
			return;
		}
	}
	//////////////////////////////////////////////////////////////////////////
	void AstralaxEmitter::setListener( ParticleEmitterListenerInterface* _listener )
	{
		m_listener = _listener;
	}
	//////////////////////////////////////////////////////////////////////////
	void AstralaxEmitter::getPosition( mt::vec2f & _pos )
	{
		MAGIC_POSITION pos;
		Magic_GetEmitterPosition( m_id, &pos );
		_pos.x = pos.x;
		_pos.y = pos.y;
	}
	//////////////////////////////////////////////////////////////////////////
	void AstralaxEmitter::setPosition( const mt::vec2f & _pos )
	{
		//bool mode = Magic_GetEmitterPositionMode( m_id );
		//if( mode == false )
		//{
		//	// временно устанавливаем режим перемещения вместе с частицами
		//	Magic_SetEmitterPositionMode( m_id, 1 );
		//}

		MAGIC_POSITION pos;
		pos.x = _pos.x;
		pos.y = _pos.y;
		Magic_SetEmitterPosition( m_id, &pos );

		//if( mode == false )
		//{
		//	// возвращаем на место старый режим перемещения
		//	Magic_SetEmitterPositionMode( m_id, 0 );
		//}	
	}
	//////////////////////////////////////////////////////////////////////////
	void AstralaxEmitter::restart()
	{
		Magic_Restart( m_id );
		if( Magic_IsInterval1( m_id ) == true )
		{
			//Magic_SetInterval1( m_id, m_leftBorder );
			Magic_EmitterToInterval1( m_id, NULL );
		}
	}
	//////////////////////////////////////////////////////////////////////////
	void AstralaxEmitter::setAngle( float _radians )
	{
		m_angle = _radians * 180.0f / MAGIC_PI;

		Magic_SetDiagramAddition( m_id, MAGIC_DIAGRAM_DIRECTION, -1, m_angle );
		int k_par = Magic_GetParticlesTypeCount( m_id );
		for( int j = 0; j < k_par; j++ )
		{
			Magic_SetDiagramAddition( m_id, MAGIC_DIAGRAM_DIRECTION, j, m_angle );
		}
	}
	//////////////////////////////////////////////////////////////////////////
}