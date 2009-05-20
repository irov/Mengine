
#	include "OALSoundSystem.h"
#	include "Interface/LogSystemInterface.h"
#	include "Config/Config.h"
#	include "SulkSystem.h"

#	include "OALSoundBufferBase.h"
#	include "OALSoundBufferStream.h"
#	include "OALSoundSource.h"

#	include <algorithm>

#	define MAX_SOUND_SOURCES 32

#	ifndef MENGE_MASTER_RELEASE
#		define LOG( message )\
	if( m_logSystem ) m_logSystem->logMessage( message + String("\n"), LM_LOG );
#	else
#		define LOG( message )
#	endif

#	define LOG_ERROR( message )\
	if( m_logSystem ) m_logSystem->logMessage( message + String("\n"), LM_ERROR );

//////////////////////////////////////////////////////////////////////////
bool initInterfaceSystem( Menge::SoundSystemInterface** _interface )
{
	if( _interface == 0 )
	{
		return false;
	}

	*_interface = new Menge::OALSoundSystem();

	return true;
}
//////////////////////////////////////////////////////////////////////////
void releaseInterfaceSystem( Menge::SoundSystemInterface* _interface )
{
	if( _interface )
	{
		delete static_cast<Menge::OALSoundSystem*>( _interface );
	}
}
//////////////////////////////////////////////////////////////////////////

namespace Menge
{

	//////////////////////////////////////////////////////////////////////////
	OALSoundSystem::OALSoundSystem()
		: m_logSystem( NULL )
		, m_initialized( false )
		, m_context( NULL )
		, m_device( NULL )
		, m_sulk( NULL )
	{

	}
	//////////////////////////////////////////////////////////////////////////
	OALSoundSystem::~OALSoundSystem()
	{
		for( TSoundSourceVector::iterator it = m_soundSourcePool.begin(), it_end = m_soundSourcePool.end();
			it != it_end;
			it++ )
		{
			OALSoundSource* source = (*it);
			source = new (source) OALSoundSource( NULL );
			delete source;
		}
		m_soundSourcePool.clear();

		if( m_sulk != NULL )
		{
			delete m_sulk;
			m_sulk = NULL;
		}

		if( m_monoPool.empty() == false )
		{
			alDeleteSources( m_monoPool.size(), &(m_monoPool[0]) );
			m_monoPool.clear();
		}
		if( m_stereoPool.empty() == false )
		{
			alDeleteSources( m_stereoPool.size(), &(m_stereoPool[0]) );
			m_stereoPool.clear();
		}

		alcMakeContextCurrent( NULL );

		if( m_context )
		{
			alcDestroyContext( m_context );
			m_context = NULL;
		}

		if( m_device )
		{
			alcCloseDevice( m_device );
			m_device = NULL;
		}

		m_initialized = false;
	}
	//////////////////////////////////////////////////////////////////////////
	bool OALSoundSystem::initialize( LogSystemInterface* _logSystem )
	{
		if( m_initialized == true )
		{
			LOG_ERROR( "OALSoundSystem: system have been already initialized" );
			return false;
		}
		m_logSystem = _logSystem;
		LOG( "Starting OpenAL Sound System..." );

		//const ALCchar* str = alcGetString( NULL, ALC_DEVICE_SPECIFIER );
		m_device = alcOpenDevice( NULL );	// open default device
		if( m_device == NULL )
		{
			LOG_ERROR( "OALSoundSystem: Failed to open default sound device" );
			return false;
		}

		m_context = alcCreateContext( m_device, NULL );
		if( m_context == NULL )
		{
			LOG_ERROR( "OALSoundSystem: Failed to create context" );
			alcCloseDevice( m_device );
			m_device = NULL;
			return false;
		}

		alcMakeContextCurrent( m_context );

		LOG( "OpenAL driver properties" );
		m_logSystem->logMessage( "Version: " );
		LOG( alGetString( AL_VERSION ) );
		m_logSystem->logMessage( "Vendor: " );
		LOG( alGetString( AL_VENDOR ) );
		m_logSystem->logMessage( "Renderer: " );
		LOG( alGetString( AL_RENDERER ) );
		if( alcIsExtensionPresent(NULL, "ALC_ENUMERATION_EXT") == AL_TRUE )
		{
			m_logSystem->logMessage( "Device Specifier: " );
			LOG( alcGetString( m_device, ALC_DEVICE_SPECIFIER ) );
		}
		//LOG( alGetString( AL_EXTENSIONS ) );

		float lposition[] = { 0.0f, 0.0f, 0.0f };
		float lvelocity[] = { 0.0f, 0.0f, 0.0f };
		float lorient[] = { 0.0f, 0.0f, 1.0f, 0.0f, 1.0f, 0.0f };
		alListenerfv( AL_POSITION, lposition );
		alListenerfv( AL_VELOCITY, lvelocity );
		alListenerfv( AL_ORIENTATION, lorient );


		m_stereoPool.reserve( MAX_SOUND_SOURCES );
		m_monoPool.reserve( MAX_SOUND_SOURCES );
		bool stereo = false;
		int sourceCount = 0;

		for( int i = 0; i != MAX_SOUND_SOURCES; ++i )
		{
			ALuint sourceName = 0;
			alGenSources( 1, &sourceName );

			if( alGetError() != AL_NO_ERROR )
			{
				break;
			}

			if( stereo )
			{
				m_stereoPool.push_back( sourceName );
			}
			else
			{
				m_monoPool.push_back( sourceName );
			}

			stereo = !stereo;
		}

		m_sulk = new SulkSystem();

		m_initialized = true;

		return true;
	}

	//////////////////////////////////////////////////////////////////////////
	void OALSoundSystem::setListenerOrient( const float* _position, const float* _front, const float* _top )
	{
		if( m_initialized == false )
		{
			return;
		}

		float orient[] = { _front[0], _front[1], _front[2], _top[0], _top[1], _top[2] };
		alListenerfv( AL_POSITION, _position );
		alListenerfv( AL_ORIENTATION, orient );
	}
	//////////////////////////////////////////////////////////////////////////
	SoundSourceInterface* OALSoundSystem::createSoundSource( bool _isHeadMode, SoundBufferInterface * _sample )
	{
		OALSoundSource* soundSource = NULL;

		if( m_soundSourcePool.empty() == false )
		{
			soundSource = m_soundSourcePool.back();
			soundSource = new (soundSource) OALSoundSource( this );
			m_soundSourcePool.pop_back();
		}
		else
		{
			soundSource = new OALSoundSource( this );
		}
		
		soundSource->setHeadMode( _isHeadMode );
		soundSource->loadBuffer( _sample );

		return soundSource;
	}
	//////////////////////////////////////////////////////////////////////////
	SoundBufferInterface* OALSoundSystem::createSoundBuffer( SoundDecoderInterface* _soundDecoder, bool _isStream )
	{
		OALSoundBufferBase* buffer = NULL;
		if( m_initialized == false )
		{
			buffer = new OALSoundBufferBase();
		}
		else
		{
			if( _isStream == false )
			{
				buffer = new OALSoundBuffer();
			}
			else
			{
				buffer = new OALSoundBufferStream();
			}
		}

		if( buffer->load( _soundDecoder ) == false )
		{
			LOG_ERROR( "OALSoundSystem: Failed to create sound buffer from stream" );
			buffer->release();
			buffer = NULL;
		}

		return buffer;
	}
	//////////////////////////////////////////////////////////////////////////
	//SoundBufferInterface* OALSoundSystem::createSoundBufferFromMemory( void * _buffer, int _size, bool _newmem )
	//{

	//	return NULL;
	//}
	//////////////////////////////////////////////////////////////////////////
	void OALSoundSystem::releaseSoundBuffer( SoundBufferInterface * _soundBuffer )
	{
		if( _soundBuffer != NULL )
		{
			_soundBuffer->release();
		}
	}
	//////////////////////////////////////////////////////////////////////////
	void OALSoundSystem::releaseSoundNode( SoundSourceInterface * _sn )
	{
		if( _sn != NULL )
		{
			OALSoundSource* soundSource = static_cast<OALSoundSource*>( _sn );
			// put into pool
			soundSource->~OALSoundSource();
			m_soundSourcePool.push_back( soundSource );
		}
	}
	//////////////////////////////////////////////////////////////////////////
	bool OALSoundSystem::setBlow( bool _active )
	{
		if( m_sulk )
		{
			return m_sulk->activate( _active );
		}

		return false;
	}
	//////////////////////////////////////////////////////////////////////////
	float OALSoundSystem::getBlow()
	{
		if( m_sulk )
		{
			return m_sulk->getBlow();
		}

		return 0.f;
	}
	//////////////////////////////////////////////////////////////////////////
	void OALSoundSystem::setEnoughBlow( float _enoughBlow )
	{
		if( m_sulk )
		{
			m_sulk->setEnoughBlow( _enoughBlow );
		}
	}
	//////////////////////////////////////////////////////////////////////////
	void OALSoundSystem::setBlowCallback( SoundSulkCallbackInterface * _callback )
	{
		if( m_sulk )
		{
			m_sulk->setCallback( _callback );
		}
	}
	//////////////////////////////////////////////////////////////////////////
	void OALSoundSystem::update( float _timing )
	{
		if( m_sulk )
		{
			m_sulk->update();
		}
	}
	//////////////////////////////////////////////////////////////////////////
	ALuint OALSoundSystem::popSource( bool _isStereo )
	{
		ALuint source = 0;
		if( _isStereo == true && m_stereoPool.empty() == false )
		{
			source = m_stereoPool.back();
			m_stereoPool.pop_back();
		}
		else if( _isStereo == false && m_monoPool.empty() == false )
		{
			source = m_monoPool.back();
			m_monoPool.pop_back();
		}
		return source;
	}
	//////////////////////////////////////////////////////////////////////////
	void OALSoundSystem::pushSource( ALuint _source, bool _isStereo )
	{
		if( _source == 0 )
		{
			return;
		}

		if( _isStereo == true )
		{
			m_stereoPool.push_back( _source );
		}
		else
		{
			m_monoPool.push_back( _source );
		}
	}
	//////////////////////////////////////////////////////////////////////////
}	// namespace Menge
