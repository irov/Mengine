#	include "FSLSoundSource.h"

class FSLSoundSystem : public SoundSystemInterface
{
public:
	FSLSoundSystem();
	~FSLSoundSystem();
	void				setListenerOrient(const float* _position, const float* _updir);
	SoundSourceInterface*		loadSoundNode(const SoundDataDesc& _desc, SoundNodeListenerInterface*	_listener = 0);
	void				releaseSoundNode(SoundSourceInterface* _sn);
};

bool	initInterfaceSystem(SoundSystemInterface** );
void	releaseInterfaceSystem(SoundSystemInterface* );
