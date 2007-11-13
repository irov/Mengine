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
	void	play( bool _leftVisible );
	void	stop();
	void	pause();
	void	update();
	void	setLooped( bool _loop );
	bool	getLooped() const;
	void	getBoundingBox( int & left, int & top, int & right, int & bottom )  const;
	int		getNumTypes() const;
public:
	HM_EMITTER	getId() const;
private:
	HM_EMITTER	m_id;
	bool		m_start;
	void	_leftVisibleInterval();
};