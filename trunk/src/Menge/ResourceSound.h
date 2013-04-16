#	pragma once

#	include "Kernel/ResourceReference.h"

namespace Menge
{
	class SoundBufferInterface;

	class ResourceSound
		: public ResourceReference
	{
		RESOURCE_DECLARE( ResourceSound )

	public:
		ResourceSound();

	public:
		void setPath( const FilePath& _path );
		const FilePath& getPath() const;
				
		void setCodec( const ConstString& _codec );
		void setConverter( const ConstString& _converter );

	public:
		bool isStreamable() const;		
		float getDefaultVolume() const;

	public:
		bool _loader( const Metabuf::Metadata * _parser ) override;
        bool _convert() override;

	public:
		SoundBufferInterface * createSoundBuffer() const;
        void releaseSoundBuffer( SoundBufferInterface * _soundBufer ) const;

	protected:
		bool _compile() override;
		void _release() override;

	protected:
		bool convert_();

	protected:
		FilePath m_path;

		ConstString m_codec;
		ConstString m_converter;
		float m_defaultVolume;

		bool m_isStreamable;
		SoundBufferInterface * m_soundBuffer;
	};
}
