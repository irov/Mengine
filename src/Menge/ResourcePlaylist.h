#	pragma once

#	include "ResourceReference.h"

#	include <vector>

class FileDataInterface;

namespace Menge
{
	typedef	std::vector<std::string> TVecTrack;

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
		ResourcePlaylist( const std::string & _name );

	public:
		void loader( TiXmlElement * _xml ) override;

	public:
		//! ���������� ������ ������.
		/*!
		\return ������ ������
		*/
		const TVecTrack & getTracks() const;

		//! ���������� ��� _track �����
		/*!
		\param _track ������ �����
		\return ��� �����
		*/
		const std::string & getTrack( size_t _track ) const;

		//! ���������� ���� ������������
		/*!
		\return ���� ������������
		*/
		bool getLoop() const;

	protected:
		bool _compile() override;
		void _release() override;

	private:
		bool	  m_loop;
		TVecTrack m_tracks;

		std::string m_filename;
	};
}