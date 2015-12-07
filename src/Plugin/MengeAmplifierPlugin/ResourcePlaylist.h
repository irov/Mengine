#	pragma once

#	include "Kernel/ResourceReference.h"

#	include <stdex/stl_vector.h>

namespace Menge
{
	//////////////////////////////////////////////////////////////////////////
	struct TrackDesc
	{
		ConstString path;
		ConstString codec;
		float volume;
		bool external;
	};
	//////////////////////////////////////////////////////////////////////////
	typedef stdex::vector<TrackDesc> TVectorTrackDesc;
	//////////////////////////////////////////////////////////////////////////
	class ResourcePlaylist
		: public ResourceReference
	{
		RESOURCE_DECLARE( ResourcePlaylist )

	public:
		ResourcePlaylist();

	public:
		const TVectorTrackDesc & getTracks() const;
		const TrackDesc * getTrack( uint32_t _track ) const;

		bool getLoop() const;
		bool getShuffle() const;

	public:
		bool _loader( const Metabuf::Metadata * _parser ) override;

	private:
		bool m_loop;
		bool m_shuffle;

		TVectorTrackDesc m_tracks;
	};
	//////////////////////////////////////////////////////////////////////////
	typedef stdex::intrusive_ptr<ResourcePlaylist> ResourcePlaylistPtr;
}
