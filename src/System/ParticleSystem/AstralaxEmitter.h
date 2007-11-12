#	pragma once

#	include "Interface\ParticleSystemInterface.h"

#	include "magic.h"

#	include <vector>

class AstralaxEmitter : public EmitterInterface
{
public:
	AstralaxEmitter( HM_EMITTER _id );

public:
	void	play( bool _leftVisible );
	void	stop();
	void	update();
	bool	isPlaying() const;
	int		getNumTypes() const;
public:
	HM_EMITTER	getId() const;
private:
	HM_EMITTER	m_id;
	bool		m_pause;
	std::string  m_name;
	void	_leftVisibleInterval();
};