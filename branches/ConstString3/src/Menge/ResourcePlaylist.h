#	pragma once

#	include "ResourceReference.h"

#	include <vector>

//class FileDataInterface;

namespace Menge
{
	//! ResourcePlaylist - xml - файл, который хранит список имен треков и признак зациклинности. Имя трека это имя файла.

    /*! xml - файл имеет следующую структуру:
	*
	* <Tracks>
	*	<Loop Value = "1/0"/>
	*	<Track File = "имя_файла0"/>
	*  ...
	*	<Track File = "имя_файлаN"/>
	* </Tracks>
	*	
	*/

	struct TrackDesc
	{
		String path;
		ConstString codec;
	};

	typedef std::vector<TrackDesc> TVectorTrackDesc;

	class ResourcePlaylist
		: public ResourceReference
	{
		RESOURCE_DECLARE( ResourcePlaylist )

	public:
		ResourcePlaylist();

	public:
		const TVectorTrackDesc & getTracks() const;
		const TrackDesc * getTrack( unsigned int _track ) const;

		bool getLoop() const;
		bool getShuffle() const;

		void setFilePath( const String& _path );
		const String& getFilePath() const;

	public:
		void loader( XmlElement * _xml ) override;
		void loaderTracks_( XmlElement * _xml );
		void loaderTrack_( XmlElement * _xml );

	protected:
		bool _compile() override;
		void _release() override;

	private:
		bool m_loop;
		bool m_shuffle;

		TVectorTrackDesc m_tracks;

		String m_filename;
	};
}
