#	include "Playlist.h"

#	include "ResourcePlaylist.h"

#	include "ResourceManager.h"

#	include "Core/String.h"

#	include <algorithm>

#	include <ctime>

namespace Menge
{
	//////////////////////////////////////////////////////////////////////////
	Playlist::Playlist( ResourcePlaylist * _resource )
		: m_loop( false )
		, m_oneTrackPlayed( false )
		, m_oneTrackLooped( false )
		, m_playlistResource( _resource )
		, m_category("")
	{
		if( m_playlistResource ) 
		{
			setPlaylistResource( m_playlistResource );
		}
	}
	//////////////////////////////////////////////////////////////////////////
	Playlist::~Playlist()
	{
		Holder<ResourceManager>::hostage()->releaseResource( m_playlistResource );
	}
	//////////////////////////////////////////////////////////////////////////
	bool Playlist::setPlaylistResource( ResourcePlaylist * _resource )
	{
		m_category = m_playlistResource->getFactoryParams().category;
		
		const std::vector<String>& tracks = m_playlistResource->getTracks();

		std::copy( tracks.begin(), tracks.end(), std::back_inserter( m_tracks ) );

		bool is_looped = m_playlistResource->getLoop();
		
		setLooped( is_looped );

		bool is_shuffled = m_playlistResource->getShuffle();

		if( is_shuffled == true )
		{
			shuffle();
		}

		return true;
	}
	//////////////////////////////////////////////////////////////////////////
	void Playlist::setLooped( bool _loop )
	{
		m_loop = _loop;
	}
	//////////////////////////////////////////////////////////////////////////
	void Playlist::setLooped1( bool _loop )
	{
		m_oneTrackLooped = _loop;
	}
	//////////////////////////////////////////////////////////////////////////
	bool Playlist::getLooped() const
	{
		return m_oneTrackPlayed ? m_oneTrackLooped : m_loop;
	}
	//////////////////////////////////////////////////////////////////////////
	bool Playlist::isEnded() const
	{
		return m_track == m_tracks.end() && m_loop == false;
	}
	//////////////////////////////////////////////////////////////////////////
	void Playlist::next()
	{
		if ( m_oneTrackPlayed )
		{
			return;
		}

		if ( ++m_track == m_tracks.end() && m_loop == true )
		{
			m_track = m_tracks.begin();
		}
	}
	//////////////////////////////////////////////////////////////////////////
	void Playlist::first()
	{
		m_track = m_tracks.begin();
		m_oneTrackPlayed = false;
	}
	//////////////////////////////////////////////////////////////////////////
	String Playlist::getTrack() const
	{
		if( m_track == m_tracks.end() )
		{
			return Utils::emptyString();
		}

		return *m_track;
	}
	//////////////////////////////////////////////////////////////////////////
	void Playlist::shuffle()
	{
		srand( (unsigned)std::time( NULL ) );

		std::random_shuffle( m_tracks.begin(), m_tracks.end() );

		first();
	}
	//////////////////////////////////////////////////////////////////////////
	void Playlist::previous()
	{
		if( m_track == m_tracks.begin() )
		{
			m_track = m_tracks.end();
		}

		--m_track;
	}
	//////////////////////////////////////////////////////////////////////////
	void Playlist::last()
	{
		m_track = m_tracks.end() - 1;
	}
	//////////////////////////////////////////////////////////////////////////
	std::size_t	 Playlist::numTracks() const
	{
		return m_tracks.size();
	}
	//////////////////////////////////////////////////////////////////////////
	String Playlist::getTrackByIndex( std::size_t _index )
	{
		if( _index >= m_tracks.size() )
		{
			return Utils::emptyString();
		}

		return m_tracks[_index];
	}
	//////////////////////////////////////////////////////////////////////////
	void Playlist::setTrack( std::size_t _index )
	{
		const String & _name = m_tracks[_index];

		TStringVector::iterator it = std::find( m_tracks.begin(), m_tracks.end(), _name );

		if( it == m_tracks.end() )
		{
			first();
		}

		m_oneTrackPlayed = true;

		m_track = it;
	}
	//////////////////////////////////////////////////////////////////////////
	const String& Playlist::getCategory() const
	{
		return m_category;
	}
	//////////////////////////////////////////////////////////////////////////
}
