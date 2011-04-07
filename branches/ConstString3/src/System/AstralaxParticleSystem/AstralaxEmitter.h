#	pragma once

#	include "Interface/ParticleSystemInterface.h"

#	include "magic.h"

#	include <string>
#	include <vector>

class AstralaxEmitter 
	: public Menge::EmitterInterface
{
public:
	AstralaxEmitter( HM_EMITTER _id, const std::string & _name );
	~AstralaxEmitter();

public:
	const std::string & getName() const override;

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
	int getNumTypes() const override;

	bool isIntensive() const override;

public:
	void changeEmitterImage( int _width, int _height, unsigned char* _data, int _bytes ) override;

public:
	void setListener( Menge::ParticleEmitterListenerInterface* _listener ) override;
	void getPosition( float & _x, float & _y ) override;
	void setPosition( float _x, float _y ) override;
	void setAngle( float _radians ) override;
public:
	HM_EMITTER	getId() const;

private:
	HM_EMITTER	m_id;
	std::string m_name;

	float		m_leftBorder;
	double		m_total_rate;

	bool		m_start;
	bool		m_looped;
	Menge::ParticleEmitterListenerInterface* m_listener;
	float		m_angle;
	//float m_posX, m_posY; 

	float m_factor[20];
	int m_typesCount;
};
