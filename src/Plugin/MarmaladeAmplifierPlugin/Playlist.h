#	pragma once

#	include "Config/Typedef.h"
#	include "Core/ConstString.h"

#	include "ResourcePlaylist.h"

#	include <vector>

namespace	Menge
{
	class Playlist
	{
	public:
		Playlist( ServiceProviderInterface * _serviceProvider );
		~Playlist();

    public:
        bool initialize( const ConstString & _playlistResource );

	public:
		void setLooped( bool _loop );
		bool getLooped() const;

		bool isEnded() const;


		void next();
		void previous();
		
		void first();
		void last();

		void shuffle();
		
		const TrackDesc * getTrack() const;
        
		uint32_t currentTrackIndex() const;
		uint32_t numTracks() const;
		const TrackDesc * getTrackByIndex( uint32_t _index );

		void setTrack( uint32_t _index );
		void setLooped1( bool _loop );

		const ConstString & getCategory() const;

	private:
        ServiceProviderInterface * m_serviceProvider;
		ResourcePlaylist * m_playlistResource;

		TVectorTrackDesc m_tracks;
		uint32_t m_trackIndex;
		
		bool m_loop;

		bool m_oneTrackPlayed;
		bool m_oneTrackLooped;
		ConstString m_category;
	};

};
