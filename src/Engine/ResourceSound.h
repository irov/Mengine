#pragma once

#include "Interface/SoundBufferInterface.h"

#include "Kernel/Resource.h"
#include "Kernel/Content.h"

namespace Mengine
{
    class ResourceSound
        : public Resource
		, public Content
    {
        DECLARE_VISITABLE( Resource );

    public:
        ResourceSound();
        ~ResourceSound() override;

    public:
        bool isStreamable() const;
        float getDefaultVolume() const;

    public:
        bool _loader( const Metabuf::Metadata * _parser ) override;
        bool _convert() override;

    public:
        SoundBufferInterfacePtr createSoundBuffer() const;

    protected:
        bool _compile() override;
        void _release() override;

    protected:
        void _debugIncrementReference() override;
        void _debugDecrementReference() override;

    protected:
        float m_defaultVolume;

        SoundBufferInterfacePtr m_soundBufferNoStreamableCache;

        bool m_isStreamable;
    };
    //////////////////////////////////////////////////////////////////////////
    typedef IntrusiveResourcePtr<ResourceSound> ResourceSoundPtr;
}
