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
	void	play( bool _leftVisible ) override;
	void	stop() override;
	void	pause() override;
	void	update( float _timing ) override;

	void	setLooped( bool _loop ) override;
	bool	getLooped() const override;

	void	getBoundingBox( int & left, int & top, int & right, int & bottom )  const override;
	int		getNumTypes() const override;

public:
	HM_EMITTER	getId() const;

private:
	HM_EMITTER	m_id;
	bool		m_start;
	void	_leftVisibleInterval();
};