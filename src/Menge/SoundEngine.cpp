#	include "SoundEngine.h"

#	include "SoundEmitter.h"
#	include "LogEngine.h"
#	include "ProfilerEngine.h"
#	include "FileEngine.h"

#	include <algorithm>

namespace Menge
{
	//////////////////////////////////////////////////////////////////////////
	SoundSulkCallback::~SoundSulkCallback()
	{

	}
	//////////////////////////////////////////////////////////////////////////
	SoundEngine::SoundEngine( SoundSystemInterface * _interface )
	: m_interface( _interface )
	, m_sulkcallback(0)
	, m_soundVolume( 1.0f )
	, m_commonVolume( 1.0f )
	, m_initialized( false )
	, m_muted( false )
	{
	}
	//////////////////////////////////////////////////////////////////////////
	SoundEngine::~SoundEngine()
	{
	}
	//////////////////////////////////////////////////////////////////////////
	void	SoundEngine::setListenerOrient( const mt::vec3f& _position, const mt::vec3f& _front, const mt::vec3f& top )
	{
		m_interface->setListenerOrient((float*)_position.m,(float*)_front.m,(float*)top.m);
	}
	//////////////////////////////////////////////////////////////////////////
	SoundSourceInterface *	SoundEngine::createSoundSource(
			bool _isHeadMode, 
			SoundBufferInterface * _sample,
			SoundNodeListenerInterface * _listener = 0 )
	{

		SoundSourceInterface* sound = m_interface->createSoundSource( _isHeadMode, _sample, _listener );
		return	sound;
	}
	//////////////////////////////////////////////////////////////////////////
	SoundBufferInterface * SoundEngine::createSoundBufferFromFile( const String & _filename, bool _isStream )
	{
		if( m_initialized == false )
		{
			return 0;
		}
		FileEngine* fileEngine = Holder<FileEngine>::hostage();
		DataStreamInterface* stream = fileEngine->openFile( _filename );

		SoundBufferInterface* sample = 
			m_interface->createSoundBuffer( stream, _isStream );

		if( _isStream  == true )
		{
			m_bufferStreams.insert( std::make_pair( sample, stream ) );
		}
		else
		{
			fileEngine->closeStream( stream );
		}

		return sample;
	}
	//////////////////////////////////////////////////////////////////////////
	SoundBufferInterface *	SoundEngine::createSoundBufferFromMemory( void* _buffer, int _size, bool _newmem )
	{
		SoundBufferInterface * sample =  
			m_interface->createSoundBufferFromMemory( _buffer, _size, _newmem );

		return sample;
	}
	//////////////////////////////////////////////////////////////////////////
	void SoundEngine::releaseSoundBuffer( SoundBufferInterface* _soundBuffer )
	{
		TMapBufferStreams::iterator it_find = m_bufferStreams.find( _soundBuffer );
		DataStreamInterface* stream = 0;
		if( it_find != m_bufferStreams.end() )
		{
			stream = it_find->second;
			m_bufferStreams.erase( it_find );
		}
		m_interface->releaseSoundBuffer( _soundBuffer );
		if( stream != 0 )
		{
			Holder<FileEngine>::hostage()->closeStream( stream );
		}
	}
	//////////////////////////////////////////////////////////////////////////
	void SoundEngine::releaseSoundSource( SoundSourceInterface* _soundSource )
	{
		m_interface->releaseSoundNode(_soundSource);
	}
	//////////////////////////////////////////////////////////////////////////
	void SoundEngine::setSoundSourceVolume( float _volume )
	{
		m_soundVolume = _volume;
		for( TSetSoundEmitters::iterator it = m_soundEmitters.begin(); it != m_soundEmitters.end(); ++it )
		{
			(*it)->setVolume( _volume );
		}
	}
	//////////////////////////////////////////////////////////////////////////
	void SoundEngine::registerSoundEmitter( SoundEmitter * _emitter )
	{
		//MENGE_LOG( MENGE_TEXT("Sound Registered!") );
		_emitter->setVolume( m_soundVolume );
		m_soundEmitters.insert( _emitter );
	}
	//////////////////////////////////////////////////////////////////////////
	void SoundEngine::unregisterSoundEmitter( SoundEmitter * _emitter )
	{
		//MENGE_LOG( MENGE_TEXT("Sound UnRegistered!") );
		m_soundEmitters.erase( _emitter );
	}
	//////////////////////////////////////////////////////////////////////////
	float SoundEngine::getSoundSourceVolume() const
	{
		return m_soundVolume;
	}
	//////////////////////////////////////////////////////////////////////////	
	void SoundEngine::setCommonVolume( float _volume )
	{
		m_commonVolume = _volume;
		for( TSetSoundEmitters::iterator it = m_soundEmitters.begin(); it != m_soundEmitters.end(); ++it )
		{
			(*it)->updateVolume();
		}
	}
	//////////////////////////////////////////////////////////////////////////
	float SoundEngine::getCommonVolume() const
	{
		if( m_muted )
		{
			return 0.0f;
		}
		return m_commonVolume;
	}
	//////////////////////////////////////////////////////////////////////////
	bool SoundEngine::setBlow( bool _active )
	{
		return m_interface->setBlow( _active );
	}
	//////////////////////////////////////////////////////////////////////////
	float SoundEngine::getBlow()
	{
		return m_interface->getBlow();
	}
	//////////////////////////////////////////////////////////////////////////
	void SoundEngine::setEnoughBlow( float _enoughBlow )
	{
		m_interface->setEnoughBlow( _enoughBlow );
	}
	//////////////////////////////////////////////////////////////////////////
	void SoundEngine::update( float _timing )
	{
		Holder<ProfilerEngine>::hostage()->beginProfile( MENGE_TEXT("SoundEngine") );
		m_interface->update( _timing );
		Holder<ProfilerEngine>::hostage()->endProfile( MENGE_TEXT("SoundEngine") );
	}
	//////////////////////////////////////////////////////////////////////////
	void SoundEngine::setSulkCallback( SoundSulkCallback * _sulkcallback )
	{
		delete m_sulkcallback;
		m_sulkcallback = _sulkcallback;
		m_interface->setBlowCallback( m_sulkcallback );
	}
	//////////////////////////////////////////////////////////////////////////
	bool SoundEngine::initialize()
	{
		if( m_interface != 0 )
		{
			m_initialized = m_interface->initialize( Holder<LogEngine>::hostage()->getInterface() );
		}
		return m_initialized;
	}
	//////////////////////////////////////////////////////////////////////////
	void SoundEngine::mute( bool _mute )
	{
		m_muted = _mute;
		setCommonVolume( m_commonVolume );
	}
	//////////////////////////////////////////////////////////////////////////
}