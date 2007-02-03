#	include "SoundEngine.h"
#	include "SoundNode.h"

#	include "FileEngine.h"

#	include "SoundSystemInterface.h"
#	include "FileSystemInterface.h"

#	include	<assert.h>

namespace Menge
{
	SoundEngine::SoundEngine(const std::string& _dllModule)
			: DllModuleInterface<SoundSystemInterface>(_dllModule)
	{
		Keeper<SoundEngine>::keep(this);
	}
	//////////////////////////////////////////////////////////////////////////
	SoundEngine::~SoundEngine()
	{
	}
	//////////////////////////////////////////////////////////////////////////
	void	SoundEngine::setListenerOrient(const float* _position, const float* _updir)
	{
		m_interface->setListenerOrient(_position,_updir);
	}
	//////////////////////////////////////////////////////////////////////////
	bool		SoundEngine::addSoundNode(
		SoundSourceInterface* &_node,
		FileDataInterface* &_data,
		const std::string& _filename,
		SoundNodeListenerInterface*	_listener,
		bool _isStreamAudioFile)
	{
		// ogg - может быть и Ogg и OGG и т.д.
		SoundDataDesc::SOUND_TYPE	typeOfSoundFile = _filename.find(".ogg") != std::string::npos ? SoundDataDesc::OGG : SoundDataDesc::WAV;
		
		_data = Keeper<FileEngine>::hostage()->openFile(_filename);

		if (_data)
		{
			SoundDataDesc	desc_to_load = {
				typeOfSoundFile,
				(void*)_data->getBuffer(),
				_data->size(),
				_isStreamAudioFile
			};

			_node = m_interface->loadSoundNode(desc_to_load, _listener);
		}
		else
		{
			assert(!"Invalid sound node!");
			return false;
		}

		return	true;
	}
	//////////////////////////////////////////////////////////////////////////
	void		SoundEngine::deleteSound(SoundSourceInterface* _soundSource)
	{
		m_interface->releaseSoundNode(_soundSource);
	}
}