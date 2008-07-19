#	include "Playlist.h"

#	include "ResourcePlaylist.h"

#	include "ResourceManager.h"

#	include "Utils.h"

namespace Menge
{
	//////////////////////////////////////////////////////////////////////////
	Playlist::Playlist( ResourcePlaylist * _resource )
		: m_loop( false )
		, m_playlistResource( _resource )
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
		const std::vector<std::string> & tracks = m_playlistResource->getTracks();

		std::copy( tracks.begin(), tracks.end(), std::back_inserter( m_tracks ) );

		bool is_looped = m_playlistResource->getLoop();
		
		setLooped( is_looped );

		bool is_shuffled = m_playlistResource->getShuffle();

		if(is_shuffled == true)
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
	bool Playlist::getLooped() const
	{
		return m_loop;
	}
	//////////////////////////////////////////////////////////////////////////
	bool Playlist::isEnded() const
	{
		return m_track == m_tracks.end() && m_loop == false;
	}
	//////////////////////////////////////////////////////////////////////////
	void Playlist::next()
	{
		if ( ++m_track == m_tracks.end() && m_loop == true )
		{
			m_track = m_tracks.begin();
		}
	}
	//////////////////////////////////////////////////////////////////////////
	void Playlist::first()
	{
		m_track = m_tracks.begin();
	}
	//////////////////////////////////////////////////////////////////////////
	const std::string &	Playlist::getTrack() const
	{
		if( m_track == m_tracks.end() )
		{
			return emptyString();
		}

		return	*m_track;
	}
	//////////////////////////////////////////////////////////////////////////
	void Playlist::shuffle()
	{
		for( TVecTrack::size_type 
			it = 0,
			it_end = m_tracks.size(); 
		it != it_end; 
		++it ) 
		{
			TVecTrack::size_type rnd = rand() % it_end;
			std::swap( m_tracks[it], m_tracks[rnd] );
		}

		first();
	}
	//////////////////////////////////////////////////////////////////////////
	void Playlist::previos()
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
}