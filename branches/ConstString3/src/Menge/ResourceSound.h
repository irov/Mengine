#	pragma once

#	include "ResourceReference.h"

namespace Menge
{
	class SoundBufferInterface;

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
		ResourceSound();

	public:
		void setPath( const String& _path );
		void setCodec( const ConstString& _path );

	public:
		bool isStreamable() const;
		const String& getFilename() const;

	public:
		void loader( XmlElement * _xml ) override;

	public:
		SoundBufferInterface * get();

	protected:
		bool _compile() override;
		void _release() override;

	protected:
		String m_path;
		ConstString m_codec;
		bool m_isStreamable;
		SoundBufferInterface * m_interface;
	};
}
