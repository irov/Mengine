#	pragma once

#	include "Kernel/ResourceReference.h"

#	include <vector>

//class FileDataInterface;

namespace Menge
{
	struct TrackDesc
	{
		FilePath path;
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
		bool _loader( const Metabuf::Metadata * _parser ) override;

	private:
		bool m_loop;
		bool m_shuffle;

		TVectorTrackDesc m_tracks;
	};
}
