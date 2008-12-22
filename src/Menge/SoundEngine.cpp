#	include "SoundEngine.h"

#	include "SoundEmitter.h"
#	include "LogEngine.h"
//#	include "FileEngine.h"
#	include "Codec.h"
#	include "Interface/SoundCodecInterface.h"

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
			return NULL;
		}

		SoundDecoderInterface* soundDecoder = CodecManager<SoundDecoderInterface>::createDecoder( _filename );
		if( soundDecoder == NULL )
		{
			MENGE_LOG_ERROR( "Error: Can't create sound decoder for file \"%s\"",
						_filename.c_str() );
			return NULL;
		}

		SoundBufferInterface* sample = 
			m_interface->createSoundBuffer( soundDecoder, _isStream );

		if( _isStream  == true )
		{
			m_bufferStreams.insert( std::make_pair( sample, soundDecoder ) );
		}
		else
		{
			CodecManager<SoundDecoderInterface>::releaseDecoder( soundDecoder );
		}

		return sample;
	}
	//////////////////////////////////////////////////////////////////////////
	SoundBufferInterface *	SoundEngine::createSoundBufferFromMemory( void* _buffer, int _size, bool _newmem )
	{
		//SoundBufferInterface * sample =  
		//	m_interface->createSoundBufferFromMemory( _buffer, _size, _newmem );
		assert( 0 && "Not Implemented yet" );
		return NULL;
	}
	//////////////////////////////////////////////////////////////////////////
	void SoundEngine::releaseSoundBuffer( SoundBufferInterface* _soundBuffer )
	{
		TMapBufferStreams::iterator it_find = m_bufferStreams.find( _soundBuffer );
		SoundDecoderInterface* soundDecoder = NULL;
		if( it_find != m_bufferStreams.end() )
		{
			soundDecoder = it_find->second;
			m_bufferStreams.erase( it_find );
		}
		m_interface->releaseSoundBuffer( _soundBuffer );
		if( soundDecoder != NULL )
		{
			CodecManager<SoundDecoderInterface>::releaseDecoder( soundDecoder );
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
		m_interface->update( _timing );
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
	void SoundEngine::onFocus( bool _focus )
	{
		if( _focus == false )
		{
			for( TSetSoundEmitters::iterator it = m_soundEmitters.begin(), it_end = m_soundEmitters.end();
				it != it_end;
				it++ )
			{
				if( (*it)->isPlaying() == true )
				{
					(*it)->pause();
					m_focusEmitters.push_back( (*it) );
				}
			}
			update( 0.0f );
		}
		else
		{
			for( TSoundEmitterVector::iterator it = m_focusEmitters.begin(), it_end = m_focusEmitters.end();
				it != it_end;
				it++ )
			{
				(*it)->play();
			}
			m_focusEmitters.clear();
		}
	}
	//////////////////////////////////////////////////////////////////////////
}