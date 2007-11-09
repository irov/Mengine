#	include "Amplifier.h"

#	include "Playlist.h"

#	include "XmlParser/XmlParser.h"

#	include "FileEngine.h"

#	include "SoundEngine.h"

#	include "ResourcePlaylist.h"

#	include "ResourceManager.h"

namespace Menge
{
	//////////////////////////////////////////////////////////////////////////
	Amplifier::Amplifier()
	: m_music(0)
	, m_sampleMusic(0)
	, m_currentPlayList(0)
	, m_volume( 1.0f )
	, m_changeTrack( false )		// for FIX bug SQUALL :(
	{}
	//////////////////////////////////////////////////////////////////////////
	Amplifier::~Amplifier()
	{
		for each( const TPlayListMap::value_type & it in m_playLists )
		{
			delete it.second;
		}
	}
	//////////////////////////////////////////////////////////////////////////
	void	Amplifier::play( const std::string & _playlist )
	{
		TPlayListMap::iterator it = m_playLists.find( _playlist );

		if ( it == m_playLists.end() )
		{			
			ResourcePlaylist * playlistResource = 
				Holder<ResourceManager>::hostage()
					->getResourceT<ResourcePlaylist>( _playlist );

			if( playlistResource == NULL )
			{
				printf("Amplifier: no found playlist with name %s \n", _playlist.c_str() );
				return;
			}

			Playlist * playlist = new Playlist( playlistResource );

			it = m_playLists.insert( std::make_pair( _playlist, playlist ) ).first;
		}

		m_currentPlayList = it->second;
		m_currentPlayList->first();

		play_();
	}
	//////////////////////////////////////////////////////////////////////////
	void	Amplifier::stop()
	{
		if( m_music )
		{
			m_music->stop();
			release_();
		}
	}
	//////////////////////////////////////////////////////////////////////////
	void	Amplifier::listenStopped()
	{
		m_changeTrack = !m_currentPlayList->isEnded();
	}
	//////////////////////////////////////////////////////////////////////////
	void	Amplifier::listenPaused( bool _pause )
	{
	}
	//////////////////////////////////////////////////////////////////////////
	void	Amplifier::setVolume( float _volume )
	{
		m_volume = _volume * Holder<SoundEngine>::hostage()->getCommonVolume();

		if( m_music )
		{
			m_music->setVolume( _volume );
		}
	}
	//////////////////////////////////////////////////////////////////////////
	float	Amplifier::getVolume() const
	{
		return m_volume;
	}
	//////////////////////////////////////////////////////////////////////////
	void	Amplifier::update( float _timing )
	{
		if( m_changeTrack == true ) //SQUALL bug :(
		{
			play_();
		}
	}
	//////////////////////////////////////////////////////////////////////////
	void	Amplifier::play_()
	{
		release_();

		const std::string & filename = m_currentPlayList->getTrackName();

		m_sampleMusic = Holder<SoundEngine>::hostage()->createSoundBufferFromFile( filename.c_str(), true );

		if( m_sampleMusic == 0 )
		{
			printf("Amplifier can't load sample '%s'\n"
				, filename.c_str() 
				);

			return;			
		}

		m_music = Holder<SoundEngine>::hostage()->createSoundSource( false, m_sampleMusic, this );

		if( m_music == 0 )
		{
			printf("Warning: Amplifier '%s' can't create sound source\n"
				, filename.c_str()
				);

			return;
		}

		m_music->play();
		m_music->setVolume( m_volume );

		m_currentPlayList->next();

		m_changeTrack = false;
	}
	//////////////////////////////////////////////////////////////////////////
	void	Amplifier::release_()
	{
		Holder<SoundEngine>::hostage()->releaseSoundSource( m_music);
		Holder<SoundEngine>::hostage()->releaseSoundBuffer( m_sampleMusic );
		m_music = NULL;
		m_sampleMusic = NULL;
	}
	//////////////////////////////////////////////////////////////////////////
}