#	pragma once

#	include "Config/Typedef.h"
#	include "Core/ConstString.h"

#	include "ResourcePlaylist.h"

#	include <stdex/stl_vector.h>

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
        
		size_t currentTrackIndex() const;
		size_t numTracks() const;
		const TrackDesc * getTrackByIndex( size_t _index );

		void setTrack(size_t _index);
		void setLooped1( bool _loop );

		const ConstString& getCategory() const;

	private:
        ServiceProviderInterface * m_serviceProvider;
		ResourcePlaylist * m_playlistResource;

		TVectorTrackDesc m_tracks;
		size_t m_trackIndex;
		
		bool m_loop;

		bool m_oneTrackPlayed;
		bool m_oneTrackLooped;
		ConstString m_category;
	};

};
