#	pragma once

#	include "Interface/ServiceInterface.h"
#   include "Interface/SoundCodecInterface.h"

#	include "Config/Typedef.h"

#	include "Factory/Factorable.h"

#	include "Core/ConstString.h"
#	include "Core/FilePath.h"

#   include <Math/vec3.h>

namespace Menge
{
	class SoundSourceInterface;
	class ServiceProviderInterface;

    enum ESoundSourceType
    {
        EST_SOUND,
        EST_MUSIC,
        EST_VOICE
    };

	class SoundNodeListenerInterface
	{
	public:
		virtual void listenSoundNodePaused() = 0;
		virtual void listenSoundNodeStopped() = 0;
	};

	class SoundBufferInterface
        : public Factorable
	{
	public:
		virtual void update() = 0;
	};

	class SoundSourceInterface
        : public Factorable
	{
	public:
		virtual bool play() = 0;
		virtual void pause() = 0;
		virtual void stop() = 0;

		virtual bool isPlaying() const = 0;

		virtual void setVolume( float _volume ) = 0;
		virtual float getVolume() const = 0;

		virtual void setPosition( const mt::vec3f & _pos ) = 0;
		virtual const mt::vec3f & getPosition() const = 0;

		virtual void setLoop( bool _loop ) = 0;
		virtual bool getLoop() const = 0;

		virtual float getLengthMs() const = 0;
		virtual float getPosMs() const = 0;
		virtual bool setPosMs( float _posMs ) = 0;

		virtual void setSoundBuffer( SoundBufferInterface* _soundBuffer ) = 0;
		virtual SoundBufferInterface* getSoundBuffer() const = 0;
	};

	class SoundSulkCallbackInterface
	{
	public:
		virtual void blow( float _blow ) = 0;
	};

	class SoundSystemInterface
        : public ServiceInterface
    {
        SERVICE_DECLARE("SoundSystem")

	public:
		virtual bool initialize() = 0;
        virtual void finalize() = 0;

    public:
		virtual void onTurnSound( bool _turn ) = 0;

    public:
		virtual SoundSourceInterface * createSoundSource( bool _isHeadMode, SoundBufferInterface * _sample ) = 0;
		virtual SoundBufferInterface* createSoundBuffer( const SoundDecoderInterfacePtr & _soundDecoder, bool _isStream ) = 0;
	};

#   define SOUND_SYSTEM( serviceProvider )\
    (Menge::Helper::getService<Menge::SoundSystemInterface>(serviceProvider))

	class SoundServiceInterface
		: public ServiceInterface
	{
        SERVICE_DECLARE("SoundService")

    public:
        virtual bool initialize( bool _silent ) = 0;
        virtual void finalize() = 0;

	public:
		virtual void update( float _timing ) = 0;

	public:
		virtual void onTurnStream( bool _turn ) = 0;
		virtual void onTurnSound( bool _turn ) = 0;

	public:
		virtual unsigned int createSoundSource( bool _isHeadMode, SoundBufferInterface * _sample, ESoundSourceType _type, bool _streamable ) = 0;

    public:
		virtual SoundBufferInterface * createSoundBufferFromFile( const ConstString& _pakName, const FilePath & _filename, const ConstString & _codecType, bool _isStream ) = 0; 

	public:
		virtual void setSoundVolume( const ConstString & _type, float _volume ) = 0;
		virtual float getSoundVolume( const ConstString & _type ) const = 0;

		virtual void setCommonVolume( const ConstString & _type, float _volume ) = 0;
		virtual float getCommonVolume( const ConstString & _type ) const = 0;

		virtual void setMusicVolume( const ConstString & _type, float _volume ) = 0;
		virtual float getMusicVolume( const ConstString & _type ) const = 0;

        virtual void setVoiceVolume( const ConstString & _type, float _volume ) = 0;
        virtual float getVoiceVolume( const ConstString & _type ) const = 0;

    public:
		virtual bool setSourceVolume( unsigned int _emitter, float _volume ) = 0;
		virtual float getSourceVolume( unsigned int _emitter ) const = 0;

		virtual void releaseSoundSource( unsigned int _sourceID ) = 0;
		virtual bool validSoundSource( unsigned int _sourceID ) const = 0;

	public:
		virtual void setSourceListener( unsigned int _emitter, SoundNodeListenerInterface* _listener ) = 0;

	public:
		virtual bool play( unsigned int _emitter ) = 0;
		virtual bool pause( unsigned int _emitter ) = 0;
		virtual bool stop( unsigned int _emitter ) = 0;

	public:
		virtual void setLoop( unsigned int _emitter, bool _looped ) = 0;
		virtual bool getLoop( unsigned int _emitter ) const = 0;

	public:
		virtual float getLengthMs( unsigned int _emitter ) const = 0;
		
		virtual bool setPosMs( unsigned int _emitter, float _pos ) = 0;
		virtual float getPosMs( unsigned int _emitter ) const = 0;
		
    public:
		virtual void mute( bool _mute ) = 0;
		virtual bool isMute() const = 0;

        virtual bool isSilent() const = 0;
	};

#   define SOUND_SERVICE( serviceProvider )\
    (Menge::Helper::getService<Menge::SoundServiceInterface>(serviceProvider))
}