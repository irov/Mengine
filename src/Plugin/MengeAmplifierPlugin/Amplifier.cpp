#	include "Amplifier.h"

#   include "Interface/ResourceInterface.h"
#   include "Interface/StringizeInterface.h"

#	include "Kernel/ResourceMusic.h"

#   include "Logger/Logger.h"

#	include <cmath>

//////////////////////////////////////////////////////////////////////////
SERVICE_FACTORY( Amplifier, Menge::Amplifier);
//////////////////////////////////////////////////////////////////////////
namespace Menge
{
	//////////////////////////////////////////////////////////////////////////
	Amplifier::Amplifier()
		: m_sourceID(0)
		, m_play(false)
	{
	}
	//////////////////////////////////////////////////////////////////////////
	Amplifier::~Amplifier()
	{
	}
	//////////////////////////////////////////////////////////////////////////
	bool Amplifier::_initialize()
	{
		return true;
	}
	//////////////////////////////////////////////////////////////////////////
	void Amplifier::_finalize()
	{
		this->stop();
	}
	//////////////////////////////////////////////////////////////////////////
	bool Amplifier::playMusic( const ConstString& _resourceMusic, float _pos, bool _looped )
	{
		if( m_play == true )
		{
			this->stop();
		}

		ResourceMusicPtr resourceMusic = RESOURCE_SERVICE()
			->getResourceReferenceT<ResourceMusicPtr>( _resourceMusic );

		if( resourceMusic == nullptr )
		{
			LOGGER_ERROR("Amplifier::playMusic can't found resource '%s'"
				, _resourceMusic.c_str()
				);

			return false;
		}

		const ConstString & category = resourceMusic->getCategory();
		const FilePath & path = resourceMusic->getPath();
		const ConstString & codec = resourceMusic->getCodec();
		bool external = resourceMusic->isExternal();
		float volume = resourceMusic->getVolume();

		FileGroupInterfacePtr fileGroup = FILE_SERVICE()
			->getFileGroup( category );

		if( fileGroup == nullptr )
		{
			LOGGER_ERROR("Amplifier::playMusic can't found file group '%s'"
				, category.c_str()
				);

			return false;
		}

		SoundBufferInterfacePtr buffer;

		if( fileGroup->isPacked() == false || external == false )
		{
			buffer = SOUND_SERVICE()
				->createSoundBufferFromFile( category, path, codec, true );
		}
		else
		{
			buffer = SOUND_SERVICE()
				->createSoundBufferFromFile( ConstString::none(), path, codec, true );
		}

		if( buffer == nullptr )
		{
			LOGGER_ERROR("Amplifier::playMusic can't load sample '%s'"
				, path.c_str()
				);

			return false;
		}

		m_sourceID = SOUND_SERVICE()
			->createSoundSource( false, buffer, ESST_MUSIC, true );

		if( m_sourceID == 0 )
		{
			LOGGER_ERROR("Amplifier::playMusic can't create sound source '%s'"
				, path.c_str()
				);

			return false;
		}

		if( SOUND_SERVICE()
			->setSourceVolume( m_sourceID, volume, 0.f ) == false )
		{
			LOGGER_ERROR("Amplifier::playMusic can't set sound '%s' volume '%f'"
				, path.c_str()
				, volume
				);

			return false;
		}

		if( SOUND_SERVICE()
			->setPosMs( m_sourceID, _pos ) == false )
		{
			LOGGER_ERROR("Amplifier::playMusic can't set sound '%s' pos '%f'"
				, path.c_str()
				, _pos
				);

			return false;
		}

		if( SOUND_SERVICE()
			->setLoop( m_sourceID, _looped ) == false )
		{
			LOGGER_ERROR("Amplifier::playMusic can't set sound '%s' lood '%d'"
				, path.c_str()
				, _looped
				);

			return false;
		}

		if( SOUND_SERVICE()->play( m_sourceID ) == false )
		{
			LOGGER_ERROR("Amplifier::playMusic '%s' invalid play %d"
				, path.c_str()
				, m_sourceID
				);

			return false;
		}

		m_play = true;

		return true;
	}
	//////////////////////////////////////////////////////////////////////////
	void Amplifier::stop()
	{
		m_play = false;

		if( m_sourceID != 0 )
		{
			uint32_t sourceId = m_sourceID;
			m_sourceID = 0;

			SOUND_SERVICE()
                ->stop( sourceId );
		
			SOUND_SERVICE()
				->releaseSoundSource( sourceId );
		}				
	}
	//////////////////////////////////////////////////////////////////////////
	bool Amplifier::pause()
	{
		if( m_sourceID == 0 )
		{
			return false;
		}

		m_play = false;

		SOUND_SERVICE()
            ->pause( m_sourceID );

		return true;
	}
	//////////////////////////////////////////////////////////////////////////
	bool Amplifier::resume()
	{
		if( m_sourceID == 0 )
		{
			return false;
		}

        m_play = true;

        SOUND_SERVICE()
            ->play( m_sourceID );

		return true;
	}
	//////////////////////////////////////////////////////////////////////////
	float Amplifier::getDuration() const
	{
		if( m_sourceID == 0 )
		{
			return 0.f;
		}

		float pos = SOUND_SERVICE()
			->getDuration( m_sourceID );

		return pos;
	}
	//////////////////////////////////////////////////////////////////////////
	float Amplifier::getPosMs() const
	{
		if( m_sourceID == 0 )
		{
			return 0.f;
		}

		float pos = SOUND_SERVICE()
            ->getPosMs( m_sourceID );
		
		return pos;
	}
	//////////////////////////////////////////////////////////////////////////
	void Amplifier::setPosMs( float _posMs )
	{
		if( m_sourceID == 0 )
		{
            return;
        }
		
        SOUND_SERVICE()
            ->setPosMs( m_sourceID, _posMs );
	}
	//////////////////////////////////////////////////////////////////////////
}
