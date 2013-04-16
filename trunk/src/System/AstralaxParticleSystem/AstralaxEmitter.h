#	pragma once

#	include "Interface/ParticleSystemInterface.h"

#pragma warning(push, 0) 
#	include "magic.h"
#pragma warning(pop) 

#	include <string>
#	include <vector>

namespace Menge
{
	class AstralaxEmitterContainer;

	class AstralaxEmitter 
		: public EmitterInterface
	{
	public:
		AstralaxEmitter( AstralaxEmitterContainer * _container, HM_EMITTER _id, const ConstString & _name );
		~AstralaxEmitter();

    public:
        bool initialize();

	public:
		const ConstString & getName() const override;

	public:
		AstralaxEmitterContainer * getContainer() const;

	public:
		void play() override;
		void stop() override;
		void pause() override;
		void restart() override;
		bool update( float _timing ) override;
		void setLeftBorder( float _leftBorder ) override;
		void seek( float _timming ) override;
		void setLoop( bool _loop ) override;
		bool getLoop() const override;
		
		float getLeftBorder() const override;
		float getRightBorder() const override;
		float getDuration() const override;
		
		void setRandomMode( bool _randomMode ) override;
		bool getRandomMode() const override;

	public:
		void interrupt() override;

	public:
		//void getBoundingBox( int & left, int & top, int & right, int & bottom )  const override;
		void getBoundingBox( mt::box2f& _box ) const override;
		//void getBoundingBox( Rect & _rect )  const override;
		void setEmitterTranslateWithParticle( bool _value ) override;
		bool isIntensive() const override;

	public:
		bool changeEmitterImage( int _width, int _height, unsigned char* _data, int _bytes ) override;
		bool changeEmitterModel( float * _points, int _count ) override;
		
	public:
		void setListener( ParticleEmitterListenerInterface* _listener ) override;

	public:
		void setPosition( const mt::vec3f & _pos ) override;
		void getPosition( mt::vec3f & _pos ) override;

        void getBasePosition( mt::vec3f & _pos ) override;

		void setScale( float _scale ) override;

		void setAngle( float _radians ) override;

        bool getBackgroundBox( mt::box2f & _box ) override;

	public:
		HM_EMITTER getId() const;

    public:
		float getUpdateTemp() const;
		
	protected:
		AstralaxEmitterContainer * m_container;

		HM_EMITTER m_id;
        mt::vec3f m_basePosition;

		ConstString m_name;
		double m_tempScale;
		double m_leftBorder;
		double m_rightBorder;
		
        double m_rate;
		double m_total_rate;

        double m_time;
		
		ParticleEmitterListenerInterface* m_listener;
		float m_angle;
		//float m_posX, m_posY; 

		float m_factor[20];
		int m_typesCount;

		bool m_start;
		bool m_looped;

        bool m_firstUpdate;
	};
}