#	include "SoundEngine.h"

#	include "FileEngine.h"
#	include "SoundNode.h"

#	include "SoundSystemInterface.h"
#	include "FileSystemInterface.h"

#include	<assert.h>




namespace RvEngine
{
	SoundEngine::SoundEngine(const std::string& _dllModule)
			: DllModuleInterface<SoundSystemInterface>(_dllModule)
	{
	}

	SoundEngine::~SoundEngine()
	{
	}

	bool	SoundEngine::init(FileEngine*	_fileEngine)
	{
		if(!_fileEngine)
		{
			return	false;
		}
		mFileEngine = _fileEngine;
		return	true;
	}

	void	SoundEngine::setListenerOrient(const float* _position, const float* _updir)
	{
		m_interface->setListenerOrient(_position,_updir);
	}

	NodePtr		SoundEngine::addSoundNode(const std::string& _filename, SoundNodeListenerInterface*	_listener, bool _isStreamAudioFile)
	{
		TMapSoundSource::iterator it_find = mSoundSources.find(_filename.c_str());

		NodePtr node = NULL;

		if (it_find != mSoundSources.end())
		{
			node = it_find->second;
		}
		else
		{
			SOUND_TYPE	typeOfSoundFile = _filename.find(".ogg")!=std::string::npos ? OGG : WAV;
			
			if (FileDataInterface* data = mFileEngine->openFile(_filename))
			{
				SoundDataDesc	desc_to_load = {
					typeOfSoundFile,
					(void*)data->getBuffer(),
					data->size(),
					_isStreamAudioFile
				};

				SoundSourceInterface *nfc = m_interface->loadSoundNode(desc_to_load, _listener);
				NodePtr shared(new SoundNode(m_interface,nfc));
				mSoundSources.insert(std::make_pair(_filename,shared));
			}
			else
			{
				assert(!"Invalid sound node!");
			}
		}
		return	node;
	}

	void		SoundEngine::deleteSoundNode(const std::string& _name)
	{
		mSoundSources.erase(_name);
	}

	void		SoundEngine::update()
	{
		/*
			check for unstreamed source sounds, then
			delete them.
		*/
		for (TMapSoundSource::iterator 
			it = mSoundSources.begin();
			it != mSoundSources.end();)
		{	
			if( !(*it).second->update() )
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