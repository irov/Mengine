#	include "Playlist.h"

#	include "ResourceManager.h"

#	include "Core/String.h"

#	include <algorithm>

#	include <ctime>

namespace Menge
{
	//////////////////////////////////////////////////////////////////////////
	Playlist::Playlist( ResourcePlaylist * _resource )
		: m_loop(false)
		, m_playlistResource(_resource)    
		, m_oneTrackPlayed(false)
		, m_oneTrackLooped(false)
	{
		if( m_playlistResource ) 
		{
			setPlaylistResource( m_playlistResource );
		}
	}
	//////////////////////////////////////////////////////////////////////////
	Playlist::~Playlist()
	{
		if( m_playlistResource != 0 )
		{
			m_playlistResource->decrementReference();
			m_playlistResource = 0;
		}
	}
	//////////////////////////////////////////////////////////////////////////
	bool Playlist::setPlaylistResource( ResourcePlaylist * _resource )
	{
		m_category = m_playlistResource->getCategory();
		
		const TVectorTrackDesc & tracks = m_playlistResource->getTracks();

		m_tracks.assign( tracks.begin(), tracks.end() );

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
	const TrackDesc * Playlist::getTrack() const
	{
		if( m_track == m_tracks.end() )
		{
			return 0;
		}

		return &*m_track;
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
	size_t	 Playlist::numTracks() const
	{
		return m_tracks.size();
	}
	//////////////////////////////////////////////////////////////////////////
	const TrackDesc * Playlist::getTrackByIndex( size_t _index )
	{
		if( _index >= m_tracks.size() )
		{
			return 0;
		}

		return &m_tracks[_index];
	}
	//////////////////////////////////////////////////////////////////////////
	void Playlist::setTrack( size_t _index )
	{
		if( _index >= m_tracks.size() )
		{
			first();
		}
		else
		{
			m_track = m_tracks.begin();
			std::advance( m_track, _index );
			m_oneTrackPlayed = true;
		}
	}
	//////////////////////////////////////////////////////////////////////////
	const ConstString & Playlist::getCategory() const
	{
		return m_category;
	}
	//////////////////////////////////////////////////////////////////////////
}
