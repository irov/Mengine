#	include "Amplifier.h"

#	include "Playlist.h"

#	include "SoundEngine.h"

#	include "Application.h"

#	include "ResourcePlaylist.h"

#	include "LogEngine.h"

#	include "ResourceManager.h"

namespace Menge
{
	//////////////////////////////////////////////////////////////////////////
	Amplifier::Amplifier()
	: m_music(0)
	, m_buffer(0)
	, m_currentPlayList(0)
	, m_volume( 1.0f )
	{}
	//////////////////////////////////////////////////////////////////////////
	Amplifier::~Amplifier()
	{
		if(m_music)
		{
			m_music->setSoundNodeListener(NULL);
		}

		for( TMapPlayList::iterator 
			it = m_mapPlayLists.begin(),
			it_end = m_mapPlayLists.end();
		it != it_end;
		++it)
		{
			delete it->second;
		}
	}
	//////////////////////////////////////////////////////////////////////////
	void Amplifier::play( const std::string & _playlist )
	{
		bool enabled = Holder<Application>::hostage()->getSoundEnabled();
		if( !enabled )
		{
			return;
		}
		TMapPlayList::iterator it = m_mapPlayLists.find( _playlist );

		if ( it == m_mapPlayLists.end() )
		{			
			ResourcePlaylist * playlistResource = 
				Holder<ResourceManager>::hostage()
					->getResourceT<ResourcePlaylist>( _playlist );

			if( playlistResource == NULL )
			{
				MENGE_LOG("Amplifier: no found playlist with name %s \n", _playlist.c_str() );
				return;
			}

			Playlist * playlist = new Playlist( playlistResource );

			it = m_mapPlayLists.insert( std::make_pair( _playlist, playlist ) ).first;
		}

		m_currentPlayList = it->second;
		m_currentPlayList->first();

		_play();
	}
	//////////////////////////////////////////////////////////////////////////
	void Amplifier::stop()
	{
		if( m_music )
		{
			m_music->stop();
			_release();
		}
	}
	//////////////////////////////////////////////////////////////////////////
	void Amplifier::listenStopped()
	{
		_play();	//	play next track in playlist
	}
	//////////////////////////////////////////////////////////////////////////
	void Amplifier::listenPaused( bool _pause )
	{
	}
	//////////////////////////////////////////////////////////////////////////
	void Amplifier::setVolume( float _volume )
	{
		float commonValue = Holder<SoundEngine>::hostage()->getCommonVolume();
		m_volume = _volume * commonValue;

		if( m_music )
		{
			m_music->setVolume( m_volume );
		}
	}
	//////////////////////////////////////////////////////////////////////////
	float Amplifier::getVolume() const
	{
		return m_volume;
	}
	//////////////////////////////////////////////////////////////////////////
	void Amplifier::_play()
	{
		_release();

		const std::string & filename = m_currentPlayList->getTrack();

		m_buffer = Holder<SoundEngine>::hostage()->createSoundBufferFromFile( filename.c_str(), true );

		if( m_buffer == 0 )
		{
			MENGE_LOG("Warning: Amplifier can't load sample '%s'\n"
				, filename.c_str() 
				);

			return;			
		}

		m_music = Holder<SoundEngine>::hostage()->createSoundSource( false, m_buffer, this );

		if( m_music == 0 )
		{
			MENGE_LOG("Warning: Amplifier '%s' can't create sound source\n"
				, filename.c_str()
				);

			return;
		}

		m_music->setVolume( m_volume );
		m_music->play();

		m_currentPlayList->next();
	}
	//////////////////////////////////////////////////////////////////////////
	void Amplifier::_release()
	{
		Holder<SoundEngine>::hostage()->releaseSoundSource( m_music );
		Holder<SoundEngine>::hostage()->releaseSoundBuffer( m_buffer );
		m_music = NULL;
		m_buffer = NULL;
	}
	//////////////////////////////////////////////////////////////////////////
}