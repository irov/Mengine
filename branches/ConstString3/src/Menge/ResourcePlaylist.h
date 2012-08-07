#	pragma once

#	include "ResourceReference.h"

#	include <vector>

//class FileDataInterface;

namespace Menge
{
	struct TrackDesc
	{
		WString path;
		ConstString codec;
		float volume;
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

	public:
		void loader( const Metabuf::Metadata * _parser ) override;

	private:
		bool m_loop;
		bool m_shuffle;

		TVectorTrackDesc m_tracks;
	};
}
