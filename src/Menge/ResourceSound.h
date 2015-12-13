#	pragma once

#	include "Interface/SoundSystemInterface.h"

#	include "Kernel/ResourceReference.h"
#	include "Kernel/ResourceCacher.h"

namespace Menge
{
	class ResourceSound
		: public ResourceReference
	{
		DECLARE_VISITABLE();

	public:
		ResourceSound();

	public:
		void setFilePath( const FilePath & _path );
		const FilePath& getFilePath() const;
				
		void setCodecType( const ConstString & _codec );
		const ConstString & getCodecType() const;
		
	public:
		bool isStreamable() const;
		float getDefaultVolume() const;

	public:
		bool _loader( const Metabuf::Metadata * _parser ) override;
        bool _convert() override;

    protected:
        bool _isValid() const override;

	public:
		SoundBufferInterfacePtr createSoundBuffer() const;
		SoundBufferInterfacePtr createSoundBufferNoCache() const;
		void destroySoundBuffer( const SoundBufferInterfacePtr & _soundBuffer ) const;

	protected:
		void _cache() override;
		void _uncache() override;

	protected:
		bool _compile() override;
		void _release() override;

	protected:
		FilePath m_filePath;

		ConstString m_codecType;
		ConstString m_converter;
		float m_defaultVolume;

		typedef ResourceCacher<SoundBufferInterfacePtr> TCacherSoundBuffer;
		mutable TCacherSoundBuffer m_soundBufferCacher;

		bool m_isStreamable;
	};
	//////////////////////////////////////////////////////////////////////////
	typedef stdex::intrusive_ptr<ResourceSound> ResourceSoundPtr;
}
