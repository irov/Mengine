#	pragma once

#	include "Interface\ParticleSystemInterface.h"

#	include "magic.h"

#	include <vector>

class AstralaxEmitter : public EmitterInterface
{
public:
	AstralaxEmitter( HM_EMITTER _id );
	~AstralaxEmitter();

public:
	void	play() override;
	void	stop() override;
	void	pause() override;
	void	restart() override;
	void	update( float _timing ) override;
	void	setLeftBorder( float _leftBorder ) override;

	void	setLooped( int _loop ) override;
	int		getLooped() const override;

	void	getBoundingBox( int & left, int & top, int & right, int & bottom )  const override;
	int		getNumTypes() const override;

	bool	isIntensive() const override;

	void	setListener( ParticleEmitterListenerInterface* _listener ) override;
	void	setPosition( float _x, float _y ) override;
public:
	HM_EMITTER	getId() const;

private:
	float		m_leftBorder;
	double		m_total_rate;
	HM_EMITTER	m_id;
	bool		m_start;
	bool		m_looped;
	ParticleEmitterListenerInterface* m_listener;

	float m_factor[20];
	int m_typesCount;
};