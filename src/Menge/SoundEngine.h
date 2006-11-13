#	pragma once

#	include "DllModule.h"
#	include "Keeper.h"

#	include <boost/shared_ptr.hpp>

#	include	<map>
#	include	<string>

class	SoundSystemInterface;
class	SoundSourceInterface;
class	SoundNodeListenerInterface;

namespace Menge
{
	class SoundNode;

	typedef std::map<std::string, boost::shared_ptr<SoundSourceInterface> > TMapSoundSource;

	class SoundEngine : public DllModuleInterface<SoundSystemInterface>
	{
	public:
		SoundEngine(const std::string& _dllModule);
		~SoundEngine();
		void			setListenerOrient(const float* _position, const float* _updir);
		bool			addSoundNode(SoundNode*, const std::string& _filename, SoundNodeListenerInterface*	_listener = 0, bool _isStreamAudioFile = false);
		void			deleteSoundNode(const std::string& _name);		
		void			processSoundSources();
	private:
		TMapSoundSource	mSoundSources;
	};
};