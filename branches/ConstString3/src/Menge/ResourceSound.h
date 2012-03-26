#	pragma once

#	include "ResourceReference.h"

namespace Menge
{
	class SoundBufferInterface;

	//! ResourceSound - ресурс-файл, который содержит звуковые данные.

    /*! xml - файл имеет следующую структуру:
	* <Resource Name = "имя_ресурса" Type = "ResourceSound" >
	*	<File Path = "имя_файла"/>
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
		void setCodec( const ConstString& _codec );
		void setConverter( const ConstString& _converter );
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
		bool _convert();
	protected:
		ConstString m_path;
		ConstString m_codec;
		ConstString m_converter;
		float m_defaultVolume;

		bool m_isStreamable;
		SoundBufferInterface * m_interface;
	};
}
