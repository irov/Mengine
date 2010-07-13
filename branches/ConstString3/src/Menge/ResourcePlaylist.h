#	pragma once

#	include "ResourceReference.h"
#	include "Loadable.h"

#	include <vector>

//class FileDataInterface;

namespace Menge
{
	//! ResourcePlaylist - xml - ����, ������� ������ ������ ���� ������ � ������� �������������. ��� ����� ��� ��� �����.

    /*! xml - ���� ����� ��������� ���������:
	*
	* <Tracks>
	*	<Loop Value = "1/0"/>
	*	<Track File = "���_�����0"/>
	*  ...
	*	<Track File = "���_�����N"/>
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

	public:
		void loader( BinParser * _parser ) override;
		void loaderTrack_( BinParser * _parser );

	private:
		bool m_loop;
		bool m_shuffle;

		TVectorTrackDesc m_tracks;
	};
}
