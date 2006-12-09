#	pragma once

#	include "DllModule.h"
#	include "Keeper.h"

#	include <boost/shared_ptr.hpp>

#	include	<map>
#	include	<string>

class	SoundSystemInterface;
class	SoundSourceInterface;
class	FileDataInterface;
class	SoundNodeListenerInterface;

namespace Menge
{
	class SoundNode;

	class SoundEngine : public DllModuleInterface<SoundSystemInterface>
	{
	public:
		SoundEngine(const std::string& _dllModule);
		~SoundEngine();
		void			setListenerOrient(const float* _position, const float* _updir);
		bool			addSoundNode(
			SoundSourceInterface* &_node,
			FileDataInterface* &_data, 
			const std::string& _filename,
			SoundNodeListenerInterface* _listener = 0,
			bool _isStreamAudioFile = true);
		void			deleteSound(SoundSourceInterface* _node);		
	};
};