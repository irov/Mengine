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

		ResourceMusicPtr resourceMusic = RESOURCE_SERVICE( m_serviceProvider )
			->getResourceReferenceT<ResourceMusicPtr>( _resourceMusic );

		if( resourceMusic == nullptr )
		{
			LOGGER_ERROR( m_serviceProvider )("Amplifier::playMusic can't found resource '%s'"
				, _resourceMusic.c_str()
				);

			return false;
		}

		const ConstString & category = resourceMusic->getCategory();
		const FilePath & path = resourceMusic->getPath();
		const ConstString & codec = resourceMusic->getCodec();
		bool external = resourceMusic->isExternal();
		float volume = resourceMusic->getVolume();

		FileGroupInterfacePtr fileGroup = FILE_SERVICE( m_serviceProvider )
			->getFileGroup( category );

		if( fileGroup == nullptr )
		{
			LOGGER_ERROR( m_serviceProvider )("Amplifier::playMusic can't found file group '%s'"
				, category.c_str()
				);

			return false;
		}

		SoundBufferInterfacePtr buffer;

		if( fileGroup->isPacked() == false || external == false )
		{
			buffer = SOUND_SERVICE( m_serviceProvider )
				->createSoundBufferFromFile( category, path, codec, true );
		}
		else
		{
			buffer = SOUND_SERVICE( m_serviceProvider )
				->createSoundBufferFromFile( ConstString::none(), path, codec, true );
		}

		if( buffer == nullptr )
		{
			LOGGER_ERROR( m_serviceProvider )("Amplifier::playMusic can't load sample '%s'"
				, path.c_str()
				);

			return false;
		}

		m_sourceID = SOUND_SERVICE( m_serviceProvider )
			->createSoundSource( false, buffer, ESST_MUSIC, true );

		if( m_sourceID == 0 )
		{
			LOGGER_ERROR( m_serviceProvider )("Amplifier::playMusic can't create sound source '%s'"
				, path.c_str()
				);

			return false;
		}

		if( SOUND_SERVICE( m_serviceProvider )
			->setSourceVolume( m_sourceID, volume, 0.f ) == false )
		{
			LOGGER_ERROR( m_serviceProvider )("Amplifier::playMusic can't set sound '%s' volume '%f'"
				, path.c_str()
				, volume
				);

			return false;
		}

		if( SOUND_SERVICE( m_serviceProvider )
			->setPosMs( m_sourceID, _pos ) == false )
		{
			LOGGER_ERROR( m_serviceProvider )("Amplifier::playMusic can't set sound '%s' pos '%f'"
				, path.c_str()
				, _pos
				);

			return false;
		}

		if( SOUND_SERVICE( m_serviceProvider )
			->setLoop( m_sourceID, _looped ) == false )
		{
			LOGGER_ERROR( m_serviceProvider )("Amplifier::playMusic can't set sound '%s' lood '%d'"
				, path.c_str()
				, _looped
				);

			return false;
		}

		if( SOUND_SERVICE( m_serviceProvider )->play( m_sourceID ) == false )
		{
			LOGGER_ERROR( m_serviceProvider )("Amplifier::playMusic '%s' invalid play %d"
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

			SOUND_SERVICE(m_serviceProvider)
                ->stop( sourceId );
		
			SOUND_SERVICE(m_serviceProvider)
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

		SOUND_SERVICE(m_serviceProvider)
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

        SOUND_SERVICE(m_serviceProvider)
            ->play( m_sourceID );

		return true;
	}
	//////////////////////////////////////////////////////////////////////////
	float Amplifier::getLengthMs() const
	{
		if( m_sourceID == 0 )
		{
			return 0.f;
		}

		float pos = SOUND_SERVICE( m_serviceProvider )
			->getLengthMs( m_sourceID );

		return pos;
	}
	//////////////////////////////////////////////////////////////////////////
	float Amplifier::getPosMs() const
	{
		if( m_sourceID == 0 )
		{
			return 0.f;
		}

		float pos = SOUND_SERVICE(m_serviceProvider)
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
		
        SOUND_SERVICE(m_serviceProvider)
            ->setPosMs( m_sourceID, _posMs );
	}
	//////////////////////////////////////////////////////////////////////////
}
