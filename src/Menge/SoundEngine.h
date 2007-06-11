#	pragma once

#	include "SystemDLL.h"
#	include "Holder.h"

#	include "SoundSystemInterface.h"

#	include	<map>
#	include	<string>

class	FileDataInterface;

namespace Menge
{
	class ResourceSound;

	class SoundEngine
		: public SystemDLL<SoundSystemInterface>
	{
	public:
		SoundEngine(const std::string& _dllModule);
		~SoundEngine();
		void			setListenerOrient(float* _position, float* _updir);
		bool			addSoundNode(
			SoundSourceInterface* &_node,
			FileDataInterface* &_data, 
			const std::string& _filename,
			SoundNodeListenerInterface* _listener = 0,
			bool _isStreamAudioFile = true);
		void			deleteSound(SoundSourceInterface* _node);		
	};
};