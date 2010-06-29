#	pragma once

#	include "Config/Typedef.h"
#	include "ConstManager.h"

#	include <vector>

namespace	Menge
{
	class ResourcePlaylist;

	//! Playlist - список имен треков. Использует ресурс ResourcePlaylist для загрузки имен треков и определения зацикленности.
	/*! 
	* 
	*/

	struct TrackDesc
	{
		String path;
		ConstString codec;
	};

	typedef std::vector<TrackDesc> TVectorTrackDesc;

	class Playlist
	{
	public:
		Playlist( ResourcePlaylist * _resource = NULL );
		~Playlist();
	public:
		void setLooped( bool _loop );
		bool getLooped() const;

		bool isEnded() const;


		void next();
		void previous();
		
		void first();
		void last();

		void shuffle();
		
		TrackDesc * getTrack() const;

		bool setPlaylistResource( ResourcePlaylist * _resource );

		std::size_t	numTracks() const;
		const TrackDesc & getTrackByIndex( std::size_t _index );

		void setTrack(std::size_t _index);
		void setLooped1( bool _loop );

		const ConstString& getCategory() const;

	private:
		ResourcePlaylist * m_playlistResource;
		
		TVectorTrackDesc m_tracks;
		TVectorTrackDesc::iterator m_track;
		
		bool m_loop;

		bool m_oneTrackPlayed;
		bool m_oneTrackLooped;
		ConstString m_category;
	};

};
