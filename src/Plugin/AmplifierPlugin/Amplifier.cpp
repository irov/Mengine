#include "Amplifier.h"

#include "Interface/ResourceInterface.h"
#include "Interface/StringizeInterface.h"

#include "Kernel/ResourceMusic.h"

#include "Logger/Logger.h"

#include <cmath>

//////////////////////////////////////////////////////////////////////////
SERVICE_FACTORY( Amplifier, Mengine::Amplifier);
//////////////////////////////////////////////////////////////////////////
namespace Mengine
{
	//////////////////////////////////////////////////////////////////////////
	Amplifier::Amplifier()
		: m_play(false)
	{
	}
	//////////////////////////////////////////////////////////////////////////
	Amplifier::~Amplifier()
	{
	}
	//////////////////////////////////////////////////////////////////////////
	bool Amplifier::_initializeService()
	{
		return true;
	}
	//////////////////////////////////////////////////////////////////////////
	void Amplifier::_finalizeService()
	{
	}
	//////////////////////////////////////////////////////////////////////////
	bool Amplifier::playMusic( const ConstString& _resourceMusic, float _pos, bool _looped )
	{
		if( m_play == true )
		{
			this->stopMusic();
		}

		ResourceMusicPtr resourceMusic = RESOURCE_SERVICE()
			->getResourceReference( _resourceMusic );

		if( resourceMusic == nullptr )
		{
			LOGGER_ERROR("Amplifier::playMusic can't found resource '%s'"
				, _resourceMusic.c_str()
				);

			return false;
		}

		const FileGroupInterfacePtr & category = resourceMusic->getCategory();
		const FilePath & path = resourceMusic->getPath();
		const ConstString & codec = resourceMusic->getCodec();
		bool external = resourceMusic->isExternal();
		float volume = resourceMusic->getVolume();

		SoundBufferInterfacePtr buffer;

		if( category->isPacked() == false || external == false )
		{
			buffer = SOUND_SERVICE()
				->createSoundBufferFromFile( category, path, codec, true );
		}
		else
		{
            const FileGroupInterfacePtr & defaultFileGroup = FILE_SERVICE()
                ->getDefaultFileGroup();

			buffer = SOUND_SERVICE()
				->createSoundBufferFromFile( defaultFileGroup, path, codec, true );
		}

		if( buffer == nullptr )
		{
			LOGGER_ERROR("Amplifier::playMusic can't load sample '%s'"
				, path.c_str()
				);

			return false;
		}

		m_soundEmitter = SOUND_SERVICE()
			->createSoundIdentity( false, buffer, ESST_MUSIC, true );

		if( m_soundEmitter == nullptr )
		{
			LOGGER_ERROR("Amplifier::playMusic can't create sound source '%s'"
				, path.c_str()
				);

			return false;
		}

		if( SOUND_SERVICE()
            ->setSourceVolume( m_soundEmitter, volume, 0.f, false ) == false )
		{
			LOGGER_ERROR("Amplifier::playMusic can't set sound '%s' volume '%f'"
				, path.c_str()
				, volume
				);

			return false;
		}

		if( SOUND_SERVICE()
			->setPosMs( m_soundEmitter, _pos ) == false )
		{
			LOGGER_ERROR("Amplifier::playMusic can't set sound '%s' pos '%f'"
				, path.c_str()
				, _pos
				);

			return false;
		}

		if( SOUND_SERVICE()
			->setLoop( m_soundEmitter, _looped ) == false )
		{
			LOGGER_ERROR("Amplifier::playMusic can't set sound '%s' lood '%d'"
				, path.c_str()
				, _looped
				);

			return false;
		}

		if( SOUND_SERVICE()->playEmitter( m_soundEmitter ) == false )
		{
			LOGGER_ERROR("Amplifier::playMusic '%s' invalid play %d"
				, path.c_str()
				, m_soundEmitter->getId()
				);

			return false;
		}

		m_play = true;

		return true;
	}
	//////////////////////////////////////////////////////////////////////////
	void Amplifier::stopMusic()
	{
		m_play = false;

		if( m_soundEmitter != nullptr )
        {
            SoundIdentityInterfacePtr keep_sourceEmitter = m_soundEmitter;
			m_soundEmitter = nullptr;

			SOUND_SERVICE()
                ->stopEmitter( keep_sourceEmitter );
		
			SOUND_SERVICE()
				->releaseSoundSource( keep_sourceEmitter );
		}				
	}
	//////////////////////////////////////////////////////////////////////////
	bool Amplifier::pauseMusic()
	{
		if( m_soundEmitter == nullptr )
		{
			return false;
		}

		m_play = false;

		SOUND_SERVICE()
            ->pauseEmitter( m_soundEmitter );

		return true;
	}
	//////////////////////////////////////////////////////////////////////////
	bool Amplifier::resumeMusic()
	{
		if( m_soundEmitter == nullptr )
		{
			return false;
		}

        m_play = true;

        SOUND_SERVICE()
            ->playEmitter( m_soundEmitter );

		return true;
	}
	//////////////////////////////////////////////////////////////////////////
	float Amplifier::getDuration() const
	{
		if( m_soundEmitter == nullptr )
		{
			return 0.f;
		}

		float pos = SOUND_SERVICE()
			->getDuration( m_soundEmitter );

		return pos;
	}
	//////////////////////////////////////////////////////////////////////////
	float Amplifier::getPosMs() const
	{
		if( m_soundEmitter == nullptr )
		{
			return 0.f;
		}

		float pos = SOUND_SERVICE()
            ->getPosMs( m_soundEmitter );
		
		return pos;
	}
	//////////////////////////////////////////////////////////////////////////
	void Amplifier::setPosMs( float _posMs )
	{
		if( m_soundEmitter == nullptr )
		{
            return;
        }
		
        SOUND_SERVICE()
            ->setPosMs( m_soundEmitter, _posMs );
	}
	//////////////////////////////////////////////////////////////////////////
    void Amplifier::_stopService()
    {
        this->stopMusic();
    }
    //////////////////////////////////////////////////////////////////////////
}
