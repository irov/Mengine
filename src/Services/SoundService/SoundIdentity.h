#pragma once

#include "Interface/SoundIdentityInterface.h"

#include "Kernel/Factorable.h"

#include "ThreadWorkerSoundBufferUpdate.h"

namespace Mengine
{
    //////////////////////////////////////////////////////////////////////////
    class SoundIdentity
        : public SoundIdentityInterface
    {
        DECLARE_FACTORABLE( SoundIdentity );

    public:
        SoundIdentity();
        ~SoundIdentity() override;

    public:
        bool initialize( const SoundSourceInterfacePtr & _source, ESoundSourceCategory _category, bool _streamable, bool _turn ) override;
        void finalize() override;

    public:
        UniqueId getId() const override;

    public:
        const SoundSourceInterfacePtr & getSoundSource() const override;

        void setSoundListener( const SoundListenerInterfacePtr & _listener ) override;
        const SoundListenerInterfacePtr & getSoundListener() const override;
        SoundListenerInterfacePtr popSoundListener() override;

    public:
        void setWorkerUpdateBuffer( const ThreadWorkerInterfacePtr & _worker ) override;
        const ThreadWorkerInterfacePtr & getWorkerUpdateBuffer() const override;

    public:
        void setWorkerId( UniqueId _id ) override;
        UniqueId getWorkerId() const override;

    public:
        void setStreamable( bool _value ) override;
        bool getStreamable() const override;
        
        void setLoop( bool _value ) override;
        bool getLoop() const override;

        void setTurn( bool _turn ) override;
        bool getTurn() const override;

    public:
        ESoundSourceCategory getCategory() const override;

    public:
        void setState( ESoundSourceState _state ) override;
        ESoundSourceState getState() const override;

    public:
        void setTimeLeft( float _time ) override;
        float getTimeLeft() const override;

    public:
        const MixerValueInterfacePtr & getMixerVolume() const override;

    protected:
        UniqueId m_id;

        SoundSourceInterfacePtr m_source;
        SoundListenerInterfacePtr m_listener;

        ThreadWorkerInterfacePtr m_worker;
        UniqueId m_workerId;

        float m_timeLeft;
        MixerValueInterfacePtr m_mixerVolume;

        ESoundSourceState m_state;
        ESoundSourceCategory m_category;

        bool m_streamable;
        bool m_loop;
        bool m_turn;
    };
    //////////////////////////////////////////////////////////////////////////
    typedef IntrusivePtr<SoundIdentity, SoundIdentityInterface> SoundIdentityPtr;
    //////////////////////////////////////////////////////////////////////////
}