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
		mSoundSources.clear();
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
	bool		SoundEngine::addSoundNode(NodePtr node, const std::string& _filename, SoundNodeListenerInterface*	_listener, bool _isStreamAudioFile)
	{
		TMapSoundSource::iterator it_find = mSoundSources.find(_filename.c_str());

		if (it_find != mSoundSources.end())
		{
			node = it_find->second;
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

				SoundSourceInterface *nfc = m_interface->loadSoundNode(desc_to_load, _listener);
				NodePtr shared(new SoundNode());
				shared->setSoundSourceInterface(nfc);
				mSoundSources.insert(std::make_pair(_filename,shared));
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
	void		SoundEngine::deleteSoundNode(const std::string& _name)
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