#	pragma once

#	include "ResourceReference.h"

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
	class ResourcePlaylist
		: public ResourceReference
	{
		RESOURCE_DECLARE( ResourcePlaylist )

	public:
		//! �����������.
		/*!
		\param _name ��� �������.
		*/
		ResourcePlaylist();

	public:
		//! ���������� ������ ������.
		/*!
		\return ������ ������
		*/
		const TVectorString & getTracks() const;

		//! ���������� ��� _track �����
		/*!
		\param _track ������ �����
		\return ��� �����
		*/
		const String& getTrack( unsigned int _track ) const;

		//! ���������� ���� ������������
		/*!
		\return ���� ������������
		*/
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
		bool	  m_loop;
		bool	  m_shuffle;
		TVectorString m_tracks;

		String m_filename;
	};
}
