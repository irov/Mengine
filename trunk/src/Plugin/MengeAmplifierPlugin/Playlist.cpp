#	include "Playlist.h"

#	include "Interface/ResourceInterface.h"

#   include "Logger/Logger.h"
#	include "Core/String.h"

#	include <algorithm>

#	include <ctime>

namespace Menge
{
	//////////////////////////////////////////////////////////////////////////
	Playlist::Playlist( ServiceProviderInterface * _serviceProvider )
		: m_serviceProvider(_serviceProvider)
		, m_playlistResource(nullptr)
        , m_loop(false)		
		, m_oneTrackPlayed(false)
		, m_oneTrackLooped(false)
		, m_trackIndex(0)
	{
	}
	//////////////////////////////////////////////////////////////////////////
	Playlist::~Playlist()
	{
		if( m_playlistResource != nullptr )
		{
			m_playlistResource->decrementReference();
			m_playlistResource = nullptr;
		}
	}
	//////////////////////////////////////////////////////////////////////////
	bool Playlist::initialize( const ConstString & _playlistResource )
	{
        m_playlistResource = RESOURCE_SERVICE(m_serviceProvider)
            ->getResourceT<ResourcePlaylist>( _playlistResource );

        if( m_playlistResource == nullptr )
        {
            LOGGER_ERROR(m_serviceProvider)("Playlist::setPlaylistResource no found playlist with name '%s'"
                , _playlistResource.c_str()
                );

            return false;
        }

		m_category = m_playlistResource->getCategory();
		
		const TVectorTrackDesc & tracks = m_playlistResource->getTracks();

		m_tracks.assign( tracks.begin(), tracks.end() );
		m_trackIndex = 0;

		bool is_looped = m_playlistResource->getLoop();
		
		this->setLooped( is_looped );

		bool is_shuffled = m_playlistResource->getShuffle();

		if( is_shuffled == true )
		{
			this->shuffle();
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
		return m_trackIndex == m_tracks.size() && m_loop == false;
	}
	//////////////////////////////////////////////////////////////////////////
	void Playlist::next()
	{
		if( m_oneTrackPlayed == true )
		{
			return;
		}

		++m_trackIndex;

		if( m_trackIndex == m_tracks.size() && m_loop == true )
		{
			m_trackIndex = 0;
		}
	}
	//////////////////////////////////////////////////////////////////////////
	void Playlist::first()
	{
		m_trackIndex = 0;
		m_oneTrackPlayed = false;
	}
	//////////////////////////////////////////////////////////////////////////
	const TrackDesc * Playlist::getTrack() const
	{
		if( m_trackIndex >= m_tracks.size() )
		{
			return nullptr;
		}

		const TrackDesc & desc = m_tracks[m_trackIndex];

		return &desc;
	}
	//////////////////////////////////////////////////////////////////////////
	void Playlist::shuffle()
	{
		std::random_shuffle( m_tracks.begin(), m_tracks.end() );

		this->first();
	}
	//////////////////////////////////////////////////////////////////////////
	void Playlist::previous()
	{
		if( m_trackIndex == 0 )
		{
			if( m_tracks.empty() == true )
			{
				m_trackIndex = 0;
			}
			else
			{
				m_trackIndex = m_tracks.size() - 1;
			}
		}
		else
		{
			--m_trackIndex;
		}		
	}
	//////////////////////////////////////////////////////////////////////////
	void Playlist::last()
	{
		if( m_tracks.empty() == true )
		{
			m_trackIndex = 0;
		}
		else
		{
			m_trackIndex = m_tracks.size() - 1;
		}
	}
	//////////////////////////////////////////////////////////////////////////
	uint32_t Playlist::currentTrackIndex() const
	{
		return m_trackIndex;
	}
	//////////////////////////////////////////////////////////////////////////
	uint32_t Playlist::numTracks() const
	{
		return m_tracks.size();
	}
	//////////////////////////////////////////////////////////////////////////
	const TrackDesc * Playlist::getTrackByIndex( uint32_t _index )
	{
		if( _index >= m_tracks.size() )
		{
			return nullptr;
		}

		return &m_tracks[_index];
	}
	//////////////////////////////////////////////////////////////////////////
	void Playlist::setTrack( uint32_t _index )
	{
		if( _index >= m_tracks.size() )
		{
			this->first();
		}
		else
		{
			m_trackIndex = _index;
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
