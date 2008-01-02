#	pragma once

#	include "ResourceReference.h"

class SoundBufferInterface;

namespace Menge
{
	//! ResourceSound - ������-����, ������� �������� �������� ������.

    /*! xml - ���� ����� ��������� ���������:
	* <Resource Name = "���_�������" Type = "ResourceSound" >
	*	<File Path = "���_�����"/>
	*	<IsStreamable Value = "1/0"/>
	* </Resource>
	*/

	class ResourceSound
		: public ResourceReference
	{
		RESOURCE_DECLARE( ResourceSound )

	public:
		//! �����������.
		/*!
		\param _name ��� �������.
		*/
		ResourceSound( const std::string & _name );

	public:
		void loader( XmlElement * _xml ) override;

	public:
		//! ���������� �������� ������ � �������.
		/*!
		\return �������� ������
		*/
		SoundBufferInterface * get();

	protected:
		bool _compile() override;
		void _release() override;

	private:
		std::string	m_filename;
		bool m_isStreamable;
		SoundBufferInterface * m_interface;
	};
}