#	pragma once

#	include "DllModule.h"
#	include "Keeper.h"

#	include <boost/intrusive_ptr.hpp>

#	include	<map>
#	include	<string>

class	SoundSystemInterface;
class	SoundSourceInterface;
class	SoundNodeListenerInterface;

/*
	Forward declaration
*/

namespace Menge
{
	class SoundNode;
}

namespace Menge
{
	/*
		Sound	Manager.
		Provide managing of sound resources.
		see SoundSystemInterface for Sound System.

		contain pointer to impl from DllModuleInterface in m_interface (SoundSystemInterface*). 
		loaded in SceneManager by loadSystemModule.
	*/

	typedef boost::intrusive_ptr<SoundNode> NodePtr;

	class SoundEngine : public DllModuleInterface<SoundSystemInterface>
	{
	public:
		SoundEngine(const std::string& _dllModule);
		~SoundEngine();
		void			setListenerOrient(const float* _position, const float* _updir);
		NodePtr			addSoundNode(const std::string& _filename, SoundNodeListenerInterface*	_listener = 0, bool _isStreamAudioFile = false);
		void			deleteSoundNode(const std::string& _name);		
		void			update();
	private:
		typedef std::map<std::string,NodePtr> TMapSoundSource;

		TMapSoundSource	mSoundSources;
	};
};