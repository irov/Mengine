#pragma once

#include "Interface/ServiceInterface.h"
#   include "Interface/SoundCodecInterface.h"

#include "Config/Typedef.h"

#include "Factory/Factorable.h"

#include "Core/ConstString.h"
#include "Core/FilePath.h"

#   include "math/vec3.h"

namespace Mengine
{
    enum ESoundSourceType
    {
        ESST_SOUND,
        ESST_MUSIC,
        ESST_VOICE
    };
    //////////////////////////////////////////////////////////////////////////
	class SoundListenerInterface
        : public FactorablePtr
	{
	public:
		virtual void onSoundPause( uint32_t _soundId ) = 0;
		virtual void onSoundStop( uint32_t _soundId ) = 0;
	};
    //////////////////////////////////////////////////////////////////////////
    typedef stdex::intrusive_ptr<SoundListenerInterface> SoundListenerInterfacePtr;
    //////////////////////////////////////////////////////////////////////////
	class SoundBufferInterface
        : public ServantInterface
	{
	public:
		virtual bool update() = 0;

	public:
		virtual const SoundDecoderInterfacePtr & getDecoder() const = 0;
	};
    //////////////////////////////////////////////////////////////////////////
	typedef stdex::intrusive_ptr<SoundBufferInterface> SoundBufferInterfacePtr;
    //////////////////////////////////////////////////////////////////////////
	class SoundSourceInterface
        : public ServantInterface
	{
	public:
		virtual bool play() = 0;
		virtual void pause() = 0;
		virtual void stop() = 0;

		virtual bool isPlay() const = 0;
		virtual bool isPause() const = 0;

		virtual void setVolume( float _volume ) = 0;
		virtual float getVolume() const = 0;

		virtual void setLoop( bool _loop ) = 0;
		virtual bool getLoop() const = 0;

		virtual float getDuration() const = 0;
		virtual float getPosition() const = 0;
		virtual bool setPosition( float _posMs ) = 0;

		virtual void setSoundBuffer( const SoundBufferInterfacePtr & _soundBuffer ) = 0;
		virtual SoundBufferInterfacePtr getSoundBuffer() const = 0;
	};
    //////////////////////////////////////////////////////////////////////////
	typedef stdex::intrusive_ptr<SoundSourceInterface> SoundSourceInterfacePtr;
    //////////////////////////////////////////////////////////////////////////
	class SoundSulkCallbackInterface
	{
	public:
		virtual void blow( float _blow ) = 0;
	};
    //////////////////////////////////////////////////////////////////////////
	class SoundSystemInterface
        : public ServiceInterface
    {
        SERVICE_DECLARE("SoundSystem")

	public:
		virtual void update() = 0;

	public:
		virtual bool isSilent() const = 0;

    public:
		virtual void onTurnSound( bool _turn ) = 0;

	public:
		virtual SoundBufferInterfacePtr createSoundBuffer( const SoundDecoderInterfacePtr & _soundDecoder, bool _streamable ) = 0;

    public:
		virtual SoundSourceInterfacePtr createSoundSource( bool _isHeadMode, const SoundBufferInterfacePtr & _sample ) = 0;		
	};
    //////////////////////////////////////////////////////////////////////////
#   define SOUND_SYSTEM()\
    ((Mengine::SoundSystemInterface *)SERVICE_GET(Mengine::SoundSystemInterface))
    //////////////////////////////////////////////////////////////////////////
	class SoundVolumeProviderInterface
		: public ServantInterface
	{
	public:
		virtual void onSoundChangeVolume( float _sound, float _music, float _voice, bool _mute ) = 0;
	};
    //////////////////////////////////////////////////////////////////////////
	typedef stdex::intrusive_ptr<SoundVolumeProviderInterface> SoundVolumeProviderInterfacePtr;
    //////////////////////////////////////////////////////////////////////////
	class SoundServiceInterface
		: public ServiceInterface
	{
        SERVICE_DECLARE("SoundService")

	public:
		virtual void tick( float _time, float _timing ) = 0;

	public:
		virtual bool supportStreamSound() const = 0;

	public:
		virtual void addSoundVolumeProvider( const SoundVolumeProviderInterfacePtr & _soundVolumeProvider ) = 0;
		virtual bool removeSoundVolumeProvider(const SoundVolumeProviderInterfacePtr & _soundVolumeProvider ) = 0;

	public:
		virtual void onTurnStream( bool _turn ) = 0;
		virtual void onTurnSound( bool _turn ) = 0;

		virtual void updateVolume() = 0;

	public:
		virtual uint32_t createSoundSource( bool _isHeadMode, const SoundBufferInterfacePtr & _sample, ESoundSourceType _type, bool _streamable ) = 0;

    public:
		virtual SoundBufferInterfacePtr createSoundBufferFromFile( const ConstString& _pakName, const FilePath & _fileName, const ConstString & _codecType, bool _isStream ) = 0; 

	public:
		virtual void setSoundVolume( const ConstString & _type, float _volume, float _default ) = 0;
		virtual float getSoundVolume( const ConstString & _type ) const = 0;
		virtual float mixSoundVolume() const = 0;

		virtual void setCommonVolume( const ConstString & _type, float _volume, float _default ) = 0;
		virtual float getCommonVolume( const ConstString & _type ) const = 0;
		virtual float mixCommonVolume() const = 0;

		virtual void setMusicVolume( const ConstString & _type, float _volume, float _default ) = 0;
		virtual float getMusicVolume( const ConstString & _type ) const = 0;
		virtual float mixMusicVolume() const = 0;

		virtual void setVoiceVolume( const ConstString & _type, float _volume, float _default ) = 0;
        virtual float getVoiceVolume( const ConstString & _type ) const = 0;
		virtual float mixVoiceVolume() const = 0;

    public:
		virtual bool setSourceVolume( uint32_t _emitter, float _volume, float _default ) = 0;
		virtual float getSourceVolume( uint32_t _emitter ) const = 0;

	public:
		virtual bool setSourceMixerVolume( uint32_t _emitter, const ConstString & _mixer, float _volume, float _default ) = 0;
		virtual float getSourceMixerVolume( uint32_t _emitter, const ConstString & _mixer ) const = 0;

	public:
		virtual bool releaseSoundSource( uint32_t _sourceID ) = 0;
		virtual bool validSoundSource( uint32_t _sourceID ) const = 0;

	public:
		virtual void setSourceListener( uint32_t _emitter, const SoundListenerInterfacePtr & _listener ) = 0;

	public:
		virtual bool play( uint32_t _emitter ) = 0;
		virtual bool pause( uint32_t _emitter ) = 0;
		virtual bool resume( uint32_t _emitter ) = 0;
		virtual bool stop( uint32_t _emitter ) = 0;

	public:
		virtual bool setLoop( uint32_t _emitter, bool _looped ) = 0;
		virtual bool getLoop( uint32_t _emitter ) const = 0;

	public:
		virtual float getDuration( uint32_t _emitter ) const = 0;
		
		virtual bool setPosMs( uint32_t _emitter, float _pos ) = 0;
		virtual float getPosMs( uint32_t _emitter ) = 0;
		
    public:
		virtual void mute( bool _mute ) = 0;
		virtual bool isMute() const = 0;
	};
    //////////////////////////////////////////////////////////////////////////
#   define SOUND_SERVICE()\
    ((Mengine::SoundServiceInterface *)SERVICE_GET(Mengine::SoundServiceInterface))
}