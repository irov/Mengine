#	pragma once

#	include "Interface\ParticleSystemInterface.h"

#	include "magic.h"

#	include <vector>

class Emitter : public EmitterInterface
{
public:
	Emitter( HM_EMITTER _id );

public:
	void	play( bool _leftVisible );
	void	stop();
	void	update();
	bool	isPlaying() const;

	int		getParticlesTypeCount() const;
public:
	HM_EMITTER	getId() const;
private:
	HM_EMITTER	m_id;
	bool		m_pause;
	void	_leftVisibleInterval();
};