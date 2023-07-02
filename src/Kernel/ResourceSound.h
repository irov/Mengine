#pragma once

#include "Interface/SoundBufferInterface.h"

#include "Kernel/Resource.h"

namespace Mengine
{
    //////////////////////////////////////////////////////////////////////////
    class ResourceSound
        : public Resource
    {
        DECLARE_FACTORABLE( ResourceSound );
        DECLARE_VISITABLE( Resource );

    public:
        ResourceSound();
        ~ResourceSound() override;

    public:
        void setStreamable( bool _streamable );
        bool isStreamable() const;

        void setDefaultVolume( float _defaultVolume );
        float getDefaultVolume() const;

    public:
        SoundBufferInterfacePtr createSoundBuffer( const DocumentInterfacePtr & _doc ) const;

    protected:
        bool _compile() override;
        void _release() override;

    protected:
        float m_defaultVolume;

        SoundBufferInterfacePtr m_soundBufferNoStreamableCache;

        bool m_isStreamable;
    };
    //////////////////////////////////////////////////////////////////////////
    typedef IntrusiveResourcePtr<ResourceSound> ResourceSoundPtr;
    //////////////////////////////////////////////////////////////////////////
}
