#	include "SquallSoundSource.h"

class SQUALLSoundSystem
	: public SoundSystemInterface
{
public:
	SQUALLSoundSystem();
	~SQUALLSoundSystem();
	void	setListenerOrient(float* _position, float* _updir);
	SoundSourceInterface*	loadSoundNode(const SoundDataDesc& _desc, SoundNodeListenerInterface*	_listener = 0);
	void	releaseSoundNode(SoundSourceInterface* _sn);
};
bool	initInterfaceSystem(SoundSystemInterface** );
void	releaseInterfaceSystem(SoundSystemInterface* );
