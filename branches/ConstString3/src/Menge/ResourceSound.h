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
		void setPath( const ConstString& _path );
		void setCodec( const ConstString& _path );

	public:
		bool isStreamable() const;
		const ConstString& getFilename() const;
		float getDefaultVolume() const;

	public:
		void loader( BinParser * _parser ) override;

	public:
		SoundBufferInterface * get();

	protected:
		bool _compile() override;
		void _release() override;

	protected:
		ConstString m_path;
		ConstString m_codec;
		float m_defaultVolume;

		bool m_isStreamable;
		SoundBufferInterface * m_interface;
	};
}
