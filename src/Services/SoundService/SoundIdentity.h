#pragma once

#include "Interface/SoundIdentityInterface.h"

#include "Kernel/Factorable.h"

#include "ThreadWorkerSoundBufferUpdate.h"

namespace Mengine
{
    //////////////////////////////////////////////////////////////////////////
    class SoundIdentity
        : public SoundIdentityInterface
        , public Factorable
    {
    public:
        SoundIdentity();
        ~SoundIdentity() override;

    public:
        bool initialize() override;
        void finalize() override;

    public:
        void setId( uint32_t _id );
        uint32_t getId() const override;

    public:
        void setSoundSource( const SoundSourceInterfacePtr & _source ) override;
        const SoundSourceInterfacePtr & getSoundSource() const override;

        void setSoundListener( const SoundListenerInterfacePtr & _listener ) override;
        const SoundListenerInterfacePtr & getSoundListener() const override;

    public:
        bool isStreamable() const override;
        bool getLoop() const override;

    public:
        ESoundSourceCategory getCategory() const override;
        ESoundSourceState getState() const override;
        const MixerValue & getVolume() const override;

    protected:
        uint32_t m_id;

    public:
        SoundSourceInterfacePtr source;
        SoundListenerInterfacePtr listener;

        ThreadWorkerSoundBufferUpdatePtr worker;
        uint32_t bufferId;

        float time_left;
        MixerValue volume;

        ESoundSourceState state;
        ESoundSourceCategory category;

        bool streamable;
        bool looped;
        bool turn;

#ifdef MENGINE_DEBUG
        Document doc;
#endif
    };
    //////////////////////////////////////////////////////////////////////////
    typedef IntrusivePtr<SoundIdentity, SoundIdentityInterface> SoundIdentityPtr;
    //////////////////////////////////////////////////////////////////////////
}