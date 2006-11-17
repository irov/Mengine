#	include "SoundEngine.h"
#	include "Sound.h"

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
	bool		SoundEngine::addSound(Sound* node, const std::string& _filename, SoundNodeListenerInterface*	_listener, bool _isStreamAudioFile)
	{
		TMapSoundSource::iterator it_find = mSoundSources.find(_filename);

		if (it_find != mSoundSources.end())
		{
			node->setSoundSourceInterface(&*it_find->second);
			return	true;
		}
		else
		{
			SOUND_TYPE	typeOfSoundFile = _filename.find(".ogg") != std::string::npos ? OGG : WAV;
			
			if (FileDataInterface* data = Keeper<FileEngine>::hostage()->openFile(_filename))
			{
				SoundDataDesc	desc_to_load = {
					typeOfSoundFile,
					(void*)data->getBuffer(),
					data->size(),
					_isStreamAudioFile
				};

				SoundSourceInterface *newSoundSource = m_interface->loadSoundNode(desc_to_load, _listener);

				mSoundSources.insert(
					std::make_pair(_filename,newSoundSource));
			}
			else
			{
				assert(!"Invalid sound node!");
				return false;
			}
		}
		return	true;
	}
	//////////////////////////////////////////////////////////////////////////
	void		SoundEngine::deleteSound(const std::string& _name)
	{
		mSoundSources.erase(_name);
	}
	//////////////////////////////////////////////////////////////////////////
	void		SoundEngine::processSoundSources()
	{
		for (TMapSoundSource::iterator it = mSoundSources.begin(); it != mSoundSources.end(); )
		{	
			if( !(*it).second->updateSoundBuffer() )
			{
				mSoundSources.erase(it++);
			}
			else
			{
				++it;
			}
		}
	}
}