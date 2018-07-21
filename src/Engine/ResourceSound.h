#pragma once

#include "Interface/SoundSystemInterface.h"

#include "Kernel/Resource.h"
#include "Kernel/ResourceCacher.h"

namespace Mengine
{
	class ResourceSound
		: public Resource
	{
		DECLARE_VISITABLE( Resource );

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

	protected:
		bool _compile() override;
		void _release() override;

    protected:
        void _debugIncrementReference() override;
        void _debugDecrementReference() override;

	protected:
		FilePath m_filePath;

		ConstString m_codecType;
		ConstString m_converterType;
		float m_defaultVolume;

		SoundBufferInterfacePtr m_soundBufferNoStreamableCache;

		bool m_isStreamable;
	};
	//////////////////////////////////////////////////////////////////////////
	typedef IntrusiveResourcePtr<ResourceSound> ResourceSoundPtr;
}
