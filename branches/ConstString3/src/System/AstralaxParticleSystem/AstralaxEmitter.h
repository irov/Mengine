#	pragma once

#	include "Interface/ParticleSystemInterface.h"

#	include "magic.h"

#	include <string>
#	include <vector>

namespace Menge
{
	class AstralaxEmitterContainer;

	class AstralaxEmitter 
		: public EmitterInterface
	{
	public:
		AstralaxEmitter( AstralaxEmitterContainer * _container, HM_EMITTER _id, const std::string & _name );
		~AstralaxEmitter();

	public:
		const String & getName() const override;

	public:
		AstralaxEmitterContainer * getContainer() const;

	public:
		void play() override;
		void stop() override;
		void pause() override;
		void restart() override;
		void update( float _timing ) override;
		void setLeftBorder( float _leftBorder ) override;

		void setLoop( int _loop ) override;
		int	getLoop() const override;

		void getBoundingBox( int & left, int & top, int & right, int & bottom )  const override;

		void setEmitterTranslateWithParticle( bool _value ) override;
		bool isIntensive() const override;

	public:
		void changeEmitterImage( int _width, int _height, unsigned char* _data, int _bytes ) override;
		void changeEmitterModel( float * _points, int _count ) override;

	public:
		void setListener( ParticleEmitterListenerInterface* _listener ) override;

	public:
		void setPosition( const mt::vec2f & _pos ) override;
		void getPosition( mt::vec2f & _pos ) override;

		void setAngle( float _radians ) override;

	public:
		HM_EMITTER getId() const;

	private:
		AstralaxEmitterContainer * m_container;

		HM_EMITTER	m_id;
		std::string m_name;

		double		m_leftBorder;
		double		m_total_rate;

		bool		m_start;
		bool		m_looped;
		ParticleEmitterListenerInterface* m_listener;
		float		m_angle;
		//float m_posX, m_posY; 

		float m_factor[20];
		int m_typesCount;
	};
}